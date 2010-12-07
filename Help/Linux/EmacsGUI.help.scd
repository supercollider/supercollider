/// The Emacs interface actually has some nice options for user interfaces as well. Here are some examples:

// create a buffer:
p = EmacsBuffer.new;

// show the buffer:
p.front;

// create a key action for the buffer:

p.defineKey( "hello", { "hey there".postln; } );

// type hello on the window and look at the postbuffer
p.front;

p.defineKey( "hey there", { "hello".postln; } );

// type hey there and look at the postbuffer
p.front;

// put some text in the buffer:

p.insert( "this is a really interesting text to read in the buffer" );
p.front;

// make a newline:

p.newline;

// make a button:

p.button( [ "on", "off", "in between" ], { |v| v.postln; } );
p.front;

// make a button with a different look:
p.button( [ "on", "off", "in between" ], { |v| v.postln; }, "******", "+++++" );
p.front;

// create a close button:
p.closeButton;
// clicking it will close the buffer!

p = EmacsBuffer.new;

p.editableField( "write something here", "like this?", { |v| v.postln; } );
p.front;

// moving the cursor around in the buffer:
p.gotoBob; // beginning
p.gotoEob; // end
p.goto( 5 ); // go to fifth position

// making it a sclang-mode buffer to write code in:
Emacs.evalLispExpression( p.use( [ 'sclang-mode' ] ).asLispString );

// close it:
p.free;

//

p = EmacsBuffer.new; // args: name
p.front;

// making a number box
n = EmacsNumber.new( p, "number box", [0,5].asSpec, { |v| v.postln; } ); // args: buffer, tag/label, spec, action
p.front;

// making a button:
x = EmacsButton( p, [ "on", "off", "in between" ], { |v| v.postln; }, "******", "+++++" );
// args: buffer, states, action, prefix, suffix
p.front;

t = EmacsText( p, "hello", 30 ); // args: buffer, string, size, align
p.front;

p.newline;
t = EmacsText( p, "helloooo", 30, \right ); // args: buffer, string, size, align

p.newline;
e = EmacsEditableField( p, "edit field", "edit me" ).action_( { |v| v.postln; } );
p.front;

b = EmacsPushButton( p, "hello" ).action_( { "do it".postln; } );
p.front;

// you can disable things:

b.enabled_( false );
p.front;



// and last but not least, it provides a class browser:

EmacsClassBrowser.new( EmacsBuffer );

