#!/bin/bash`

> ~/power_up_tool/power-up/v4/config/conf_pid.txt
while read -r line
do
	name="$line"
	pid=`wmctrl -l -p | grep -i -m1 $name | cut -f4 -d' '`
	echo $pid >> ~/power_up_tool/power-up/v4/config/conf_pid.txt	

done < ~/power_up_tool/power-up/v4/config/conf.txt

exit 0
