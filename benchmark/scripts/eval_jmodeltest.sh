#!/bin/sh

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp
#       restore_ckp = {true, false}

jmodeltest_bin=bin/jmodeltest/jModelTest.jar
JMT_ARGS="-i -g 4 -f -s 11 -BIC -AIC -AICc -DT -t BIONJ -tr 1"

input_msa=$1
output_dir=$2
restore_ckp=$3

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_fname=$output_dir/$input_fname.out

if [ -f $out_fname ]; then
  echo Result already exists: $out_fname
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

# BUILD RESULTS

res_fname=$output_dir/$input_fname.results
rm -f $res_fname

aic_line=`fgrep -n "AIC MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`
aicc_line=`fgrep -n "AICc MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`
bic_line=`fgrep -n "BIC MODEL SELECTION : Selection uncertainty" $out_fname | cut -d':' -f1`

bic_data=`sed "$((bic_line+4))q;d" $out_fname`
aic_data=`sed "$((aic_line+4))q;d" $out_fname`
aicc_data=`sed "$((aicc_line+4))q;d" $out_fname`

t_time=`grep "Computation of likelihood scores completed" $out_fname | rev | cut -d' ' -f1 | rev`
h_time=`echo $t_time | cut -d'h' -f1`
m_time=`echo $t_time | cut -d':' -f2`
s_time=`echo $t_time | cut -d':' -f3`
t_time=`echo $((h_time*3600 + m_time*60 + s_time))`

echo "time $t_time"

echo "BIC  `echo $bic_data | xargs | cut -d' ' -f1,2,4,6`" #>> $res_fname
echo "AIC  `echo $aic_data | xargs | cut -d' ' -f1,2,4,6`" #>> $res_fname
echo "AICc `echo $aicc_data | xargs | cut -d' ' -f1,2,4,6`" #>> $res_fname

