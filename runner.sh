tries=10
for count in 10 100 1000 10000
do
echo "${count} length ; random order"
for i in $( seq 0 $tries )
do
entries=($(shuf -i 0-20000 -n "${count}"))
echo "${entries[@]}";
time ./main "${entries[@]}";
done
echo "${count} length ; reverse order"
for i in $( seq 0 $tries )
do
entries=($(shuf -i 0-20000 -n "${count}" | sort -rn))
echo "${entries[@]}";
time ./main "${entries[@]}";
done
done
