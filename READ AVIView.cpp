// READ AVIView.cpp : implementation of the CREADAVIView class
//

#include "stdafx.h"
#include "READ AVI.h"

#include "UserWatermark.h"
#include "UserWatermark66bit.h"

#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "io.h"
#include "stdio.h"

#include "READ AVIDoc.h"
#include "READ AVIView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView

IMPLEMENT_DYNCREATE(CREADAVIView, CScrollView)

BEGIN_MESSAGE_MAP(CREADAVIView, CScrollView)
	//{{AFX_MSG_MAP(CREADAVIView)
	ON_COMMAND(ID_BUTTON_PLAY, OnButtonPlay)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PLAY, OnUpdateButtonPlay)
	ON_COMMAND(ID_BUTTON_STOP, OnButtonStop)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_STOP, OnUpdateButtonStop)
	ON_COMMAND(ID_BUTTON_BACK, OnButtonBack)
	ON_COMMAND(ID_BUTTON_FORWARD, OnButtonForward)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BACK, OnUpdateButtonBack)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FORWARD, OnUpdateButtonForward)
	ON_COMMAND(ID_BUTTON_RESET, OnButtonReset)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RESET, OnUpdateButtonReset)
	ON_UPDATE_COMMAND_UI(ID_TIME_SPATIAL, OnUpdateTimeSpatialEmbedding)
	ON_UPDATE_COMMAND_UI(ID_PSEUDO_RANDOM, OnUpdatePseudoRandomGenerator)
	ON_COMMAND(ID_PSEUDO_RANDOM, OnPseudoRandom_Generator)
	ON_COMMAND(ID_TIME_SPATIAL, OnTimeSpatial_Embedding)
	ON_COMMAND(ID_BUTTON_BEGIN, OnButtonBegin)
	ON_COMMAND(ID_BUTTON_END, OnButtonEnd)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BEGIN, OnUpdateButtonBegin)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_END, OnUpdateButtonEnd)
	ON_UPDATE_COMMAND_UI(ID_BINARY_WATERMARK, OnUpdateBinaryWatermark)
	ON_COMMAND(ID_BINARY_WATERMARK, OnOpenBinaryWatermark)
	ON_COMMAND(ID_RANDOM, OnRandom)
	ON_UPDATE_COMMAND_UI(ID_RANDOM, OnUpdateRandom)
	ON_COMMAND(ID_TIME_EMBEDDING2, OnTimeEmbeddingSecond)
	ON_COMMAND(ID_WATERMARK_EXTRACT, OnWatermarkExtract)
	ON_UPDATE_COMMAND_UI(ID_WATERMARK_EXTRACT, OnUpdateWatermarkExtract)
	ON_COMMAND(ID_SAVE_CURRENTWATERMARK, OnSaveCurrentwatermark)
	ON_UPDATE_COMMAND_UI(ID_SAVE_CURRENTWATERMARK, OnUpdateSaveCurrentwatermark)
	ON_COMMAND(ID_WATERMARK_ATTACKED, OnOpenAttackedFile)
	ON_COMMAND(ID_EXTRACT_WATERMARKATTACKED, OnExtractWatermarkattacked)
	ON_COMMAND(ID_EXTRACT_CURRENT_WMARKATTACKED, OnExtractCurrentWmarkattacked)
	ON_COMMAND(ID_SAVE_WMARKATTACKED, OnSaveWatermarkattacked)
	ON_COMMAND(ID_TIME_EMBEDDING_IMPROVED, OnTimeEmbeddingImproved)
	ON_COMMAND(ID_SAVE_CURRENTFRAME, OnSaveCurrentframe)
	ON_COMMAND(ID_WATERMARK_DETECTION, OnWatermarkRegionDetection)
	ON_COMMAND(ID_USER_WATERMARK, OnUserWatermark)
	ON_COMMAND(ID_DETECTION_CURRFRAME_SEQUENCE, OnDetectionCurrframeSequence)
	ON_COMMAND(ID_EXTRACT_CURRENT_WMARKSEQUENCEATTACKED, OnExtractCurrentWmarksequenceattacked)
	ON_COMMAND(ID_EXTRACT_GOP_WMARKSEQUENCEATTACKED, OnExtractGopWmarksequenceattacked)
	ON_COMMAND(ID_NONBLIND_DETECTION_CURRFRAME_SEQUENCE, OnNonblindExtractionCurrframeSequence)
	ON_COMMAND(ID_NOBLINDEXTRACT_CURRENT_WMARKATTACKED, OnNoblindextractCurrentWmarkattacked)
	ON_COMMAND(ID_NOBLINDEXTRACT_GOP_WMARKSEQUENCEATTACKED, OnNoblindextractGopWmarksequenceattacked)
	ON_COMMAND(ID_EDGE_ENHANCE, OnEdgeEnhance)
	ON_COMMAND(ID_IMAGE_BLUR, OnImageBlur)
	ON_COMMAND(ID_EDGE_DETECTION, OnEdgeDetection)
	ON_COMMAND(ID_FRAME_DIFFERENCE, OnFrameDifference)
	ON_COMMAND(ID_SAVE_CURRENT_ORIGINAL_FRAME, OnSaveCurrentOriginalFrame)
	ON_UPDATE_COMMAND_UI(ID_SAVE_CURRENT_ORIGINAL_FRAME, OnUpdateSaveCurrentOriginalFrame)
	ON_COMMAND(ID_PREDICTION_FILTER, OnPredictionFilter7x7)
	ON_COMMAND(ID_TIME_AXIS_EMBEDDING, OnTimeAxisEmbedding)
	ON_COMMAND(ID_66bit_User_Watermark, On66bitUserWatermark)
	ON_COMMAND(ID_EXTRACT_WATERMARK_ALONG_TIME_AXIS, OnExtractWatermarkAlongTimeAxis)
	ON_COMMAND(ID_EXTRACT_WMARKSEQUENCEALONGTIMEAXISATTACKED, OnExtractWmarksequencealongtimeaxisattacked)
	ON_COMMAND(ID_SAVE_CURRENT_ATTACKED_FRAME, OnSaveCurrentAttackedFrame)
	ON_COMMAND(ID_ADAPTIVE_TIME_EMBEDDING_SPREADSPECTRUM, OnTimeEmbeddingSpreadspectrum)
	ON_COMMAND(ID_PREDICTION_FILTER_3x3, OnPredictionFilter3x3)
	ON_COMMAND(ID_Laplacian_Filter, OnLaplacianFilter3x3)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_SSSEQUENCE, OnExtractionCurrframeSSsequence)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_SSSEQUENCE_ATTACKED, OnExtractionCurrframeSSsequenceAttacked)
	ON_COMMAND(ID_EXTRACT_BITPLANEWATERMARK_ALONG_TIME_AXIS, OnExtractBitplaneWatermarkAlongTimeAxis)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EXTRACT_BITPLANEWMARKSEQUENCEALONGTIMEAXISATTACKED, OnExtractBitplanewmarksequencealongtimeaxisattacked)
	ON_COMMAND(ID_WATERMARK_ATTACKED_COLOR, OnOpenAttackedColorFile)
	ON_COMMAND(ID_TIME_AXIS_EMBEDDING_DUAL_WATERMARKS, OnTimeAxisEmbeddingDualWatermarks)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_ADDEDSSSEQUENCE, OnExtractionCurrframeAddedSSsequence)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_ADDEDSSSEQUENCE_ATTACKED, OnExtractionCurrframeAddedSSSequenceAttacked)
	ON_COMMAND(ID_BLINDEXTRACTION_CURRFRAME_ADDEDSSSEQUENCE_ATTACKED, OnBlindExtractionCurrFrameAddedSSsequenceAttacked)
	ON_COMMAND(ID_ADAPTIVE_DCT_EMBEDDING, OnAdaptiveDCTEmbedding)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_DCTDOMAINADAPTIVE, OnExtractionCurrFrameDCTdomainAdaptive)
	ON_COMMAND(ID_EXTRACTION_CURRFRAME_DCTDOMAIN_ADAPTIVE_ATTACKED, OnExtractionCurrFrameDCTdomainAdaptiveAttacked)
	ON_COMMAND(ID_DEW_ALGORITHM, OnDewAlgorithm)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView construction/destruction

CREADAVIView::CREADAVIView()
{
	// TODO: add construction code here
	m_rect.left=15;
	m_rect.top=15;
    m_rect.right=367;
	m_rect.bottom=303;

	m_rect_large.left=15;
	m_rect_large.top=15;
	m_rect_large.right=735;
	m_rect_large.bottom=591;

	m_rect_framenumber.left=318;
	m_rect_framenumber.top=345;
    m_rect_framenumber.right=345;
	m_rect_framenumber.bottom=365;

	m_rect_framenumber_large.left=410;
	m_rect_framenumber_large.top=605;
	m_rect_framenumber_large.right=440;
	m_rect_framenumber_large.bottom=625;

	m_rect_watermarked.left=395;
    m_rect_watermarked.top=15;
	m_rect_watermarked.right=747;
    m_rect_watermarked.bottom=303;

	m_rect_watermark.left=15;
    m_rect_watermark.top=400;
	m_rect_watermark.right=367;
    m_rect_watermark.bottom=688;

    m_rect_framenumber_watermarked.left=698;
	m_rect_framenumber_watermarked.top=345;
    m_rect_framenumber_watermarked.right=725;
	m_rect_framenumber_watermarked.bottom=365;

	m_rect_watermarkextracted.left=395;
    m_rect_watermarkextracted.top=400;
    m_rect_watermarkextracted.right=747;
    m_rect_watermarkextracted.bottom=720;

	m_rect_attacked.left=775;
    m_rect_attacked.top=15;
	m_rect_attacked.right=1127;
	m_rect_attacked.bottom=303;

	m_rect_attackedcolor.left=775;
    m_rect_attackedcolor.top=380;
	m_rect_attackedcolor.right=1127;
	m_rect_attackedcolor.bottom=668;

	m_rect_framenumber_attacked.left=1070;
	m_rect_framenumber_attacked.top=342;
	m_rect_framenumber_attacked.right=1100;
	m_rect_framenumber_attacked.bottom=365;

	m_rect_watermarkattacked.left=775;
    m_rect_watermarkattacked.top=400;
    m_rect_watermarkattacked.right=1127;
    m_rect_watermarkattacked.bottom=720;

	WatermarkedFile_Opened=FALSE;	
	m_watermark=FALSE;
   
}

CREADAVIView::~CREADAVIView()
{
}

BOOL CREADAVIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView drawing

