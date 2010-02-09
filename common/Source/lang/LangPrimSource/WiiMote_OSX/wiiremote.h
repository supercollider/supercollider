// wiiremote.h
// Copyright by Masayuki Akamatsu
// Based on "DarwiinRemote" by Hiroaki Kimura

#define BLUETOOTH_VERSION_USE_CURRENT
//#include <AvailabilityMacros.h>
#include <math.h>
#include <CoreFoundation/CoreFoundation.h>
//#include <CoreFoundation/CFMachPort.h>
#include <IOBluetooth/IOBluetoothUserLib.h>

typedef unsigned char WiiIRModeType;
enum {
	kWiiIRModeBasic			= 0x01,
	kWiiIRModeExtended		= 0x03,
	kWiiIRModeFull			= 0x05
};

typedef struct {
	int x, y, s;
} IRData;

typedef struct {
	unsigned char accX_zero, accY_zero, accZ_zero, accX_1g, accY_1g, accZ_1g;
} WiiAccCalibData;

typedef struct {
	unsigned char x_min, x_max, x_center, y_min, y_max, y_center;
} WiiJoyStickCalibData;

typedef UInt16 WiiButtonType;
enum {
	WiiRemoteAButton,
	WiiRemoteBButton,
	WiiRemoteOneButton,
	WiiRemoteTwoButton,
	WiiRemoteMinusButton,
	WiiRemoteHomeButton,
	WiiRemotePlusButton,
	WiiRemoteUpButton,
	WiiRemoteDownButton,
	WiiRemoteLeftButton,
	WiiRemoteRightButton,

	WiiNunchukZButton,
	WiiNunchukCButton,

	WiiClassicControllerXButton,
	WiiClassicControllerYButton,
	WiiClassicControllerAButton,
	WiiClassicControllerBButton,
	WiiClassicControllerLButton,
	WiiClassicControllerRButton,
	WiiClassicControllerZLButton,
	WiiClassicControllerZRButton,
	WiiClassicControllerUpButton,
	WiiClassicControllerDownButton,
	WiiClassicControllerLeftButton,
	WiiClassicControllerRightButton,
	WiiClassicControllerMinusButton,
	WiiClassicControllerHomeButton,
	WiiClassicControllerPlusButton
};


typedef UInt16 WiiExpansionPortType;
enum{
	WiiExpNotAttached,
	WiiNunchuk,
	WiiClassicController
};

typedef UInt16 WiiAccelerationSensorType;
enum{
	WiiRemoteAccelerationSensor,
	WiiNunchukAccelerationSensor
};


typedef UInt16 WiiJoyStickType;
enum{
	WiiNunchukJoyStick,
	WiiClassicControllerLeftJoyStick,
	WiiClassicControllerRightJoyStick
};


typedef struct _WiiRemoteRec
{
	IOBluetoothDeviceInquiryRef	inquiry;
	IOBluetoothDeviceRef		device;
	IOBluetoothL2CAPChannelRef	ichan;
	IOBluetoothL2CAPChannelRef	cchan;

	CFStringRef		address;

	unsigned char	accX;
	unsigned char	accY;
	unsigned char	accZ;
	unsigned short	buttonData;

	float			lowZ;
	float			lowX;
	int				orientation;
	int				leftPoint; // is point 0 or 1 on the left. -1 when not tracking.

	float			posX;
	float			posY;
	float			angle;
	Boolean			tracking;

	WiiExpansionPortType expType;
	WiiAccCalibData	wiiCalibData, nunchukCalibData;
	WiiJoyStickCalibData nunchukJoyStickCalibData;
	WiiIRModeType	wiiIRMode;
	IRData			irData[4];
	double			batteryLevel;

	Boolean			readingRegister;
	Boolean			isMotionSensorEnabled;
	Boolean			isIRSensorEnabled;
	Boolean			isVibrationEnabled;
	Boolean			isExpansionPortEnabled;
	Boolean			initExpPort;
	Boolean			isLED1Illuminated;
	Boolean			isLED2Illuminated;
	Boolean			isLED3Illuminated;
	Boolean			isLED4Illuminated;

	Boolean			isExpansionPortAttached;

	IOBluetoothUserNotificationRef	disconnectNotification;

	//nunchuk
	unsigned char	nStickX;
	unsigned char	nStickY;
	unsigned char	nAccX;
	unsigned char	nAccY;
	unsigned char	nAccZ;
	unsigned char	nButtonData;

	float			nLowZ;
	float			nLowX;
	int				nOrientation;

	//classic controller
	unsigned short	cButtonData;
	unsigned char	cStickX1;
	unsigned char	cStickY1;
	unsigned char	cStickX2;
	unsigned char	cStickY2;
	unsigned char	cAnalogL;
	unsigned char	cAnalogR;

}	WiiRemoteRec, *WiiRemoteRef;

void			wiiremote_init(WiiRemoteRef wiiremote);
Boolean			wiiremote_isconnected(WiiRemoteRef wiiremote);
Boolean			wiiremote_search(WiiRemoteRef wiiremote, char *address);
Boolean			wiiremote_stopsearch(WiiRemoteRef wiiremote);
Boolean			wiiremote_connect(WiiRemoteRef wiiremote);
Boolean			wiiremote_disconnect(WiiRemoteRef wiiremote);
void			wiiremote_getaddress(WiiRemoteRef wiiremote, char *address);
Boolean			wiiremote_motionsensor(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_irsensor(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_vibration(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_led(WiiRemoteRef wiiremote, Boolean enabled1, Boolean enabled2, Boolean enabled3, Boolean enabled4);
Boolean			wiiremote_expansion(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_getstatus(WiiRemoteRef wiiremote);

