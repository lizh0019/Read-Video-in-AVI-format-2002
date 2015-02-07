// READ AVIDoc.h : interface of the CREADAVIDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_READAVIDOC_H__5B3FC3F4_D96A_4CE6_A7B4_0E65FA2BE2C2__INCLUDED_)
#define AFX_READAVIDOC_H__5B3FC3F4_D96A_4CE6_A7B4_0E65FA2BE2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FRAMESIZE 1244160//414720 720*576   304128 101376*3
#define PI 3.14159536

class CREADAVIDoc : public CDocument
{
protected: // create from serialization only
	CREADAVIDoc();
	DECLARE_DYNCREATE(CREADAVIDoc)

// Attributes
public:
    unsigned char FrameBytes[FRAMESIZE];
	unsigned char FrameBytes_Embedding[101376];
    unsigned char FrameBytes_Inserting[288][352];
    unsigned char FrameBytes_Attacked[101376];
	unsigned char FrameBytes_AttackedColor[304128];
    unsigned char FrameBytes_Difference[101376];
    unsigned char FrameBytes_Difference2[101376];
    unsigned char FrameBytes_ForWatermarkExtraction[101376];
	
	unsigned char FrameInfo[1064];
	unsigned char FrameInfoAttacked[1064];
	unsigned char FrameInfoAttackedColor[1064];

	unsigned char AVIFileInfo[4096];
	unsigned char FrameHeader[8];
	unsigned char m_Standard2BitBmpFileInfo[62];
	unsigned char m_Standard2BitBmpHeaderInfo[48];

	unsigned char m_Standard256GrayBmpFileInfo[1078];

    unsigned char PseudoRandomSequence[101376];
	unsigned char PseudoRandomSequenceBytes[12672];//101376/8=12672
    unsigned char Watermark[101376];//�������ԭʼˮӡ����
	unsigned char Watermark_Extracted[101376];//���������ȡ������ˮӡ
	unsigned char Watermark_Attacked[101376];//������ž���������ˮӡ
	unsigned char FrameBytes_Attacked_Watermark[101376];//������ȡ����������ˮӡ

	unsigned char FrameBlock_Shuffled[1584][8][8];

	unsigned char m_OriginalWatermarkBytes[12672+62];//�����洢�򿪵Ķ�ֵˮӡͼ��
	unsigned char m_ExtractedWatermarkBytes[12672];//�����洢��ȡ�Ķ�ֵˮӡͼ��
	unsigned char m_AttackedWatermarkBytes[12672];//�����洢���������Ķ�ֵˮӡͼ��
	unsigned char Watermark_Permuted[1024];
	unsigned char m_OriginalWatermarkInformationSequence[100];//�����洢66λ��ˮӡ��Ϣ����
    unsigned char m_ExtractedOriginalWatermarkInformationSequence[100];//�����洢66λ��ˮӡ��Ϣ����
	unsigned char m_ExtractedOriginalWatermarkInformationSequence_Attacked[100];//�����洢�����������66λˮӡ��Ϣ����
    unsigned char m_HammingCodedOriginalWatermarkInformationSequence[100];//�����洢�������������ˮӡ��Ϣ���� 66->90 (15,11)
	unsigned char m_ExtractedHammingCodedOriginalWatermarkInformationSequence[100];//�����洢�������������ˮӡ��Ϣ���� 66->90 (15,11)
	unsigned char m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[100];//�����洢�ܵ�������ľ������������ˮӡ��Ϣ���� 66->90 (15,11)

    short int m_SpreadSpectrumWatermarkSequence[30][30];//�����洢�������׺��ˮӡ���� 
    short int m_ModulatedSpreadSpectrumWatermarkSequence[30][30];//�����洢�������ƺ����Ƶˮӡ���� 

	short int m_TimeEmbeddingSpreadSpectrumWatermarkSequence[64][64];//�����洢�������׺��ˮӡ���� 
    short int m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[64][64];//�����洢�������ƺ����Ƶˮӡ���� 

	short int BitplanePositionSequence[512];//��Ƶ������ĳ���Ϊ300
    short int EmbeddingRegionSequence[150][1584];//��¼Ƕ��ˮӡ��λ�� 
    short int EmbeddingRegionSumSequence[300];//��¼Ƕ��ˮӡ�������������� 

