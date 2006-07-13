Platform
{
	name { ^this.subclassResponsibility }
	systemExtensionDir { ^this.subclassResponsibility }
	userExtensionDir { ^this.subclassResponsibility }
	pathSeparator { ^this.subclassResponsibility }
	startup { }
	shutdown { }
}

UnixPlatform : Platform
{
	pathSeparator { ^$/ }
}
