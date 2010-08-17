#if defined(__APPLE__) || defined(SC_IPHONE)

#ifndef _SC_StandAloneInfo_
#define _SC_StandAloneInfo_
#endif

class SC_StandAloneInfo;

class SC_StandAloneInfo
{
private:
	static bool sIsStandAlone;
	static bool haveCheckedBundleStatus;
	static char dirPath[PATH_MAX];

	static void SC_StandAloneInfoInit();
public:
	static bool IsStandAlone();
	static void GetResourceDir(char* pathBuf, int length);
};
#endif
