// wiiremote.c
// Copyright by Masayuki Akamatsu
// Based on "DarwiinRemote" by Hiroaki Kimura

#include <unistd.h>
#include "wiiremote.h"


// this type is used a lot (data array):
typedef unsigned char darr[];

#define	kTrial	10
#define kWait	10000
//  the unit of kWait is microseconds, thus 10000 means 10ms

#define kWiiIRPixelsWidth 1024.0
#define kWiiIRPixelsHeight 768.0


Boolean	requestUpdates(WiiRemoteRef wiiremote);
void myEventListener(IOBluetoothL2CAPChannelRef channel, void *refCon, IOBluetoothL2CAPChannelEvent *event);


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

void wiiremote_init(WiiRemoteRef wiiremote)
{
	wiiremote->inquiry = nil;
	wiiremote->device = nil;
	wiiremote->ichan = nil;
	wiiremote->cchan = nil;

	wiiremote->address = nil;

	wiiremote->accX = 0x10;
	wiiremote->accY = 0x10;
	wiiremote->accZ = 0x10;
	wiiremote->buttonData = 0;

	wiiremote->lowZ = 0;
	wiiremote->lowX = 0;
	wiiremote->leftPoint = -1;
	wiiremote->tracking = false;

	wiiremote->batteryLevel = 0;

	wiiremote->readingRegister = false;
	wiiremote->isMotionSensorEnabled = false;
	wiiremote->isVibrationEnabled = false;
	wiiremote->isIRSensorEnabled = false;
	wiiremote->wiiIRMode = kWiiIRModeExtended;
	wiiremote->isExpansionPortEnabled = false;
	wiiremote->isExpansionPortAttached = false;
	wiiremote->expType = WiiExpNotAttached;

	wiiremote->isLED1Illuminated = false;
	wiiremote->isLED2Illuminated = false;
	wiiremote->isLED3Illuminated = false;
	wiiremote->isLED4Illuminated = false;

	wiiremote->nAccX = 0x10;
	wiiremote->nAccY = 0x10;
	wiiremote->nAccZ = 0x10;
	wiiremote->nButtonData = 0;

	wiiremote->nLowZ = 0;
	wiiremote->nLowX = 0;

}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

Boolean	openCChan(WiiRemoteRef wiiremote)
{
	short		i;
	IOReturn	ret;

	// open L2CAPChannel : BluetoothL2CAPPSM = 17
	for (i=0; i<kTrial; i++)
	{
		ret = IOBluetoothDeviceOpenL2CAPChannelSync(wiiremote->device, &(wiiremote->cchan), 17, myEventListener, (void *)wiiremote);
		if ( ret == kIOReturnSuccess)
			break;
		usleep(kWait); //  wait 10ms
	}
	if (i==kTrial)
	{
		wiiremote->cchan = nil;
		IOBluetoothDeviceCloseConnection(wiiremote->device);
		return false;
	}
	IOBluetoothObjectRetain(wiiremote->cchan);

	return (ret==kIOReturnSuccess);
}

Boolean	openIChan(WiiRemoteRef wiiremote)
{
	short		i;
	IOReturn	ret;

	// open L2CAPChannel : BluetoothL2CAPPSM = 19
	for (i=0; i<kTrial; i++)
	{
		ret = IOBluetoothDeviceOpenL2CAPChannelSync(wiiremote->device, &(wiiremote->ichan), 19, myEventListener, (void *)wiiremote);
		if ( ret == kIOReturnSuccess)
			break;
		usleep(kWait); //  wait 10ms
	}
	if (i==kTrial)
	{
		wiiremote->ichan = nil;
		IOBluetoothL2CAPChannelCloseChannel(wiiremote->cchan);
		IOBluetoothObjectRelease(wiiremote->cchan);
		IOBluetoothDeviceCloseConnection(wiiremote->device);
		return false;
	}
	IOBluetoothObjectRetain(wiiremote->ichan);

	return (ret==kIOReturnSuccess);
}

//--------------------------------------------------------------------------------------------

Boolean sendCommand(WiiRemoteRef wiiremote, unsigned char *data, unsigned short length)
{
	unsigned char buf[40];
	IOReturn	ret;
	int			i;

	memset(buf,0,40);
	buf[0] = 0x52;
	memcpy(buf+1, data, length);
	if (buf[1] == 0x16)
		length=23;
	else
		length++;

	usleep(kWait); //  wait 10ms	// Done to make sure commands don't happen too fast.

	for (i = 0; i<kTrial; i++)
	{
		ret = IOBluetoothL2CAPChannelWriteSync(wiiremote->cchan, buf, length);
		if (ret == kIOReturnSuccess)
			break;
		usleep(kWait);
	}

	if (ret != kIOReturnSuccess)
		wiiremote_disconnect(wiiremote);

	return (ret==kIOReturnSuccess);
}

