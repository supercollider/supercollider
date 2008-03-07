versionline=$( grep "classvar" VERSION )
mainfile="build/SCClassLibrary/DefaultLibrary/Main.sc"
line=$( grep "classvar scVersion" $mainfile )

echo $versionline
echo $line

sed "s/$line/$versionline/" <$mainfile >tempfile
cp tempfile $mainfile
rm tempfile
