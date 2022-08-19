#!/bin/sh

cd "$(dirname "$0")"

echo "Sass version $(sass --version)"

if [ "$1" == "verify" ]; then
    VERIFY_FOLDER="verify"
    mkdir -p "$VERIFY_FOLDER"
    echo "Verify built files"

    find . -name "*.scss"|while read fname; do
        REPO_FILE="${fname//scss/css}"
        VERIFY_FILE="$VERIFY_FOLDER/${fname//scss/css}"

        # echo "Build $VERIFY_FILE"
        sass $fname $VERIFY_FILE
        if cmp -s -- "$REPO_FILE" "$VERIFY_FILE"; then
            echo "$REPO_FILE is identical"
        else
            echo "$REPO_FILE differs"
            rm -r $VERIFY_FOLDER
            exit 1
        fi
    done

    if cmp -s -- "verify/default.css" "../scdoc.css"; then
        echo "scdoc.css matches default.css"
    else
        echo "scdoc.css differs from scdoc.css"
        rm -r $VERIFY_FOLDER
        exit 1
    fi
    rm -r $VERIFY_FOLDER
    echo "Everything looks fine"
    exit 0
fi


find . -name "*.scss"|while read fname; do
    echo "Build $fname";
    sass $fname ${fname//scss/css}
done

# update scdoc.css in case HTML docs
# are used w/o SC IDE
echo "Copy default.css to ../scdoc.css"
cp default.css ../scdoc.css

echo "Finished building CSS files"
