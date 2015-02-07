// READ AVIView.h : interface of the CREADAVIView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_READAVIVIEW_H__422A6579_0E48_4F94_AF64_F106832CA2D7__INCLUDED_)
#define AFX_READAVIVIEW_H__422A6579_0E48_4F94_AF64_F106832CA2D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CREADAVIView : public CScrollView
{
protected: // create from serialization only
	CREADAVIView();
	DECLARE_DYNCREATE(CREADAVIView)

// Attributes
public:
	CREADAVIDoc* GetDocument();
    
	RECT m_rect;//显示原始视频的区域
    RECT m_rect_large;//显示原始视频(高分辨率)的区域
	RECT m_rect_framenumber;//显示原始视频帧数的区域
	RECT m_rect_framenumber_large;//显示原始视频帧数(针对高分辨率)的区域
    RECT m_rect_watermarked;//显示嵌入水印视频的区域
	RECT m_rect_framenumber_watermarked;//显示嵌入水印视频帧数的区域
	RECT m_rect_watermark;//显示嵌入水印的区域
	RECT m_rect_watermarkextracted;//显示提取水印的区域
	RECT m_rect_attacked;//显示经过攻击的视频的区域
    RECT m_rect_attackedcolor;//显示经过攻击的视频的区域(彩色) 

    RECT m_rect_framenumber_attacked;//显示经过攻击的视频帧数的区域
	RECT m_rect_watermarkattacked;//显示经过攻击后的水印的区域

	BOOL WatermarkedFile_Opened;//嵌入水印的文件是否打开
	BOOL m_watermark;
   
    int width;//The width of bmp
    int height;//The height of bmp

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CREADAVIView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CREADAVIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CREADAVIView)
	afx_msg void OnButtonPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateButtonPlay(CCmdUI* pCmdUI);
	afx_msg void OnButtonStop();
	afx_msg void OnUpdateButtonStop(CCmdUI* pCmdUI);
	afx_msg void OnButtonBack();
	afx_msg void OnButtonForward();
	afx_msg void OnUpdateButtonBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonForward(CCmdUI* pCmdUI);
	afx_msg void OnButtonReset();
	afx_msg void OnUpdateButtonReset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTimeSpatialEmbedding(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePseudoRandomGenerator(CCmdUI* pCmdUI);
	afx_msg void OnPseudoRandom_Generator();
	afx_msg void OnTimeSpatial_Embedding();
	afx_msg void OnButtonBegin();
	afx_msg void OnButtonEnd();
	afx_msg void OnUpdateButtonBegin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonEnd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBinaryWatermark(CCmdUI* pCmdUI);
	afx_msg void OnOpenBinaryWatermark();
	afx_msg void OnRandom();
	afx_msg void OnUpdateRandom(CCmdUI* pCmdUI);
	afx_msg void OnTimeEmbeddingSecond();
	afx_msg void OnWatermarkExtract();
	afx_msg void OnUpdateWatermarkExtract(CCmdUI* pCmdUI);
	afx_msg void OnSaveCurrentwatermark();
	afx_msg void OnUpdateSaveCurrentwatermark(CCmdUI* pCmdUI);
	afx_msg void OnOpenAttackedFile();
	afx_msg void OnExtractWatermarkattacked();
	afx_msg void OnExtractCurrentWmarkattacked();
	afx_msg void OnSaveWatermarkattacked();
	afx_msg void OnTimeEmbeddingImproved();
	afx_msg void OnSaveCurrentframe();
	afx_msg void OnWatermarkRegionDetection();
	afx_msg void OnUserWatermark();
	afx_msg void OnDetectionCurrframeSequence();
	afx_msg void OnExtractCurrentWmarksequenceattacked();
	afx_msg void OnExtractGopWmarksequenceattacked();
	afx_msg void OnNonblindExtractionCurrframeSequence();
	afx_msg void OnNoblindextractCurrentWmarkattacked();
	afx_msg void OnNoblindextractGopWmarksequenceattacked();
	afx_msg void OnEdgeEnhance();
	afx_msg void OnImageBlur();
	afx_msg void OnEdgeDetection();
	afx_msg void OnFrameDifference();
	afx_msg void OnSaveCurrentOriginalFrame();
	afx_msg void OnUpdateSaveCurrentOriginalFrame(CCmdUI* pCmdUI);
	afx_msg void OnPredictionFilter7x7();
	afx_msg void OnTimeAxisEmbedding();
	afx_msg void On66bitUserWatermark();
	afx_msg void OnExtractWatermarkAlongTimeAxis();
	afx_msg void OnExtractWmarksequencealongtimeaxisattacked();
	afx_msg void OnSaveCurrentAttackedFrame();
	afx_msg void OnTimeEmbeddingSpreadspectrum();
	afx_msg void OnPredictionFilter3x3();
	afx_msg void OnLaplacianFilter3x3();
	afx_msg void OnExtractionCurrframeSSsequence();
	afx_msg void OnExtractionCurrframeSSsequenceAttacked();
	afx_msg void OnExtractBitplaneWatermarkAlongTimeAxis();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnExtractBitplanewmarksequencealongtimeaxisattacked();
	afx_msg void OnOpenAttackedColorFile();
	afx_msg void OnTimeAxisEmbeddingDualWatermarks();
	afx_msg void OnExtractionCurrframeAddedSSsequence();
	afx_msg void OnExtractionCurrframeAddedSSSequenceAttacked();
	afx_msg void OnBlindExtractionCurrFrameAddedSSsequenceAttacked();
	afx_msg void OnAdaptiveDCTEmbedding();
	afx_msg void OnExtractionCurrFrameDCTdomainAdaptive();
	afx_msg void OnExtractionCurrFrameDCTdomainAdaptiveAttacked();
	afx_msg void OnDewAlgorithm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in READ AVIView.cpp
inline CREADAVIDoc* CREADAVIView::GetDocument()
   { return (CREADAVIDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READAVIVIEW_H__422A6579_0E48_4F94_AF64_F106832CA2D7__INCLUDED_)
