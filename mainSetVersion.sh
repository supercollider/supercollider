versionline=$( grep "classvar" VERSION )
mainfile="build/SCClassLibrary/DefaultLibrary/Main.sc"
line=$( grep "classvar" $mainfile )

sed "s/$line/$versionline/" <$mainfile >tempfile
cp tempfile $mainfile
rm tempfile
