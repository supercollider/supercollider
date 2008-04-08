#!/bin/bash
#SCvim pipe
#this is a script to pass things from scvim to sclang
#
#Copyright 2007 Alex Norman
#
#This file is part of SCVIM.
#
#SCVIM is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#SCVIM is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with SCVIM.  If not, see <http://www.gnu.org/licenses/>.

if [ $SCVIM_PIPE_LOC  ]; then
	pipe_loc=$SCVIM_PIPE_LOC
else
	pipe_loc="/tmp/sclang-pipe"
fi

if [ $SCLANG_RUNDIR ]; then
	rundir=$SCLANG_RUNDIR
else
	rundir="/tmp"
fi

if [ $SCVIM_PIPE_PID_LOC ]; then
	pid_loc=$SCVIM_PIPE_PID_LOC
else
	pid_loc="/tmp/sclangpipe_app-pid"
fi

app=`which sclang`
app=$app" -d "$rundir

#don't mess with these
sclangargs=""
readfifo=false

while getopts "RXS:hH" opt; do
	case $opt in
		R  )	readfifo=true;;
#S	)	sclangargs=$OPTARG;;
		h  )	echo "sclangpipe is a wrapper script for sclang"
#echo -e "usage:\nsclangpipe [-S \"sclang arguments\"] [-h] [-H]"
#echo -e "usage:\nsclangpipe [-h] [-H]"
				echo -e "usage:\nsclangpipe [-h]"
				echo -e "\t -h prints this help"
#echo -e "\t -H simply calls sclang -h so that you can see the arguments you can give to -S"
				exit 0;;
#		H  )	sclang -h
#				exit 0;;
		\? )	echo "blah"
				exit 1
	esac
done

#if we should read from the fifo then do that
if [ $readfifo == true ] 
then
	if [ -e $pipe_loc ]
	then
		rm $pipe_loc
	fi
	mkfifo $pipe_loc
	
	while [ 1==1 ];
	do
		cat $pipe_loc
	done
	exit 0
fi

echo $$ > $pid_loc

#otherwise
trap "rm $pipe_loc && rm $pid_loc && exit" INT
$0 -R | $app -d $rundir