Boolean	writeData(WiiRemoteRef wiiremote, const unsigned char *data, unsigned long address, unsigned short length)
{
	unsigned char cmd[22];
	int i;
	unsigned long addr = address;


	for(i=0 ; i<length ; i++)
		cmd[i+6] = data[i];

	for(;i<16 ; i++)
		cmd[i+6]= 0;

	cmd[0] = 0x16;
	cmd[1] = (addr>>24) & 0xFF;
	cmd[2] = (addr>>16) & 0xFF;
	cmd[3] = (addr>> 8) & 0xFF;
	cmd[4] = (addr>> 0) & 0xFF;
	cmd[5] = length;

	// and of course the vibration flag, as usual
	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;

	data = cmd;

	return sendCommand(wiiremote, cmd, 22);
}

Boolean readData(WiiRemoteRef wiiremote, unsigned long address, unsigned short length)
{

	unsigned char cmd[7];
	unsigned long addr = address;
	unsigned short len = length;

	cmd[0] = 0x17;
	cmd[1] = (addr>>24)&0xFF;
	cmd[2] = (addr>>16)&0xFF;
	cmd[3] = (addr>> 8)&0xFF;
	cmd[4] = (addr>> 0)&0xFF;

	cmd[5] = (len >> 8)&0xFF;
	cmd[6] = (len >> 0)&0xFF;

	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;
	if (cmd[1] & 0x02) wiiremote->readingRegister = true;

	return sendCommand(wiiremote, cmd, 7);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

void checkDevice(WiiRemoteRef wiiremote, IOBluetoothDeviceRef device)
{
	CFStringRef	name;
	CFStringRef	address;

	if (wiiremote_isconnected(wiiremote))
		return;

	name = IOBluetoothDeviceGetName(device);
	address = IOBluetoothDeviceGetAddressString(device);
	if (name != nil && address != nil)
	{
		if (CFStringCompare(name, CFSTR("Nintendo RVL-CNT-01"), 0) == kCFCompareEqualTo)
		{
			if ( CFStringGetLength(wiiremote->address) == 0
			|| CFStringCompare(address, wiiremote->address, kCFCompareCaseInsensitive) == kCFCompareEqualTo)
			{
				wiiremote->device = IOBluetoothObjectRetain(device);
				if ( wiiremote_connect(wiiremote) == false )
					wiiremote_disconnect(wiiremote);
			}
		}
	}
}

void myFoundFunc(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device)
{
	checkDevice((WiiRemoteRef)refCon, device);
}

void myUpdatedFunc(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device, uint32_t devicesRemaining)
{
	checkDevice((WiiRemoteRef)refCon, device);
}

void myCompleteFunc(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOReturn error, Boolean aborted)
{
	IOReturn	ret;

	if (aborted) return; // called by stop ;)

	if (error != kIOReturnSuccess)
	{
		wiiremote_stopsearch((WiiRemoteRef)refCon);
		return;
	}

	/*
	 ret = IOBluetoothDeviceInquiryStart(((WiiRemoteRef)refCon)->inquiry);
	if (ret != kIOReturnSuccess)
	{
		wiiremote_stopsearch((WiiRemoteRef)refCon);
	}
	*/
}

//--------------------------------------------------------------------------------------------

Boolean wiiremote_isconnected(WiiRemoteRef wiiremote)
{
	Boolean	result;

	result = wiiremote->device != nil && IOBluetoothDeviceIsConnected(wiiremote->device);
	return result;
}

Boolean wiiremote_search(WiiRemoteRef wiiremote, char *address)
{
	IOReturn	ret;

	if (wiiremote->inquiry != nil)
		return true;

	wiiremote->inquiry = IOBluetoothDeviceInquiryCreateWithCallbackRefCon((void *)wiiremote);
	IOBluetoothDeviceInquirySetDeviceFoundCallback(wiiremote->inquiry, myFoundFunc);
	IOBluetoothDeviceInquirySetDeviceNameUpdatedCallback(wiiremote->inquiry, myUpdatedFunc);
	IOBluetoothDeviceInquirySetCompleteCallback(wiiremote->inquiry, myCompleteFunc);

	if (wiiremote->address != nil)
		CFRelease(wiiremote->address);
	wiiremote->address = CFStringCreateWithCString(nil, address, kCFStringEncodingMacRoman);

	ret = IOBluetoothDeviceInquiryStart(wiiremote->inquiry);
	if (ret != kIOReturnSuccess)
	{
		IOBluetoothDeviceInquiryDelete(wiiremote->inquiry);
		wiiremote->inquiry = nil;
		return false;
	}
	return true;
}

Boolean wiiremote_stopsearch(WiiRemoteRef wiiremote)
{
	IOReturn	ret;

	if (wiiremote->inquiry == nil)
	{
		return true;	// already stopped
	}

	ret = IOBluetoothDeviceInquiryStop(wiiremote->inquiry);

	if (ret != kIOReturnSuccess && ret != kIOReturnNotPermitted)
	{
		// kIOReturnNotPermitted is if it's already stopped
	}

	IOBluetoothDeviceInquiryDelete(wiiremote->inquiry);
	wiiremote->inquiry = nil;

	return (ret==kIOReturnSuccess);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

unsigned char decrypt(unsigned char data)
{
	return (data ^ 0x17) + 0x17;
}

//--------------------------------------------------------------------------------------------

/**
* Handle report 0x21 (Read Data) from wiimote.
 * dp[0] = Bluetooth header
 * dp[1] = (0x21) Report/Channel ID
 * dp[2] = Wiimote Buttons
 * dp[3] = Wiimote Buttons
 * dp[4] = High 4 bits = payload size; Low 4 bits = Error flag (0 = all good)
 * dp[5] = Offset of memory read
 * dp[6] = Offset of memory read
 * dp[7+] = the Data.
 **/

void handleRAMData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	// specify attached expasion device
	if ((dp[5] == 0x00) && (dp[6] == 0xF0))
	{
		if (decrypt(dp[21]) == 0x00)
		{
			wiiremote->expType = WiiNunchuk;
		}
		else
		if (decrypt(dp[21]) == 0x01)
		{
			wiiremote->expType = WiiClassicController;
		}
		else
		{
			wiiremote->expType = WiiExpNotAttached;
		}
		//		initExpPort = NO;
		return;
	}

	// wiimote calibration data
	if (!wiiremote->readingRegister && dp[5] == 0x00 && dp[6] == 0x20)
	{
		wiiremote->wiiCalibData.accX_zero = dp[7];
		wiiremote->wiiCalibData.accY_zero = dp[8];
		wiiremote->wiiCalibData.accZ_zero = dp[9];

		//dp[10] - unknown/unused

		wiiremote->wiiCalibData.accX_1g = dp[11];
		wiiremote->wiiCalibData.accY_1g = dp[12];
		wiiremote->wiiCalibData.accZ_1g = dp[13];
		return;
	}

	// expansion device calibration data.
	if (wiiremote->readingRegister && dp[5] == 0x00 && dp[6] == 0x20)
	{
		if (wiiremote->expType == WiiNunchuk)
		{
			//nunchuk calibration data
			wiiremote->nunchukCalibData.accX_zero =  decrypt(dp[7]);
			wiiremote->nunchukCalibData.accY_zero =  decrypt(dp[8]);
			wiiremote->nunchukCalibData.accZ_zero =  decrypt(dp[9]);

			wiiremote->nunchukCalibData.accX_1g =  decrypt(dp[11]);
			wiiremote->nunchukCalibData.accY_1g =  decrypt(dp[12]);
			wiiremote->nunchukCalibData.accZ_1g =  decrypt(dp[13]);

			wiiremote->nunchukJoyStickCalibData.x_max =  decrypt(dp[15]);
			wiiremote->nunchukJoyStickCalibData.x_min =  decrypt(dp[16]);
			wiiremote->nunchukJoyStickCalibData.x_center =  decrypt(dp[17]);

			wiiremote->nunchukJoyStickCalibData.y_max =  decrypt(dp[18]);
			wiiremote->nunchukJoyStickCalibData.y_min =  decrypt(dp[19]);
			wiiremote->nunchukJoyStickCalibData.y_center =  decrypt(dp[20]);

			return;
		}
		else
		if (wiiremote->expType == WiiClassicController)
		{
			//classic controller calibration data (probably)
		}
	}

	// wii remote buttons
	wiiremote->buttonData = ((short)dp[2] << 8) + dp[3];
}

void handleStatusReport(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	wiiremote->batteryLevel = (double)dp[7];
	wiiremote->batteryLevel /= (double)0xC0; // C0 = fully charged.

	if ((dp[4] & 0x02)) //some device attached to Wiimote
	{
		wiiremote->isExpansionPortAttached = true;
		// initExpPort = YES;

		Boolean ret = writeData(wiiremote, (darr){0x00}, 0x04A40040, 1); // Initialize the device

		if (ret == false)
		{
			wiiremote->isExpansionPortAttached = false;
			return;
		}

		usleep(kWait); // Give the write a chance to be processed.

		ret = readData(wiiremote, 0x04A400F0, 16); // read expansion device type
		if (ret == false)
		{
			wiiremote->isExpansionPortAttached = false;
		}
	}
	else
	{ // unplugged
		wiiremote->isExpansionPortAttached = false;
		wiiremote->expType = WiiExpNotAttached;
	}

	if (dp[4] & 0x10)
		wiiremote->isLED1Illuminated = true;
	else
		wiiremote->isLED1Illuminated = false;

	if (dp[4] & 0x20)
		wiiremote->isLED2Illuminated = true;
	else
		wiiremote->isLED2Illuminated = false;

	if (dp[4] & 0x40)
		wiiremote->isLED3Illuminated = true;
	else
		wiiremote->isLED3Illuminated = false;

	if (dp[4] & 0x80)
		wiiremote->isLED4Illuminated = true;
	else
		wiiremote->isLED4Illuminated = false;
}

void handleExtensionData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	unsigned char startByte;

	switch (dp[1]) {
		case 0x34 :
			startByte = 4;
			break;
		case 0x35 :
			startByte = 7;
			break;
		case 0x36 :
			startByte = 14;
			break;
		case 0x37 :
			startByte = 17;
			break;
		default:
			return; // This shouldn't ever happen.
			break;
	}

	if (wiiremote->expType == WiiNunchuk)
	{
		wiiremote->nStickX = decrypt(dp[startByte]);
		wiiremote->nStickY = decrypt(dp[startByte +1]);
		wiiremote->nAccX   = decrypt(dp[startByte +2]);
		wiiremote->nAccY   = decrypt(dp[startByte +3]);
		wiiremote->nAccZ   = decrypt(dp[startByte +4]);
		wiiremote->nButtonData = decrypt(dp[startByte +5]);

		wiiremote->nLowZ = wiiremote->nLowZ * .9 + wiiremote->nAccZ * .1;
		wiiremote->nLowX = wiiremote->nLowX * .9 + wiiremote->nAccX * .1;

		float absx = abs(wiiremote->nLowX - 128);
		float absz = abs(wiiremote->nLowZ - 128);

		if (wiiremote->nOrientation == 0 || wiiremote->nOrientation == 2) absx -= 5;
		if (wiiremote->nOrientation == 1 || wiiremote->nOrientation == 3) absz -= 5;

		if (absz >= absx)
		{
			if (absz > 5)
				wiiremote->nOrientation = (wiiremote->nLowZ > 128) ? 0 : 2;
		}
		else
		{
			if (absx > 5)
				wiiremote->nOrientation = (wiiremote->nLowX > 128) ? 3 : 1;
		}
	}
	else
	if (wiiremote->expType == WiiClassicController)
	{
		wiiremote->cButtonData = (unsigned short)(decrypt(dp[startByte + 4]) << 8) + decrypt(dp[startByte + 5]);
		wiiremote->cButtonData = ~wiiremote->cButtonData;	// bit reverse

		wiiremote->cStickX1 = decrypt(dp[startByte]) & 0x3F;
		wiiremote->cStickY1 = decrypt(dp[startByte + 1]) & 0x3F;

		wiiremote->cStickX2 = (((decrypt(dp[startByte +0]) & 0xC0) >> 3) |
							   ((decrypt(dp[startByte +1]) & 0xC0) >> 5) |
							   ((decrypt(dp[startByte +2]) & 0x80) >> 7)) & 0x1F;
		wiiremote->cStickY2 = decrypt(dp[startByte + 2]) & 0x1F;

		wiiremote->cAnalogL = (((decrypt(dp[startByte +2]) & 0x60) >> 2) |
							   ((decrypt(dp[startByte +3]) & 0xE0) >> 5)) & 0x1F;
		wiiremote->cAnalogR =  decrypt(dp[startByte + 3]) & 0x1F;
	}
}

