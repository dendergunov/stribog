#!/bin/sh
# Первый аргумент - префикс
# Второй аргумент - название папки с размером
# Третий аргумент - название папки с арностью
prefix=$1

if [ $# -ne 3 ]; then
echo wrong number of args
exit 1
fi

for layer in ${prefix}*
do
size=`wc -c < $layer`
flag=0
if [ $size -ge 2000 ]; then
 size=`expr $size / 2`
 streams=16
 elif [ $size -ge 1000 ]; then
  streams=8
 elif [ $size -ge 500 ]; then
  size=`expr $size \* 2`
  streams=4
 elif [ $size -ge 250 ]; then
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
cd ../Projects/stribog/reports/${prefix}/${2}/${3}
mkdir $layer
cd $layer
mv /home/yegorius/sts-2.1.2/${layer} .
cp -r /home/yegorius/sts-2.1.2/experiments/AlgorithmTesting/* .
cd $current
done