void CREADAVIView::OnDraw(CDC* pDC)
{
	CREADAVIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// TODO: add draw code for native data here
    int FrameWidth=352;
	int FrameHeight=288;

	int i; 

	//���ñ�����ɫ
	pDC->SetBkColor(14675180);//R 236 G 236 B 223 -> BGR 0xDFECEC 14675180 
	
	if (pDoc->PositionFlag==1)//ԭʼ��Ƶ����
	{
      if (pDoc->FrameSizeInfo[0]==96)//352*288
	  {
		::StretchDIBits(pDC->m_hDC,15,15,352,288,0,0,352,288,pDoc->FrameBytes,(BITMAPINFO *)pDoc->FrameInfo,DIB_RGB_COLORS,SRCCOPY);
      
	    if (pDoc->FrameColorInfo[0]==8)//�Ҷ�   
		   pDC->TextOut(15,315,"ԭʼ��Ƶ����(�Ҷ�)");
        else
		   pDC->TextOut(15,315,"ԭʼ��Ƶ����(��ɫ)");   
          
	  
	    pDC->TextOut(205,315,"352x288");

	    if (pDoc->TotalFrameNumber==150)
    	  pDC->TextOut(295,315,"�� 150 ֡");
	    else
		{   
		  if (pDoc->TotalFrameNumber==300)
            pDC->TextOut(295,315,"�� 300 ֡");
		  else
		  {
			  if (pDoc->TotalFrameNumber==260)
                  pDC->TextOut(295,315,"�� 260 ֡");  
              else
			  {  if (pDoc->TotalFrameNumber==60)
                     pDC->TextOut(295,315,"��  60 ֡");
			     else
				 {
					 pDC->TextOut(295,315,"��  30 ֡");
				 }
			  }			      
		  }
	  }

      pDC->TextOut(295,345,"��         ֡");
      char temp[10];
      sprintf(temp,"%d",pDoc->FrameNumber);
	  pDC->TextOut(317,345,temp);
	 }

    if (pDoc->FrameSizeInfo[0]==208)//720*576 
	{
		::StretchDIBits(pDC->m_hDC,15,15,720,576,0,0,720,576,pDoc->FrameBytes,(BITMAPINFO *)pDoc->FrameInfo,DIB_RGB_COLORS,SRCCOPY);
      
	    if (pDoc->FrameColorInfo[0]==8)//�Ҷ�   
		   pDC->TextOut(15,605,"ԭʼ��Ƶ����(�Ҷ�)");
        else
		   pDC->TextOut(15,605,"ԭʼ��Ƶ����(��ɫ)");   
          
	  
	    pDC->TextOut(205,605,"720x576");

	    if (pDoc->TotalFrameNumber==150)
    	  pDC->TextOut(295,605,"�� 150 ֡");
	    else
		{   
		  if (pDoc->TotalFrameNumber==300)
            pDC->TextOut(295,605,"�� 300 ֡");
		  else
		  {
			  if (pDoc->TotalFrameNumber==260)
                  pDC->TextOut(295,605,"�� 260 ֡");  
              else
			  {  if (pDoc->TotalFrameNumber==60)
                     pDC->TextOut(295,605,"��  60 ֡");
			     else
				 {
					 if (pDoc->TotalFrameNumber==30)
                         pDC->TextOut(295,605,"��  30 ֡");
			         else
                         pDC->TextOut(295,605,"��  220 ֡");
				 }
			  }
			      
		  }
	  }

      pDC->TextOut(390,605,"��         ֡");
      char temp[10];
      sprintf(temp,"%d",pDoc->FrameNumber);
	  pDC->TextOut(412,605,temp);
	}
}
	if (pDoc->PositionFlag_Watermarked==1)//Ƕ��ˮӡ����Ƶ����
	{  
      ::StretchDIBits(pDC->m_hDC,395,15,352,288,0,0,352,288,pDoc->FrameBytes_Embedding,(BITMAPINFO *)pDoc->FrameInfo,DIB_RGB_COLORS,SRCCOPY);

	  if (pDoc->FrameColorInfo[0]==8)//�Ҷ�
	      pDC->TextOut(395,315,"Ƕ��ˮӡ����Ƶ����");
      else
          pDC->TextOut(395,315,"Ƕ��ˮӡ����Ƶ����");
	  
	  pDC->TextOut(600,315,"352x288");

	  if (pDoc->TotalFrameNumber==150)
    	  pDC->TextOut(675,315,"�� 150 ֡");
	  else
	  {
		  if (pDoc->TotalFrameNumber==300)
              pDC->TextOut(675,315,"�� 300 ֡");
		  else
		  {	  
			  if (pDoc->TotalFrameNumber==260)
                  pDC->TextOut(675,315,"�� 260 ֡");
			  else 
			  {
				  if (pDoc->TotalFrameNumber==60)
                      pDC->TextOut(675,315,"�� 60 ֡");
                  else
                      pDC->TextOut(675,315,"�� 30 ֡");
			  }
		  }			
	  }

      pDC->TextOut(675,345,"��        ֡");
	  char temp_watermarked[10];
      sprintf(temp_watermarked,"%d",pDoc->FrameNumber_Watermarked);
	  pDC->TextOut(700,345,temp_watermarked);
	}
	
	if(pDoc->PositionFlag_Watermark==1)
	{
	  ::StretchDIBits(pDC->m_hDC,15,400,352,288,0,0,352,288,pDoc->PseudoRandomSequenceBytes,(BITMAPINFO *)pDoc->m_Standard2BitBmpHeaderInfo,DIB_RGB_COLORS,SRCCOPY);
	}
    
    if (pDoc->PositionFlag_UserWatermark==1)
	{
   
      char temp_user_watermark[70];
	  if (pDoc->m_OriginalWatermarkBytes[0]==0)
		   strcpy(temp_user_watermark,"0" );
      if (pDoc->m_OriginalWatermarkBytes[0]==1)
		   strcpy(temp_user_watermark,"1" );
	  for (i=1;i<64;i++)
	  {
		  if (pDoc->m_OriginalWatermarkBytes[i]==0)
			  strcat(temp_user_watermark,"0" );
          if (pDoc->m_OriginalWatermarkBytes[i]==1)
			  strcat(temp_user_watermark,"1" );
	  }
      pDC->TextOut(15,420,temp_user_watermark);
	  pDC->TextOut(15,450,"ԭʼˮӡ 64λ");
	}

	if (pDoc->PositionFlag_UserWatermark66bit==1)
	{
   
      char temp_user_watermark[70];
	  if (pDoc->m_OriginalWatermarkInformationSequence[0]==0)
		   strcpy(temp_user_watermark,"0" );
      if (pDoc->m_OriginalWatermarkInformationSequence[0]==1)
		   strcpy(temp_user_watermark,"1" );
	  for (i=1;i<66;i++)
	  {
		  if (pDoc->m_OriginalWatermarkInformationSequence[i]==0)
			  strcat(temp_user_watermark,"0" );
          if (pDoc->m_OriginalWatermarkInformationSequence[i]==1)
			  strcat(temp_user_watermark,"1" );
	  }
      pDC->TextOut(15,520,temp_user_watermark);
	  pDC->TextOut(15,550,"ԭʼˮӡ 66λ");
	}
	
	if (m_watermark==TRUE)
	{
//		::StretchDIBits(pDC->m_hDC,30,430,width,height,0,0,width,height,pDoc->m_watermark,(BITMAPINFO *)pDoc->m_binarywatermark_infoheader,DIB_RGB_COLORS,SRCCOPY);
        ::StretchDIBits(pDC->m_hDC,15,400,width,height,0,0,width,height,pDoc->m_OriginalWatermarkBytes+62,(BITMAPINFO *)(pDoc->m_OriginalWatermarkBytes+14),DIB_RGB_COLORS,SRCCOPY);
	
		if ((width==352)&&(height==288))
          pDC->TextOut(15,700,"ԭʼˮӡ   352*288");
		if ((width==128)&&(height==128))
          pDC->TextOut(15,540,"ԭʼˮӡ   128*128");
		if ((width==64)&&(height==64))
          pDC->TextOut(15,475,"ԭʼˮӡ   64*64");
		if ((width==32)&&(height==32))
          pDC->TextOut(15,440,"ԭʼˮӡ   32*32");
        if ((width==16)&&(height==16))
          pDC->TextOut(15,426,"ԭʼˮӡ   16*16");
        if ((width==8)&&(height==8))
          pDC->TextOut(15,420,"ԭʼˮӡ   8*8");

	}

	if(pDoc->WatermarkFlag_Permuted==1)
	  ::StretchDIBits(pDC->m_hDC,100,400,width,height,0,0,width,height,pDoc->Watermark_Permuted,(BITMAPINFO *)(pDoc->m_OriginalWatermarkBytes+14),DIB_RGB_COLORS,SRCCOPY);

	if(pDoc->CurrentWatermark_Extracted==1)
	{
		::StretchDIBits(pDC->m_hDC,395,400,width,height,0,0,width,height,pDoc->m_ExtractedWatermarkBytes,(BITMAPINFO *)(pDoc->m_OriginalWatermarkBytes+14),DIB_RGB_COLORS,SRCCOPY);
		pDC->TextOut(395,700,"��ȡˮӡ   352*288");
		pDC->TextOut(595,700,"NC=");
        char temp_nc[20];
        sprintf(temp_nc,"%8f",pDoc->NC_Value);
		pDC->TextOut(625,700,temp_nc);
	}

    if(pDoc->CurrentWatermarkSequence_Extracted==1)
	{ 
	  char temp_user_watermark_extract[70];
	  if (pDoc->Watermark_Extracted[0]==0)
		   strcpy(temp_user_watermark_extract,"0" );
      if (pDoc->Watermark_Extracted[0]==1)
		   strcpy(temp_user_watermark_extract,"1" );
	  for (i=1;i<64;i++)
	  {
		  if (pDoc->Watermark_Extracted[i]==0)
			  strcat(temp_user_watermark_extract,"0" );
          if (pDoc->Watermark_Extracted[i]==1)
			  strcat(temp_user_watermark_extract,"1" );
	  }
      pDC->TextOut(15,520,temp_user_watermark_extract);
	  pDC->TextOut(15,550,"��ȡˮӡ 64λ");

	  pDC->TextOut(160,550,"��ȷ��λ��");
      char temp_correct_bit[10];
      sprintf(temp_correct_bit,"%d",pDoc->correct_bit_number);
	  pDC->TextOut(260,550,temp_correct_bit);

      pDC->TextOut(360,550,"��ȷ��");
	  char temp_correct_rate[30];
      sprintf(temp_correct_rate,"%8f",pDoc->correct_rate);
	  pDC->TextOut(420,550,temp_correct_rate);
      pDC->TextOut(510,550,"%"); 

	}

	 if(pDoc->CurrentSSWatermarkSequence_Extracted==1)
	{ 
	  char temp_user_watermark_extract[70];
	  if (pDoc->Watermark_Extracted[0]==0)
		   strcpy(temp_user_watermark_extract,"0" );
      if (pDoc->Watermark_Extracted[0]==1)
		   strcpy(temp_user_watermark_extract,"1" );
	  for (i=1;i<64;i++)
	  {
		  if (pDoc->Watermark_Extracted[i]==0)
			  strcat(temp_user_watermark_extract,"0" );
          if (pDoc->Watermark_Extracted[i]==1)
			  strcat(temp_user_watermark_extract,"1" );
	  }
      pDC->TextOut(15,490,temp_user_watermark_extract);
	  pDC->TextOut(15,520,"��ȡˮӡ 64λ");

	  pDC->TextOut(160,520,"��ȷ��λ��");
      char temp_correct_bit[10];
      sprintf(temp_correct_bit,"%d",pDoc->correct_bit_number);
	  pDC->TextOut(260,520,temp_correct_bit);

      pDC->TextOut(360,520,"��ȷ��");
	  char temp_correct_rate[30];
      sprintf(temp_correct_rate,"%8f",pDoc->correct_rate);
	  pDC->TextOut(420,520,temp_correct_rate);
      pDC->TextOut(510,520,"%"); 

	}


	//��ʾ��ʱ����Ƕ���ˮӡ��δ�ܹ���������µ���ȡ���
	if(pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted==1)
	{
	  char temp_user_watermark_time_axis_extract[100];
/*	  if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]==0)
		   strcpy(temp_user_watermark_time_axis_extract,"0" );
      if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]==1)
		   strcpy(temp_user_watermark_time_axis_extract,"1" );
	  for (i=1;i<90;i++)
	  {
		  if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i]==0)
			  strcat(temp_user_watermark_time_axis_extract,"0" );
          if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i]==1)
			  strcat(temp_user_watermark_time_axis_extract,"1" );
	  }
      pDC->TextOut(15,620,temp_user_watermark_time_axis_extract);
	  pDC->TextOut(15,650,"��ȡˮӡ 90λ");
*/

	  if (pDoc->m_ExtractedOriginalWatermarkInformationSequence[0]==0)
		   strcpy(temp_user_watermark_time_axis_extract,"0" );
      if (pDoc->m_ExtractedOriginalWatermarkInformationSequence[0]==1)
		   strcpy(temp_user_watermark_time_axis_extract,"1" );
	  for (i=1;i<66;i++)
	  {
		  if (pDoc->m_ExtractedOriginalWatermarkInformationSequence[i]==0)
			  strcat(temp_user_watermark_time_axis_extract,"0" );
          if (pDoc->m_ExtractedOriginalWatermarkInformationSequence[i]==1)
			  strcat(temp_user_watermark_time_axis_extract,"1" );
	  }
      pDC->TextOut(15,620,temp_user_watermark_time_axis_extract);
	  pDC->TextOut(15,650,"��ȡˮӡ 66λ");

	  
	  pDC->TextOut(160,650,"��ȷ��λ��");
      char temp_correct_bit[10];
      sprintf(temp_correct_bit,"%d",pDoc->correct_bit_number);
	  pDC->TextOut(260,650,temp_correct_bit);

      pDC->TextOut(360,650,"��ȷ��");
	  char temp_correct_rate[30];
      sprintf(temp_correct_rate,"%8f",pDoc->correct_rate);
	  pDC->TextOut(420,650,temp_correct_rate);
      pDC->TextOut(510,650,"%"); 
	}

	//��ʾ��ʱ����Ƕ���ˮӡ�ھ��ܹ���������µ���ȡ���
	if(pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted==1)
	{
	  char temp_attacked_user_watermark_time_axis_extract[100];
/*	  if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]==0)
		   strcpy(temp_user_watermark_time_axis_extract,"0" );
      if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]==1)
		   strcpy(temp_user_watermark_time_axis_extract,"1" );
	  for (i=1;i<90;i++)
	  {
		  if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i]==0)
			  strcat(temp_user_watermark_time_axis_extract,"0" );
          if (pDoc->m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i]==1)
			  strcat(temp_user_watermark_time_axis_extract,"1" );
	  }
      pDC->TextOut(15,620,temp_user_watermark_time_axis_extract);
	  pDC->TextOut(15,650,"��ȡˮӡ 90λ");
*/

	  if (pDoc->m_ExtractedOriginalWatermarkInformationSequence_Attacked[0]==0)
		   strcpy(temp_attacked_user_watermark_time_axis_extract,"0" );
      if (pDoc->m_ExtractedOriginalWatermarkInformationSequence_Attacked[0]==1)
		   strcpy(temp_attacked_user_watermark_time_axis_extract,"1" );
	  for (i=1;i<66;i++)
	  {
		  if (pDoc->m_ExtractedOriginalWatermarkInformationSequence_Attacked[i]==0)
			  strcat(temp_attacked_user_watermark_time_axis_extract,"0" );
          if (pDoc->m_ExtractedOriginalWatermarkInformationSequence_Attacked[i]==1)
			  strcat(temp_attacked_user_watermark_time_axis_extract,"1" );
	  }
      pDC->TextOut(15,720,temp_attacked_user_watermark_time_axis_extract);
	  pDC->TextOut(15,750,"��ȡˮӡ 66λ");

	  
	  pDC->TextOut(160,750,"��ȷ��λ��");
      char temp_correct_bit_attacked[10];
      sprintf(temp_correct_bit_attacked,"%d",pDoc->correct_bit_number_attacked);
	  pDC->TextOut(260,750,temp_correct_bit_attacked);

      pDC->TextOut(360,750,"��ȷ��");
	  char temp_correct_rate_attacked[30];
      sprintf(temp_correct_rate_attacked,"%8f",pDoc->correct_rate_attacked);
	  pDC->TextOut(420,750,temp_correct_rate_attacked);
      pDC->TextOut(510,750,"%");
	}
	 
	if(pDoc->PositionFlag_AttackedVideo==1)
	{
       ::StretchDIBits(pDC->m_hDC,775,15,352,288,0,0,352,288,pDoc->FrameBytes_Attacked,(BITMAPINFO *)pDoc->FrameInfoAttacked,DIB_RGB_COLORS,SRCCOPY);
	  
	  pDC->TextOut(775,315,"������������Ƶ����");
      	  
	  pDC->TextOut(965,315,"352x288");

	  if (pDoc->TotalFrameNumber==150)
    	  pDC->TextOut(1050,315,"�� 150 ֡");
	  else
	  {
		  if (pDoc->TotalFrameNumber==300)
            pDC->TextOut(1050,315,"�� 300 ֡");
		  else
		  {
			  if (pDoc->TotalFrameNumber==260)
				 pDC->TextOut(1050,315,"�� 260 ֡");
			  else
			  { 
				  if (pDoc->TotalFrameNumber==60)
				      pDC->TextOut(1050,315,"�� 60 ֡");
				  else
                      pDC->TextOut(1050,315,"�� 30 ֡");
			  }
			
		  }
	  }

      pDC->TextOut(1050,345,"��        ֡");
      char temp_attacked[10];
      sprintf(temp_attacked,"%d",pDoc->FrameNumber_Attacked);
	  pDC->TextOut(1076,345,temp_attacked);
	
	}

	if(pDoc->PositionFlag_AttackedColorVideo==1)
	{
       ::StretchDIBits(pDC->m_hDC,775,380,352,288,0,0,352,288,pDoc->FrameBytes_AttackedColor,(BITMAPINFO *)pDoc->FrameInfoAttackedColor,DIB_RGB_COLORS,SRCCOPY);
	  
	   pDC->TextOut(775,680,"������������Ƶ����(��ɫ)");
      	  
	   pDC->TextOut(1020,680,"352x288");

/*	  if (pDoc->TotalFrameNumber==150)
    	  pDC->TextOut(1050,315,"�� 150 ֡");
	  else
	  {
		  if (pDoc->TotalFrameNumber==300)
            pDC->TextOut(1050,315,"�� 300 ֡");
		  else
		  {
			  if (pDoc->TotalFrameNumber==260)
				 pDC->TextOut(1050,315,"�� 260 ֡");
			  else
			  { 
				  if (pDoc->TotalFrameNumber==60)
				      pDC->TextOut(1050,315,"�� 60 ֡");
				  else
                      pDC->TextOut(1050,315,"�� 30 ֡");
			  }
			
		  }
	  }
*/
	}

	if(pDoc->PositionFlag_AttackedWatermark==1)
	{
        ::StretchDIBits(pDC->m_hDC,775,400,352,288,0,0,352,288,pDoc->m_AttackedWatermarkBytes,(BITMAPINFO *)(pDoc->m_OriginalWatermarkBytes+14),DIB_RGB_COLORS,SRCCOPY);
		pDC->TextOut(775,700,"��ȡˮӡ   352*288");
		pDC->TextOut(975,700,"NC=");
        char temp_attacked_nc[20];
        sprintf(temp_attacked_nc,"%8f",pDoc->NC_Attacked_Value);
		pDC->TextOut(1005,700,temp_attacked_nc);
		
	}

	if(pDoc->PositionFlag_AttackedWatermarkSequence==1)
	{
	  char temp_user_watermark_attacked[70];
	  if (pDoc->Watermark_Attacked[0]==0)
		   strcpy(temp_user_watermark_attacked,"0" );
      if (pDoc->Watermark_Attacked[0]==1)
		   strcpy(temp_user_watermark_attacked,"1" );
	  for (i=1;i<64;i++)
	  {
		  if (pDoc->Watermark_Attacked[i]==0)
			  strcat(temp_user_watermark_attacked,"0" );
          if (pDoc->Watermark_Attacked[i]==1)
			  strcat(temp_user_watermark_attacked,"1" );
	  }
      pDC->TextOut(15,620,temp_user_watermark_attacked);
	  pDC->TextOut(15,650,"��ȡˮӡ 64λ");

	  pDC->TextOut(160,650,"��ȷ��λ��");
      char temp_correct_bit_attacked[10];
      sprintf(temp_correct_bit_attacked,"%d",pDoc->correct_bit_number_attacked);
	  pDC->TextOut(260,650,temp_correct_bit_attacked);

      pDC->TextOut(360,650,"��ȷ��");
	  char temp_correct_rate_attacked[30];
      sprintf(temp_correct_rate_attacked,"%8f",pDoc->correct_rate_attacked);
	  pDC->TextOut(420,650,temp_correct_rate_attacked);
      pDC->TextOut(510,650,"%"); 	
	}

	if (pDoc->AttackedCurrentSSWatermarkSequence_Extracted==1)//��ʾ����������ˮӡ���У��������׵ķ������ɣ�
	{
	  char temp_user_watermark_attacked[70];
	  if (pDoc->Watermark_Attacked[0]==0)
		   strcpy(temp_user_watermark_attacked,"0" );
      if (pDoc->Watermark_Attacked[0]==1)
		   strcpy(temp_user_watermark_attacked,"1" );
	  for (i=1;i<64;i++)
	  {
		  if (pDoc->Watermark_Attacked[i]==0)
			  strcat(temp_user_watermark_attacked,"0" );
          if (pDoc->Watermark_Attacked[i]==1)
			  strcat(temp_user_watermark_attacked,"1" );
	  }
      pDC->TextOut(15,560,temp_user_watermark_attacked);
	  pDC->TextOut(15,590,"��ȡˮӡ 64λ");

	  pDC->TextOut(160,590,"��ȷ��λ��");
      char temp_correct_bit_attacked[10];
      sprintf(temp_correct_bit_attacked,"%d",pDoc->correct_bit_number_attacked);
	  pDC->TextOut(260,590,temp_correct_bit_attacked);

      pDC->TextOut(360,590,"��ȷ��");
	  char temp_correct_rate_attacked[30];
      sprintf(temp_correct_rate_attacked,"%8f",pDoc->correct_rate_attacked);
	  pDC->TextOut(420,590,temp_correct_rate_attacked);
      pDC->TextOut(510,590,"%"); 		
	}

	if (pDoc->PositionFlag_Difference==1)
	{
	  ::StretchDIBits(pDC->m_hDC,1155,15,352,288,0,0,352,288,pDoc->FrameBytes_Difference,(BITMAPINFO *)pDoc->FrameInfo,DIB_RGB_COLORS,SRCCOPY);
	  pDC->TextOut(1155,315,"Ƕ��ˮӡ����Ƶ֡��ԭʼ��Ƶ֮֡��Ĳ�ֵ");
	}

    if (pDoc->PositionFlag_Difference2==1)
	{
	  ::StretchDIBits(pDC->m_hDC,1535,15,352,288,0,0,352,288,pDoc->FrameBytes_Difference2,(BITMAPINFO *)pDoc->FrameInfo,DIB_RGB_COLORS,SRCCOPY);
	  pDC->TextOut(1535,315,"�ܵ���������Ƶ֡��ԭʼ��Ƶ֮֡��Ĳ�ֵ");
	}

    
}

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView printing

