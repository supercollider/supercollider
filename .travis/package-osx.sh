#!/bin/sh

mkdir -p $HOME/artifacts

cd $TRAVIS_BUILD_DIR/BUILD/Install/SuperCollider
echo "Removing Finder cruft..."
xattr -cr SuperCollider.app
echo "Attempting codesigning"
security set-key-partition-list -S apple-tool:,apple: -s -k $SC_KEYCHAIN_PASSWORD supercollider.keychain
codesign --deep --force --verify --verbose --timestamp --sign "Developer ID Application: Joshua Parmenter" --options runtime SuperCollider.app
codesign --deep --force --verify --verbose --timestamp --sign "Developer ID Application: Joshua Parmenter" --options runtime SuperCollider.app/Contents/Resources/scsynth 
codesign --deep --force --verify --verbose --timestamp --sign "Developer ID Application: Joshua Parmenter" --options runtime SuperCollider.app/Contents/Resources/plugins/*
#MUST be last!
codesign --deep --force --verify --verbose --timestamp --sign "Developer ID Application: Joshua Parmenter" --options runtime SuperCollider.app/Contents/MacOS/SuperCollider 

ditto -c -k --rsrc --keepParent SuperCollider.app SuperCollider.app.zip

#parse the output of this commmand for the UUID for this norarization:
#The RequestUUID is what will be needed in the following commands
#No errors uploading 'SuperCollider.app.zip'.
#RequestUUID = 07c23ea0-7b88-4912-a120-4d1613684a78
xcrun altool --notarize-app -t osx -f SuperCollider.app.zip --primary-bundle-id net.sourceforge.supercollider \
 -u $APPLE_USER_NAME -p $APPLE_USER_PASSWORD --output-format json 

cd ..
echo "Packaging..."
zip -q -r --symlinks $HOME/artifacts/SC-$TRAVIS_COMMIT.zip SuperCollider
echo "Copying build!"
cp $HOME/artifacts/SC-$TRAVIS_COMMIT.zip $HOME/SuperCollider-$VERSION_TO_BUILD-macOS.zip
echo "Done!"
