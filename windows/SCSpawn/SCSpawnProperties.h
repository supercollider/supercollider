#if !defined(_MYDATA_H)
#define _MYDATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyData.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "PropertyListCtrl.h"

const int ID_SCSPAWNPROP_CONNECTION_TYPE = 1; //
const int ID_SCSPAWNPROP_PORT_NUM = 2; //
const int ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS = 3;
const int ID_SCSPAWNPROP_TCP_PASSWORD = 4; //
const int ID_SCSPAWNPROP_NB_CTRL_BUS_CHANNELS = 5; //
const int ID_SCSPAWNPROP_NB_AUDIO_BUS_CHANNELS = 6; //
const int ID_SCSPAWNPROP_NB_INPUT_BUS_CHANNELS = 7; //
const int ID_SCSPAWNPROP_NB_OUTPUT_BUS_CHANNELS = 8; //
const int ID_SCSPAWNPROP_BLOCK_SIZE = 9; //
const int ID_SCSPAWNPROP_HARDWARE_BUFFER_SIZE = 10; //
const int ID_SCSPAWNPROP_HARDWARE_SAMPLE_RATE = 11; // 
const int ID_SCSPAWNPROP_NB_SAMPLE_BUFFERS = 12; //
const int ID_SCSPAWNPROP_MAX_NB_NODES = 13; //
const int ID_SCSPAWNPROP_MAX_NB_SYNTHDEFS = 14; //
const int ID_SCSPAWNPROP_RT_MEM_SIZE = 15; //
const int ID_SCSPAWNPROP_NB_WIRE_BUFFERS = 16;  //
const int ID_SCSPAWNPROP_NB_RND_SEEDS = 17; //
const int ID_SCSPAWNPROP_LOAD_SYNTHDEFS = 18; //
const int ID_SCSPAWNPROP_NB_MAX_LOGINS = 19; //
const int ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE = 20; //
const int ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS = 21; //
const int ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE = 22; //

class SCSpawn_PropertyItemManager;
class CMyAdaptablePropertyItemManager;

// Only as an example
/////////////////////////////////////////////////////////////////////////////
// CMyData
/*
		GET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PORT_NUM, pMyData->_tcpPortNum)
		GET_ITEM_STRING(ID_SCSPAWNPROP_UDP_PORT_NUM, pMyData->_udpPortNum)
		GET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PASSWORD, pMyData->_tcpPassword)
    GET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE, pMyData->_audioInputDev)
		GET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS, pMyData->_audioNbInChans)
    GET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE, pMyData->_audioOutputDev)
		GET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS, pMyData->_audioNbOutChans)
*/
class CMyData : public CObject
{
	CMyData(const CMyData& d);
	CMyData& operator=(const CMyData& d);

public:
	CMyData();
	~CMyData();

	// Data
private:
  int ConnectionType;
  CString PortNumber;
  CString TCPPassword ;
  CString MaxNumLogins;
  CString NumControlBusChannels;
  CString NumAudioBusChannels;
  CString NumInputBusChannels;
  CString NumOutputBusChannels;
  CString BlockSize;
  CString HardwareBufferSize;
  CString HardwareSampleRate;
  CString NumSampleBuffers;
  CString MaxNumNodes;
  CString MaxNumSynthDefs;
  CString RTMemSize;
  CString NumWireBuffers;
  CString NumRandomSeeds;
  int     BoolLoadSynthdefs;
  
  // PortAudio stuff...
  CString AudioInputDevice;  // not used by GUI
  int AudioInputDeviceIndex;
  CString AudioInputDeviceNumChannels;

  CString AudioOutputDevice; // not used by GUI
  int AudioOutputDeviceIndex; 
  CString AudioOutputDeviceNumChannels;

	friend SCSpawn_PropertyItemManager;
  friend class CPropertyListCtrlDlg;
};

inline CMyData::~CMyData()
{
}


/////////////////////////////////////////////////////////////////////////////
// SCSpawn_PropertyItemManager

class SCSpawn_PropertyItemManager : public CPropertyItemManager
{
	SCSpawn_PropertyItemManager(const SCSpawn_PropertyItemManager& d);
	SCSpawn_PropertyItemManager& operator=(const SCSpawn_PropertyItemManager& d);

public:
	SCSpawn_PropertyItemManager();
	~SCSpawn_PropertyItemManager();

	bool SetData(const CObject* pData);
	bool GetData(CObject* pData) const;
};

#endif // !defined(AFX_PROPERTYLISTCTRLDLG_H__466FB4E7_6072_11D3_A7E3_006008C8B630__INCLUDED_)