void handleIRData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	int i;

	if (dp[1] == 0x33)
	{ // 12 IR bytes
		int startByte = 0;
		for(i=0 ; i < 4 ; i++)
		{
			startByte = 7 + 3 * i;
			wiiremote->irData[i].x = (dp[startByte +0] | ((dp[startByte +2] & 0x30) << 4)) & 0x3FF;
			wiiremote->irData[i].y = (dp[startByte +1] | ((dp[startByte +2] & 0xC0) << 2)) & 0x3FF;
			wiiremote->irData[i].s =  dp[startByte +2] & 0x0F;
		}
 	}
	else
	{ // 10 IR bytes
		int shift = (dp[1] == 0x36) ? 4 : 7;
		int startByte = 0;
		for (i=0; i < 2; i++) {
			startByte = shift + 5 * i;
			wiiremote->irData[2*i].x = (dp[startByte +0] | ((dp[startByte +2] & 0x30) << 4)) & 0x3FF;
			wiiremote->irData[2*i].y = (dp[startByte +1] | ((dp[startByte +2] & 0xC0) << 2)) & 0x3FF;
			wiiremote->irData[2*i].s = ((wiiremote->irData[2*i].x == wiiremote->irData[2*i].y) && (wiiremote->irData[2*i].x == 0x3FF)) ? 0x0F : 0x05;  // No size is given in 10 byte report.

			wiiremote->irData[(2*i)+1].x = (dp[startByte +3] | ((dp[startByte +2] & 0x03) << 8)) & 0x3FF;
			wiiremote->irData[(2*i)+1].y = (dp[startByte +4] | ((dp[startByte +2] & 0x0C) << 6)) & 0x3FF;
			wiiremote->irData[(2*i)+1].s = ((wiiremote->irData[(2*i)+1].x == wiiremote->irData[(2*i)+1].y) && (wiiremote->irData[(2*i)+1].x == 0x3FF)) ? 0x0F : 0x05;  // No size is given in 10 byte report.
		}
	}

	int p1 = -1;
	int p2 = -1;
	// we should modify this loop to take the points with the lowest s (the brightest ones)
	for (i=0 ; i<4 ; i++) {
		if (p1 == -1) {
			if (wiiremote->irData [i].s < 0x0F)
				p1 = i;
		} else {
			if (wiiremote->irData [i].s < 0x0F) {
				p2 = i;
				break;
			}
		}
	}

	double ox, oy;
	if ((p1 > -1) && (p2 > -1))
	{
		int l = wiiremote->leftPoint;
		if (wiiremote->leftPoint == -1)
		{
			switch (wiiremote->orientation)
			{
				case 0: l = (wiiremote->irData[p1].x < wiiremote->irData[p2].x) ? 0 : 1; break;
				case 1: l = (wiiremote->irData[p1].y > wiiremote->irData[p2].y) ? 0 : 1; break;
				case 2: l = (wiiremote->irData[p1].x > wiiremote->irData[p2].x) ? 0 : 1; break;
				case 3: l = (wiiremote->irData[p1].y < wiiremote->irData[p2].y) ? 0 : 1; break;
			}

			wiiremote->leftPoint = l;
		}

		int r = 1-l;

		double dx = wiiremote->irData[r].x - wiiremote->irData[l].x;
		double dy = wiiremote->irData[r].y - wiiremote->irData[l].y;
		double d = hypot (dx, dy);

		dx /= d;
		dy /= d;

		double cx = (wiiremote->irData[l].x + wiiremote->irData[r].x)/kWiiIRPixelsWidth - 1;
		double cy = (wiiremote->irData[l].y + wiiremote->irData[r].y)/kWiiIRPixelsHeight - 1;

		ox = -dy*cy-dx*cx;
		oy = -dx*cy+dy*cx;

		// cam:
		// Compensate for distance. There must be fewer than 0.75*768 pixels between the spots for this to work.
		// In other words, you have to be far enough away from the sensor bar for the two spots to have enough
		// space on the image sensor to travel without one of the points going off the image.
		// note: it is working very well ...
		double gain = 4;
		if (d < (0.75 * kWiiIRPixelsHeight))
			gain = 1 / (1 - d/kWiiIRPixelsHeight);

		ox *= gain;
		oy *= gain;

		wiiremote->angle = atan2(dy, dx);
		wiiremote->tracking = true;
	}
	else
	{
		ox = oy = -100;
		wiiremote->leftPoint = -1;	// not tracking
		wiiremote->angle = -100;
		wiiremote->tracking = false;
	}

	wiiremote->posX = ox;
	wiiremote->posY = oy;
}

