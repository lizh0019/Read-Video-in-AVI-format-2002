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
    unsigned char Watermark[101376];//用来存放原始水印数据
	unsigned char Watermark_Extracted[101376];//用来存放提取出来的水印
	unsigned char Watermark_Attacked[101376];//用来存放经过攻击的水印
	unsigned char FrameBytes_Attacked_Watermark[101376];//用来提取经过攻击的水印

	unsigned char FrameBlock_Shuffled[1584][8][8];

	unsigned char m_OriginalWatermarkBytes[12672+62];//用来存储打开的二值水印图象
	unsigned char m_ExtractedWatermarkBytes[12672];//用来存储提取的二值水印图象
	unsigned char m_AttackedWatermarkBytes[12672];//用来存储经过攻击的二值水印图象
	unsigned char Watermark_Permuted[1024];
	unsigned char m_OriginalWatermarkInformationSequence[100];//用来存储66位的水印信息序列
    unsigned char m_ExtractedOriginalWatermarkInformationSequence[100];//用来存储66位的水印信息序列
	unsigned char m_ExtractedOriginalWatermarkInformationSequence_Attacked[100];//用来存储经过攻击后的66位水印信息序列
    unsigned char m_HammingCodedOriginalWatermarkInformationSequence[100];//用来存储经过汉明编码的水印信息序列 66->90 (15,11)
	unsigned char m_ExtractedHammingCodedOriginalWatermarkInformationSequence[100];//用来存储经过汉明编码的水印信息序列 66->90 (15,11)
	unsigned char m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[100];//用来存储受到攻击后的经过汉明编码的水印信息序列 66->90 (15,11)

    short int m_SpreadSpectrumWatermarkSequence[30][30];//用来存储经过扩谱后的水印序列 
    short int m_ModulatedSpreadSpectrumWatermarkSequence[30][30];//用来存储经过调制后的扩频水印序列 

	short int m_TimeEmbeddingSpreadSpectrumWatermarkSequence[64][64];//用来存储经过扩谱后的水印序列 
    short int m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[64][64];//用来存储经过调制后的扩频水印序列 

	short int BitplanePositionSequence[512];//视频序列最长的长度为300
    short int EmbeddingRegionSequence[150][1584];//记录嵌入水印的位置 
    short int EmbeddingRegionSumSequence[300];//记录嵌入水印块数总数的序列 

	double PSNR[300];//用来存放每一帧的PSNR
    double SNR[300];//用来存放每一帧的SNR
    double PSNR_Attacked[300];//用来存放每一帧的PSNR
	double MSE[300];//用来存放每一帧的MSE
	double single_perceptible_distance[300];//用来存放每一帧的感知距离
    double single_perceptible_distance_JND;//用来存放每一帧的感知距离
	int RandomKey[16384];//随机置乱所采用的方法
	int Flag_AdaptiveDCT;

    double NC_Value;//NC值
	double NC_Attacked_Value;//NC值

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
	int TotalFrameNumber;//150帧或300帧
	unsigned char FrameColorInfo[2];//8或24
    unsigned char FrameSizeInfo[4];//101376或414720
	
	int PositionFlag;//原始视频显示位置
	int PositionFlag_Watermark;//水印的显示位置
    int PositionFlag_Difference;//对应帧差值的显示位置
    int PositionFlag_Difference2;//对应帧差值的显示位置

    int PositionFlag_UserWatermark;//用户水印的显示位置
    int PositionFlag_UserWatermark66bit;//用户水印的显示位置
	int PositionFlag_Watermarked;//嵌入水印视频的显示位置
	int WatermarkFlag_Permuted;//水印置乱的标志
	int CurrentWatermark_Extracted;//当前水印提取的标志

	int CurrentWatermarkSequence_Extracted;//当前水印序列提取的标志
    int CurrentSSWatermarkSequence_Extracted;//当前水印序列提取的标志
    int AttackedCurrentSSWatermarkSequence_Extracted;//经过攻击的水印序列其提取标志

	int CurrentWatermarkSequenceAlongTimeAxis_Extracted;//当前水印序列提取的标志
    int AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted;//经过攻击的当前水印序列提取的标志

	int PositionFlag_AttackedVideo;//经过攻击的视频显示位置
	int PositionFlag_AttackedColorVideo;//经过攻击的视频显示位置
    int PositionFlag_AttackedWatermark;//经过攻击的水印显示位置
	int PositionFlag_AttackedWatermarkSequence;//经过攻击的水印序列显示位置


	BOOL m_button_play;//“播放”按钮的控制标志
	BOOL m_button_stop;//“暂停”按钮的控制标志
    BOOL m_button_begin;//“第一帧”按钮的控制标志
    BOOL m_button_end;//“最后一帧”按钮的控制标志
    BOOL m_button_back;//“帧退”按钮的控制标志
	BOOL m_button_forward;//“帧进”按钮的控制标志
	BOOL m_button_reset;//“停止”按钮的控制标志
    BOOL m_button_save_current_original_frame;//“保存当前帧”按钮的控制标志
    BOOL m_timespatialembedding;//“时／空域嵌入”菜单的控制标志
    BOOL m_pseudorandom_generator;//“伪随机序列”菜单的控制标志
    BOOL m_binary_watermark;//“二值水印”菜单的控制标志
	BOOL m_watermark_permuted;//"伪随机处理"菜单的控制标志
	BOOL m_extract_currentwatermark;//"提取当前帧水印"菜单的控制标志
	BOOL m_save_currentwatermark;//"保存当前帧水印"菜单的控制标志

	BOOL AttackedFile_Opened;//"经过攻击的视频文件"是否已打开
    BOOL AttackedColorFile_Opened;//"经过攻击的视频文件"是否已打开


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
