#!/bin/bash

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp skip_eval data_type
#       restore_ckp = {true, false}
#       skip_eval   = {true, false}
#       data_type   = {nt, aa}

jmodeltest_bin=bin/jmodeltest/jModelTest.jar
JMT_ARGS="-i -g 4 -f -s 11 -BIC -AIC -AICc -DT -t BIONJ -tr 1"

input_msa=$1
output_dir=$2
restore_ckp=$3
skip_eval=$4
data_type=$5

if [ "$data_type" != "nt" ]; then
  echo "Wrong datatype"
  exit
fi

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_fname=$output_dir/$input_fname.out

if [ "$skip_eval" == "false" ]; then
  if [ -f $out_fname ]; then
    echo "Result already exists: $out_fname"
  else
    #ckp_fname=$output_dir/$input_fname.ckp
    #if [ $restore_ckp == true -a -f $ckp_fname ]; then
    #  mv $ckp_fname $input_msa.ckp
    #fi

    jmodeltest_args="-d $input_msa $JMT_ARGS -o $input_msa.out"
    echo "COMMAND:   java -jar $jmodeltest_bin $jmodeltest_args "
    java -jar $jmodeltest_bin $jmodeltest_args 
    jmodeltest_outfiles="$input_msa.out"

    mv $jmodeltest_outfiles $output_dir
  fi
fi

# BUILD RESULTS

if [ ! -f $out_fname ]; then
  echo "JMT"
else
  res_fname=$output_dir/$input_fname.results
  rm -f $res_fname

  aic_line=`fgrep -n "AIC MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`
  aicc_line=`fgrep -n "AICc MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`
  bic_line=`fgrep -n "BIC MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`

  if [ ! -z $aicc_line ]; then
  bic_data=`sed "$((bic_line+4))q;d" $out_fname`
  aic_data=`sed "$((aic_line+4))q;d" $out_fname`
  aicc_data=`sed "$((aicc_line+4))q;d" $out_fname`

  t_time=`grep "Computation of likelihood scores completed" $out_fname | rev | cut -d' ' -f1 | rev`
  h_time=`echo $t_time | cut -d'h' -f1`
  m_time=`echo $t_time | cut -d':' -f2`
  s_time=`echo $t_time | cut -d':' -f3`
  t_time=`echo $h_time*3600 + $m_time*60 + $s_time | bc -l`

  bic_name=`echo $bic_data | cut -d' ' -f1`
  bic_lnl=`echo $bic_data | cut -d' ' -f2`
  bic_score=`echo $bic_data | cut -d' ' -f4`
  bic_w=`echo $bic_data | cut -d' ' -f6`

  aic_name=`echo $aic_data | cut -d' ' -f1`
  aic_lnl=`echo $aic_data | cut -d' ' -f2`
  aic_score=`echo $aic_data | cut -d' ' -f4`
  aic_w=`echo $aic_data | cut -d' ' -f6`

  aicc_name=`echo $aicc_data | cut -d' ' -f1`
  aicc_lnl=`echo $aicc_data | cut -d' ' -f2`
  aicc_score=`echo $aicc_data | cut -d' ' -f4`
  aicc_w=`echo $aicc_data | cut -d' ' -f6`
  else
    echo "JMT"
  fi
  printf "JMT    %10s %10s %15s %15s %10s %10s %15s %15s %10s %10s %15s %15s %10s \n" $t_time \
                                         $bic_name $bic_lnl $bic_score $bic_w \
                                         $aic_name $aic_lnl $aic_score $aic_w \
                                         $aicc_name $aicc_lnl $aicc_score $aicc_w
fi