void handleButtonReport(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	// wiimote buttons
	wiiremote->buttonData = ((short)dp[2] << 8) + dp[3];

	// report contains extension data
	switch (dp[1])
	{
		case 0x34 :
		case 0x35 :
		case 0x36 :
		case 0x37 :
			handleExtensionData(wiiremote, dp, dataLength);
			break;
	}

	// report contains IR data
	if (dp[1] & 0x02)
	{
		handleIRData(wiiremote, dp, dataLength);
	}

	// report contains motion sensor data
	if (dp[1] & 0x01)
	{
		wiiremote->accX = dp[4];
		wiiremote->accY = dp[5];
		wiiremote->accZ = dp[6];

		wiiremote->lowZ = wiiremote->lowZ * 0.9 + wiiremote->accZ * 0.1;
		wiiremote->lowX = wiiremote->lowX * 0.9 + wiiremote->accX * 0.1;

		float absx = abs(wiiremote->lowX-128);
		float absz = abs(wiiremote->lowZ-128);

		if (wiiremote->orientation == 0 || wiiremote->orientation == 2) absx -= 5;
		if (wiiremote->orientation == 1 || wiiremote->orientation == 3) absz -= 5;

		if (absz >= absx)
		{
			if (absz > 5)
				wiiremote->orientation = (wiiremote->lowZ > 128)?0:2;
		}
		else
		{
			if (absx > 5)
				wiiremote->orientation = (wiiremote->lowX > 128)?3:1;
		}
	}
}

