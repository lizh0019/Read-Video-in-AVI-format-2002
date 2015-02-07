#if !defined(AFX_USERWATERMARK66BIT_H__1AB5BADA_B453_4527_8F47_EFA72AA7C11F__INCLUDED_)
#define AFX_USERWATERMARK66BIT_H__1AB5BADA_B453_4527_8F47_EFA72AA7C11F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserWatermark66bit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark66bit dialog

class CUserWatermark66bit : public CDialog
{
// Construction
public:
	CUserWatermark66bit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserWatermark66bit)
	enum { IDD = IDD_66BIT_USER_WATERMARK };
	CString	m_user_watermark_66bit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserWatermark66bit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserWatermark66bit)
	afx_msg void OnInitialization();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERWATERMARK66BIT_H__1AB5BADA_B453_4527_8F47_EFA72AA7C11F__INCLUDED_)
