// =====================================================================
// file:		sclang.sc
// content:		sclang startup file
// author:		stefan kersten <steve@k-hornz.de>
// =====================================================================

// =====================================================================
// install this file as ~/.sclang.sc
//
// the startup file, if present, is executed by sclang after class
// library initialization from Main.startup; you can use it to set
// some defaults that you don't want to put in a custom Main class.
// =====================================================================

// set up Server to look for scsynth in $PATH
Server.program = "scsynth";

// same for Score
Score.program = Server.program;

// EOF