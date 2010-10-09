#!/bin/sh

mainfile="build/SCClassLibrary/DefaultLibrary/Main.sc"

# Double-check that there's exactly one in each!
count1=$( grep -c "classvar" ../VERSION )
count2=$( grep -c "classvar scVersion" $mainfile )
if [ $count1 != 1 ] || [ $count2 != 1 ]; then
	echo "ERROR in setMainVersion.sh: we require exactly one 'classvar' line to be detected. Check ../VERSION and Main.sc."
	exit
fi

versionline=$( grep "classvar" ../VERSION )
line=$( grep "classvar scVersion" $mainfile )

echo $versionline
echo $line

# only update if they don't already match
if [ "$versionline" != "$line" ]; then
	sed "s/$line/$versionline/" <$mainfile >tempfile
	cp tempfile $mainfile
	rm tempfile
fi
