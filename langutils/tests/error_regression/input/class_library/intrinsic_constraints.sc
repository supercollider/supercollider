//# VERSION 3.16
// no-prelude
// intrinsic: AbstractObjectExperimental
// intrinsic: Object : AbstractObjectExperimental

/*
Contains all the common classes, which are required to compile, but breaks an intrinsic constraint, given as the VM
would, see sc_inspect's --intrinsic: Object's superclass must be AbstractObjectExperimental.
*/

AbstractObjectExperimental {}
Object : Class {}
Class {}
String {}
True {}
False {}
Nil {}
Float {}
Integer {}
Char {}
Symbol {}
Ref {}
Array {}
Event {}
