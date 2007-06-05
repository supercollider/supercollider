WiiCalibrationInfo {
	var <accX_zero, <accY_zero, <accZ_zero, <accX_lg, <accY_lg, <accZ_lg;
	var <nAccX_zero, <nAccY_zero, <nAccZ_zero, <nAccX_lg, <nAccY_lg, <nAccZ_lg;
	var <nX_max, <nX_min, <nX_center, <nY_max, <nY_min, <nY_center;

	printOn { | stream |
		stream
		<< this.class.name << $(
		<< "accX_zero: " << accX_zero << ", "
		<< "accY_zero: " << accY_zero << ", "
		<< "accZ_zero: " << accZ_zero << ", "
		<< "accX_lg: " << accX_lg << ", "
		<< "accY_lg: " << accY_lg << ", "
		<< "accZ_lg: " << accZ_lg << ", "
		<< "nunchuck accX_zero: " << nAccX_zero << ", "
		<< "nunchuck accY_zero: " << nAccY_zero << ", "
		<< "nunchuck accZ_zero: " << nAccZ_zero << ", "
		<< "nunchuck accX_lg: " << nAccX_lg << ", "
		<< "nunchuck accY_lg: " << nAccY_lg << ", "
		<< "nunchuck accZ_lg: " << nAccZ_lg << ", "
		<< "nunchuck x_min: " << nX_min << ", "
		<< "nunchuck x_max: " << nX_max << ", "
		<< "nunchuck x_center: " << nX_center << ", "
		<< "nunchuck y_min: " << nY_min << ", "
		<< "nunchuck y_max: " << nY_max << ", "
		<< "nunchuck y_center: " << nY_center
		<< $)
	}
}

