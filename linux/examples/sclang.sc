// =====================================================================
// file:		sclang.sc
// content:		sclang startup file
// author:		stefan kersten <steve@k-hornz.de>
// =====================================================================

// =====================================================================
// install this file as ~/.sclang.sc
//
// the startup file, if present, is executed by sclang after class
// library initialization; you can use it to set some defaults that
// you don't want to put in a custom Main class.
// =====================================================================

// set up Server to look for scsynth in $PATH
Server.program = "scsynth"

// when running jack with root privileges for realtime operation, i use 
// this to start scsynth as root.
// you must have /etc/sudoers set up correctly, e.g. not prompting for 
// a password for scsynth.

// Server.program = "sudo scsynth"

// EOF