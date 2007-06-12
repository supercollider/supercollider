
A note for using Xcode 2.1.

Xcode 2.1 changed where it puts build products, putting them in per configuration build directories. This causes builds of SC to fail because they are expected to all be in the same location. The fix for this is as follows:

Open a Terminal window. Type this on the command line:

defaults write com.apple.Xcode UsePerConfigurationBuildLocations NO

This will cause Xcode to revert to placing build products directly in the build directory as in Xcode version 2.0.

--- james mccartney 
