data_dir=datasets/stamatak
results_dir=results/stamatak


datasets=`find $data_dir -name "*.phy" -o -name "*.fas" | sort`
n_datasets=`echo $datasets | wc -w`
echo "found ${n_datasets} datasets"
echo


for ((idx=1;idx<=${n_datasets};idx++)); do
  msa_file=`echo ${datasets} | cut -d' ' -f${idx}`
  tree_file=${data_dir}/dataset${idx}.tree

  out_file=`echo ${msa_file} | sed "s/datasets/results/g"`
  out_basedir=`echo ${out_file} | rev | cut -d'/' -f2- | rev`

  TODO
done
