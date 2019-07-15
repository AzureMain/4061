#!/bin/csh
# CSci4061 Spring 2017 Assignment 1 
# Name: <Full name1>, <Full name2> 
# Student ID: <ID1>, <ID2>
# CSELabs machine: <machine>
# Additional comments 

if ( $#argv < 3 ) then
  echo "Wrong number of arguments!"
  echo "Script terminated."
  exit
endif

set input_dir=$1
set output_dir=$2
set noglob
set file_patterns=( $argv[3-] )
set files=()
set converted=()
set newthumbs=()
set htmlfilename="pic_name_xx"
echo "What is the theme?"
set theme=$<


#If the output directory does not exist, then make the directory
mkdir -p $output_dir/thumbs

foreach n ($file_patterns[*])
	if (! ($n:e == "gif" | $n:e == "tif" | $n:e == "png" | $n:e == "tiff")) then
		echo "The pattern" '"' $n '"' "has an incorrect file extension and will be skipped."
		continue
	endif
	
	#Generate an array of files matching the pattern provided
	set files=`find $input_dir -name "$n"`
	#Iterate through the list of files, file by file
	foreach m ($files[*])
		#Changes the filename to uppercase with .jpg extension
		set b=` echo "$m:t:r" | tr [a-z] [A-Z] `
		set b=$b.jpgar
		#Tests if file exists
		if ( -f "$output_dir/$b" ) then
			#File exists, check for thumbnail
			if ( -f "$output_dir/thumbs/$b:r_thumb.jpg") then
				#If thumbnail exists, say so.
			else
				#Else if it doesn't, notify and act.
				convert -thumbnail x200 $output_dir/$b $output_dir/thumbs/$b:r_thumb.jpg
				set newthumbs=( $newthumbs $output_dir/thumbs/$b:r_thumb.jpg )
			endif
		else
			#File does not exist, so follow through with the file conversion
			convert $m $output_dir/$b
			set converted=( $converted $output_dir/$b )

			if ( -f "$output_dir/thumbs/$b:r_thumb.jpg") then
			else
				convert -thumbnail x200 $output_dir/$b $output_dir/thumbs/$b:r_thumb.jpg
				set newthumbs=( $newthumbs $output_dir/thumbs/$b:r_thumb.jpg )

			endif
		endif
	end
end


if ( ${#converted} == 0 && ${#newthumbs} == 0) then
	echo "No files changed, exiting."
	exit
else
	if (${#converted} == 0) then 
	echo "Thumbs created: $newthumbs" 
	endif
	if (${#newthumbs} == 0) then 
	echo "Files converted: $converted" 
	endif
	if (${#newthumbs} > 0 && ${#converted} > 0) then 
		echo "Files converted: $converted"
		echo "Thumbs created: $newthumbs"
	endif
	echo "${#converted} files converted. ${#newthumbs} thumbnails created"
endif

cat << EOF > $htmlfilename.html
<html><head><title>Test Images</title></head><body>
<table border = 2>
<tr>
EOF
foreach m ($converted[*])
cat << EOF >> $htmlfilename.html
	<td><a href="./$output_dir//$m:t:r.jpg">
	<img src="./$output_dir//thumbs/$m:t:r_thumb.jpg"/></a>
	</td>
EOF
end
cat << EOF >> $htmlfilename.html
</tr>
<tr>
EOF
foreach m ($converted[*])
	echo "	<td align="center">`identify -format '%w x %h' $output_dir/thumbs/$m:t:r_thumb.jpg`</td>" >> $htmlfilename.html
end
cat << EOF >> $htmlfilename.html
</tr>
<tr>
EOF
foreach m ($converted[*])
	set modtime=( `stat -c %y $m` )
	echo "	<td align="center">"$modtime[1]"</td>" >> $htmlfilename.html
end
cat << EOF >> $htmlfilename.html
</tr>
</table>
<p> Theme: < $theme > </p> 
<p> Date & day: < `date +%A` `date +%d-%m-%Y` > </p> 
</body></html>
EOF



echo "Script done. Exiting."
echo 