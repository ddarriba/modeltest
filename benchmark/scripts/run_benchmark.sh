#!/bin/bash

echo "----------------------------------------"
echo "|       modeltest benchmark suite      |"
echo "----------------------------------------"

# select whether you want to restore existing checkpoint files
# warning: execution times might not be valid
restore_ckp=true
skip_eval=false

tmp_dir=/tmp
data_dir=datasets/stamatak
results_dir=results/stamatak

if [ ! -d ${results_dir} ]; then
  echo building results directory: ${results_dir}
  mkdir -p ${results_dir}
fi

datasets=`find $data_dir -name "*.phy" -o -name "*.fas" | sort`
n_datasets=`echo $datasets | wc -w`
echo "found ${n_datasets} datasets"
echo

for ((idx=1;idx<=${n_datasets};idx++)); do
  idx=68
  msa_file=`echo ${datasets} | cut -d' ' -f${idx}`
  tree_file=${data_dir}/dataset${idx}.tree
  out_file=`echo ${msa_file} | sed "s/datasets/results/g"`

  out_basedir=`echo ${out_file} | rev | cut -d'/' -f2- | rev`
  out_mt_file=${out_file}.mt.out
  out_jmt_file=${out_file}.jmt.out

  mkdir -p ${out_basedir}
  echo evaluating dataset ${idx}/${n_datasets}: ${msa_file} 

  # JMODELTEST
  mkdir -p ${out_basedir}/jmt
  scripts/eval_jmodeltest.sh $msa_file $out_basedir/jmt $restore_ckp $skip_eval

  # MODELTEST
  mkdir -p ${out_basedir}/mt
  scripts/eval_modeltest.sh $msa_file $out_basedir/mt $restore_ckp $skip_eval

  mkdir -p ${out_basedir}/iqtree
  scripts/eval_iqtree.sh $msa_file $out_basedir/iqtree $restore_ckp $skip_eval

exit
done

echo
