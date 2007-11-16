#!/bin/sh

DATE=`date "+%Y-%m-%d"`

if [ -d SuperCollider_Source ]; then
	echo "Please remove the ./SuperCollider_Source directory before running this script."
	exit 1
fi

mkdir SuperCollider_Source
svn export --force ./ SuperCollider_Source
ditto -ck --sequesterRsrc --keepParent SuperCollider_Source "SuperCollider_Source_$DATE.zip"
