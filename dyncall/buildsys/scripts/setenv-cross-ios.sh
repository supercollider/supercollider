
IOS_ARCH="armv6"
IOS_SDK_VERSION="4.3" 

export TARGET_MACH="-arch ${IOS_ARCH}" 
export TARGET_ARCH="-arch ${IOS_ARCH} -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS${IOS_SDK_VERSION}.sdk" 


