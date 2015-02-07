// READ AVI.h : main header file for the READ AVI application
//

#if !defined(AFX_READAVI_H__0DB3D3FF_4A4D_4BC2_94C5_653322F3EA42__INCLUDED_)
#define AFX_READAVI_H__0DB3D3FF_4A4D_4BC2_94C5_653322F3EA42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CREADAVIApp:
// See READ AVI.cpp for the implementation of this class
//

class CREADAVIApp : public CWinApp
{
public:
	CREADAVIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CREADAVIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CREADAVIApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READAVI_H__0DB3D3FF_4A4D_4BC2_94C5_653322F3EA42__INCLUDED_)
