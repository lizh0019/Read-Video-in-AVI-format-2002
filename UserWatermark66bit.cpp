// UserWatermark66bit.cpp : implementation file
//

#include "stdafx.h"
#include "READ AVI.h"
#include "UserWatermark66bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark66bit dialog


CUserWatermark66bit::CUserWatermark66bit(CWnd* pParent /*=NULL*/)
	: CDialog(CUserWatermark66bit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserWatermark66bit)
	m_user_watermark_66bit = _T("");
	//}}AFX_DATA_INIT
}


void CUserWatermark66bit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserWatermark66bit)
	DDX_Text(pDX, IDC_USER_WATERMARKINFO, m_user_watermark_66bit);
	DDV_MaxChars(pDX, m_user_watermark_66bit, 66);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserWatermark66bit, CDialog)
	//{{AFX_MSG_MAP(CUserWatermark66bit)
	ON_BN_CLICKED(IDINITIALIZATION, OnInitialization)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserWatermark66bit message handlers

void CUserWatermark66bit::OnInitialization() 
{
	// TODO: Add your control notification handler code here
	m_user_watermark_66bit="010101010101010101010101011010101010101010101010101010101010101010";
    
	UpdateData(FALSE);//更新编辑框中的数值
}

void CUserWatermark66bit::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CUserWatermark66bit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
