/////////////////////////////////////////////////////////////////////////////
//
// Parts Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SCSpawnProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemListYesNo

/*BEGIN_LIST_ITEM_DATA_TYPE(CPropertyItemListYesNo)
	LPCTSTR_STRING_ITEM_DATA(_T("No")),
	LPCTSTR_STRING_ITEM_DATA(_T("Yes"))
END_LIST_ITEM_DATA_TYPE(CPropertyItemListYesNo)
*/

// let's expand those macros to understand what's going on...
class CPropertyItemListYesNo : public CPropertyItemList 
{ 
	CPropertyItemListYesNo(const CPropertyItemListYesNo& d); 
	CPropertyItemListYesNo& operator=(const CPropertyItemListYesNo& d); 
public: 
	CPropertyItemListYesNo(LPCTSTR pStrName = NULL) : CPropertyItemList(pStrName) {}; 
	virtual ~CPropertyItemListYesNo() {}; 
	virtual LPCTSTR GetItemData(int nItem) const; 
private: 
	static CSmartString m_data[]; 
}; 

CSmartString CPropertyItemListYesNo::m_data[] = {
	CSmartString(LPCTSTR(_T("No"))),
  CSmartString(LPCTSTR(_T("Yes")))
};

LPCTSTR CPropertyItemListYesNo::GetItemData(int nItem) const
{
	return nItem < sizeof(CPropertyItemListYesNo::m_data)/sizeof(CSmartString) ? LPCTSTR(m_data[nItem]) : NULL;
}

class CPropertyItemListStringArray : public CPropertyItemList 
{ 
	CPropertyItemListStringArray(const CPropertyItemListStringArray& d); 
	CPropertyItemListStringArray& operator=(const CPropertyItemListStringArray& d); 
public: 
	CPropertyItemListStringArray(LPCTSTR pStrName, CStringArray& stringArray);
	virtual ~CPropertyItemListStringArray() {}; 
	virtual LPCTSTR GetItemData(int nItem) const; 
private: 
	CStringArray _stringArray; 
}; 

inline CPropertyItemListStringArray::CPropertyItemListStringArray(LPCTSTR pStrName, CStringArray& stringArray) : 
CPropertyItemList(pStrName)
{
  for(int i = 0; i < stringArray.GetCount(); ++i) {
    _stringArray.Add(stringArray.GetAt(i));
  }
}; 


