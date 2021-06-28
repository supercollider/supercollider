#!/bin/sh

find . -name "*.scss"|while read fname; do
    echo "Build $fname";
    sass $fname ${fname//scss/css}
done

# update scdoc.css in case HTML docs
# are used w/o SC IDE
cp default.css ../scdoc.css

echo "Finished building CSS files"