WiiMote {
	var dataPtr, <spec, <>action, <actionSpec; // <slots
	var <>closeAction, <>connectAction, <>disconnectAction;
	var <calibration;
	var <remote_led, <remote_buttons, <remote_motion, <remote_ir;
	var <nunchuck_buttons, <nunchuck_motion, <nunchuck_stick;
	var <classic_buttons, <classic_stick1, <classic_stick2, <classic_analog;
	var <>dumpEvents = false;
	classvar all;
	classvar < eventLoopIsRunning = false;
	
	*initClass {
		all = [];
	}

	*deviceSpec {
		^(
			ax: { remote_motion[0] };
			ay: { remote_motion[1] };
			az: { remote_motion[2] };
			ao: { remote_motion[3] };

			bA: { remote_buttons[0] };
			bB: { remote_buttons[1] };
			bOne: { remote_buttons[2] };
			bTwo: { remote_buttons[3] };
			bMinus: { remote_buttons[4] };
			bHome: { remote_buttons[5] };
			bPlus: { remote_buttons[6] };
			bUp: { remote_buttons[7] };
			bDown: { remote_buttons[8] };
			bLeft: { remote_buttons[9] };
			bRight: { remote_buttons[10] };

			px: { remote_ir[0] };
			py: { remote_ir[1] };
			angle: { remote_ir[2] };
			tracking: { remote_ir[3] };

			nax: { nunchuck_motion[0] };
			nay: { nunchuck_motion[1] };
			naz: { nunchuck_motion[2] };
			nao: { nunchuck_motion[4] };

			nsx: { nunchuck_stick[0] };
			nsy: { nunchuck_stick[1] };

			nbZ: { nunchuck_buttons[0] };
			nbC: { nunchuck_buttons[1] };

			cbX: { classic_buttons[0] };
			cbY: { classic_buttons[1] };
			cbA: { classic_buttons[2] };
			cbB: { classic_buttons[3] };
			cbL: { classic_buttons[4] };
			cbR: { classic_buttons[5] };
			cbZL: { classic_buttons[6] };
			cbZR: { classic_buttons[7] };
			cbUp: { classic_buttons[8] };
			cbDown: { classic_buttons[9] };
			cbLeft: { classic_buttons[10] };
			cbRight: { classic_buttons[11] };
			cbMinus: { classic_buttons[12] };
			cbHome: { classic_buttons[13] };
			cbPlus: { classic_buttons[14] };

			csx1: { classic_stick1[0] };
			csy1: { classic_stick1[1] };

			csx2: { classic_stick2[0] };
			csy2: { classic_stick2[1] };

			caleft: { classic_analog[0] };
			caright: { classic_analog[1] };
		)
	}

	*all {
		^all.copy
	}
	*closeAll {
		all.copy.do({ | dev | dev.close });
	}
	*new { 
		super.new.prInit();
	}
	isOpen {
		^dataPtr.notNil
	}
	close {
		if (this.isOpen) {
			this.prClose;
			all.remove(this);
		};
	}
	setAction{ |key,keyAction|
		actionSpec.put( key, keyAction );
	}
	removeAction{ |key|
		actionSpec.removeAt( key );
	}
	at { | controlName |
		^this.spec.atFail(controlName, {
			Error("invalid control name").throw
		}))
	}
	getLEDState { |id|
		prWiiGetLED( led[0], led[1], led[2], led[3] )
		^led[id]
	}
	setLEDState { |id,state|
		led[id] = state;
		^this.prWiiSetLED( led[0], led[1], led[2], led[3] )
	}

	*start{
		UI.registerForShutdown({
			this.closeAll;
			this.prStop;
		});
		this.prStart;
	}

	*stop{
		this.prStop;
	}

	// PRIVATE
	prInit {
		remote_led = Array.fill( 4, 0 );
		remote_buttons = Array.fill( 11, 0 );
		remote_motion = Array.fill( 4, 0 );
		remote_ir = Array.fill( 4, 0 );
		nunchuck_buttons = Array.fill( 2, 0 );
		nunchuck_motion = Array.fill( 4, 0 );
		nunchuck_stick = Array.fill( 2, 0 );
		classic_buttons = Array.fill( 15, 0 );
		classic_stick1 = Array.fill( 2, 0 );
		classic_stick2 = Array.fill( 2, 0 );
		classic_analog = Array.fill( 2, 0 );

		this.prOpen;
		all = all.add(this);
		closeAction = {};
		connectAction = {};
		disconnectAction = {};
		this.prWiiGetLED;
		calibration = this.prCalibration(WiiCalibrationInfo.new);
 		spec = this.class.deviceSpec;
		actionSpec = IdentityDictionary.new;
// 		caps = IdentityDictionary.new;
// 		slots = IdentityDictionary.new;
	}
	*prStart { 
		eventLoopIsRunning = true;
		_WiiStart
		^this.primitiveFailed
	}
	*prStop {
		eventLoopIsRunning = false;
		_WiiStop
		^this.primitiveFailed
	}
	prOpen { 
		_WiiOpen
		^this.primitiveFailed
	}
	prClose {
		_WiiClose
		^this.primitiveFailed
	}
	prAddress { |address|
		_WiiAddress
		^this.primitiveFailed
	}
	prConnect {
		_WiiConnect
		^this.primitiveFailed
	}
	prDisconnect {
		_WiiDisconnect
		^this.primitiveFailed
	}
	prCalibration { |calib|
		_WiiCalibration
		^this.primitiveFailed
	}
	prGetExpansion {
		_WiiGetExpansion
		^this.primitiveFailed
	}
	prGetBattery {
		_WiiGetBattery
		^this.primitiveFailed
	}
	prEnableExpansion { |onoff|
		_WiiEnableExpansion
		^this.primitiveFailed
	}
	prEnableIRSensor { |onoff|
		_WiiEnableIRSensor
		^this.primitiveFailed
	}
	prEnableMotionSensor { |onoff|
		_WiiEnableMotionSensor
		^this.primitiveFailed
	}
	prSetVibration { |onoff|
		_WiiSetVibration
		^this.primitiveFailed
	}
	prWiiGetLED { | ledstate |
		_WiiGetLED
		^this.primitiveFailed
	}
	prWiiSetLED { |enable1,enable2,enable3,enable4|	// added by Marije Baalman
		// set LED value
		_WiiSetLED
		^this.primitiveFailed
	}
	prHandleEvent { | buttonData, posX, posY, angle, tracking, accX, accY, accZ, orientation, cButtonData, cStickX1, cStickY1, cStickX2, cStickY2, cAnalogL, cAnalogR, nButtonData, nStickX, nStickY, nAccX, nAccY, nAccZ, nOrientation |
		remote_buttons.do{ |it,i| remote_buttons[i] = buttonData.bitTest( i ).asInteger };
		remote_motion = [ accX, accY, accZ, orientation ];
		remote_ir = [ posX, posY, angle, tracking ];
		nunchuck_buttons.do{ |it,i| nunchuck_buttons[i] = nButtonData.bitTest( i ).asInteger };
		nunchuck_motion = [ nAccX, nAccY, nAccZ, nOrientation ];
		nunchuck_stick = [nStickX, nStickY];
		classic_buttons.do{ |it,i| classic_buttons[i] = cButtonData.bitTest( i ).asInteger };
		classic_stick1 = [cStickX1, cStickY1];
		classic_stick2 = [cStickX2, cStickY2];
		classic_analog = [cAnalogL, cAnalogR];

		// event callback
		spec.keysValuesDo{ |key,val,i|
			actionSpec.at( key ).value( val.value );
			if ( dumpEvents, { (key + val.value.round(0.00001)).postln; });
		}
	}
	// this prevents a high cpu cycle when device was detached; added by marije
	prReadError{
		this.close;
		("WARNING: WiiMote was closed unexpectedly").postln;
		closeAction.value;
	}

	prConnectAction{
		"WiiMote connected".postln;
		connectAction.value;
	}

	prDisconnectAction{
		"WiiMote disconnected!".warn;
		disconnectAction.value;
	}
}

// EOF