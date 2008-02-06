+ SynthDef {	

    // This fixes the bug of a '-p' directory being created on windows.
    // -p is a unix option to mkdir, it is not recognised by the windows mkdir program
	*initClass {
		synthDefDir = Platform.userAppSupportDir ++ "\\synthdefs\\";
		("mkdir " + synthDefDir.quote).systemCmd; // Ensure exists
	}
    
}