BOOL CREADAVIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CREADAVIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CREADAVIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView diagnostics

#ifdef _DEBUG
void CREADAVIView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CREADAVIView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CREADAVIDoc* CREADAVIView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CREADAVIDoc)));
	return (CREADAVIDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CREADAVIView message handlers



void CREADAVIView::OnButtonPlay() 
{
	// TODO: Add your command handler code here
	//Install a system timer
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	int iInstallResult;

	iInstallResult=SetTimer(1,28,NULL);//����ʱ����Ϊ33����
	if(iInstallResult==0)
	{
	  MessageBox("cannot install timer!");
	}

	pDoc->m_button_play=FALSE;
	pDoc->m_button_stop=TRUE;
    pDoc->m_button_reset=FALSE;
	pDoc->m_button_forward=FALSE;
	pDoc->m_button_back=FALSE;	
    pDoc->m_button_begin=FALSE;
	pDoc->m_button_end=FALSE;
	pDoc->m_extract_currentwatermark=FALSE;
    pDoc->m_save_currentwatermark=FALSE;

    pDoc->CurrentWatermark_Extracted=0;
	InvalidateRect(&m_rect_watermarkextracted,TRUE);
	
	pDoc->PositionFlag_AttackedWatermark=0;
    InvalidateRect(&m_rect_watermarkattacked,TRUE);   
}

void CREADAVIView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

   if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
   {
     if (pDoc->AttackedFile_Opened==TRUE)
	 {  
		 if (pDoc->AttackedColorFile_Opened==TRUE)
		 {
			 if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1))
				 &&(pDoc->FrameNumber_Attacked<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_AttackedColor<(pDoc->TotalFrameNumber-1)))//���Ʋ��ŵ�֡��������ͬ���� 
			{
	         //ԭʼ��Ƶ�ļ�
		     pDoc->FramePlay();
             InvalidateRect(&m_rect,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������

             //Ƕ��ˮӡ����Ƶ�ļ�
		     pDoc->FramePlay_Watermarked();
             InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������

		     //������������Ƶ�ļ�
		     pDoc->FramePlay_Attacked();
             InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

			 //������������Ƶ�ļ�(��ɫ)
		     pDoc->FramePlay_AttackedColor();
             InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ������
			}
	        else
			{
		     pDoc->m_button_play=FALSE;
		     pDoc->m_button_stop=FALSE;
             pDoc->m_button_back=TRUE;
		     pDoc->m_button_forward=FALSE;
		     pDoc->m_button_reset=TRUE;
		     pDoc->m_button_begin=TRUE;
             pDoc->m_button_end=FALSE;
		     pDoc->m_extract_currentwatermark=TRUE;//������ȡ��ǰ֡ˮӡ
             KillTimer(1);//�����ʱ��     
	 	     return;
			}
		 }
         else
		 {
			if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber<(pDoc->TotalFrameNumber-1)))//���Ʋ��ŵ�֡��������ͬ���� 
			{
	         //ԭʼ��Ƶ�ļ�
		     pDoc->FramePlay();
             InvalidateRect(&m_rect,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������

             //Ƕ��ˮӡ����Ƶ�ļ�
		     pDoc->FramePlay_Watermarked();
             InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������

		     //������������Ƶ�ļ�
		     pDoc->FramePlay_Attacked();
             InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ������
		     InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������
			}
	        else
			{
		     pDoc->m_button_play=FALSE;
		     pDoc->m_button_stop=FALSE;
             pDoc->m_button_back=TRUE;
		     pDoc->m_button_forward=FALSE;
		     pDoc->m_button_reset=TRUE;
		     pDoc->m_button_begin=TRUE;
             pDoc->m_button_end=FALSE;
		     pDoc->m_extract_currentwatermark=TRUE;//������ȡ��ǰ֡ˮӡ
             KillTimer(1);//�����ʱ��     
	 	     return;
			}
		 }
	 
	 }
	 else
	 {
	   if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1)))//���Ʋ��ŵ�֡��������ͬ���� 
	   {
	    //ԭʼ��Ƶ�ļ�
		pDoc->FramePlay();
        InvalidateRect(&m_rect,FALSE);//ˢ��ָ������
		InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������

        //Ƕ��ˮӡ����Ƶ�ļ�
		pDoc->FramePlay_Watermarked();
        InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ������
		InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������
	   }
	   else
	   {
		pDoc->m_button_play=FALSE;
		pDoc->m_button_stop=FALSE;
        pDoc->m_button_back=FALSE;
		pDoc->m_button_forward=FALSE;
		pDoc->m_button_reset=TRUE;
		pDoc->m_button_begin=FALSE;
        pDoc->m_button_end=FALSE;
		pDoc->m_extract_currentwatermark=TRUE;//������ȡ��ǰ֡ˮӡ
        KillTimer(1);//�����ʱ��     
		return;
	   }	 
	 }
   }
   else//Ƕ��ˮӡ����Ƶ�ļ�δ��
   {//����ԭʼ��Ƶ�ļ�
    if (pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))//���Ʋ��ŵ�֡�� 
    {
	    pDoc->FramePlay();
		if (pDoc->FrameSizeInfo[0]==96)//352*288
		{
		  InvalidateRect(&m_rect,FALSE);//ˢ��ָ������
	      InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������
		}
		if (pDoc->FrameSizeInfo[0]==208)//720*576
		{
           InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ������
		   InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������
		}
	}
	else
	{	
		pDoc->m_button_play=FALSE;
		pDoc->m_button_stop=FALSE;
        pDoc->m_button_back=FALSE;
		pDoc->m_button_forward=FALSE;
		pDoc->m_button_reset=TRUE;
        pDoc->m_button_begin=FALSE;
        pDoc->m_button_end=FALSE;
	    KillTimer(1);//�����ʱ��     
		return;
	}  
  }
	
	CScrollView::OnTimer(nIDEvent);
}

