#!/bin/bash

echo "----------------------------------------"
echo "|       modeltest benchmark suite      |"
echo "----------------------------------------"

# select whether you want to restore existing checkpoint files
# warning: execution times might not be valid
restore_ckp=false
skip_eval=true

testname=$1
data_type=$2

[[ -z $testname ]] && echo "Dataset argument missing..." && exit
[[ -z $data_type ]] && echo "Data type {nt,aa} argument missing..." && exit

tmp_dir=/tmp
data_dir=datasets/$testname
file_list=datasets/$testname.list
results_dir=results/$testname

[[ ! -d $data_dir ]] && echo "Dataset $data_dir does not exist..." && exit

if [ ! -d ${results_dir} ]; then
  echo building results directory: ${results_dir}
  mkdir -p ${results_dir}
fi

if [ -f $file_list ]; then
  datasets=`cat $file_list`
else
  datasets=`find $data_dir -name "*.phy" -o -name "*.fas" | sort`
fi

n_datasets=`echo $datasets | wc -w`
echo "found ${n_datasets} datasets"
echo

for ((idx=1;idx<=${n_datasets};idx++)); do
  msa_file=`echo ${datasets} | cut -d' ' -f${idx}`
  tree_file=${data_dir}/dataset${idx}.tree
  out_file=`echo ${msa_file} | sed "s/datasets/results/g"`

  out_basedir=`echo ${out_file} | rev | cut -d'/' -f2- | rev`
  out_mt_file=${out_file}.mt.out
  out_jmt_file=${out_file}.jmt.out

  mkdir -p ${out_basedir}
  echo evaluating dataset ${idx}/${n_datasets}: ${msa_file} 

  # JMODELTEST
if [ "$data_type" == "nt" ]; then
  mkdir -p ${out_basedir}/jmt
  scripts/eval_jmodeltest.sh $msa_file $out_basedir/jmt $restore_ckp $skip_eval $data_type
else
  mkdir -p ${out_basedir}/pt
  scripts/eval_prottest.sh $msa_file $out_basedir/jmt $restore_ckp $skip_eval $data_type
fi

  # MODELTEST
  mkdir -p ${out_basedir}/mt
  scripts/eval_modeltest.sh $msa_file $out_basedir/mt $restore_ckp $skip_eval $data_type

  #IQ-TREE
  mkdir -p ${out_basedir}/iqtree
  scripts/eval_iqtree.sh $msa_file $out_basedir/iqtree $restore_ckp $skip_eval $data_type

done

echo