	double PSNR[300];//�������ÿһ֡��PSNR
    double SNR[300];//�������ÿһ֡��SNR
    double PSNR_Attacked[300];//�������ÿһ֡��PSNR
	double MSE[300];//�������ÿһ֡��MSE
	double single_perceptible_distance[300];//�������ÿһ֡�ĸ�֪����
    double single_perceptible_distance_JND;//�������ÿһ֡�ĸ�֪����
	int RandomKey[16384];//������������õķ���
	int Flag_AdaptiveDCT;

    double NC_Value;//NCֵ
	double NC_Attacked_Value;//NCֵ

	int correct_bit_number;
	double correct_rate;
	int correct_bit_number_attacked;
	double correct_rate_attacked;

    char FileName[_MAX_PATH],FileTitle[_MAX_PATH];//The Filename and FileTitle 
	char FileName_Attacked[_MAX_PATH],FileTitle_Attacked[_MAX_PATH];//The Filename and FileTitle 
	char FileName_AttackedColor[_MAX_PATH],FileTitle_AttackedColor[_MAX_PATH];//The Filename and FileTitle 

	CString FilePath;
	int FileSize;
	int FrameNumber,FrameNumber_Watermarked,FrameNumber_Attacked,FrameNumber_AttackedColor;
	unsigned char TotalFrameNumberInfo[2];
	int TotalFrameNumber;//150֡��300֡
	unsigned char FrameColorInfo[2];//8��24
    unsigned char FrameSizeInfo[4];//101376��414720
	
	int PositionFlag;//ԭʼ��Ƶ��ʾλ��
	int PositionFlag_Watermark;//ˮӡ����ʾλ��
    int PositionFlag_Difference;//��Ӧ֡��ֵ����ʾλ��
    int PositionFlag_Difference2;//��Ӧ֡��ֵ����ʾλ��

    int PositionFlag_UserWatermark;//�û�ˮӡ����ʾλ��
    int PositionFlag_UserWatermark66bit;//�û�ˮӡ����ʾλ��
	int PositionFlag_Watermarked;//Ƕ��ˮӡ��Ƶ����ʾλ��
	int WatermarkFlag_Permuted;//ˮӡ���ҵı�־
	int CurrentWatermark_Extracted;//��ǰˮӡ��ȡ�ı�־

	int CurrentWatermarkSequence_Extracted;//��ǰˮӡ������ȡ�ı�־
    int CurrentSSWatermarkSequence_Extracted;//��ǰˮӡ������ȡ�ı�־
    int AttackedCurrentSSWatermarkSequence_Extracted;//����������ˮӡ��������ȡ��־

	int CurrentWatermarkSequenceAlongTimeAxis_Extracted;//��ǰˮӡ������ȡ�ı�־
    int AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted;//���������ĵ�ǰˮӡ������ȡ�ı�־

	int PositionFlag_AttackedVideo;//������������Ƶ��ʾλ��
	int PositionFlag_AttackedColorVideo;//������������Ƶ��ʾλ��
    int PositionFlag_AttackedWatermark;//����������ˮӡ��ʾλ��
	int PositionFlag_AttackedWatermarkSequence;//����������ˮӡ������ʾλ��


	BOOL m_button_play;//�����š���ť�Ŀ��Ʊ�־
	BOOL m_button_stop;//����ͣ����ť�Ŀ��Ʊ�־
    BOOL m_button_begin;//����һ֡����ť�Ŀ��Ʊ�־
    BOOL m_button_end;//�����һ֡����ť�Ŀ��Ʊ�־
    BOOL m_button_back;//��֡�ˡ���ť�Ŀ��Ʊ�־
	BOOL m_button_forward;//��֡������ť�Ŀ��Ʊ�־
	BOOL m_button_reset;//��ֹͣ����ť�Ŀ��Ʊ�־
    BOOL m_button_save_current_original_frame;//�����浱ǰ֡����ť�Ŀ��Ʊ�־
    BOOL m_timespatialembedding;//��ʱ������Ƕ�롱�˵��Ŀ��Ʊ�־
    BOOL m_pseudorandom_generator;//��α������С��˵��Ŀ��Ʊ�־
    BOOL m_binary_watermark;//����ֵˮӡ���˵��Ŀ��Ʊ�־
	BOOL m_watermark_permuted;//"α�������"�˵��Ŀ��Ʊ�־
	BOOL m_extract_currentwatermark;//"��ȡ��ǰ֡ˮӡ"�˵��Ŀ��Ʊ�־
	BOOL m_save_currentwatermark;//"���浱ǰ֡ˮӡ"�˵��Ŀ��Ʊ�־

