Platform
{
	name { ^this.subclassResponsibility }
	systemAppSupportDir { ^this.subclassResponsibility }
	userAppSupportDir { ^this.subclassResponsibility }
	systemExtensionDir { ^this.subclassResponsibility }
	userExtensionDir { ^this.subclassResponsibility }
	pathSeparator { ^this.subclassResponsibility }
	startup { }
	shutdown { }
	
	*userAppSupportDir { ^thisProcess.platform.userAppSupportDir }
	*userExtensionDir { ^thisProcess.platform.userExtensionDir }
}

UnixPlatform : Platform
{
	pathSeparator { ^$/ }
}
