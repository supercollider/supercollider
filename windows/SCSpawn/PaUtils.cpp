#include "stdafx.h"
#include "portaudio.h"

CStringArray g_portAudioInDevList; // NOT static ! GUI uses them
CStringArray g_portAudioOutDevList;
static bool g_PaDevListCreated = false;

void PaUtils_CreatePaDevList( )
{
  g_PaDevListCreated = true;  
  PaError paerr;
  paerr = Pa_Initialize( );
  if (paerr != paNoError)
    throw "Error when initializing PortAudio.";
  PaDeviceIndex   i;
  PaDeviceIndex   numDevices;
  const PaDeviceInfo *pdi;
  PaDeviceIndex result = paNoDevice;
  numDevices = Pa_GetDeviceCount();
  for( i=0; i<numDevices; i++ ) {
    pdi = Pa_GetDeviceInfo( i );
    CString devInfo;
    const PaHostApiInfo *apiInfo = Pa_GetHostApiInfo(pdi->hostApi);
    int maxIns = pdi->maxInputChannels;
    int maxOuts = pdi->maxOutputChannels;
    if (maxIns == 0 && maxOuts == 0) {
      devInfo.Format("%s : %s (%d) (%d ins %d outs)",apiInfo->name,pdi->name, i, maxIns, maxOuts);
      CString msg;
      msg.Format("Warning : the (%s) device has 0 inputs and 0 outputs !",devInfo);
      ::MessageBox(NULL,msg,"SCSpawn Warning",MB_ICONERROR | MB_OK);
      g_portAudioInDevList.Add(devInfo);
      g_portAudioOutDevList.Add(devInfo);
    }
    else if (maxIns == 0 && maxOuts > 0) {
      devInfo.Format("%s : %s (%d) (%d ins %d outs)",apiInfo->name,pdi->name, i, maxIns, maxOuts);
      g_portAudioInDevList.Add(devInfo);
      g_portAudioOutDevList.Add(devInfo);
    }
    else if (maxIns > 0 && maxOuts == 0) {
      devInfo.Format("%s : %s (%d) (%d ins %d outs)",apiInfo->name,pdi->name, i, maxIns, maxOuts);
      g_portAudioInDevList.Add(devInfo);
      g_portAudioOutDevList.Add(devInfo);
    }
    else if (maxIns > 0 && maxOuts > 0) {
      devInfo.Format("%s : %s (%d) (%d ins %d outs)",apiInfo->name,pdi->name, i, maxIns, maxOuts);
      g_portAudioInDevList.Add(devInfo);
      g_portAudioOutDevList.Add(devInfo);
    }
  }
}

CString PaUtils_GetPaInDevNameFromIndex( int index )
{
  assert(g_PaDevListCreated);
  if (index < 0 || index >= g_portAudioInDevList.GetCount( ))
    return "<unknown input device>";
  else
    return g_portAudioInDevList.GetAt(index);
}

CString PaUtils_GetPaOutDevNameFromIndex( int index )
{
  assert(g_PaDevListCreated);
  if (index < 0 || index >= g_portAudioOutDevList.GetCount( ))
    return "<unknown output device>";
  else
    return g_portAudioOutDevList.GetAt(index);
}

int PaUtils_GetIndexFromPaInDevName(const CString& inDevName)
{
  assert(g_PaDevListCreated);
  for(int i = 0; i < g_portAudioInDevList.GetCount( ); ++i) {
    if (inDevName == g_portAudioInDevList.GetAt(i))
      return i;
  }
  return -1;
}

int PaUtils_GetIndexFromPaOutDevName(const CString& outDevName)
{
  assert(g_PaDevListCreated);
  for(int i = 0; i < g_portAudioOutDevList.GetCount( ); ++i) {
    if (outDevName == g_portAudioOutDevList.GetAt(i))
      return i;
  }
  return -1;
}

int PaUtils_GetDefaultInputDeviceIndex( )
{
  return Pa_GetDefaultInputDevice( );
}

int PaUtils_GetDefaultOutputDeviceIndex( )
{
  return Pa_GetDefaultOutputDevice( );
}

