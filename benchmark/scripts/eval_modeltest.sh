#!/bin/bash

# model selection scripts keep the same structure for every software
# ARGS: input_msa output_dir restore_ckp skip_eval data_type
#       restore_ckp = {true, false}
#       skip_eval   = {true, false}
#       data_type   = {nt, aa}

modeltest_bin=modeltest-cmd
MT_ARGS="-t mp -f ef -h uigf"

input_msa=$1
output_dir=$2
restore_ckp=$3
skip_eval=$4
data_type=$5

input_fname=`echo $input_msa | rev | cut -d'/' -f1 | rev`
input_dir=`echo $input_msa | rev | cut -d'/' -f2- | rev`

out_fname=$output_dir/$input_fname.out

if [ "$data_type" == "aa" ]; then
  MT_ARGS="$MT_ARGS -d aa -m DAYHOFF,LG,DCMUT,JTT,MTREV,WAG,RTREV,CPREV,VT,BLOSUM62,MTMAM,MTART,MTZOA,PMB,HIVB,HIVW,JTTDCMUT,FLU,STMTREV"
fi

if [ "$skip_eval" == "false" ]; then
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
  echo "MTNG"
else
  res_fname=$output_dir/$input_fname.results
  rm -f $res_fname
  outdata=`grep -e "^       1  " $out_fname | xargs | cut -d' ' -f2,4,5,7,9,11,12,14,16,18,19,21,23,25,26,28`

  t_time=`grep "Computation of likelihood scores completed" $log_fname | rev | cut -d' ' -f1 | rev`
  h_time=`echo $t_time | cut -d'h' -f1`
  m_time=`echo $t_time | cut -d':' -f2`
  s_time=`echo $t_time | cut -d':' -f3`
  t_time=`echo $h_time*3600 + $m_time*60 + $s_time | bc -l`

  bic_name=`echo $outdata | cut -d' ' -f1`
  bic_lnl=`echo $outdata | cut -d' ' -f2`
  bic_score=`echo $outdata | cut -d' ' -f3`
  bic_w=`echo $outdata | cut -d' ' -f4`

  aic_name=`echo $outdata | cut -d' ' -f5`
  aic_lnl=`echo $outdata | cut -d' ' -f6`
  aic_score=`echo $outdata | cut -d' ' -f7`
  aic_w=`echo $outdata | cut -d' ' -f8`

  aicc_name=`echo $outdata | cut -d' ' -f9`
  aicc_lnl=`echo $outdata | cut -d' ' -f10`
  aicc_score=`echo $outdata | cut -d' ' -f11`
  aicc_w=`echo $outdata | cut -d' ' -f12`

  printf "MTNG   %10s %10s %15s %15s %10s %10s %15s %15s %10s %10s %15s %15s %10s \n" $t_time \
                                         $bic_name $bic_lnl $bic_score $bic_w \
                                         $aic_name $aic_lnl $aic_score $aic_w \
                                         $aicc_name $aicc_lnl $aicc_score $aicc_w
fi