void CREADAVIView::OnDestroy() 
{
	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(1);//�����ʱ��
   
	
}

void CREADAVIView::OnUpdateButtonPlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_play);


}

void CREADAVIView::OnButtonStop() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    KillTimer(1);//�����ʱ����������ͣ

    pDoc->m_button_stop=FALSE;
    pDoc->m_button_back=TRUE;
	pDoc->m_button_forward=TRUE;
	pDoc->m_button_play=TRUE;
    pDoc->m_button_reset=TRUE;
    pDoc->m_button_begin=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_extract_currentwatermark=TRUE;

//	pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���
	pDoc->m_extract_currentwatermark=TRUE;

	InvalidateRect(&m_rect,FALSE);//ˢ��ָ������
}

void CREADAVIView::OnUpdateButtonStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_stop);
	
}

void CREADAVIView::OnButtonBack() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
    
	if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
	{
      if (pDoc->AttackedFile_Opened==TRUE)
	  {
		  if (pDoc->AttackedColorFile_Opened==TRUE)
		  {
			  if ((pDoc->FrameNumber>=1)&&(pDoc->FrameNumber_Watermarked>=1)
				  &&(pDoc->FrameNumber_Attacked>=1)&&(pDoc->FrameNumber_AttackedColor>=1))
			  {
               pDoc->FrameNumber=pDoc->FrameNumber-2;
		       pDoc->FramePlay();

               pDoc->FrameNumber_Watermarked=pDoc->FrameNumber_Watermarked-2;
		       pDoc->FramePlay_Watermarked();

		       pDoc->FrameNumber_Attacked=pDoc->FrameNumber_Attacked-2;
		       pDoc->FramePlay_Attacked();

               pDoc->FrameNumber_AttackedColor=pDoc->FrameNumber_AttackedColor-2;
		       pDoc->FramePlay_AttackedColor();

		       pDoc->m_button_forward=TRUE;
		       pDoc->m_button_end=TRUE;
               pDoc->m_button_play=TRUE;
               if ((pDoc->FrameNumber<1)&&(pDoc->FrameNumber_Watermarked<1)
				   &&(pDoc->FrameNumber_Attacked<1)&&(pDoc->FrameNumber_AttackedColor<1))
			   {
		          pDoc->m_button_back=FALSE;
		          pDoc->m_button_begin=FALSE;
			   }
	           InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	           InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	  
		       InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	           InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
               InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
               InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

			   InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ���Ļ�������

		       pDoc->m_extract_currentwatermark=TRUE;
	           pDoc->m_save_currentwatermark=FALSE;
		       pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���
		       pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
    	       InvalidateRect(&m_rect_watermarkextracted,TRUE);
		       pDoc->PositionFlag_AttackedWatermark=0;
		       InvalidateRect(&m_rect_watermarkattacked,TRUE);
			 }
	         else
			 {
	           pDoc->m_button_back=FALSE;
	           pDoc->m_extract_currentwatermark=TRUE;
		       pDoc->m_save_currentwatermark=FALSE;
		       pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
    	       InvalidateRect(&m_rect_watermarkextracted,TRUE);
			 }
		  }
		  else
		  {
		     if ((pDoc->FrameNumber>=1)&&(pDoc->FrameNumber_Watermarked>=1)&&(pDoc->FrameNumber_Attacked>=1))
			 {
               pDoc->FrameNumber=pDoc->FrameNumber-2;
		       pDoc->FramePlay();

               pDoc->FrameNumber_Watermarked=pDoc->FrameNumber_Watermarked-2;
		       pDoc->FramePlay_Watermarked();

		       pDoc->FrameNumber_Attacked=pDoc->FrameNumber_Attacked-2;
		       pDoc->FramePlay_Attacked();

		       pDoc->m_button_forward=TRUE;
		       pDoc->m_button_end=TRUE;
               pDoc->m_button_play=TRUE;
               if ((pDoc->FrameNumber<1)&&(pDoc->FrameNumber_Watermarked<1)&&(pDoc->FrameNumber_Attacked<1))
			   {
		          pDoc->m_button_back=FALSE;
		          pDoc->m_button_begin=FALSE;
			   }
	           InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	           InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	  
		       InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	           InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
               InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
               InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

		       pDoc->m_extract_currentwatermark=TRUE;
	           pDoc->m_save_currentwatermark=FALSE;
		       pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���
		       pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
    	       InvalidateRect(&m_rect_watermarkextracted,TRUE);
		       pDoc->PositionFlag_AttackedWatermark=0;
		       InvalidateRect(&m_rect_watermarkattacked,TRUE);
			 }
	         else
			 {
	           pDoc->m_button_back=FALSE;
	           pDoc->m_extract_currentwatermark=TRUE;
		       pDoc->m_save_currentwatermark=FALSE;
		       pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
    	       InvalidateRect(&m_rect_watermarkextracted,TRUE);
			 }
		  }
	  }
	  else
	  {
		if ((pDoc->FrameNumber>=1)&&(pDoc->FrameNumber_Watermarked>=1))
		{
         pDoc->FrameNumber=pDoc->FrameNumber-2;
		 pDoc->FramePlay();

         pDoc->FrameNumber_Watermarked=pDoc->FrameNumber_Watermarked-2;
		 pDoc->FramePlay_Watermarked();

		 pDoc->m_button_forward=TRUE;
		 pDoc->m_button_end=TRUE;
         pDoc->m_button_play=TRUE;
         if ((pDoc->FrameNumber<1)&&(pDoc->FrameNumber_Watermarked<1))
		 {
		   pDoc->m_button_back=FALSE;
		   pDoc->m_button_begin=FALSE;
		 }
	     InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	     InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	  
		 InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	     InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
		 pDoc->m_extract_currentwatermark=TRUE;
	     pDoc->m_save_currentwatermark=FALSE;
		 pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���		
    	 InvalidateRect(&m_rect_watermarkextracted,TRUE);
		}
	    else
		{
	     pDoc->m_button_back=FALSE;
	     pDoc->m_extract_currentwatermark=TRUE;
		 pDoc->m_save_currentwatermark=FALSE;
		 pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
    	 InvalidateRect(&m_rect_watermarkextracted,TRUE);
		}
	  }
	}
	else//Ƕ��ˮӡ����Ƶ�ļ�û�д�
	{
	  if (pDoc->FrameNumber>=1)
	  {
         pDoc->FrameNumber=pDoc->FrameNumber-2;
		 pDoc->FramePlay();
		 pDoc->m_button_forward=TRUE;
		 pDoc->m_button_end=TRUE;
         pDoc->m_button_play=TRUE;
         if (pDoc->FrameNumber<1)
		 {
		   pDoc->m_button_back=FALSE;
		   pDoc->m_button_begin=FALSE;
		 }
		 if (pDoc->FrameSizeInfo[0]==96)//352*288
		 {
			InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	        InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
		 }
		 if (pDoc->FrameSizeInfo[0]==208)//720*576
		 {
			InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ���Ļ�������
	        InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������ʾ����֡����
		 }

	  }
	  else
	     pDoc->m_button_back=FALSE;
	}
	pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
	pDoc->AttackedCurrentSSWatermarkSequence_Extracted=0;
	pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
	Invalidate();
}

