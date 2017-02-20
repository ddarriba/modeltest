echo "----------------------------------------"
echo "|       modeltest benchmark suite      |"
echo "----------------------------------------"

tmp_dir=/tmp
data_dir=datasets/stamatak
results_dir=results/stamatak

jmodeltest_bin=~/Repositories/phylogenetics/jmodeltest2/dist/jModelTest.jar

if [ ! -d ${results_dir} ]; then
  echo building results directory: ${results_dir}
  mkdir -p ${results_dir}
fi

datasets=`find $data_dir -name "*.phy" -o -name "*.fas" | sort`
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
#  echo "java -jar ${jmodeltest_bin} -d ${msa_file} -u ${tree_file} -i -g 4 -f -s 11"
#  time java -jar ${jmodeltest_bin} -d ${msa_file} -u ${tree_file} -i -g 4 -f -s 11 -BIC -AIC -AICc -DT 2>&1 > ${out_jmt_file}

  # MODELTEST
  echo "${modeltest_bin} -i ${msa_file} $MT_ARGS 2>&1 > ${out_mt_file}"
  mkdir -p ${out_basedir}/mt
  scripts/eval_modeltest.sh $msa_file $out_basedir/mt

  mkdir -p ${out_basedir}/iqtree
  scripts/eval_iqtree.sh $msa_file $out_basedir/iqtree

  exit
done

echo
