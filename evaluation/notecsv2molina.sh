#!/bin/bash

input_csv=$1

if [ $# != 2 ]; then
  minimum_duration=0
else
  minimum_duration=$2
fi

awk -v md="$minimum_duration" 'BEGIN {FS=","}; {if ($2 >= md) {$2=$1+$2; $3 = 69+(log($3/440)/log(2)*12); print $1,$2,$3;}}' < $input_csv