void CREADAVIView::OnButtonForward() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
	{
	  if(pDoc->AttackedFile_Opened==TRUE)
	  {
        if (pDoc->AttackedColorFile_Opened==TRUE)
		{
			if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1))
				&&(pDoc->FrameNumber_Attacked<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_AttackedColor<(pDoc->TotalFrameNumber-1)))
			{
        	 pDoc->FramePlay();
	    	 pDoc->FramePlay_Watermarked();
			 pDoc->FramePlay_Attacked();
             pDoc->FramePlay_AttackedColor();

		     pDoc->m_button_back=TRUE;
             pDoc->m_button_reset=TRUE;
             pDoc->m_button_begin=TRUE;

		     if ((pDoc->FrameNumber==pDoc->TotalFrameNumber-1)&&(pDoc->FrameNumber_Watermarked==pDoc->TotalFrameNumber-1)
				 &&((pDoc->FrameNumber_Attacked==pDoc->TotalFrameNumber-1))&&((pDoc->FrameNumber_AttackedColor==pDoc->TotalFrameNumber-1)))
			 {
			  pDoc->m_button_forward=FALSE;
			  pDoc->m_button_end=FALSE;
			 }
	         InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	         InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	 
		     InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	         InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
             InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
             InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

			 InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ���Ļ�������

	         pDoc->m_extract_currentwatermark=TRUE;
		     pDoc->m_save_currentwatermark=FALSE;
		     pDoc->CurrentWatermark_Extracted=0;	
		     InvalidateRect(&m_rect_watermarkextracted,TRUE);
			 pDoc->PositionFlag_AttackedWatermark=0;
			 InvalidateRect(&m_rect_watermarkattacked,TRUE);
			}
	        else
			{
		     pDoc->m_button_forward=FALSE;
		     pDoc->m_extract_currentwatermark=TRUE;
	         pDoc->m_save_currentwatermark=FALSE;
		     pDoc->CurrentWatermark_Extracted=0;	
		     InvalidateRect(&m_rect_watermarkextracted,TRUE);
			} 

		}
		else
		{
			if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Attacked<(pDoc->TotalFrameNumber-1)))
			{
        	 pDoc->FramePlay();
	    	 pDoc->FramePlay_Watermarked();
			 pDoc->FramePlay_Attacked();

		     pDoc->m_button_back=TRUE;
             pDoc->m_button_reset=TRUE;
             pDoc->m_button_begin=TRUE;

		     if ((pDoc->FrameNumber==pDoc->TotalFrameNumber-1)&&(pDoc->FrameNumber_Watermarked==pDoc->TotalFrameNumber-1)&&((pDoc->FrameNumber_Attacked==pDoc->TotalFrameNumber-1)))
			 {
			  pDoc->m_button_forward=FALSE;
			  pDoc->m_button_end=FALSE;
			 }
	         InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	         InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	 
		     InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	         InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
             InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
             InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

	         pDoc->m_extract_currentwatermark=TRUE;
		     pDoc->m_save_currentwatermark=FALSE;
		     pDoc->CurrentWatermark_Extracted=0;	
		     InvalidateRect(&m_rect_watermarkextracted,TRUE);
			 pDoc->PositionFlag_AttackedWatermark=0;
			 InvalidateRect(&m_rect_watermarkattacked,TRUE);
			}
	        else
			{
		     pDoc->m_button_forward=FALSE;
		     pDoc->m_extract_currentwatermark=TRUE;
	         pDoc->m_save_currentwatermark=FALSE;
		     pDoc->CurrentWatermark_Extracted=0;	
		     InvalidateRect(&m_rect_watermarkextracted,TRUE);
			} 
		}
        
	  }
      else
	  {
	    if ((pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))&&(pDoc->FrameNumber_Watermarked<(pDoc->TotalFrameNumber-1)))
		{
        	pDoc->FramePlay();
 
	    	pDoc->FramePlay_Watermarked();

		    pDoc->m_button_back=TRUE;
            pDoc->m_button_reset=TRUE;
            pDoc->m_button_begin=TRUE;

		    if ((pDoc->FrameNumber==pDoc->TotalFrameNumber-1)&&(pDoc->FrameNumber_Watermarked==pDoc->TotalFrameNumber-1))
			{
			 pDoc->m_button_forward=FALSE;
			 pDoc->m_button_end=FALSE;
			}
	        InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	        InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	 
		    InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	        InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	  
	        pDoc->m_extract_currentwatermark=TRUE;
		    pDoc->m_save_currentwatermark=FALSE;
		    pDoc->CurrentWatermark_Extracted=0;	
		    InvalidateRect(&m_rect_watermarkextracted,TRUE);
		}
	    else
		{
		    pDoc->m_button_forward=FALSE;
		    pDoc->m_extract_currentwatermark=TRUE;
	        pDoc->m_save_currentwatermark=FALSE;
		    pDoc->CurrentWatermark_Extracted=0;	
		    InvalidateRect(&m_rect_watermarkextracted,TRUE);
		}

	  }
	}
	else
	{
	  if (pDoc->FrameNumber<(pDoc->TotalFrameNumber-1))
	  {
    	pDoc->FramePlay();

		pDoc->m_button_back=TRUE;
        pDoc->m_button_reset=TRUE;
        pDoc->m_button_begin=TRUE;
		if (pDoc->FrameNumber==pDoc->TotalFrameNumber-1)
        {
			pDoc->m_button_forward=FALSE;
			pDoc->m_button_end=FALSE;
		}
        if (pDoc->FrameSizeInfo[0]==96)//352*288
		{
		   InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	       InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
		}
		if (pDoc->FrameSizeInfo[0]==208)//720*576
		{
		   InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ���Ļ�������
	       InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������ʾ����֡����
		}
	  }
	 else
		pDoc->m_button_forward=FALSE;	
	}

    //�����޸�		
	pDoc->CurrentWatermarkSequence_Extracted=0;
	pDoc->CurrentSSWatermarkSequence_Extracted=0;
	pDoc->AttackedCurrentSSWatermarkSequence_Extracted=0;
	pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
	pDoc->PositionFlag_AttackedWatermarkSequence=0;
	pDoc->PositionFlag_Difference=0;
	pDoc->PositionFlag_Difference2=0;
	Invalidate();
		
	

}

void CREADAVIView::OnUpdateButtonBack(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_back);
}

void CREADAVIView::OnUpdateButtonForward(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_forward);	
}

