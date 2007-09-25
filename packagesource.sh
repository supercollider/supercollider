#!/bin/sh

DATE=`date "+%Y-%m-%d"`

if [ -d SuperCollider-source ]; then
	echo "Please remove the ./SuperCollider-source directory before running this script."
	exit 1
fi


mkdir SuperCollider-source

for item in Headers Source build Synth.xcodeproj Plugins.xcodeproj Language.xcodeproj Psycollider Resources SConstruct "Standalone resources" Windows linux clean-compile.sh compile.sh COPYING package.sh "README OS X"; do
  echo "Exporting: $item"
  svn export -r COMMITTED --force "$item" SuperCollider-source/"$item"
done


zip -r -9 "SuperCollider-source_$DATE.zip" SuperCollider-source
