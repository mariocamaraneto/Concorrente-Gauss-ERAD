#!/bin/bash

num=10000;
saida="arquivo"                
saida_result="result.txt"

rm $saida 2> /dev/null
rm $saida_result 2> /dev/null

declare -A A

echo $num >> $saida

for(( i=1; i<=$num; i++))
	do
		x[$i]=$(($RANDOM % 10))
		echo ${x[$i]} >> $saida_result
	done


for(( i=1; i<=$num; i++))
	do
		for(( j=1; j<=$num; j++))
			do
				A[$i,$j]=$(($RANDOM % 10))
			done
	done

for(( i=1; i<=$num; i++))
	do
		for(( j=1; j<=$num; j++))
			do
				echo -n "${A[$i,$j]} " >> $saida
			done
		echo "" >> $saida
	done

for(( i=1; i<=$num; i++))
	do
		soma=0
		for(( j=1; j<=$num; j++))
			do
				aux=$(( ${A[$i,$j]} * ${x[$j]} ))
				soma=$(($aux+$soma))
			done
	    echo -n "$soma " >> $saida
	done



