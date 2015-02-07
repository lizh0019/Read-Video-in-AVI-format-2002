// UserWatermark.cpp : implementation file
//

#include "stdafx.h"
#include "READ AVI.h"
#include "UserWatermark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark dialog


CUserWatermark::CUserWatermark(CWnd* pParent /*=NULL*/)
	: CDialog(CUserWatermark::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserWatermark)
	m_userwatermark_info = _T("");
	//}}AFX_DATA_INIT
}


void CUserWatermark::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserWatermark)
	DDX_Text(pDX, IDC_USER_WATERMARKINFO, m_userwatermark_info);
	DDV_MaxChars(pDX, m_userwatermark_info, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserWatermark, CDialog)
	//{{AFX_MSG_MAP(CUserWatermark)
	ON_BN_CLICKED(IDPSEUDO, OnPseudo_UserWatermark)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark message handlers

BOOL CUserWatermark::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

//	m_userwatermark_info="0101010101010101010101010100000000000000000000000000000000000000";
//	UpdateData(FALSE);//更新编辑框中的数值

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserWatermark::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CUserWatermark::OnPseudo_UserWatermark() 
{
	// TODO: Add your control notification handler code here

	//m_userwatermark_info="0101010101010101010101010100000000000000000000000000000000000000";
	m_userwatermark_info="0101010101010101010101010110101010101010101010101010101010101010";
    
	UpdateData(FALSE);//更新编辑框中的数值
}
