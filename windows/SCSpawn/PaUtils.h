extern void PaUtils_CreatePaDevList( );
extern CString PaUtils_GetPaInDevNameFromIndex( int index );
extern CString PaUtils_GetPaOutDevNameFromIndex( int index );
extern int PaUtils_GetIndexFromPaInDevName(const CString& inDevName);
extern int PaUtils_GetIndexFromPaOutDevName(const CString& outDevName);
extern int PaUtils_GetDefaultInputDeviceIndex( );
extern int PaUtils_GetDefaultOutputDeviceIndex( );
