+ Main
{
	platformClass { ^WindowsPlatform }
}

+ Char {
	isPathSeparator {
		^#[$\\, $/].includes(this)
	}
}