#!/bin/sh
# Первый аргумент - префикс
# Второй аргумент - название подпапки (с размером или конкретным распределением)
prefix=$1

if [ $# -ne 2 ]; then
echo wrong number of args
exit 1
fi

mkdir /home/yegorius/Projects/stribog/reports/${prefix}/${2}

for arity in 2 3 4 5 6 7 8
do
./FT-stribog $arity $prefix $prefix
mkdir /home/yegorius/Projects/stribog/reports/${prefix}/${2}/${arity}

for layer in ${prefix}_*
do
size=`wc -c < $layer`
flag=0
if [ $size -ge 4000 ]; then
 size=`expr $size / 2`
 streams=16
 elif [ $size -ge 2000 ]; then
  streams=8
 elif [ $size -ge 1000 ]; then
  size=`expr $size \* 2`
  streams=4
 elif [ $size -ge 500 ]; then
  size=`expr $size \* 4`
  streams=2
 else
  size=`expr $size \* 8`
  streams=1
  flag=1
fi

if [ $flag -eq 0 ]; then
./assess $size << ANSWERS
0
$layer
1
0
$streams
1
ANSWERS
else
./assess $size << ANSWERS
0
$layer
0
111110100000000
0
$streams
1
ANSWERS
fi

# Определение энтропии
./entropy_test $layer >> ./experiments/AlgorithmTesting/finalAnalysisReport.txt

# Перенос результатов в другую папку
current=`pwd`
cd /home/yegorius/Projects/stribog/reports/${prefix}/${2}/${arity}
mkdir $layer
cd $layer
mv /home/yegorius/sts-2.1.2/${layer} .
cp -r /home/yegorius/sts-2.1.2/experiments/AlgorithmTesting/* .
cd $current
done
done
mv $prefix /home/yegorius/Projects/stribog/reports/${prefix}/${2}
