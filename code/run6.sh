#!/bin/bash

valgrind __targets_valgrind/graupel_generate \
	--prototype=../crystal-prototypes/spheroid-lowpoly.ice \
	--deformations=r_x:1:0,r_y:1:0,r_z:1:0,s:1:0.25 \
	--D_max=100 --dump-geometry=/home/torbjorr/Skrivbord/graupel2.obj \
	--dump-stats=/home/torbjorr/Skrivbord/graupel2.txt
