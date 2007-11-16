#!/bin/sh

DATE=`date "+%Y-%m-%d"`

if [ -d SuperCollider_Source ]; then
	echo "Please remove the ./SuperCollider_Source directory before running this script."
	exit 1
fi

mkdir SuperCollider_Source
svn export --force ./ SuperCollider_Source
zip -r -9 "SuperCollider_Source_$DATE.zip" SuperCollider_Source
