#!/bin/bash

make brainstem

sshpass -p gummy82 scp brainstem.out root@fuzzy:/home/root/

sshpass -p gummy82 ssh root@fuzzy 
