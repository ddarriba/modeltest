#!/bin/bash

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp
#       restore_ckp = {true, false}

modeltest_bin=modeltest-cmd
MT_ARGS="-t mp -f ef -h uigf"

input_msa=$1
output_dir=$2
restore_ckp=$3
skip_eval=$4

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_fname=$output_dir/$input_fname.out

if [ "$skip_eval" == "true" ]; then
  echo "Evaluation skipped"
else
  if [ -f $out_fname ]; then
    echo "Result already exists: $out_fname"
  else
    ckp_fname=$output_dir/$input_fname.ckp
    if [ $restore_ckp == true -a -f $ckp_fname ]; then
      mv $ckp_fname $input_msa.ckp
    fi

    modeltest_args="-i $input_msa $MT_ARGS"
    modeltest_outfiles="$input_msa.ckp $input_msa.tree $input_msa.log $input_msa.out"
    $modeltest_bin $modeltest_args

    mv $modeltest_outfiles $output_dir
  fi
fi

# BUILD RESULTS

log_fname=$output_dir/$input_fname.log
if [ ! -f $log_fname ]; then
  echo "Results file missing"
else
  res_fname=$output_dir/$input_fname.results
  rm -f $res_fname
  outdata=`fgrep "   1 " $out_fname | xargs | cut -d' ' -f2,4,5,7,9,11,12,14,16,18,19,21,23,25,26,28`

  t_time=`grep "Computation of likelihood scores completed" $log_fname | rev | cut -d' ' -f1 | rev`
  h_time=`echo $t_time | cut -d'h' -f1`
  m_time=`echo $t_time | cut -d':' -f2`
  s_time=`echo $t_time | cut -d':' -f3`
  t_time=`echo $h_time*3600 + $m_time*60 + $s_time | bc -l`

  echo "time $t_time"
  echo "BIC  `echo $outdata | cut -d' ' -f1-4`" #>> $res_fname
  echo "AIC  `echo $outdata | cut -d' ' -f5-8`" #>> $res_fname
  echo "AICc `echo $outdata | cut -d' ' -f9-12`" #>> $res_fname
  echo "DT   `echo $outdata | cut -d' ' -f13-16`" #>> $res_fname
fi
