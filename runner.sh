tries=5
for threads in 1 2 3 4
do
for count in 51000
do
printf $"\n ${count} length ; ${threads} threads; random order"
for i in $( seq 0 $tries )
do
rm input.in;
countN=$count;
while [ $countN -gt 0 ]; do
if [ $countN -gt 20000 ]; then
countS=20000
else 
countS=$countN
fi
shuf -i 0-100000 -n $countS | tr " " "\n" >> input.in; 
countN=$[countN-20000];
done
./main $threads input.in -q;
done
done
done
