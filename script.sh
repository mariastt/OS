#!/bin/bash
exec 3<>gp2y_data
set -o noglob
exec 4<>keypad_data
STR=''
while :
do
read -u 4 linekp
read -u 3 linegp2
Lgp2=$(bc -l<<<"scale=4;(61.3899*e(1.1076*l(1000/($linegp2*0.1875))))")
Lgp2R=$(echo ${Lgp2%%.*})
if [ "*" = "$linekp" ]
then
echo "Dis=$Lgp2R"
	if [ $Lgp2R -ge 30 ]
	then
		if [ $Lgp2R -le 50 ]
		then
			if [ -z "$STR" ]
			then echo "error empty corner" 
			else
				sudo ./step_motor -q 1000 $STR
				echo "corner: $STR, distance: $Lgp2R"
				STR=''
			fi
		fi
	fi
else
	STR="$STR$linekp"
fi
echo "$STR"

done

