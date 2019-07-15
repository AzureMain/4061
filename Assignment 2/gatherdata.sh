#!/bin/csh

make clean;
make;

set noglob
set i = 1

while ( $i < 11 )
set j = 0

	echo "Testing ./parellel_convert $i out in"
	while ( $j < 20	)
		cp -n -a _master_in/. in/
		/usr/bin/time -o testing/temp.txt -v ./parallel_convert $i out in
		awk 'NR==2' testing/temp.txt >> testing/nthreads_$i.txt
		@ j++
	end
	sed -i -r 's/[^0-9. ]*//g' testing/nthreads_$i.txt
	awk '{ total += $1; count++ } END { print count?total/count:"NaN" }' testing/nthreads_$i.txt >> testing/averages.txt
	@ i++
end

echo "Script done. Exiting."
echo 
