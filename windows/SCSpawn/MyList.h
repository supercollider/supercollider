#if !defined(_MYLIST_H)
#define _MYLIST_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MyList.h
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////
// CMyTypedPtrList

template<class BASE_CLASS, class TYPE>
class CMyTypedPtrList : public CTypedPtrList<BASE_CLASS, TYPE>
{
public:
	CMyTypedPtrList(int nBlockSize = 10) 
		: CTypedPtrList<BASE_CLASS, TYPE>(nBlockSize) 
	{
	}

	~CMyTypedPtrList()
	{
		Destroy();
	}

	void Destroy()
	{
		POSITION pos = GetHeadPosition();

		while(pos != NULL)
		{
			TYPE node = GetNext(pos);
			delete node;
		}

		RemoveAll();
	}
};

#endif //!defined(_MYLIST_H)