//--------------------------------------------------------------------------------------------

 void myDataListener(IOBluetoothL2CAPChannelRef channel, void *dataPointer, UInt16 dataLength, void *refCon)
{
	WiiRemoteRef	wiiremote = (WiiRemoteRef)refCon;
	unsigned char* dp = (unsigned char*)dataPointer;

	if (!wiiremote->device)
		return;

	//controller status (expansion port and battery level data) - received when report 0x15 sent to Wiimote (getCurrentStatus:) or status of expansion port changes.
	if (dp[1] == 0x20 && dataLength >= 8)
	{
		handleStatusReport(wiiremote, dp, dataLength);
		requestUpdates(wiiremote); // Make sure we keep getting state change reports.
		return;
	}

	if (dp[1] == 0x21)
	{
		handleRAMData(wiiremote, dp, dataLength);
		return;
	}

	if (dp[1] == 0x22)
	{ // Write data response
		//NSLog(@"Write data response: %00x %00x %00x %00x", dp[2], dp[3], dp[4], dp[5]);
		return;
	}

	// report contains button info
	if ((dp[1] & 0xF0) == 0x30)
	{
		handleButtonReport(wiiremote, dp, dataLength);
	}
}

void myEventListener(IOBluetoothL2CAPChannelRef channel, void *refCon, IOBluetoothL2CAPChannelEvent *event)
{
	if (event->eventType == kIOBluetoothL2CAPChannelEventTypeData)
	{
		// In thise case:
		// event->u.newData.dataPtr  is a pointer to the block of data received.
		// event->u.newData.dataSize is the size of the block of data.
		myDataListener(channel, event->u.data.dataPtr, event->u.data.dataSize, refCon);
	}
	else
	if (event->eventType == kIOBluetoothL2CAPChannelEventTypeClosed)
	{
		// In this case:
		// event->u.terminatedChannel is the channel that was terminated. It can be converted in an IOBluetoothL2CAPChannel
		// object with [IOBluetoothL2CAPChannel withL2CAPChannelRef:]. (see below).
	}
}

