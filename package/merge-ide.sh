#!/bin/bash
# merge sclang.app into a SuperCollider IDE app package
while getopts "fml" arg; do
  case $arg in
    f)
      force=true
      ;;
    m)
      move=true
      ;;
    l)
      copy_libs=true
      ;;
  esac
done

shift $(($OPTIND - 1))

sclang=$1
scide=$2
new_sclang=$scide/Contents/MacOS/sclang.app

if [ -d $new_sclang ]; then
	if [ $force ]; then
		rm -rf $new_sclang
	else
		echo "$new_sclang already exists at . Use -f option to overwrite the existing version."
		exit 1
	fi
fi

if [ move == true ]; then
	echo "Moving $sclang to $new_sclang"
	mv $sclang $new_sclang
else
	echo "Copying $sclang to $new_sclang"
	cp -R $sclang $new_sclang
fi


if [ ! copy_libs == true ]; then
	echo "Symlinking Frameworks and PlugIns folders"

	link_dirs=( Frameworks PlugIns )

	for link_dir in "${link_dirs[@]}"
	do
		(
		cd $new_sclang/Contents/$link_dir

		for d in *; do
			ide_framework="../../../../$link_dir/$d"
			if [ -d $ide_framework ]; then
				echo "   Symlinking $d -> $ide_framework"
				rm -rf $d
				ln -s $ide_framework $d
			fi
		done 
		)
	done
fi