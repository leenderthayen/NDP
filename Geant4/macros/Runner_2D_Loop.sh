#!/bin/bash
echo "Commencing the batch GEANT4 Simulations"
echo "First arugment to the a.out is the kinetic energy of the incident particle"
echo "Second arugment to the a.out is the incident angle"
echo "Third arugment to the a.out is particle type that are defined in the GEANT4 simulation"
echo "(ie. alpha, gamma, proton, e-, e+, ...)"

for i in {25..1000..25}
  do 
     for j in {0..20..1}
  	do 
 	    ./a.out $i $j e-
 	    ./NDD basic_looped.mac
 	done
 done