void myDisconnectedFunc(void * refCon, IOBluetoothUserNotificationRef inRef, IOBluetoothObjectRef objectRef)
{
	CFStringRef	itsAddress, myAddress;

	itsAddress = IOBluetoothDeviceGetAddressString(objectRef);
	if (itsAddress != nil)
	{
		myAddress = IOBluetoothDeviceGetAddressString(((WiiRemoteRef)refCon)->device);
		if (myAddress != nil)
		{
			if (CFStringCompare(itsAddress, myAddress, 0) == kCFCompareEqualTo)
			{
				wiiremote_disconnect((WiiRemoteRef)refCon);
			}
			CFRelease(myAddress);
		}
		CFRelease(itsAddress);
	}
}

//--------------------------------------------------------------------------------------------

void wiiremote_getaddress(WiiRemoteRef wiiremote, char *address)
{
	CFStringRef	cfstring;

	cfstring = IOBluetoothDeviceGetAddressString(wiiremote->device);
	CFStringGetCString(cfstring, address, 32, kCFStringEncodingMacRoman);
	CFRelease(cfstring);

}

//--------------------------------------------------------------------------------------------

Boolean wiiremote_connect(WiiRemoteRef wiiremote)
{
	IOReturn	ret;
	Boolean		result;
	short		i;

	if (wiiremote->device == nil)
		return false;

	// connect the device
	for (i=0; i<kTrial; i++)
	{
		ret = IOBluetoothDeviceOpenConnection(wiiremote->device, nil, nil);
		if ( ret == kIOReturnSuccess)
			break;
		usleep(kWait); //  wait 10ms
	}
	if (i==kTrial)
		return false;

	wiiremote->disconnectNotification = IOBluetoothDeviceRegisterForDisconnectNotification(wiiremote->device, myDisconnectedFunc, (void *)wiiremote);

	// performs an SDP query
	for (i=0; i<kTrial; i++)
	{
		ret = IOBluetoothDevicePerformSDPQuery(wiiremote->device, nil, nil);
		if ( ret == kIOReturnSuccess)
			break;
		usleep(kWait); //  wait 10ms
	}
	if (i==kTrial)
		return false;

	result = openCChan(wiiremote);
	result = openIChan(wiiremote);

	if (result)
	{
		result = wiiremote_led(wiiremote, wiiremote->isLED1Illuminated, wiiremote->isLED2Illuminated, wiiremote->isLED3Illuminated, wiiremote->isLED4Illuminated);
	}

	if (result == false)
	{
		wiiremote_disconnect(wiiremote);
		return result;
	}

	wiiremote_getstatus(wiiremote);
	requestUpdates(wiiremote);

	readData(wiiremote, 0x0020, 7);	//	Get Accelerometer callibration data

	return true;
}