LPCTSTR CPropertyItemListStringArray::GetItemData(int nItem) const
{
  if(nItem < 0)
    return NULL;
  if(nItem >= _stringArray.GetCount( ))
    return NULL;
  return (LPCTSTR)(_stringArray.GetAt(nItem));
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemListDataType

/*
BEGIN_LIST_ITEM_DATA_TYPE(CPropertyItemListDataType)
	LPCTSTR_STRING_ITEM_DATA(_T("Only data")),
	LPCTSTR_STRING_ITEM_DATA(_T("Input line")),
	LPCTSTR_STRING_ITEM_DATA(_T("Data item")),
	LPCTSTR_STRING_ITEM_DATA(_T("List")),
	LPCTSTR_STRING_ITEM_DATA(_T("Form")),
	LPCTSTR_STRING_ITEM_DATA(_T("Macro"))
END_LIST_ITEM_DATA_TYPE(CPropertyItemListDataType)

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemListDataFormatType

BEGIN_LIST_ITEM_DATA_TYPE(CPropertyItemListDataFormatType)
	LPCTSTR_STRING_ITEM_DATA(_T("Number")),
	LPCTSTR_STRING_ITEM_DATA(_T("Currency")),
	LPCTSTR_STRING_ITEM_DATA(_T("Time")),
	LPCTSTR_STRING_ITEM_DATA(_T("Date")),
	LPCTSTR_STRING_ITEM_DATA(_T("Other type"))
END_LIST_ITEM_DATA_TYPE(CPropertyItemListDataFormatType)
*/
/////////////////////////////////////////////////////////////////////////////
// CMyData

CMyData::CMyData()
{
}

/////////////////////////////////////////////////////////////////////////////
// SCSpawn_PropertyItemManager

/*
const int ID_SCSPAWNPROP_CONNECTION_TYPE = 1;
const int ID_SCSPAWNPROP_TCP_PORT_NUM = 2;
const int ID_SCSPAWNPROP_UDP_PORT_NUM = 3;
const int ID_SCSPAWNPROP_TCP_PASSWORD = 4;
const int ID_SCSPAWNPROP_NB_CTRL_BUS_CHANNELS = 5;
const int ID_SCSPAWNPROP_NB_AUDIO_BUS_CHANNELS = 6;
const int ID_SCSPAWNPROP_NB_INPUT_BUS_CHANNELS = 7;
const int ID_SCSPAWNPROP_NB_OUTPUT_BUS_CHANNELS = 8;
const int ID_SCSPAWNPROP_BLOCK_SIZE = 9;
const int ID_SCSPAWNPROP_HARDWARE_BUFFER_SIZE = 10;
const int ID_SCSPAWNPROP_HARDWARE_SAMPLE_RATE = 11;
const int ID_SCSPAWNPROP_NB_SAMPLE_BUFFERS = 12;
const int ID_SCSPAWNPROP_MAX_NB_NODES = 13;
const int ID_SCSPAWNPROP_MAX_NB_SYNTHDEFS = 14; 
const int ID_SCSPAWNPROP_RT_MEM_SIZE = 15;
const int ID_SCSPAWNPROP_NB_WIRE_BUFFERS = 16; 
const int ID_SCSPAWNPROP_NB_RND_SEEDS = 17;
const int ID_SCSPAWNPROP_LOAD_SYNTHDEFS = 18;
const int ID_SCSPAWNPROP_NB_MAX_LOGINS = 19;
const int ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE = 20;
const int ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS = 21;
const int ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE = 22;
const int ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS = 23;
*/

extern CStringArray g_portAudioInDevList;
extern CStringArray g_portAudioOutDevList;

SCSpawn_PropertyItemManager::SCSpawn_PropertyItemManager()
{
  CStringArray connectionTypes;
  connectionTypes.Add("TCP");
  connectionTypes.Add("UDP");

	BEGIN_PROPERTY_TAB(_T("Audio Devices"), true)
		PROPERTY_ITEM_WITH_CTOR_PARAM(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE, CPropertyItemListStringArray, g_portAudioInDevList, _T("Input Device"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS, CPropertyItemString, _T("Nb Input Channels"), true)
		PROPERTY_ITEM_WITH_CTOR_PARAM(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE, CPropertyItemListStringArray, g_portAudioOutDevList, _T("Output Device"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS, CPropertyItemString, _T("Nb Output Channels"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_HARDWARE_BUFFER_SIZE , CPropertyItemString, _T("Hardware Buffer Size"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_HARDWARE_SAMPLE_RATE , CPropertyItemString, _T("Hardware Sample Rate"), true)
	END_PROPERTY_TAB()

  BEGIN_PROPERTY_TAB(_T("Network"), true)
		PROPERTY_ITEM_WITH_CTOR_PARAM(ID_SCSPAWNPROP_CONNECTION_TYPE, CPropertyItemListStringArray, connectionTypes, _T("Connection Type"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_PORT_NUM, CPropertyItemString, _T("Port Number"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_TCP_PASSWORD, CPropertyItemString, _T("TCP Session Password"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_NB_MAX_LOGINS, CPropertyItemString, _T("Max Num Connections"), true)
  END_PROPERTY_TAB()

	BEGIN_PROPERTY_TAB(_T("Busses"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_NB_CTRL_BUS_CHANNELS, CPropertyItemString, _T("Max Control Bus Channels"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_NB_AUDIO_BUS_CHANNELS, CPropertyItemString, _T("Max Audio Bus Channels"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_NB_INPUT_BUS_CHANNELS, CPropertyItemString, _T("Max Input Bus Channels"), true)
		PROPERTY_ITEM(ID_SCSPAWNPROP_NB_OUTPUT_BUS_CHANNELS, CPropertyItemString, _T("Max Output Bus Channels"), true)
	END_PROPERTY_TAB()

	BEGIN_PROPERTY_TAB(_T("Other Params"), true)
	  PROPERTY_ITEM(ID_SCSPAWNPROP_BLOCK_SIZE , CPropertyItemString, _T("Block Size"), true)
	  PROPERTY_ITEM(ID_SCSPAWNPROP_NB_SAMPLE_BUFFERS, CPropertyItemString, _T("Nb Sample Buffers"), true)
	  PROPERTY_ITEM(ID_SCSPAWNPROP_MAX_NB_NODES , CPropertyItemString, _T("Max Nb Nodes"), true)
    PROPERTY_ITEM(ID_SCSPAWNPROP_MAX_NB_SYNTHDEFS , CPropertyItemString, _T("Max Nb Synthdefs"), true)
    PROPERTY_ITEM(ID_SCSPAWNPROP_LOAD_SYNTHDEFS , CPropertyItemListYesNo, _T("Load Synthdefs from Disk"), true)
    PROPERTY_ITEM(ID_SCSPAWNPROP_RT_MEM_SIZE , CPropertyItemString, _T("Real-Time Memory Size"), true)
    PROPERTY_ITEM(ID_SCSPAWNPROP_NB_WIRE_BUFFERS , CPropertyItemString, _T("Nb Wire Buffers"), true)
    PROPERTY_ITEM(ID_SCSPAWNPROP_NB_RND_SEEDS , CPropertyItemString, _T("Nb Random Seeds"), true)
	END_PROPERTY_TAB()
}

SCSpawn_PropertyItemManager::~SCSpawn_PropertyItemManager()
{
}

bool SCSpawn_PropertyItemManager::SetData(const CObject* pData)
{
	const CMyData* pMyData = static_cast<const CMyData*>(pData);

	BEGIN_ITERATE_PROPERTY_ITEMS()
/*
    SET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PORT_NUM, pMyData->PortNumber)
		SET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PASSWORD, pMyData->TCPPassword)
    SET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE, pMyData->_audioInputDev)
		SET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS, pMyData->_audioNbInChans)
    SET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE, pMyData->_audioOutputDev)
		SET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS, pMyData->_audioNbOutChans)
 */
		SET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE, pMyData->AudioInputDeviceIndex)
		SET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS, pMyData->AudioInputDeviceNumChannels)
		SET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE, pMyData->AudioOutputDeviceIndex)
		SET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS, pMyData->AudioOutputDeviceNumChannels)
		SET_ITEM_STRING(ID_SCSPAWNPROP_HARDWARE_BUFFER_SIZE , pMyData->HardwareBufferSize)
		SET_ITEM_STRING(ID_SCSPAWNPROP_HARDWARE_SAMPLE_RATE , pMyData->HardwareSampleRate)
		SET_ITEM_LIST(ID_SCSPAWNPROP_CONNECTION_TYPE, pMyData->ConnectionType)
		SET_ITEM_STRING(ID_SCSPAWNPROP_PORT_NUM, pMyData->PortNumber)
		SET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PASSWORD, pMyData->TCPPassword)
		SET_ITEM_STRING(ID_SCSPAWNPROP_NB_MAX_LOGINS, pMyData->MaxNumLogins)
		SET_ITEM_STRING(ID_SCSPAWNPROP_NB_CTRL_BUS_CHANNELS, pMyData->NumControlBusChannels)
		SET_ITEM_STRING(ID_SCSPAWNPROP_NB_AUDIO_BUS_CHANNELS, pMyData->NumAudioBusChannels)
		SET_ITEM_STRING(ID_SCSPAWNPROP_NB_INPUT_BUS_CHANNELS, pMyData->NumInputBusChannels)
		SET_ITEM_STRING(ID_SCSPAWNPROP_NB_OUTPUT_BUS_CHANNELS, pMyData->NumOutputBusChannels)
	  SET_ITEM_STRING(ID_SCSPAWNPROP_BLOCK_SIZE , pMyData->BlockSize)
	  SET_ITEM_STRING(ID_SCSPAWNPROP_NB_SAMPLE_BUFFERS, pMyData->NumSampleBuffers)
	  SET_ITEM_STRING(ID_SCSPAWNPROP_MAX_NB_NODES , pMyData->MaxNumNodes)
    SET_ITEM_STRING(ID_SCSPAWNPROP_MAX_NB_SYNTHDEFS , pMyData->MaxNumSynthDefs)
    SET_ITEM_LIST(ID_SCSPAWNPROP_LOAD_SYNTHDEFS , pMyData->BoolLoadSynthdefs)
    SET_ITEM_STRING(ID_SCSPAWNPROP_RT_MEM_SIZE , pMyData->RTMemSize)
    SET_ITEM_STRING(ID_SCSPAWNPROP_NB_WIRE_BUFFERS , pMyData->NumWireBuffers)
    SET_ITEM_STRING(ID_SCSPAWNPROP_NB_RND_SEEDS , pMyData->NumRandomSeeds)
	END_ITERATE_PROPERTY_ITEMS()

	return true;
}

bool SCSpawn_PropertyItemManager::GetData(CObject* pData) const
{
	CMyData* pMyData = static_cast<CMyData*>(pData);

	BEGIN_ITERATE_PROPERTY_ITEMS()
		GET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE, pMyData->AudioInputDeviceIndex)
		GET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_INPUT_DEVICE_NB_CHANNELS, pMyData->AudioInputDeviceNumChannels)
		GET_ITEM_LIST(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE, pMyData->AudioOutputDeviceIndex)
		GET_ITEM_STRING(ID_SCSPAWNPROP_AUDIO_OUTPUT_DEVICE_NB_CHANNELS, pMyData->AudioOutputDeviceNumChannels)
		GET_ITEM_STRING(ID_SCSPAWNPROP_HARDWARE_BUFFER_SIZE , pMyData->HardwareBufferSize)
		GET_ITEM_STRING(ID_SCSPAWNPROP_HARDWARE_SAMPLE_RATE , pMyData->HardwareSampleRate)
		GET_ITEM_LIST(ID_SCSPAWNPROP_CONNECTION_TYPE, pMyData->ConnectionType)
		GET_ITEM_STRING(ID_SCSPAWNPROP_PORT_NUM, pMyData->PortNumber)
		GET_ITEM_STRING(ID_SCSPAWNPROP_TCP_PASSWORD, pMyData->TCPPassword)
		GET_ITEM_STRING(ID_SCSPAWNPROP_NB_MAX_LOGINS, pMyData->MaxNumLogins)
		GET_ITEM_STRING(ID_SCSPAWNPROP_NB_CTRL_BUS_CHANNELS, pMyData->NumControlBusChannels)
		GET_ITEM_STRING(ID_SCSPAWNPROP_NB_AUDIO_BUS_CHANNELS, pMyData->NumAudioBusChannels)
		GET_ITEM_STRING(ID_SCSPAWNPROP_NB_INPUT_BUS_CHANNELS, pMyData->NumInputBusChannels)
		GET_ITEM_STRING(ID_SCSPAWNPROP_NB_OUTPUT_BUS_CHANNELS, pMyData->NumOutputBusChannels)
	  GET_ITEM_STRING(ID_SCSPAWNPROP_BLOCK_SIZE , pMyData->BlockSize)
	  GET_ITEM_STRING(ID_SCSPAWNPROP_NB_SAMPLE_BUFFERS, pMyData->NumSampleBuffers)
	  GET_ITEM_STRING(ID_SCSPAWNPROP_MAX_NB_NODES , pMyData->MaxNumNodes)
    GET_ITEM_STRING(ID_SCSPAWNPROP_MAX_NB_SYNTHDEFS , pMyData->MaxNumSynthDefs)
    SET_ITEM_LIST(ID_SCSPAWNPROP_LOAD_SYNTHDEFS , pMyData->BoolLoadSynthdefs)
    GET_ITEM_STRING(ID_SCSPAWNPROP_RT_MEM_SIZE , pMyData->RTMemSize)
    GET_ITEM_STRING(ID_SCSPAWNPROP_NB_WIRE_BUFFERS , pMyData->NumWireBuffers)
    GET_ITEM_STRING(ID_SCSPAWNPROP_NB_RND_SEEDS , pMyData->NumRandomSeeds)
	END_ITERATE_PROPERTY_ITEMS()

	return true;
}
