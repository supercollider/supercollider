Platform
{
	name { ^this.subclassResponsibility }

	systemAppSupportDir { _Platform_systemAppSupportDir }
	userAppSupportDir { _Platform_userAppSupportDir }
	systemExtensionDir { _Platform_systemExtensionDir }
	userExtensionDir { _Platform_userExtensionDir }
	pathSeparator { ^this.subclassResponsibility }

	startup { }
	shutdown { }

	*systemAppSupportDir { ^thisProcess.platform.systemAppSupportDir }
	*userAppSupportDir { ^thisProcess.platform.userAppSupportDir }
	*systemExtensionDir { ^thisProcess.platform.systemExtensionDir }
	*userExtensionDir { ^thisProcess.platform.userExtensionDir }
}

UnixPlatform : Platform
{
	pathSeparator { ^$/ }
}
