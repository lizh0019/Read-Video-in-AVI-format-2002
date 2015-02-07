#if !defined(AFX_USERWATERMARK_H__1E04FF35_E342_4DC4_B40B_BEB464ADA9A2__INCLUDED_)
#define AFX_USERWATERMARK_H__1E04FF35_E342_4DC4_B40B_BEB464ADA9A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserWatermark.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark dialog

class CUserWatermark : public CDialog
{
// Construction
public:
	CUserWatermark(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CUserWatermark)
	enum { IDD = IDD_USER_WATERMARK };
	CString	m_userwatermark_info;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserWatermark)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserWatermark)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnPseudo_UserWatermark();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERWATERMARK_H__1E04FF35_E342_4DC4_B40B_BEB464ADA9A2__INCLUDED_)
