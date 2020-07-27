#!/bin/sh
# Этот скрипт запускает генерацию слоев и тесты для И стрибога, И ксора
# Первый аргумент - префикс
# Второй аргумент - название подпапки (с размером или конкретным распределением)
prefix=$1
folder=$2

if [ $# -ne 2 ]; then
echo wrong number of args
exit 1
fi

report_folder=/home/yegorius/reports/${prefix}/${folder}

for mode in stribog xor
do

mkdir -p ${report_folder}/${mode}

for arity in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
./FT-${mode} $arity $prefix $prefix
mkdir ${report_folder}/${mode}/${arity}

for layer in ${prefix}_*
do
size=`wc -c < $layer`
flag=0
if [ $size -ge 128 ]; then
 streams=`expr $size / 128`
 size=1024
 else
  size=`expr $size \* 8`
  streams=1
  flag=1
fi

if [ $flag -eq 0 ]; then
./assess $size << ANSWERS
0
$layer
0
111110100010010
2
4
3
7
0
$streams
1
ANSWERS
else
./assess $size << ANSWERS
0
$layer
0
111110000010010
2
3
3
6
0
$streams
1
ANSWERS
fi

# Определение энтропии
./entropy_test $layer >> ./experiments/AlgorithmTesting/finalAnalysisReport.txt

# Перенос результатов в другую папку
current=`pwd`
cd ${report_folder}/${mode}/${arity}
mkdir $layer
cd $layer
mv /home/yegorius/sts-2.1.2/${layer} .
cp -r /home/yegorius/sts-2.1.2/experiments/AlgorithmTesting/* .
cd $current
done
done
done
mv $prefix ${report_folder}
