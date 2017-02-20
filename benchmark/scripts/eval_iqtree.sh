#!/bin/sh

iqtree_bin=iqtree
IQ_ARGS="-m TESTONLY"

input_msa=$1
output_dir=$2

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_file=`ls $output_dir/$input_fname.iqtree`
if [ "`echo $out_file | wc -w`" -eq "1" ]; then
  echo Result already exists: $out_file
else
  ckp_file=`ls $output_dir/$input_fname.log`
  if [ "`echo $ckp_file | wc -w`" -eq "1" ]; then
    mv $output_dir/$input_fname.* $input_dir/
  fi

  iqtree_args="-s $input_msa $IQ_ARGS"
  iqtree_outfiles="$input_msa.iqtree $input_msa.model $input_msa.log $input_msa.treefile"
  $iqtree_bin $iqtree_args

  mv $iqtree_outfiles $output_dir
fi
