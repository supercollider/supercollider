Installing SuperCollider in a Jailbroken iDevice. 
January 2011

1 - jailbreak your iDevice (both blackrain on iphone and redsn0w for iOS 4.1 on 2nd gen iPod Touch are reported to work)

2 - install OpenSSH and ldid from Cydia. You probably also want to install SBSettings for a convenient way to among other things switch SSH on and off. It's a must-have app for jailbroken iDevices. You should have SSH turned off at all times except when you really need it

3 - Change the SSH password! Running around with SSH on and the default password is a Major Security Problem

4 - Get a free developer account from Apple and download and install XCode

5 - Disable code signing. For sdk 3.0 and above you need to disable code signing by editing the file /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.0.sdk/SDKSettings.plist . Follow the instructions at 

http://thebigboss.org/hosting-repository/submit-your-app/compile-for-cydia-submission/.

6 - Open the iphone xcode projects,  in the project and in the SuperCollider target settings -> build, set code signing to “Don’t code sign”.

7 - Find the architecture settings also in the build tab of the project's and the targets SuperCollider info window and set it to the architecture of the device your iDevice (armv6 or armv7)

7a - Alternatively, you can also build a "fat" binary that contains builds for both architectures and "thin" it afterwards:

cd into you SuperCollider.app directory and type
lipo SuperCollider -thin [yourArchitecture] -output [newFileName]

Then replace the SuperCollider executable inside your app folder with the "thinned" one. Of course, you have to also name it SuperCollider. And I'm sure there's also a lipo command that automatically replaces the original file.

8 - Get the App on the iDevice. You can either use Cyberduck or follow these directions:

http://blog.paulbetts.org/index.php/2008/07/22/running-your-iphone-sdk-app-on-iphone-without-the-app-store/

9 - follow the instructions in the link above to code sign the app. That will be:
chmod 755 /Applications/SuperCollider.app
ldid -S /Applications/SuperCollider.app/SuperCollider

10 - logout of your device and close SSH

11 - restart the springboard:  killall SpringBoard

11a - You can also tap respring in SBSettings

If the codesigning has worked correctly (remember to only use non-fat binaries), (and if there's no cold wind blowing from the northwest) you can now start SuperCollider.app from the Springboard and take the world by storm.