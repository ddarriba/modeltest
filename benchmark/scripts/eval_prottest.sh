#!/bin/bash

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp skip_eval data_type
#       restore_ckp = {true, false}
#       skip_eval   = {true, false}
#       data_type   = {nt, aa}

prottest_bin=bin/prottest/prottest-3.4.2.jar
PT_ARGS="-I -G -F -BIC -AIC -AICC -DT -S 0 -threads 1"

input_msa=$1
output_dir=$2
restore_ckp=$3
skip_eval=$4
data_type=$5

if [ "$data_type" != "aa" ]; then
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

    prottest_outfiles="$input_msa.out"
    prottest_args="-i $input_msa $PT_ARGS -o $prottest_outfiles"
    echo "COMMAND:   java -jar $prottest_bin $prottest_args "
    java -jar $prottest_bin $prottest_args 

    mv $prottest_outfiles $output_dir
  fi
fi

# BUILD RESULTS

if [ ! -f $out_fname ]; then
  echo "Results file missing"
else
  res_fname=$output_dir/$input_fname.results
  rm -f $res_fname

  aic_line=`fgrep -n "Best model according to AIC:" $out_fname | cut -d':' -f1`
  aicc_line=`fgrep -n "Best model according to AICc:" $out_fname | cut -d':' -f1`
  bic_line=`fgrep -n "Best model according to BIC:" $out_fname | cut -d':' -f1`

  bic_data=`sed "$((bic_line+5))q;d" $out_fname`
  aic_data=`sed "$((aic_line+5))q;d" $out_fname`
  aicc_data=`sed "$((aicc_line+5))q;d" $out_fname`

  t_time=`grep "Runtime" $out_fname | rev | cut -d' ' -f1 | rev`
  h_time=`echo $t_time | cut -d'h' -f1`
  m_time=`echo $t_time | cut -d':' -f2`
  s_time=`echo $t_time | cut -d':' -f3`
  t_time=`echo $h_time*3600 + $m_time*60 + $s_time | bc -l`

  echo "time $t_time"

  bic_name=`echo $bic_data | cut -d' ' -f1`
  bic_lnl=`echo $bic_data | cut -d' ' -f5`
  bic_score=`echo $bic_data | cut -d' ' -f3`
  bic_w=`echo $bic_data | cut -d' ' -f4`

  aic_name=`echo $aic_data | cut -d' ' -f1`
  aic_lnl=`echo $aic_data | cut -d' ' -f5`
  aic_score=`echo $aic_data | cut -d' ' -f3`
  aic_w=`echo $aic_data | cut -d' ' -f4`

  aicc_name=`echo $aicc_data | cut -d' ' -f1`
  aicc_lnl=`echo $aicc_data | cut -d' ' -f5`
  aicc_score=`echo $aicc_data | cut -d' ' -f3`
  aicc_w=`echo $aicc_data | cut -d' ' -f4`

  printf "PT     %10s %10s %15s %15s %10s %10s %15s %15s %10s %10s %15s %15s %10s \n" $t_time \
                                         $bic_name $bic_lnl $bic_score $bic_w \
                                         $aic_name $aic_lnl $aic_score $aic_w \
                                         $aicc_name $aicc_lnl $aicc_score $aicc_w
fi