void CREADAVIView::OnButtonReset() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
	{
      if (pDoc->AttackedFile_Opened==TRUE)
	  {
		  if (pDoc->AttackedColorFile_Opened==TRUE)
		  {
                pDoc->FrameNumber=-1;
				pDoc->FrameNumber_Watermarked=-1;
				pDoc->FrameNumber_Attacked=-1;
				pDoc->FrameNumber_AttackedColor=-1;

				pDoc->m_button_reset=FALSE;
				pDoc->m_button_back=FALSE;
				pDoc->m_button_begin=FALSE;
				pDoc->m_button_play=TRUE;
				pDoc->m_button_forward=TRUE;
				pDoc->m_button_end=TRUE;
				pDoc->m_button_stop=FALSE;
				pDoc->m_extract_currentwatermark=TRUE;

				pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���

				pDoc->FramePlay();
				pDoc->FramePlay_Watermarked();
				pDoc->FramePlay_Attacked();
				pDoc->FramePlay_AttackedColor();

				InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

				InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
			
				InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

				InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ���Ļ�������

				pDoc->m_extract_currentwatermark=TRUE;
				pDoc->m_save_currentwatermark=FALSE;
				pDoc->CurrentWatermark_Extracted=0;	
				InvalidateRect(&m_rect_watermarkextracted,TRUE);
        
				pDoc->PositionFlag_AttackedWatermark=0;
				InvalidateRect(&m_rect_watermarkattacked,TRUE);
		  }
          else
		  {
			    pDoc->FrameNumber=-1;
				pDoc->FrameNumber_Watermarked=-1;
				pDoc->FrameNumber_Attacked=-1;

				pDoc->m_button_reset=FALSE;
				pDoc->m_button_back=FALSE;
				pDoc->m_button_begin=FALSE;
				pDoc->m_button_play=TRUE;
				pDoc->m_button_forward=TRUE;
				pDoc->m_button_end=TRUE;
				pDoc->m_button_stop=FALSE;
				pDoc->m_extract_currentwatermark=TRUE;

				pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���

				pDoc->FramePlay();
				pDoc->FramePlay_Watermarked();
				pDoc->FramePlay_Attacked();

				InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

				InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
			
				InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

				pDoc->m_extract_currentwatermark=TRUE;
				pDoc->m_save_currentwatermark=FALSE;
				pDoc->CurrentWatermark_Extracted=0;	
				InvalidateRect(&m_rect_watermarkextracted,TRUE);
        
				pDoc->PositionFlag_AttackedWatermark=0;
				InvalidateRect(&m_rect_watermarkattacked,TRUE);
		  }
	  }
	  else
	  {
	    pDoc->FrameNumber=-1;

	    pDoc->FrameNumber_Watermarked=-1;

	    pDoc->m_button_reset=FALSE;
	    pDoc->m_button_back=FALSE;
	    pDoc->m_button_begin=FALSE;
	    pDoc->m_button_play=TRUE;
	    pDoc->m_button_forward=TRUE;
        pDoc->m_button_end=TRUE;
	    pDoc->m_button_stop=FALSE;
	    pDoc->m_extract_currentwatermark=TRUE;

	    pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���

	    pDoc->FramePlay();

        pDoc->FramePlay_Watermarked();

	    InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	    InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

	    InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	    InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	
	    pDoc->m_extract_currentwatermark=TRUE;
	    pDoc->m_save_currentwatermark=FALSE;
	    pDoc->CurrentWatermark_Extracted=0;	
	    InvalidateRect(&m_rect_watermarkextracted,TRUE);
	  }
	}
	else
	{
	  //���ֳ��߷ֱ���ͼ��͵ͷֱ���ͼ��
      if (pDoc->FrameSizeInfo[0]==96)//352*288
      {
		 pDoc->FrameNumber=-1;
	     pDoc->m_button_reset=FALSE;
	     pDoc->m_button_back=FALSE;
	     pDoc->m_button_begin=FALSE;
	     pDoc->m_button_play=TRUE;
	     pDoc->m_button_forward=TRUE;
         pDoc->m_button_end=TRUE;
	     pDoc->m_button_stop=FALSE;

	     pDoc->FramePlay();
	     InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	     InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	  }

	  if (pDoc->FrameSizeInfo[0]==208)//720*576 
	  {
		 pDoc->FrameNumber=-1;
	     pDoc->m_button_reset=FALSE;
	     pDoc->m_button_back=FALSE;
	     pDoc->m_button_begin=FALSE;
	     pDoc->m_button_play=TRUE;
	     pDoc->m_button_forward=TRUE;
         pDoc->m_button_end=TRUE;
	     pDoc->m_button_stop=FALSE;

	     pDoc->FramePlay();
	     InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ���Ļ�������
	     InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������ʾ����֡����
	  }
	pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���
	}
	pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
	pDoc->AttackedCurrentSSWatermarkSequence_Extracted=0;
	pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
	Invalidate();
}

void CREADAVIView::OnUpdateButtonReset(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_reset);	
}

void CREADAVIView::OnInitialUpdate() 
{
    // CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
    SIZE Size={2000,800};
	SIZE pSize={500,400};
	SIZE lSize={500,400};
	SetScrollSizes(MM_TEXT,Size,pSize,lSize);
	CScrollView::OnInitialUpdate();
}

void CREADAVIView::OnUpdateTimeSpatialEmbedding(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_timespatialembedding);	
	
}

void CREADAVIView::OnUpdatePseudoRandomGenerator(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_pseudorandom_generator);	
	
	
}

void CREADAVIView::OnPseudoRandom_Generator() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnPseudoRandom();
	pDoc->m_timespatialembedding=TRUE;//��ʱ������Ƕ�롱�˵�ʹ��
    
    pDoc->PositionFlag_Watermark=1;

	m_watermark=FALSE;

	InvalidateRect(&m_rect_watermark,FALSE);//ˢ��ָ���Ļ�������
}

void CREADAVIView::OnTimeSpatial_Embedding() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeSpatialEmbedding();
 
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���

    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;

	pDoc->FramePlay();

//	InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������  
	Invalidate();

	
}

void CREADAVIView::OnButtonBegin() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
	{
      if (pDoc->AttackedFile_Opened==TRUE)
	  {
		  if (pDoc->AttackedColorFile_Opened==TRUE)
		  {
			  pDoc->FrameNumber=-1;
              pDoc->FrameNumber_Watermarked=-1;
	          pDoc->FrameNumber_Attacked=-1;
			  pDoc->FrameNumber_AttackedColor=-1;

	          pDoc->m_button_reset=FALSE;
	          pDoc->m_button_back=FALSE;
	          pDoc->m_button_play=TRUE;
	          pDoc->m_button_forward=TRUE;
              pDoc->m_button_begin=FALSE;
	          pDoc->m_button_end=TRUE;

	          pDoc->FramePlay();
              pDoc->FramePlay_Watermarked();
	          pDoc->FramePlay_Attacked();
			  pDoc->FramePlay_AttackedColor();
	   
	          InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	          InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

	          InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	          InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	 
	          InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
	          InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������

			  InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ���Ļ�������
	   
	          pDoc->m_extract_currentwatermark=TRUE;//"��ȡ��ǰ֡ˮӡ"�˵�ʹ��
	          pDoc->m_save_currentwatermark=FALSE;//"���浱ǰ֡ˮӡ"�˵��ر�
	          pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
              InvalidateRect(&m_rect_watermarkextracted,TRUE);
	          pDoc->PositionFlag_AttackedWatermark=0;
	          InvalidateRect(&m_rect_watermarkattacked,TRUE);
		  }
		  else
		  {
			  pDoc->FrameNumber=-1;
              pDoc->FrameNumber_Watermarked=-1;
	          pDoc->FrameNumber_Attacked=-1;

	          pDoc->m_button_reset=FALSE;
	          pDoc->m_button_back=FALSE;
			  pDoc->m_button_play=TRUE;
			  pDoc->m_button_forward=TRUE;
			  pDoc->m_button_begin=FALSE;
			  pDoc->m_button_end=TRUE;

			  pDoc->FramePlay();
			  pDoc->FramePlay_Watermarked();
			  pDoc->FramePlay_Attacked();
			   
			  InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
			  InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

			  InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
			  InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
			 
			  InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
			  InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������
			   
			  pDoc->m_extract_currentwatermark=TRUE;//"��ȡ��ǰ֡ˮӡ"�˵�ʹ��
			  pDoc->m_save_currentwatermark=FALSE;//"���浱ǰ֡ˮӡ"�˵��ر�
			  pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
			  InvalidateRect(&m_rect_watermarkextracted,TRUE);
			  pDoc->PositionFlag_AttackedWatermark=0;
			  InvalidateRect(&m_rect_watermarkattacked,TRUE);
		  }    
	  }
	  else
	  {
	   pDoc->FrameNumber=-1;
       pDoc->FrameNumber_Watermarked=-1;
	   
	   pDoc->m_button_reset=FALSE;
	   pDoc->m_button_back=FALSE;
	   pDoc->m_button_play=TRUE;
	   pDoc->m_button_forward=TRUE;
       pDoc->m_button_begin=FALSE;
	   pDoc->m_button_end=TRUE;

	   pDoc->FramePlay();
       pDoc->FramePlay_Watermarked();
	   
	   InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	   InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

	   InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
	   InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
	
	   pDoc->m_extract_currentwatermark=TRUE;//"��ȡ��ǰ֡ˮӡ"�˵�ʹ��
	   pDoc->m_save_currentwatermark=FALSE;//"���浱ǰ֡ˮӡ"�˵��ر�
	   pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
       InvalidateRect(&m_rect_watermarkextracted,TRUE);
	  }
	}
	else
	{
	   pDoc->FrameNumber=-1;
	   pDoc->m_button_reset=FALSE;
	   pDoc->m_button_back=FALSE;
	   pDoc->m_button_play=TRUE;
	   pDoc->m_button_forward=TRUE;
       pDoc->m_button_begin=FALSE;
	   pDoc->m_button_end=TRUE;

	   pDoc->FramePlay();
	   if (pDoc->FrameSizeInfo[0]==96)//352*288
	   {
		   InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
	       InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
	   }
	   if (pDoc->FrameSizeInfo[0]==208)//720*576
	   {
		   InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ���Ļ�������
	       InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������ʾ����֡����
	   }
	}
	pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
	pDoc->AttackedCurrentSSWatermarkSequence_Extracted=0;
	pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
	Invalidate();
}

void CREADAVIView::OnButtonEnd() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	if (WatermarkedFile_Opened==TRUE)//Ƕ��ˮӡ���ļ��Ѵ�
	{
	   if (pDoc->AttackedFile_Opened==TRUE)
	   {
		   if (pDoc->AttackedColorFile_Opened==TRUE)
		   {
			    pDoc->FrameNumber=pDoc->TotalFrameNumber-2;
				pDoc->FrameNumber_Watermarked=pDoc->TotalFrameNumber-2;
				pDoc->FrameNumber_Attacked=pDoc->TotalFrameNumber-2;
				pDoc->FrameNumber_AttackedColor=pDoc->TotalFrameNumber-2;
				pDoc->m_button_reset=TRUE;
				pDoc->m_button_back=TRUE;
				pDoc->m_button_play=FALSE;
				pDoc->m_button_forward=FALSE;
				pDoc->m_button_begin=TRUE;
				pDoc->m_button_end=FALSE;
			
				pDoc->FramePlay();
				pDoc->FramePlay_Watermarked();
				pDoc->FramePlay_Attacked();
				pDoc->FramePlay_AttackedColor();
       
				InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

				InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
				
				InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������
				
				InvalidateRect(&m_rect_attackedcolor,FALSE);//ˢ��ָ���Ļ�������
				
				pDoc->m_extract_currentwatermark=TRUE;
				pDoc->m_save_currentwatermark=FALSE;
				pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
				InvalidateRect(&m_rect_watermarkextracted,TRUE);

				pDoc->PositionFlag_AttackedWatermark=0;
				InvalidateRect(&m_rect_watermarkattacked,TRUE);
		   }
		   else
		   {
				pDoc->FrameNumber=pDoc->TotalFrameNumber-2;
				pDoc->FrameNumber_Watermarked=pDoc->TotalFrameNumber-2;
				pDoc->FrameNumber_Attacked=pDoc->TotalFrameNumber-2;
				pDoc->m_button_reset=TRUE;
				pDoc->m_button_back=TRUE;
				pDoc->m_button_play=FALSE;
				pDoc->m_button_forward=FALSE;
				pDoc->m_button_begin=TRUE;
				pDoc->m_button_end=FALSE;
			
				pDoc->FramePlay();
				pDoc->FramePlay_Watermarked();
				pDoc->FramePlay_Attacked();
       
				InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

				InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
				
				InvalidateRect(&m_rect_attacked,FALSE);//ˢ��ָ���Ļ�������
				InvalidateRect(&m_rect_framenumber_attacked,FALSE);//ˢ��ָ������
				
				pDoc->m_extract_currentwatermark=TRUE;
				pDoc->m_save_currentwatermark=FALSE;
				pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
				InvalidateRect(&m_rect_watermarkextracted,TRUE);

				pDoc->PositionFlag_AttackedWatermark=0;
				InvalidateRect(&m_rect_watermarkattacked,TRUE);
			}
	   }
	   else
	   {
			pDoc->FrameNumber=pDoc->TotalFrameNumber-2;
			pDoc->FrameNumber_Watermarked=pDoc->TotalFrameNumber-2;
			pDoc->m_button_reset=TRUE;
			pDoc->m_button_back=TRUE;
			pDoc->m_button_play=FALSE;
			pDoc->m_button_forward=FALSE;
			pDoc->m_button_begin=TRUE;
			pDoc->m_button_end=FALSE;
		
			pDoc->FramePlay();
			pDoc->FramePlay_Watermarked();
       
			InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
			InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����

			InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������
			InvalidateRect(&m_rect_framenumber_watermarked,FALSE);//ˢ��ָ������ʾ����֡����
		
			pDoc->m_extract_currentwatermark=TRUE;
			pDoc->m_save_currentwatermark=FALSE;
			pDoc->CurrentWatermark_Extracted=0;//��ǰ��ȡ��ˮӡ���	
			InvalidateRect(&m_rect_watermarkextracted,TRUE);
	   }
	}
	else
	{
			pDoc->FrameNumber=pDoc->TotalFrameNumber-2;
			pDoc->m_button_reset=TRUE;
			pDoc->m_button_back=TRUE;
			pDoc->m_button_play=FALSE;
			pDoc->m_button_forward=FALSE;
			pDoc->m_button_begin=TRUE;
			pDoc->m_button_end=FALSE;
		
			pDoc->FramePlay();
			if (pDoc->FrameSizeInfo[0]==96)//352*288
			{
			   InvalidateRect(&m_rect,FALSE);//ˢ��ָ���Ļ�������
			   InvalidateRect(&m_rect_framenumber,FALSE);//ˢ��ָ������ʾ����֡����
			}
			if (pDoc->FrameSizeInfo[0]==208)//720*576
			{
			   InvalidateRect(&m_rect_large,FALSE);//ˢ��ָ���Ļ�������
			   InvalidateRect(&m_rect_framenumber_large,FALSE);//ˢ��ָ������ʾ����֡����
			}
	}
    pDoc->CurrentSSWatermarkSequence_Extracted=0;//��ǰ��ȡ��ˮӡ���
	pDoc->AttackedCurrentSSWatermarkSequence_Extracted=0;
	pDoc->CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    pDoc->AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
	Invalidate();
}

