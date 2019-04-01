#!/usr/bin/env bash
set -e
mpicc -std=gnu99 -o mcPii mcPii.c -lm
is_scaled=0
size=100000

#while [ $is_scaled -le 2 ]
#do
proc_num=1
            while [ $proc_num -le 12 ]
            do
                    mpiexec -np $proc_num ./mcPii 1000000 $is_scaled
                    mpiexec -np $proc_num ./mcPii 100000000 $is_scaled
                    mpiexec -np $proc_num ./mcPii 1000000000 $is_scaled
                    ((proc_num++))
	    done
 #    ((is_scaled++))
#done
#mpiexec mcPii 100000 0
