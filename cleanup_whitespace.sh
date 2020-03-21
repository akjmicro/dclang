#!/bin/bash

for f in `egrep -Rl "[ ]+$" | egrep ".dc" | egrep -v ".git"`
do
  sed -E -i "s/[ \t]+$//g" $f
done
