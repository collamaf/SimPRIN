
#!/bin/bash
for j in -2
do
for i in 1 0
do
varA="./exampleB1 $i $j  ../run1.mac"

echo $varA
$varA


done
done

