max=7000
for i in `seq 1 $max`
do
echo $i
./trabalhoso >> execucoes/execucao_$i
done
