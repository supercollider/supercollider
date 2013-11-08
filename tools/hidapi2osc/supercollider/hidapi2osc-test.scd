OSCFunc.trace( true )
OSCFunc.trace( false )

OSCdef( \hidStarted, { |msg| msg.postln; }, '/hidapi2osc/started' );
OSCdef( \hidQuit, { |msg| msg.postln; }, '/hidapi2osc/quit' );

OSCdef( \hidOpen, { |msg| msg.postln; }, '/hid/open' );
OSCdef( \hidOpenError, { |msg| msg.postln; }, '/hid/open/error' );
OSCdef( \hidClose, { |msg| msg.postln; }, '/hid/closed' );
OSCdef( \hidCloseError, { |msg| msg.postln; }, '/hid/close/error' );

OSCdef( \hidNumber, { |msg| msg.postln; }, '/hid/number' );
OSCdef( \hidInfo, { |msg| msg.postln; }, '/hid/info' );
OSCdef( \hidInfoError, { |msg| msg.postln; }, '/hid/info/error' );


OSCdef( \elementNumber, { |msg| msg.postln; }, '/hid/element/number' );
OSCdef( \elementInfo, { |msg| msg.postln; }, '/hid/element/info' );
OSCdef( \elementInfoError, { |msg| msg.postln; }, '/hid/element/info/error' );


OSCdef( \elementData, { |msg| msg.postln; }, '/hid/element/data' );
OSCdef( \deviceData, { |msg| msg.postln; }, '/hid/device/data' );

n = NetAddr.new( "127.0.0.1", 57151 );

n.sendMsg( "/hidapi2osc/info" );

n.sendMsg( "/hid/open", 1103, 53251 );

n.sendMsg( "/hid/open", 1452, 539 );

n.sendMsg( "/hid/info", 0);

n.sendMsg( "/hid/element/info", 0);

n.sendMsg( "/hid/close", 0);

n.sendMsg( "/hidapi2osc/quit" );