Boolean wiiremote_disconnect(WiiRemoteRef wiiremote)
{
	short	i;

	if (wiiremote->cchan)
	{
		if (IOBluetoothDeviceIsConnected(wiiremote->device))
		{
			for (i=0; i<kTrial; i++)
			{
				if (IOBluetoothL2CAPChannelCloseChannel(wiiremote->cchan) == kIOReturnSuccess)
					break;
				usleep(kWait); //  wait 10ms
			}
		}
		if (i==kTrial)	return false;
		IOBluetoothObjectRelease(wiiremote->cchan);
		wiiremote->cchan = nil;
	}

	if (wiiremote->ichan)
	{
		if (IOBluetoothDeviceIsConnected(wiiremote->device))
		{
			for (i=0; i<kTrial; i++)
			{
				if (IOBluetoothL2CAPChannelCloseChannel(wiiremote->ichan) == kIOReturnSuccess)
					break;
			}
		}
		if (i==kTrial)	return false;
		IOBluetoothObjectRelease(wiiremote->ichan);
		wiiremote->ichan = nil;
	}

	if (wiiremote->device)
	{
		if (IOBluetoothDeviceIsConnected(wiiremote->device))
		{
			for (i=0; i<kTrial; i++)
			{
				if (IOBluetoothDeviceCloseConnection(wiiremote->device) == kIOReturnSuccess)
					break;
			}
		}
		if (i==kTrial)	return false;
		IOBluetoothObjectRelease(wiiremote->device);
		wiiremote->device = nil;
	}

	if (wiiremote->disconnectNotification != nil)
	{
		IOBluetoothUserNotificationUnregister(wiiremote->disconnectNotification);
		wiiremote->disconnectNotification = nil;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

Boolean	requestUpdates(WiiRemoteRef wiiremote)
{
	Boolean	result;

	// Set the report type the Wiimote should send.
	unsigned char cmd[] = {0x12, 0x02, 0x30}; // Just buttons.

	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;

	/*
	 There are numerous status report types that can be requested.
	 The IR reports must be matched with the data format set when initializing the IR camera:
	 0x36, 0x37	- 10 IR bytes go with Basic mode
	 0x33		- 12 IR bytes go with Extended mode
	 0x3e/0x3f	- 36 IR bytes go with Full mode

	 The Nunchuk and Classic controller use 6 bytes to report their state, so the reports that
	 give more extension bytes don't provide any more info.

	 Buttons		|	Accelerometer	|	IR		|	Extension
	 --------------------+-------------------+-----------+-------------
	 0x30: Core Buttons	|					|			|
	 0x31: Core Buttons	|	Accelerometer	|			|
	 0x32: Core Buttons	|					|			|	 8 bytes
	 0x33: Core Buttons	|	Accelerometer	| 12 bytes	|
	 0x34: Core Buttons	|					|			|	19 bytes
	 0x35: Core Buttons	|	Accelerometer	|			|	16 bytes
	 0x36: Core Buttons	|					| 10 bytes	|	 9 bytes
	 0x37: Core Buttons	|	Accelerometer	| 10 bytes	|	 6 bytes
	 ?? 0x38: Core Buttons and Accelerometer with 16 IR bytes ??
	 0x3d:				|					|			|	21 bytes

	 0x3e / 0x3f: Interleaved Core Buttons and Accelerometer with 16/36 IR bytes

	 */

	if (wiiremote->isIRSensorEnabled)
	{
		if (wiiremote->isExpansionPortEnabled)
		{
			cmd[2] = 0x36;	// Buttons, 10 IR Bytes, 9 Extension Bytes
			wiiremote->wiiIRMode = kWiiIRModeBasic;
		}
		else
		{
			cmd[2] = 0x33; // Buttons, Accelerometer, and 12 IR Bytes.
			wiiremote->wiiIRMode = kWiiIRModeExtended;
		}

		// Set IR Mode
		writeData(wiiremote, (darr){ wiiremote->wiiIRMode }, 0x04B00033, 1);
		usleep(kWait); //  wait 10ms
	}
	else
	{
		if (wiiremote->isExpansionPortEnabled)
		{
			cmd[2] = 0x34;	// Buttons, 19 Extension Bytes
		}
		else
		{
			cmd[2] = 0x30; // Buttons
		}
	}

	if (wiiremote->isMotionSensorEnabled)	cmd[2] |= 0x01;	// Add Accelerometer

	usleep(kWait); //  wait 10ms
	result = sendCommand(wiiremote, cmd, 3);

	return(result);
}

//--------------------------------------------------------------------------------------------

Boolean wiiremote_motionsensor(WiiRemoteRef wiiremote, Boolean enabled)
{
	wiiremote->isMotionSensorEnabled = enabled;
	return requestUpdates(wiiremote);
}

Boolean wiiremote_vibration(WiiRemoteRef wiiremote, Boolean enabled)
{

	wiiremote->isVibrationEnabled = enabled;
	return requestUpdates(wiiremote);
}

Boolean wiiremote_led(WiiRemoteRef wiiremote, Boolean enabled1, Boolean enabled2, Boolean enabled3, Boolean enabled4)
{
	unsigned char cmd[] = {0x11, 0x00};
	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;
	if (enabled1)	cmd[1] |= 0x10;
	if (enabled2)	cmd[1] |= 0x20;
	if (enabled3)	cmd[1] |= 0x40;
	if (enabled4)	cmd[1] |= 0x80;

	wiiremote->isLED1Illuminated = enabled1;
	wiiremote->isLED2Illuminated = enabled2;
	wiiremote->isLED3Illuminated = enabled3;
	wiiremote->isLED4Illuminated = enabled4;

	return sendCommand(wiiremote, cmd, 2);
}

Boolean wiiremote_expansion(WiiRemoteRef wiiremote, Boolean enabled)
{
	wiiremote->isExpansionPortEnabled = enabled;
	if (wiiremote->isExpansionPortAttached == false)
	{
		wiiremote->isExpansionPortEnabled = false;
	}
	else
	{
		readData(wiiremote, 0x04A40020, 16);	//get calbdata
	}

	return requestUpdates(wiiremote);
}

Boolean wiiremote_irsensor(WiiRemoteRef wiiremote, Boolean enabled)
{
	Boolean	ret;

	wiiremote->isIRSensorEnabled = enabled;

	// ir enable 1
	unsigned char cmd[] = {0x13, 0x00};
	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;
	if (wiiremote->isIRSensorEnabled)	cmd[1] |= 0x04;
	if ((ret = sendCommand(wiiremote, cmd, 2)) == false)
		return ret;
	usleep(kWait);

	// set register 0x1a (ir enable 2)
	unsigned char cmd2[] = {0x1a, 0x00};
	if (enabled)	cmd2[1] |= 0x04;
	if ((ret = sendCommand(wiiremote, cmd2, 2)) == false)
		return ret;
	usleep(kWait);

	if(enabled)
	{
		// based on marcan's method, found on wiili wiki:
		// tweaked to include some aspects of cliff's setup procedure in the hopes
		// of it actually turning on 100% of the time (was seeing 30-40% failure rate before)
		// the sleeps help it it seems
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x01}, 0x04B00030, 1)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xC0}, 0x04B00000, 9)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x40, 0x00}, 0x04B0001A, 2)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x08}, 0x04B00030, 1)) == false)	return ret;
		usleep(kWait);

		requestUpdates(wiiremote);
	}
	else
	{
		// probably should do some writes to power down the camera, save battery
		// but don't know how yet.

		ret = wiiremote_motionsensor(wiiremote, wiiremote->isMotionSensorEnabled);
		ret = wiiremote_vibration(wiiremote, wiiremote->isVibrationEnabled);
		ret = wiiremote_expansion(wiiremote, wiiremote->isExpansionPortEnabled);
	}

	return ret;
}

Boolean wiiremote_getstatus(WiiRemoteRef wiiremote)
{
	unsigned char cmd[] = {0x15, 0x00};
	return sendCommand(wiiremote, cmd, 2);
}