	BOOL AttackedFile_Opened;//"������������Ƶ�ļ�"�Ƿ��Ѵ�
    BOOL AttackedColorFile_Opened;//"������������Ƶ�ļ�"�Ƿ��Ѵ�


// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CREADAVIDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void m_Sequence_Generator_fullnumber(short int *sequence, int number, int seed);
	void OnDCT_DEWAlgorithm();
	int * GoldenCodeGeneration(int p, int p1);
	void IDCT_Program(double input[][8],unsigned char output[][8],int N1,int N2);
	void OnExtraction_CurrentFrame_DCTDomainAdaptive_Attacked();
	void OnExtraction_CurrentFrame_DCTDomainAdaptive();
	void OnAdaptiveDCTDomainEmbedding();
	void OnBlindExtraction_CurrFrameAddedSSSequenceAttacked();
	void OnExtraction_CurrentFrameAddedSSsequence_Attacked();
	void OnExtraction_CurrentFrameAddedSSsequence();
	void OnTimeDomainEmbeddingDualWatermarks();
	void OnOpenAttackedColorAVIFile();
	void FramePlay_AttackedColor();
	void OnExtract_BitplaneWatermarksequencealongtimeaxisattacked();
	void OnExtract_BitplaneWatermarkSequenceAlongTimeAxis();
	double uniform( double a, double b, long int *seed);
	void OnExtraction_CurrentFrameSSsequence_Attacked();
	void OnExtraction_CurrentFrameSSsequence();
	void OnImagePredictionFilter3x3(short int a0,
		                            short int a11, short int a12, short int a13,
		                            short int a21, short int a22, short int a23,
									short int a31, short int a32, short int a33,
									short int offset);
	void OnTimeSpatialEmbedding_SpreadSpectrum();
	void OnSaveCurrentAttackedFrame();
	void OnExtract_Watermarksequencealongtimeaxisattacked();
	void OnExtract_WatermarkSequenceAlongTimeAxis();
	void OnTimeAxisWatermarkEmbedding();
	void OnImagePredictionFilter();
	void OnSave_CurrentOriginalFrame();
	void OnFrameDifferenceFunction();
	void OnImageEdgeDetection();
	void OnImageBlurFunction();
	void OnImageEdgeEnhancingFunction();
	void OnNoblindExtractGopWmarksequenceattacked();
	void OnNoBlindExtractCurrentWatermarkattacked();
	void OnNonBlindExtraction_CurrentWatermarkSequence();
	void DCT_Program(unsigned char OriginalImageBlock[][8],double DCTBlock[][8],int blockheight,int blockwidth);
	void OnExtract_GOPWatermarkSequenceAttacked();
	void OnSaveCurrentEmbeddedFrame();
	void OnExtract_CurrentWatermarksequenceattacked();
	void OnDetection_CurrentWatermarkSequence();
	void OnWatermark_Region_Detection();
	void IDCT(double inf2[8][8],unsigned char IDCTout[8][8]);
	void DCT(unsigned char inf1[8][8],double DCTout[8][8]);
	void OnTimeSpatialEmbedding_Improved();
	void OnSave_CurrentWatermark_Attacked();
	void OnExtractCurrentWatermarkattacked();
	void FramePlay_Attacked();
	void OnExtract_WatermarkAttacked();
	void OnOpenAttackedAVIFile();
	void OnSave_CurrentWatermark();
	void OnExtract_CurrentWatermark();
	void PseudoRandomSequence_Generator(unsigned char *sequence, int number, int seed);
	void OnTimeSpatialEmbedding_Second();
	void OnWatermarkRandom();
	void WatermarkPermuted(unsigned char *original, unsigned char *permuted, int *key, long int number);
	void FramePlay_Watermarked();
	void OnTimeSpatialEmbedding();
	void OnPseudoRandom();
	void FramePlay();
	virtual ~CREADAVIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CREADAVIDoc)
	afx_msg void OnEmbeddingReport();
	afx_msg void OnAttackedReport();
	afx_msg void OnSaveDifferenceimage();
	afx_msg void OnSavePseudorandomWatermark();
	afx_msg void OnCalculationJND();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READAVIDOC_H__5B3FC3F4_D96A_4CE6_A7B4_0E65FA2BE2C2__INCLUDED_)
