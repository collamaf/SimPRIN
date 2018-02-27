#!/bin/bash
for nome in  "PRINmcSimple_2000keV" "PRINmcSimple_2500keV" "PRINmcSimple_3000keV" "PRINmcSimple_3500keV" "PRINmcSimple_4000keV" "PRINmcNTHH" "PRINmcYT_AllFHH"
do
echo "##################################### Analizzo file: " $nome
root -b -l <<EOF
.L AnaPRIN.C
m=new AnaPRIN("RootBuoni/$nome");
m->Loop();
.q;
EOF
done
