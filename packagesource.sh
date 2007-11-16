#!/bin/sh

DATE=`date "+%Y-%m-%d"`

if [ -d SuperCollider-Source ]; then
	echo "Please remove the ./SuperCollider-Source directory before running this script."
	exit 1
fi

mkdir SuperCollider-Source
svn export --force ./ SuperCollider-Source
ditto -ck --sequesterRsrc --keepParent SuperCollider-Source "SuperCollider-Source-$DATE.zip"
