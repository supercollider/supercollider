// SmartString.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartString

CSmartString::CSmartString(UINT nIdText)
{
	m_bCString = true;
	m_pStrText = new CString;

	#ifdef _DEBUG
	if(!m_pStrText->LoadString(nIdText))
		TRACE("\nError loading string %d", nIdText);
	#else
	m_pStrText->LoadString(nIdText);
	#endif
}

void CSmartString::SetCString(const CString& strText)
{
	m_bCString = true;
	m_pStrText = new CString(strText);
}

void CSmartString::SetLPCTSTR(LPCTSTR pText)
{
	m_bCString = false;
	m_pText = pText;
}

void CSmartString::Clear()
{
	if(m_bCString)
		delete m_pStrText;
	m_pStrText = NULL;
}

CSmartString::CSmartString(const CSmartString& d)
{
	if(d.m_bCString)
		SetCString(*d.m_pStrText);
	else
		SetLPCTSTR(d.m_pText);
}

CSmartString::~CSmartString()
{
	if(m_bCString)
		delete m_pStrText;
}

CSmartString& CSmartString::operator=(const CSmartString& data)
{
	Clear();

	if(data.m_bCString)
		SetCString(*data.m_pStrText);
	else
		SetLPCTSTR(data.m_pText);

	return *this;
}

/*
CSmartString& CSmartString::operator=(const CString& strText)
{
	Clear();
	SetCString(strText);

	return *this;
}
*/

CSmartString& CSmartString::operator=(LPCTSTR pData)
{
	Clear();
	SetLPCTSTR(pData);

	return *this;
}

CSmartString::operator LPCTSTR() const
{
	return m_bCString ? m_pStrText->operator LPCTSTR() : m_pText;
}
