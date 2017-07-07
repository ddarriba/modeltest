#!/bin/bash

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp skip_eval data_type
#       restore_ckp = {true, false}
#       skip_eval   = {true, false}
#       data_type   = {nt, aa}
#
# OUTPUT:
#       time [execution_time]
#       BIC  [best_bic_model]  [lnL] [bic_score]  [bic_weight]
#       AIC  [best_aic_model]  [lnL] [aic_score]  [aic_weight]
#       AICc [best_aicc_model] [lnL] [aicc_score] [aicc_weight]

iqtree_bin=iqtree
IQ_ARGS="-m TESTONLY"

input_msa=$1
output_dir=$2
restore_ckp=$3
skip_eval=$4
data_type=$5

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_fname=`ls $output_dir/$input_fname.iqtree` 2> /dev/null

if [ "$skip_eval" == "false" ]; then
  if [ "`echo $out_fname | wc -w`" -eq "1" ]; then
    echo "Result already exists: $out_fname"
  else
    ckp_fname=$output_dir/$input_fname.log
    if [ $restore_ckp == true -a -f $ckp_fname ]; then
      mv $output_dir/$input_fname.* $input_dir/
    fi

    iqtree_args="-s $input_msa $IQ_ARGS"
    iqtree_outfiles="$input_msa.iqtree $input_msa.model $input_msa.log $input_msa.treefile"
    $iqtree_bin $iqtree_args

    mv $iqtree_outfiles $output_dir
  fi
else
  if [ ! -f $out_fname ]; then
    echo "IQTREE"
    exit
  fi
fi

# BUILD RESULTS
log_fname=$output_dir/$input_fname.log
sum_fname=$output_dir/$input_fname.sum

if [ ! -f $log_fname ]; then
  echo "IQTREE"
else
  if [ ! -f $sum_fname ]; then
    if [ "$data_type" == "nt" ]; then
      models_start=`fgrep -n "1  JC" $log_fname | cut -d':' -f1`
      models_end=`echo $((models_start+87))`
    else
      models_start=`fgrep -n "1  Dayhoff" $log_fname | cut -d':' -f1`
      models_end=`echo $((models_start+143))`
    fi

    # extract scores for computing weights
    aic_scores=`sed -n ${models_start},${models_end}p $log_fname | tr -s ' ' | cut -d' ' -f 6`
    aicc_scores=`sed -n ${models_start},${models_end}p $log_fname | tr -s ' ' | cut -d' ' -f 7`
    bic_scores=`sed -n ${models_start},${models_end}p $log_fname | tr -s ' ' | cut -d' ' -f 8`

    bic_m=`grep -e "^Bayesian Information Criterion" $log_fname | xargs | cut -d' ' -f4`
    bic_desc=`grep -e " $bic_m " $log_fname | head -n 1 | xargs | cut -d' ' -f2,3,7`
    bic_name=`echo $bic_desc | cut -d' ' -f1`
    bic_lnl=`echo $bic_desc | cut -d' ' -f2`
    bic_score=`echo $bic_desc | cut -d' ' -f3`
    bic_w=`R --vanilla --slave --args $bic_scores < scripts/compute_weight.r`

    aic_m=`grep -e "^Akaike Information Criterion" $log_fname | xargs | cut -d' ' -f4`
    aic_desc=`grep -e " $aic_m " $log_fname | head -n 1 | xargs | cut -d' ' -f2,3,5`
    aic_name=`echo $aic_desc | cut -d' ' -f1`
    aic_lnl=`echo $aic_desc | cut -d' ' -f2`
    aic_score=`echo $aic_desc | cut -d' ' -f3`
    aic_w=`R --vanilla --slave --args $aic_scores < scripts/compute_weight.r`

    aicc_m=`grep -e "^Corrected Akaike Information Criterion" $log_fname | xargs | cut -d' ' -f5`
    aicc_desc=`grep -e " $aicc_m " $log_fname | head -n 1 | xargs | cut -d' ' -f2,3,6`
    aicc_name=`echo $aicc_desc | cut -d' ' -f1`
    aicc_lnl=`echo $aicc_desc | cut -d' ' -f2`
    aicc_score=`echo $aicc_desc | cut -d' ' -f3`
    aicc_w=`R --vanilla --slave --args $aicc_scores < scripts/compute_weight.r`

    sel_time=`grep "CPU time for model selection" $log_fname | cut -d' ' -f6`
    t_time=`grep "Total wall-clock time used" $log_fname | cut -d' ' -f5`

    printf "IQTREE %10s %10s %15s %15s %10s %10s %15s %15s %10s %10s %15s %15s %10s %10s \n" $sel_time \
                                           $bic_name $bic_lnl $bic_score $bic_w \
                                           $aic_name $aic_lnl $aic_score $aic_w \
                                           $aicc_name $aicc_lnl $aicc_score $aicc_w \
                                           $t_time > $sum_fname
  fi
  cat $sum_fname
fi