void CREADAVIView::OnUpdateButtonBegin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_begin);	

}

void CREADAVIView::OnUpdateButtonEnd(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_end);	

}

//DEL void CREADAVIView::OnOpenBinaryImage() 
//DEL {
//DEL 	// TODO: Add your command handler code here
//DEL 	
//DEL }

void CREADAVIView::OnUpdateBinaryWatermark(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_binary_watermark);	
	
}

void CREADAVIView::OnOpenBinaryWatermark() 
{
   // TODO: Add your command handler code here
   CREADAVIDoc* pDoc = GetDocument();//Get the pDoc
   ASSERT_VALID(pDoc);//Set the pDoc valid

   OPENFILENAME ofn;//The structure to open a FileDialog 
   HGLOBAL hbit;//The handle for memory
   hbit=NULL;//Clear the memory handle
   LPBITMAPFILEHEADER bitfile;//The structures for bmp file header   
   LPBITMAPINFOHEADER bitinfo;//The structures for bmp info header   
  
   int off;//The offset of the actual image data
   long int fh;//The file handle
   unsigned char *lpbit;//The pointer of the file data
   char temp_watermark[_MAX_PATH+50];//The temporal string
   long int lenbit;//The length of the opened file
   char FileName[_MAX_PATH],FileTitle[_MAX_PATH];//The Filename and FileTitle 
   FileName[0]='\0';//NULL string
   FileTitle[0]='\0';//NULL string

//Initialize the FileOpenDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size
   ofn.hwndOwner=m_hWnd;//The Owner window handle
   ofn.hInstance=NULL; //None user dialog style template
   //The filter
   ofn.lpstrFilter=TEXT("BMPͼ���ļ� *.bmp\0*.bmp\0All File *.*\0*.*\0\0"); 
   
   //None user filter
   ofn.lpstrCustomFilter=NULL; 
   ofn.nMaxCustFilter=0; 
   
   ofn.nFilterIndex=1; //Select the item 'BMPͼ���ļ� *.bmp'
   
   ////The filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH; 
   
   ////The filename without path
   ofn.lpstrFileTitle=FileTitle; 
   ofn.nMaxFileTitle=_MAX_PATH; 
   strcpy(temp_watermark,"D:\\VC AVI\\ˮӡ");
   ofn.lpstrInitialDir=temp_watermark;
   ofn.lpstrTitle="��򿪶�ֵBMPˮӡͼ��";//The title
   ofn.Flags=OFN_FILEMUSTEXIST;//The flags
   ofn.nFileOffset;//The offset of the filename in the full path
   ofn.nFileExtension;//The offset of the extension in the full path
   ofn.lpstrDefExt="BMP"; //Default extension
   ofn.lCustData=NULL; //None
   ofn.lpfnHook=NULL; //None
   ofn.lpTemplateName=NULL; //None User defined dialog
//Initialize the FileOpenDialog structure ends

//Open the Dialog, and save the structure including the filename
   if(GetOpenFileName(&ofn)==0) return;//If cancle or error,then return
   fh=_open(FileName,_O_RDONLY|_O_BINARY);//Open the file in ReadOnly Mode
   
   //If the file cannot be opened,return directly
   if(fh==-1)
   {
	 sprintf(temp_watermark,"�Բ����޷����ļ�[%s]!",FileName);
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);
	 return;
   }
  
   lenbit=_filelength(fh);//Get the length of the opened file
   
   //If the file is larger than 352*288, return directly.
   if(lenbit>(12800+62))//352*288/8+62
   {
   	 _close(fh);//Close the file   
     sprintf(temp_watermark,"�Բ����ļ�[%s]�����ϱ�����淶!",FileName);//Get the information
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);//Display the infomation
	 return;
   }
  
   hbit=GlobalAlloc(GMEM_FIXED,lenbit);//Apply the required memory
   
   //If memory is not enough, return directly
   if(hbit==NULL)
   {
     _close(fh); //Close the file
	 MessageBox("�Բ����ڴ治��!","�ļ���",MB_ICONSTOP|MB_OK);//Display the information
	 return;
   }
  
   lpbit=(unsigned char *)hbit;//Give the memory pointer to the file data pointer
   
   //Read file into memory
   if(_read(fh,lpbit,lenbit)==-1)
   {
	 _close(fh);   
	 GlobalFree(hbit);
     sprintf(temp_watermark,"�Բ����޷���ȡ�ļ�[%s]!",FileName);
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);
	 return;
   }
   _close(fh);//Close the file
   bitfile=(LPBITMAPFILEHEADER)lpbit;//Get the fileheader
   bitinfo=(LPBITMAPINFOHEADER)(lpbit+sizeof(BITMAPFILEHEADER));//Get the info header
   
//Judge the file type starts//

//If the image is a bmp or not
   if(bitfile->bfType!=0x4d42)
   {
     GlobalFree(hbit);//Free the memory
	 sprintf(temp_watermark,"�Բ����ļ�[%s]����Bmp�ļ�!",FileName);//Get the information
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);//Display the info
	 return;
   }
   
   //If the image is a 256-gray image or not
   if(bitinfo->biBitCount!=1)
   {
   	 GlobalFree(hbit);//Free the memory
	 sprintf(temp_watermark,"�Բ����ļ�[%s]���Ǻڰ�ͼ��!",FileName);//Get the information
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);//Display the info
	 return;
   }
   
   ////If the image is compressed or not
   if(bitinfo->biCompression!=0)
   {
   	 GlobalFree(hbit);//Free the memory
	 sprintf(temp_watermark,"�Բ����ļ�[%s]��ѹ��ͼ��!",FileName);//Get the information
	 MessageBox(temp_watermark,"�ļ���",MB_ICONSTOP|MB_OK);//Display the info
	 return;
   }
//Judge the file type ends//

   //Get the important information
   width=(long int)bitinfo->biWidth;//ˮӡͼ��Ŀ�
   height=(long int)bitinfo->biHeight;//ˮӡͼ��ĸ�
   off=(long int)bitfile->bfOffBits;//ƫ����


   //memcpy(pDoc->m_OriginalWatermarkBytes,lpbit,(width*height/8+62));
   memcpy(pDoc->m_OriginalWatermarkBytes,lpbit,(width*height/8+62+30));//ͼ����ÿһ�д洢���ֽڱ�����4��������

   m_watermark=TRUE;//��ֵˮӡͼ���Ѵ�
   pDoc->m_watermark_permuted=TRUE;

   Invalidate();
   GlobalFree(hbit);
}

void CREADAVIView::OnRandom() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnWatermarkRandom();   
    Invalidate();
	pDoc->m_watermark_permuted=TRUE;

}

void CREADAVIView::OnUpdateRandom(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_watermark_permuted);	

}

void CREADAVIView::OnTimeEmbeddingSecond() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeSpatialEmbedding_Second();

	char temp_bitplane[10];
    sprintf(temp_bitplane,"λƽ��%d",pDoc->BitplanePositionSequence[0]);
    MessageBox(temp_bitplane,"λƽ��",MB_ICONSTOP|MB_OK);//Display the info
 
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���

    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;

	pDoc->FramePlay();

//	InvalidateRect(&m_rect_watermarked,FALSE);//ˢ��ָ���Ļ�������  
	Invalidate();
	
}

void CREADAVIView::OnWatermarkExtract() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtract_CurrentWatermark();
	pDoc->m_extract_currentwatermark=FALSE;//"��ȡ��ǰ֡ˮӡ"�Ĳ˵��ر�
    InvalidateRect(&m_rect_watermarkextracted,TRUE);
	
}

void CREADAVIView::OnUpdateWatermarkExtract(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_extract_currentwatermark);	
}

void CREADAVIView::OnSaveCurrentwatermark() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnSave_CurrentWatermark();	
}

void CREADAVIView::OnUpdateSaveCurrentwatermark(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_save_currentwatermark);
}

void CREADAVIView::OnOpenAttackedFile() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnOpenAttackedAVIFile();
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
    pDoc->FrameNumber_Watermarked=-1;//Ƕ��ˮӡ����Ƶ���и�λ
	pDoc->FramePlay();
	pDoc->FramePlay_Watermarked();
    Invalidate();
//	InvalidateRect(&m_rect_attacked,FALSE);

}

void CREADAVIView::OnExtractWatermarkattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnExtract_WatermarkAttacked();
	InvalidateRect(&m_rect_watermarkattacked,TRUE);
//	Invalidate();
}

void CREADAVIView::OnExtractCurrentWmarkattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnExtractCurrentWatermarkattacked();
	InvalidateRect(&m_rect_watermarkattacked,TRUE);
//	Invalidate(); 
	
}

void CREADAVIView::OnSaveWatermarkattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnSave_CurrentWatermark_Attacked();

}

void CREADAVIView::OnTimeEmbeddingImproved() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeSpatialEmbedding_Improved();

	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;

	pDoc->FramePlay();
	Invalidate();
}

void CREADAVIView::OnSaveCurrentframe() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnSaveCurrentEmbeddedFrame();	
}

void CREADAVIView::OnWatermarkRegionDetection() 
{
	// TODO: Add your command handler code here
    CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnWatermark_Region_Detection();
    Invalidate();
//	InvalidateRect(&m_rect_attacked,TRUE);
}

