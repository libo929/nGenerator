#!/usr/bin/env bash

exeName=NeutronGen

nap()
{
        exeProc=`ps -U $USER | grep ${exeName} | wc -l`

        while [ $exeProc -ge 6 ]; do
		sleep 5
		exeProc=`ps -U $USER | grep ${exeName} | wc -l`
        done
}

Ei=300

while [ ${Ei} -le 600 ]; do
	index=0
        while [ ${index} -le 9 ]; do
		let seed=${Ei}+${index}
	        echo "Ei: "${Ei} ", seed:"${seed}
		let index+=1
	        sed -e "s;\ PARTICLEENERGY;\ ${Ei};" -e "s;\ FILENAME;\ run${Ei}_${seed};" template.mac > run${Ei}_${seed}.mac
	        ./${exeName} run${Ei}_${seed}.mac ${seed} > run${Ei}_${seed}.log 2>&1 &
	        nap
	done

	let Ei+=20
done



