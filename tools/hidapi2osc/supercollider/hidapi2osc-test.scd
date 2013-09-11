OSCFunc.trace( true )
OSCFunc.trace( false )

OSCdef( \hidInfo, { |msg| msg.postln; }, '/hid/info' );

OSCdef( \elementInfo, { |msg| msg.postln; }, '/hid/element/info' );
OSCdef( \elementNumber, { |msg| msg.postln; }, '/hid/element/number' );

// OSCdef( \elementData, { |msg| msg.postln; }, '/hid/element/data' );

n = NetAddr.new( "127.0.0.1", 57151 );

n.sendMsg( "/hidapi2osc/info" );

n.sendMsg( "/hid/open", 1103, 53251 );

n.sendMsg( "/hid/info", 0);

n.sendMsg( "/hid/elements/info", 0);

n.sendMsg( "/hid/close", 0);

n.sendMsg( "/hidapi2osc/quit" );