void CREADAVIView::OnUserWatermark() 
{
	// TODO: Add your command handler code here
	int i,m_button,watermarkinfo_length;

	watermarkinfo_length=0;

	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    CUserWatermark dlg;
	m_button=dlg.DoModal();
	if (m_button==IDOK)
	{
       if (dlg.m_userwatermark_info.GetLength()<64)
	   { 
		   AfxMessageBox("�����ˮӡ��Ϣλ������64λ�����������룡");
		   return;
	   }
	   for (i=0;i<64;i++)
	   {
		   pDoc->m_OriginalWatermarkBytes[i]=dlg.m_userwatermark_info[i]-'0';//�洢ˮӡ��Ϣ
	       if ((pDoc->m_OriginalWatermarkBytes[i]!=0)&&(pDoc->m_OriginalWatermarkBytes[i]!=1))
		   { 
			   AfxMessageBox("�����ˮӡ��Ϣ����Ϊ0��1�����������룡");
			   return;
		   }
		}
	   pDoc->PositionFlag_UserWatermark=1;
	}
	if(m_button==IDPSEUDO)
	{

		  
	}
	Invalidate();
}



void CREADAVIView::OnDetectionCurrframeSequence() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnDetection_CurrentWatermarkSequence();
	Invalidate();
}

void CREADAVIView::OnExtractCurrentWmarksequenceattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnExtract_CurrentWatermarksequenceattacked();
	Invalidate(); 




}

void CREADAVIView::OnExtractGopWmarksequenceattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnExtract_GOPWatermarkSequenceAttacked();
	Invalidate();
}

void CREADAVIView::OnNonblindExtractionCurrframeSequence() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnNonBlindExtraction_CurrentWatermarkSequence();
//	pDoc->m_extract_currentwatermark=FALSE;//"��ȡ��ǰ֡ˮӡ"�Ĳ˵��ر�
//  InvalidateRect(&m_rect_watermarkextracted,TRUE);
	Invalidate();
}

void CREADAVIView::OnNoblindextractCurrentWmarkattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnNoBlindExtractCurrentWatermarkattacked();
//	InvalidateRect(&m_rect_watermarkattacked,TRUE); 
	Invalidate();
}

void CREADAVIView::OnNoblindextractGopWmarksequenceattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pDoc->OnNoblindExtractGopWmarksequenceattacked();
	Invalidate();
}

void CREADAVIView::OnEdgeEnhance() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImageEdgeEnhancingFunction();
	Invalidate(); 
}

void CREADAVIView::OnImageBlur() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImageBlurFunction();
	Invalidate();
}

void CREADAVIView::OnEdgeDetection() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImageEdgeDetection();
	Invalidate();
}

void CREADAVIView::OnFrameDifference() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnFrameDifferenceFunction();
	Invalidate();
}

void CREADAVIView::OnSaveCurrentOriginalFrame() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnSave_CurrentOriginalFrame();	
}

void CREADAVIView::OnUpdateSaveCurrentOriginalFrame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_button_save_current_original_frame);
}

void CREADAVIView::OnPredictionFilter7x7() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImagePredictionFilter();
	Invalidate();
}

void CREADAVIView::OnTimeAxisEmbedding() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeAxisWatermarkEmbedding();
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;
	pDoc->FramePlay();
	Invalidate();
}

void CREADAVIView::On66bitUserWatermark() 
{
	// TODO: Add your command handler code here
	int i,m_button,watermarkinfo_length;
	watermarkinfo_length=0;

	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    CUserWatermark66bit dlg;
	m_button=dlg.DoModal();
	if (m_button==IDOK)
	{
       if (dlg.m_user_watermark_66bit.GetLength()<66)
	   { 
		   AfxMessageBox("�����ˮӡ��Ϣλ������66λ�����������룡");
		   return;
	   }
	   for (i=0;i<66;i++)
	   {
		   pDoc->m_OriginalWatermarkInformationSequence[i]=dlg.m_user_watermark_66bit[i]-'0';//�洢ˮӡ��Ϣ
	       if ((pDoc->m_OriginalWatermarkInformationSequence[i]!=0)&&(pDoc->m_OriginalWatermarkInformationSequence[i]!=1))
		   { 
			   AfxMessageBox("�����ˮӡ��Ϣ����Ϊ0��1�����������룡");
			   return;
		   }
		}
	   pDoc->PositionFlag_UserWatermark66bit=1;
	}
	if(m_button==IDPSEUDO)
	{

		  
	}
	Invalidate();
}

void CREADAVIView::OnExtractWatermarkAlongTimeAxis() 
{
	// TODO: Add your command handler code here
    CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtract_WatermarkSequenceAlongTimeAxis();
	Invalidate();
}

void CREADAVIView::OnExtractWmarksequencealongtimeaxisattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtract_Watermarksequencealongtimeaxisattacked();
	Invalidate();

}

void CREADAVIView::OnSaveCurrentAttackedFrame() 
{
	// TODO: Add your command handler code here
    CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnSaveCurrentAttackedFrame();	
}

void CREADAVIView::OnTimeEmbeddingSpreadspectrum() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeSpatialEmbedding_SpreadSpectrum();
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;
    pDoc->FramePlay();
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
	Invalidate();
}

void CREADAVIView::OnPredictionFilter3x3() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImagePredictionFilter3x3(4, 1,-2, 1,
		                               -2, 4,-2,
									    1,-2, 1,
										128);
	Invalidate();
}


void CREADAVIView::OnLaplacianFilter3x3() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnImagePredictionFilter3x3(1, -1,-1, -1,
		                                -1, 8, -1,
									    -1,-1, -1,
										128);
	Invalidate();
}

void CREADAVIView::OnExtractionCurrframeSSsequence() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrameSSsequence();
	Invalidate();
}

void CREADAVIView::OnExtractionCurrframeSSsequenceAttacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrameSSsequence_Attacked();
	Invalidate();
}

void CREADAVIView::OnExtractBitplaneWatermarkAlongTimeAxis() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtract_BitplaneWatermarkSequenceAlongTimeAxis();
	Invalidate();
}


void CREADAVIView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
  char pText[30];
  CMainFrame * pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
  CClientDC dc(this);
  CREADAVIDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  short int temp_x,temp_y;

  sprintf(pText,"x:%d, y:%d",point.x,point.y); 
  if ((pDoc->PositionFlag==1)||(pDoc->PositionFlag_Watermarked==1)
	  ||(pDoc->PositionFlag_AttackedVideo==1)||(pDoc->PositionFlag_Difference==1)
	  ||(pDoc->PositionFlag_Difference2==1))//ԭʼ��Ƶ����
  {
    if (pDoc->FrameSizeInfo[0]==96)//352*288
	{
	   sprintf(pText,"����������ʾ");
       if ((point.x>=15)&&(point.x<(15+352))&&(point.y>=15)&&(point.y<(15+288)))
	   {
	     temp_x=point.x-15;
	     temp_y=point.y-15;
	     sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }

       if ((point.x>=395)&&(point.x<(395+352))&&(point.y>=15)&&(point.y<(15+288)))
	   {
	      temp_x=point.x-395;
	      temp_y=point.y-15;
	      sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }

       if ((point.x>=775)&&(point.x<(775+352))&&(point.y>=15)&&(point.y<(15+288)))
	   {
	      temp_x=point.x-775;
	      temp_y=point.y-15;
	      sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }

       if ((point.x>=1155)&&(point.x<(1155+352))&&(point.y>=15)&&(point.y<(15+288)))
	   {
	      temp_x=point.x-1155;
	      temp_y=point.y-15;
	      sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }

       if ((point.x>=1535)&&(point.x<(1535+352))&&(point.y>=15)&&(point.y<(15+288)))
	   {
	      temp_x=point.x-1535;
	      temp_y=point.y-15;
	      sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }
	}

	if (pDoc->FrameSizeInfo[0]==208)//720*576 
	{
	   sprintf(pText,"����������ʾ");
       if ((point.x>=15)&&(point.x<(15+720))&&(point.y>=15)&&(point.y<(15+576)))
	   {
	     temp_x=point.x-15;
	     temp_y=point.y-15;
	     sprintf(pText,"x:%d, y:%d",temp_x,temp_y); 
	   }
	}
  }
    pFrame->m_wndStatusBar.SetPaneText(1,pText,TRUE);

    COLORREF color=  dc.GetPixel(point);
    sprintf(pText,"R:%3d, G:%3d, B:%3d",color%256,(color>>8)%256,(color>>16)%256);
    pFrame->m_wndStatusBar.SetPaneText(2,pText,TRUE);

    CScrollView::OnMouseMove(nFlags, point);
}

void CREADAVIView::OnExtractBitplanewmarksequencealongtimeaxisattacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtract_BitplaneWatermarksequencealongtimeaxisattacked();
	Invalidate();
}

void CREADAVIView::OnOpenAttackedColorFile() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnOpenAttackedColorAVIFile();
//  pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
//  pDoc->FrameNumber_Watermarked=-1;//Ƕ��ˮӡ����Ƶ���и�λ
//	pDoc->FramePlay();
//	pDoc->FramePlay_Attacked();
    Invalidate();
}

void CREADAVIView::OnTimeAxisEmbeddingDualWatermarks() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnTimeDomainEmbeddingDualWatermarks();
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;
	pDoc->FramePlay();
	Invalidate();
}

void CREADAVIView::OnExtractionCurrframeAddedSSsequence() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrameAddedSSsequence();
	Invalidate();
}

void CREADAVIView::OnExtractionCurrframeAddedSSSequenceAttacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrameAddedSSsequence_Attacked();
	Invalidate();
}

void CREADAVIView::OnBlindExtractionCurrFrameAddedSSsequenceAttacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnBlindExtraction_CurrFrameAddedSSSequenceAttacked();
	Invalidate();
}

void CREADAVIView::OnAdaptiveDCTEmbedding() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);
	
	pDoc->OnAdaptiveDCTDomainEmbedding();
    pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;
    pDoc->FramePlay();
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
	Invalidate();

}

void CREADAVIView::OnExtractionCurrFrameDCTdomainAdaptive() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrame_DCTDomainAdaptive();
	Invalidate();
}

void CREADAVIView::OnExtractionCurrFrameDCTdomainAdaptiveAttacked() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnExtraction_CurrentFrame_DCTDomainAdaptive_Attacked();
	Invalidate();
}

void CREADAVIView::OnDewAlgorithm() 
{
	// TODO: Add your command handler code here
	CREADAVIDoc* pDoc = GetDocument();
 	ASSERT_VALID(pDoc);

    pDoc->OnDCT_DEWAlgorithm();
	pDoc->FrameNumber=-1;//ԭʼ��Ƶ���и�λ
	pDoc->m_button_reset=FALSE;
	pDoc->m_button_back=FALSE;
	pDoc->m_button_begin=FALSE;
	pDoc->m_button_play=TRUE;
	pDoc->m_button_forward=TRUE;
    pDoc->m_button_end=TRUE;
	pDoc->m_button_stop=FALSE;
	pDoc->m_extract_currentwatermark=TRUE;
    pDoc->FramePlay();
	WatermarkedFile_Opened=TRUE;//Ƕ��ˮӡ���ļ��ѱ���
	Invalidate();
}
