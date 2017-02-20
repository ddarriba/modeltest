#!/bin/sh

modeltest_bin=modeltest-cmd
MT_ARGS="-t mp -f ef -h uigf"

input_msa=$1
output_dir=$2

out_file=`ls $output_dir/*.out`

if [ "`echo $out_file | wc -w`" -eq "1" ]; then
  echo Result already exists: $out_file
else
  ckp_file=`ls $output_dir/*.ckp`
  if [ "`echo $ckp_file | wc -w`" -eq "1" ]; then
    mv $ckp_file $input_msa.ckp
  fi

  modeltest_args="-i $input_msa $MT_ARGS"
  modeltest_outfiles="$input_msa.ckp $input_msa.tree $input_msa.log $input_msa.out"
  $modeltest_bin $modeltest_args

  mv $modeltest_outfiles $output_dir
fi
