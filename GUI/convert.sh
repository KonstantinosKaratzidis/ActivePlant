#!/bin/bash

mkdir -p generated

for f in ui/*;
do
	pyfile=generated/$(echo $(basename $f) | sed 's/\.ui//').py
	echo $f -\> $pyfile
	pyuic5 -o $pyfile -x $f
done
