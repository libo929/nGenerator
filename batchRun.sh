#!/usr/bin/env bash

echo "Hello"

Ep=1900

while [ ${Ep} -le 2400 ]; do
#while [ ${Ep} -le 2000 ]; do
    echo ${Ep}
	sed -e "s;\ PROTONENERGY;\ ${Ep};" -e "s;\ FILENAME;\ run${Ep};" template.mac > run${Ep}.mac
	./NeutronGen run${Ep}.mac
	let Ep+=20
done
