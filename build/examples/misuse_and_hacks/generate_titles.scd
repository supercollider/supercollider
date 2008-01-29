//redFrik april'06 - title generation techniques
//osx only
//give credit if you use any of these in your code

//----------
~file= FileReader.read("/usr/share/dict/words");	//will take a little while to load
(
~fileFunc= {
	var b, c;
	b= ~file.choose[0];
	c= ~file.choose[0];
	b= b[0].toUpper++b.copyRange(1, b.size-1);
	c= c[0].toUpper++c.copyRange(1, c.size-1);
	(c++b).join
};
)
10.do{~fileFunc.value.postln};""

//----------
~file= FileReader.read("/usr/share/dict/web2a");
(
~fileFunc= {
	var b, c;
	b= ~file.choose[0];
	c= ~file.choose[0];
	b= b[0].toUpper++b.copyRange(1, b.size-1);
	c= c[0].toUpper++c.copyRange(1, c.size-1);
	(c++b).join
};
)
10.do{~fileFunc.value.postln};""

//----------
~file= FileReader.read("/usr/share/dict/web2");
(
~fileFunc= {
	var b, c;
	b= ~file.choose[0];
	c= ~file.choose[0];
	b= b[0].toUpper++b.copyRange(1, b.size-1);
	c= c[0].toUpper++c.copyRange(1, c.size-1);
	(c++b).join
};
)
10.do{~fileFunc.value.postln};""

//----------
~file= FileReader.read("/usr/share/dict/propernames");
(
~fileFunc= {
	var b, c;
	b= ~file.choose[0];
	c= ~file.choose[0];
	b= b[0].toUpper++b.copyRange(1, b.size-1);
	c= c[0].toUpper++c.copyRange(1, c.size-1);
	(c++b).join
};
)
10.do{~fileFunc.value.postln};""

//----------
~file= FileReader.read("/usr/share/dict/connectives");
(
~fileFunc= {
	var b, c;
	b= ~file.choose[0];
	c= ~file.choose[0];
	b= b[0].toUpper++b.copyRange(1, b.size-1);
	c= c[0].toUpper++c.copyRange(1, c.size-1);
	(c++b).join
};
)
10.do{~fileFunc.value.postln};""

