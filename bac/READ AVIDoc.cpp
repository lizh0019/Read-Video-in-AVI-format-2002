// READ AVIDoc.cpp : implementation of the CREADAVIDoc class
//

#include "stdafx.h"
#include "READ AVI.h"
#include "READ AVIDoc.h"
#include "math.h"

#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "io.h"
#include "stdio.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CREADAVIDoc

IMPLEMENT_DYNCREATE(CREADAVIDoc, CDocument)

BEGIN_MESSAGE_MAP(CREADAVIDoc, CDocument)
	//{{AFX_MSG_MAP(CREADAVIDoc)
	ON_COMMAND(ID_EMBEDDING_REPORT, OnEmbeddingReport)
	ON_COMMAND(ID_ATTACKED_REPORT, OnAttackedReport)
	ON_COMMAND(ID_SAVE_DIFFERENCEIMAGE, OnSaveDifferenceimage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CREADAVIDoc construction/destruction

CREADAVIDoc::CREADAVIDoc()
{
	// TODO: add one-time construction code here
    
	PositionFlag=0;
	PositionFlag_Watermarked=0;
	PositionFlag_Watermark=0;
	PositionFlag_AttackedVideo=0;
	PositionFlag_AttackedWatermark=0;

    PositionFlag_UserWatermark=0;
    PositionFlag_UserWatermark66bit=0;
	PositionFlag_AttackedWatermarkSequence=0;

	WatermarkFlag_Permuted=0;
	CurrentWatermark_Extracted=0;

	CurrentWatermarkSequence_Extracted=0;
	CurrentSSWatermarkSequence_Extracted=0;
	AttackedCurrentSSWatermarkSequence_Extracted=0;
	CurrentWatermarkSequenceAlongTimeAxis_Extracted=0;
    AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=0;

	m_button_play=FALSE;
	m_button_stop=FALSE;
    m_button_begin=FALSE;
	m_button_end=FALSE;
    m_button_back=FALSE;
	m_button_forward=FALSE;
	m_button_reset=FALSE;
    m_button_save_current_original_frame=FALSE;
	m_timespatialembedding=FALSE;
	m_pseudorandom_generator=FALSE;
	m_binary_watermark=FALSE;
	m_watermark_permuted=FALSE;
	m_extract_currentwatermark=FALSE;
	m_save_currentwatermark=FALSE;
	AttackedFile_Opened=FALSE;

}

CREADAVIDoc::~CREADAVIDoc()
{
}

BOOL CREADAVIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CREADAVIDoc serialization

void CREADAVIDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
        CFile *fp; 
		fp=ar.GetFile();         //Get the file pointer	
		FilePath=fp->GetFilePath();
		FileSize=fp->GetLength();//Get the File Size
	    // Debug********************************************************		
		char temp[500];
	    sprintf(temp,"FileSize=%d",FileSize);
		MessageBox(NULL,temp,"文件大小",MB_OK);
	    //**************************************************************		
		
		// Open the AVI file  
        

		fp->Seek(48L,CFile::begin);
		fp->Read(TotalFrameNumberInfo,2);
		
		if (TotalFrameNumberInfo[0]==150)
			TotalFrameNumber=150;
		else 
		{  
			if ((TotalFrameNumberInfo[0]==44)&&(TotalFrameNumberInfo[1]==1))
		     TotalFrameNumber=300;
            else
			{  
				if (TotalFrameNumberInfo[0]==30)
				   TotalFrameNumber=30;
				else
				{
                  if ((TotalFrameNumberInfo[0]==4)&&(TotalFrameNumberInfo[1]==1))
				      TotalFrameNumber=260;
				  else
				  {
					  if (TotalFrameNumberInfo[0]==60)
			             TotalFrameNumber=60;
					  else
					  {
						  //考虑220帧的情况
                          if (TotalFrameNumberInfo[0]==220)
			                  TotalFrameNumber=220;
					      else
						  {
						      MessageBox(NULL,"您打开了一个不符合本软件规范的无效文件!","文件打开",MB_OK|MB_ICONSTOP);
					          return;
						  }
					  }
				  }
				}
			}
		}

        //读取每一帧的大小
        fp->Seek(64L,CFile::begin);
		fp->Read(FrameSizeInfo,4);

        fp->Seek(186L,CFile::begin);
		fp->Read(FrameColorInfo,2);

		if ((FrameSizeInfo[0]==96)&&(FrameSizeInfo[1]==1)
			&&(FrameSizeInfo[2]==0)&&(FrameSizeInfo[3]==0))//352*288=101376 十六进制HEX 18C00
		{
		 if (FrameColorInfo[0]==8)//灰度
		 {
		   fp->SeekToBegin();
		   fp->Read(AVIFileInfo,4096);
		   fp->Read(FrameHeader,8);
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,1064);
		   fp->Seek(4104L,CFile::begin);
		   fp->Read(FrameBytes,101376);
		}
		else
		{
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,40);
		   fp->Seek(2056L,CFile::begin);
		   fp->Read(FrameBytes,304128);
		}
		}
	
		if ((FrameSizeInfo[0]==208)&&(FrameSizeInfo[1]==2)
			&&(FrameSizeInfo[2]==0)&&(FrameSizeInfo[3]==0))//720*576=414720 十六进制HEX 65400
		{
         if (FrameColorInfo[0]==8)//灰度
		 {
		   fp->SeekToBegin();
		   fp->Read(AVIFileInfo,4096);
		   fp->Read(FrameHeader,8);
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,1064);
		   fp->Seek(4104L,CFile::begin);
		   fp->Read(FrameBytes,414720);
		}
		else//彩色
		{
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,40);
		   fp->Seek(2056L,CFile::begin);
		   fp->Read(FrameBytes,1244160);
		}


		}

		FrameNumber=0;
		PositionFlag=1;//显示位置 1:原始序列
		PositionFlag_Watermark=0;//不显示水印
		PositionFlag_Watermarked=0;//不显示嵌入水印的视频
		m_button_play=TRUE;//“播放”按钮使能
		m_button_begin=FALSE;//“第一帧”按钮关闭
        m_button_end=TRUE;//“最后一帧”按钮关闭
		m_button_forward=TRUE;//“帧进”按钮使能
		m_button_reset=FALSE;//“停止”按钮关闭
		m_button_back=FALSE;//“帧退”按钮关闭
		m_button_reset=FALSE;//“复位”按钮关闭
		m_button_save_current_original_frame=TRUE;//“保存当前帧”按钮使能
        m_pseudorandom_generator=TRUE;//“伪随机序列”菜单使能
		m_binary_watermark=TRUE;//“伪随机序列”菜单使能
		m_timespatialembedding=FALSE;//屏蔽“时／空域嵌入”菜单
	}
}

/////////////////////////////////////////////////////////////////////////////
// CREADAVIDoc diagnostics

#ifdef _DEBUG
void CREADAVIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CREADAVIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CREADAVIDoc commands

void CREADAVIDoc::FramePlay()
{
	   int i;
	   FrameNumber=FrameNumber+1;
	   CFile ThisFile;
	   ThisFile.Open(FilePath,CFile::modeRead);
	   
	   if (FrameSizeInfo[0]==96)//352*288
	   {
		 if (FrameColorInfo[0]==8)//灰度
		 {
		   ThisFile.Seek(4104L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(101384L,CFile::current);//每一帧的数据长度（8＋101376）bytes
		   }
           ThisFile.Read(FrameBytes,101376);
		 }
	     else
		 {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           ThisFile.Read(FrameBytes,304128);
		 }
	   }

	   if (FrameSizeInfo[0]==208)//720*576
	   {
		 if (FrameColorInfo[0]==8)//灰度
		 {
		   ThisFile.Seek(4104L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(414728L,CFile::current);//每一帧的数据长度（8＋101376）bytes
		   }
           ThisFile.Read(FrameBytes,414720);
		 }
	     else//彩色
		 {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(1244168L,CFile::current);//每一帧的数据长度（8＋1244160）bytes
		   }
           ThisFile.Read(FrameBytes,1244160);
		 }
	   }


	   ThisFile.Close();

}

void CREADAVIDoc::OnPseudoRandom()
{
  int bit[32];
  int i,j,scale,temp;
  int tempkey;
 

  int number=65536*2;
  if (number<4||number>(65536*2)) return;

    //取得m序列的阶数
	scale=0; //scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
    temp=number;
    while(temp!=1)//求阶数
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
   tempkey=15;//置随机序列的种子

   for(i=0;i<101376;i++) //周期为352*288=101376,产生101376个伪随机值（种子给定）
	{
      for(j=0;j<scale;j++) //取各位0/1值-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//作模2加法
	  {
	    case 2:
		  temp=bit[0]+bit[1];
		break;
		case 3:
          temp=bit[0]+bit[2];
		break;
		case 4:
		  temp=bit[0]+bit[3];
		break;
		case 5:
		  temp=bit[0]+bit[3];
		break;
		case 6:
		  temp=bit[0]+bit[5];
		break;
		case 7:
		  temp=bit[0]+bit[4];
		break;
		case 8:
		  temp=bit[0]+bit[4]+bit[5]+bit[6];
		break;
		case 9:
		  temp=bit[0]+bit[5];
		break;
		case 10:
		  temp=bit[0]+bit[7];
		break;
		case 11:
		  temp=bit[0]+bit[9];
		break;
		case 12:
		  temp=bit[0]+bit[6]+bit[8]+bit[11];
		break;
		case 13:
		  temp=bit[0]+bit[9]+bit[10]+bit[12];
		break;
		case 14:
          temp=bit[0]+bit[4]+bit[8]+bit[13];
	    break;
		case 15:
		  temp=bit[0]+bit[14];
		break;
		case 16:
		  temp=bit[0]+bit[4]+bit[13]+bit[15];
		break;
		case 17:
		  temp=bit[0]+bit[14];
		break;
		default:
		break;
	  }
	  bit[scale]=temp&1;
      tempkey=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey>>1));
	  PseudoRandomSequence[i]=tempkey&1;
  }
   
    for (i=0;i<12672;i++)
	{
		PseudoRandomSequenceBytes[i]=0;//将数组清空
	    for (j=0;j<8;j++)
		{
           if (PseudoRandomSequence[i*8+j]==1)
           PseudoRandomSequenceBytes[i]=(unsigned char)pow(2,(7-j))+PseudoRandomSequenceBytes[i];
		}
	}
     
   	long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\2BitBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"标准[2BitBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard2BitBmpFileInfo,62)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"读取标准[2BitBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	}
    _close(fh);  

	for (i=0;i<48;i++)
      m_Standard2BitBmpHeaderInfo[i]=m_Standard2BitBmpFileInfo[i+14];  

 
}

void CREADAVIDoc::OnTimeSpatialEmbedding()
{
   //Save the watermarked avi file
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName[0]='\0';
   FileTitle[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI文件 *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存AVI文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.avi文件");
	   strcpy(FileName,temp0);
	   strcat(FileName,"\\temp.avi");
	   strcpy(FileTitle,"temp.avi");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }





  int bit[12];
  int i,j,k,scale,temp;
  int tempkey;
  int flag_0=0,flag_1=0,flag_2=0,flag_3=0;
 

  int number=512;
  if (number<4||number>512) return;

  //取得m序列的阶数
  scale=0; //scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
  temp=number;
  while(temp!=1)//求阶数
  {    
	temp=temp/2;
    scale=scale+1; 
  }
	
  tempkey=9;//置随机序列的种子

  for (i=0;i<TotalFrameNumber;i++)
    BitplanePositionSequence[i]=0;

  for(k=0;k<3;k++)
  {
   for(i=0;i<TotalFrameNumber;i++) //随机序列的长度为视频的帧数，目的是为了控制嵌入的位置
	{
      for(j=0;j<scale;j++) //取各位0/1值-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//作模2加法
	  {
	    case 2:
		  temp=bit[0]+bit[1];
		break;
		case 3:
          temp=bit[0]+bit[2];
		break;
		case 4:
		  temp=bit[0]+bit[3];
		break;
		case 5:
		  temp=bit[0]+bit[3];
		break;
		case 6:
		  temp=bit[0]+bit[5];
		break;
		case 7:
		  temp=bit[0]+bit[4];
		break;
		case 8:
		  temp=bit[0]+bit[4]+bit[5]+bit[6];
		break;
		case 9:
		  temp=bit[0]+bit[5];
		break;
		case 10:
		  temp=bit[0]+bit[7];
		break;
		case 11:
		  temp=bit[0]+bit[9];
		break;
		case 12:
		  temp=bit[0]+bit[6]+bit[8]+bit[11];
		break;
		default:
		break;
	  }
	  bit[scale]=temp&1;
      tempkey=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey>>1));
	  BitplanePositionSequence[i]=tempkey&1+BitplanePositionSequence[i];
   }

 }

  for (i=0;i<TotalFrameNumber;i++)
  {
	if ((BitplanePositionSequence[i]!=0)&&(BitplanePositionSequence[i]!=1)&&(BitplanePositionSequence[i]!=2)&&(BitplanePositionSequence[i]!=3))
    {   
	   AfxMessageBox("Error!");
	   return;
    }
  }


   
 
    FILE *fp;
    fp=fopen(FileName,"wb");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("File");
		return ;
	}
	 
	fwrite(AVIFileInfo,4096,1,fp);         

//	int i,j,k; 
	int m_FrameByte;//用来暂存原始数据
	double DifferenceSum;//象素差

	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);

	if (FrameColorInfo[0]==8)//灰度
	{
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             DifferenceSum=0.0;//针对每一帧都清零 
	         OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);
		     if (BitplanePositionSequence[k]==0)
			 {
				flag_0=flag_0+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {   
					     m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				
                         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			 }

            if (BitplanePositionSequence[k]==1)
			{
				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                           FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}

			 if (BitplanePositionSequence[k]==2)
			 {
                flag_2=flag_2+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   	         FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				    DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			 }

			 if (BitplanePositionSequence[k]==3)
			 {
				flag_3=flag_3+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		                   FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				
				    DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			 }

		
             
			fwrite(FrameHeader,8,1,fp);
			fwrite(FrameBytes_Embedding,101376,1,fp);
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);
			
//			sprintf(temp0,"%d",k);
//	        MessageBox(NULL,temp0,"",MB_OK);
		  }
          
           
 

		  //AVI文件后面的INDEX部分
/*		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//利用FrameBytes_Embedding数组
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }
   
*/		    
            

		  	char temp_flag[80];
			sprintf(temp_flag,"最低位平面： %d",flag_0);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"第一位平面： %d",flag_1);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"第二位平面： %d",flag_2);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"第三位平面： %d",flag_3);
            MessageBox(NULL,temp_flag,"",MB_OK);
              
            
		
           
            fclose(fp);
	        sprintf(temp0,"AVI文件已成功保存");
	        MessageBox(NULL,temp0,"",MB_OK);

            

/////////////////Save the avi file ends//////////

	   }

  /*	   else //彩色
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//嵌入水印的序列对应的位置
       FrameNumber_Watermarked=0;//首先将文件的位置初始化
	   
	  
}

void CREADAVIDoc::FramePlay_Watermarked()
{
       int i;
	   FrameNumber_Watermarked=FrameNumber_Watermarked+1;

	   FILE *fp;
       fp=fopen(FileName,"rb");
	   if(fp==NULL)
	   {
	    MessageBeep(-1);
		AfxMessageBox("File");
		return ;
	   }

	    if (FrameColorInfo[0]==8)//灰度
	   {
		   fseek(fp,4104L,0);

		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,101384L,1);//每一帧的数据长度（8+101376）bytes
		   }
           fread(FrameBytes_Embedding,101376,1,fp);
	   }
	   else//彩色
	   {   
/*		   fseek(fp,2056L,0);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,304136L,1);//每一帧的数据长度（8＋304128）bytes
		   }
           fread(FrameBytes,304128,1,fp);   */
	   }   
	   fclose(fp);  


/*	   CFile ThisFile;
	   ThisFile.Open(FilePath_Watermarked,CFile::modeRead);
		 
	   
	   
	   if (FrameColorInfo[0]==8)//灰度
	   {
		   ThisFile.Seek(4104L,CFile::begin);

		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         ThisFile.Seek(101384L,CFile::current);//每一帧的数据长度（8+101376）bytes
		   }
           ThisFile.Read(FrameBytes_Embedding,101376);
	   }
	   else
	   {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         ThisFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           ThisFile.Read(FrameBytes,304128);
	   }   
	   ThisFile.Close();  */
}

void CREADAVIDoc::WatermarkPermuted(unsigned char *original, unsigned char *permuted, int *key, long number)
{
    int bit[32];
    int i,j,a,b,scale,temp;
    int tempkey[65536];
    double tt;
    if(number<4||number>65536) return;
    for(i=0;i<number;i++)
  	key[i]=i;
  
    //取得m序列的阶数
	scale=0;//scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
    temp=number;
    while(temp!=1)//求阶数
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
	//产生一个[0,2^scale-1]之间的随机数a  2^scale=number
	srand((unsigned)time(NULL));
    tt=1.0/32767.0*(number-1);
	a=(long int)((double)rand()*tt);
	key[a]=0;//令该地址的数为0
	
	b=0;
	while(b==0) b=(long int)((double)rand()*tt);

//	tempkey[0]=b; //设m序列初值为1到number-1的随机数
    
	tempkey[0]=9; //设m序列初值为1到number-1的随机数

    for(i=0;i<number-1;i++)//周期为number-1,产生从1到number-1的伪随机值
	{
      for(j=0;j<scale;j++)//取各位0/1值-->bit[i]
	  {
        temp=tempkey[i]>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//作模2加法
	  {
	    case 2:
		  temp=bit[0]+bit[1];
		break;
		case 3:
          temp=bit[0]+bit[2];
		break;
		case 4:
		  temp=bit[0]+bit[3];
		break;
		case 5:
		  temp=bit[0]+bit[3];
		break;
		case 6:
		  temp=bit[0]+bit[5];
		break;
		case 7:
		  temp=bit[0]+bit[4];
		break;
		case 8:
		  temp=bit[0]+bit[4]+bit[5]+bit[6];
		break;
		case 9:
		  temp=bit[0]+bit[5];
		break;
		case 10:
		  temp=bit[0]+bit[7];
		break;
		case 11:
		  temp=bit[0]+bit[9];
		break;
		case 12:
		  temp=bit[0]+bit[6]+bit[8]+bit[11];
		break;
		case 13:
		  temp=bit[0]+bit[9]+bit[10]+bit[12];
		break;
		case 14:
          temp=bit[0]+bit[4]+bit[8]+bit[13];
	    break;
		case 15:
		  temp=bit[0]+bit[14];
		break;
		case 16:
		  temp=bit[0]+bit[4]+bit[13]+bit[15];
		break;
		default:
		break;
	  }
	  bit[scale]=temp&1;
      tempkey[i]=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey[i]>>1));
      tempkey[i+1]=tempkey[i];
	}

    for(i=0;i<number;i++)
	{
      if(i<a)
	    key[i]=tempkey[i];
	  if(i>a)
	    key[i]=tempkey[i-1];
	}
  

  //Generate ordinal sequence
  for(i=0; i<number; i++)
  {
 	j=key[i];
	permuted[i]=original[j];
  }


}

void CREADAVIDoc::OnWatermarkRandom()
{
     int i,j;
	  
	 for (i=0;i<(32*32/8);i++)
	    for (j=0;j<8;j++)
		  PseudoRandomSequence[i*8+7-j]=((m_OriginalWatermarkBytes[i+62]>>j)&1);//注意是从最低位取走的,以及保存的方式
		 
	 WatermarkPermuted(PseudoRandomSequence,PseudoRandomSequenceBytes,RandomKey,1024);//保存的是打乱后的水印

	 for (i=0;i<(32*32/8);i++)
	 {
		Watermark_Permuted[i]=0;//将数组清空
	    for (j=0;j<8;j++)
		{
           if ((PseudoRandomSequenceBytes[i*8+j]!=1)&&(PseudoRandomSequenceBytes[i*8+j]!=0))//打乱后的水印 
		   {
			   AfxMessageBox("Watermark Permuted Error!");
               return;
		   }
           if (PseudoRandomSequenceBytes[i*8+j]==1)//打乱后的水印
              Watermark_Permuted[i]=(unsigned char)pow(2,(7-j))+Watermark_Permuted[i];
		}
	 }

	 WatermarkFlag_Permuted=1;
     
/*  long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\2BitBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"标准[2BitBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard2BitBmpFileInfo,62)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"读取标准[2BitBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	}
    _close(fh);  

	for (i=0;i<48;i++)
      m_Standard2BitBmpHeaderInfo[i]=m_Standard2BitBmpFileInfo[i+14];  
     
*/
}

void CREADAVIDoc::OnEmbeddingReport() 
{
	// TODO: Add your command handler code here
    
   //Save the embedding report
   char FileName_TXT[_MAX_PATH],FileTitle_TXT[_MAX_PATH];//The Filename and FileTitle
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName_TXT[0]='\0';
   FileTitle_TXT[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("TXT Files (*.txt)\0*.txt\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'TXT文件 *.txt'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_TXT; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_TXT;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存测试报告";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="TXT";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.txt文件");
	   strcpy(FileName_TXT,temp0);
	   strcat(FileName_TXT,"\\temp.txt");
	   strcpy(FileTitle_TXT,"temp.txt");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_TXT,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_TXT)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_TXT);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_TXT,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

    FILE *fp;
    fp=fopen(FileName_TXT,"w");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Open File Error");
		return ;
	}
	fprintf(fp,"嵌入水印的视频序列所对应的PSNR");

	int i;
	for (i=0;i<TotalFrameNumber;i++)
       fprintf(fp,"\n第 %3d 帧  PSNR：  %f  dB       SNR:    %f  dB",i,PSNR[i],SNR[i]);
	fclose(fp);







	
}

void CREADAVIDoc::OnTimeSpatialEmbedding_Second()
{
 //Save the watermarked avi file
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName[0]='\0';
   FileTitle[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI文件 *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存AVI文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.avi文件");
	   strcpy(FileName,temp0);
	   strcat(FileName,"\\temp.avi");
	   strcpy(FileTitle,"temp.avi");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

  int bit[32];
  int i,j,k,scale,temp;
  int tempkey;
  int flag_0=0,flag_1=0,flag_2=0,flag_3=0;

//******************************************************
//将二值水印转换成字节形式
  
  for (i=0;i<12672;i++)
	for (j=0;j<8;j++)
	  {
		 Watermark[i*8+7-j]=(m_OriginalWatermarkBytes[i+62]>>j)&1;
         if ((Watermark[i*8+7-j]!=0)&&(Watermark[i*8+7-j]!=1))
			 AfxMessageBox("Watermark Message Error!");
	  }




//******************************************************
//控制嵌入位平面的位置
  int number=512;
  if (number<4||number>512) return;

  //取得m序列的阶数
  scale=0; //scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
  temp=number;
  while(temp!=1)//求阶数
  {    
	temp=temp/2;
    scale=scale+1; 
  }
	
  tempkey=11;//置随机序列的种子

  for (i=0;i<TotalFrameNumber;i++)
  {
//   BitplanePositionSequence[i]=3; 
     BitplanePositionSequence[i]=2;
  }

 for(k=0;k<1;k++)
  {
   for(i=0;i<TotalFrameNumber;i++) //随机序列的长度为视频的帧数，目的是为了控制嵌入的位置
	{
      for(j=0;j<scale;j++) //取各位0/1值-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//作模2加法
	  {
	    case 2:
		  temp=bit[0]+bit[1];
		break;
		case 3:
          temp=bit[0]+bit[2];
		break;
		case 4:
		  temp=bit[0]+bit[3];
		break;
		case 5:
		  temp=bit[0]+bit[3];
		break;
		case 6:
		  temp=bit[0]+bit[5];
		break;
		case 7:
		  temp=bit[0]+bit[4];
		break;
		case 8:
		  temp=bit[0]+bit[4]+bit[5]+bit[6];
		break;
		case 9:
		  temp=bit[0]+bit[5];
		break;
		case 10:
		  temp=bit[0]+bit[7];
		break;
		case 11:
		  temp=bit[0]+bit[9];
		break;
		case 12:
		  temp=bit[0]+bit[6]+bit[8]+bit[11];
		break;
		default:
		break;
	  }
	  bit[scale]=temp&1;
      tempkey=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey>>1));
	  BitplanePositionSequence[i]=(tempkey&1)+BitplanePositionSequence[i];
   }

 }

  for (i=0;i<TotalFrameNumber;i++)
  {
	if ((BitplanePositionSequence[i]!=0)&&(BitplanePositionSequence[i]!=1)&&(BitplanePositionSequence[i]!=2)&&(BitplanePositionSequence[i]!=3))
    {   
	   AfxMessageBox("Bitplane Position Error!");
	   return;
    }
  }



//*******************************************************

    FILE *fp;
    fp=fopen(FileName,"wb");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("File");
		return ;
	}
	 
	fwrite(AVIFileInfo,4096,1,fp);         

	int m_FrameByte;//用来暂存原始数据
	double DifferenceSum;//象素差
	int temp_conversion;
//	unsigned char temp_line[352];//用来将图像数据倒过来
//	unsigned char Frame_Block[1584][8][8];//分块后的图像数据

	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);

	if (FrameColorInfo[0]==8)//灰度
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             DifferenceSum=0.0;//针对每一帧都清零 
	         OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);
			
             //******************************************************
			 //对水印进行扩频
            
              number=65536*2;
              if (number<4||number>(65536*2)) return;

              //取得m序列的阶数
	          scale=0; //scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
              temp=number;
              while(temp!=1)//求阶数
			  {    
	             temp=temp/2;
                 scale=scale+1; 
			  }
	
              tempkey=15;//置随机序列的种子

              for(i=0;i<101376;i++) //周期为352*288=101376,产生101376个伪随机值（种子给定）
			  {
                  for(j=0;j<scale;j++) //取各位0/1值-->bit[i]
				  {
                        temp=tempkey>>j;
	                    bit[j]=temp&1;
				  }
	              switch(scale)//作模2加法
				  {
	               case 2:
	              	  temp=bit[0]+bit[1];
            	      break;
		           case 3:
                      temp=bit[0]+bit[2];
		              break;
		           case 4:
		              temp=bit[0]+bit[3];
		              break;
		           case 5:
		              temp=bit[0]+bit[3];
		              break;
		           case 6:
		              temp=bit[0]+bit[5];
		              break;
		           case 7:
		              temp=bit[0]+bit[4];
		              break;
		           case 8:
		              temp=bit[0]+bit[4]+bit[5]+bit[6];
		              break;
	               case 9:
		              temp=bit[0]+bit[5];
		              break;
		           case 10:
		              temp=bit[0]+bit[7];
		              break;
		           case 11:
		              temp=bit[0]+bit[9];
		              break;
		           case 12:
		              temp=bit[0]+bit[6]+bit[8]+bit[11];
		              break;
		           case 13:
		              temp=bit[0]+bit[9]+bit[10]+bit[12];
		              break;
		           case 14:
                      temp=bit[0]+bit[4]+bit[8]+bit[13];
	                  break;
		           case 15:
		              temp=bit[0]+bit[14];
		              break;
		           case 16:
		              temp=bit[0]+bit[4]+bit[13]+bit[15];
		              break;
		           case 17:
		              temp=bit[0]+bit[14];
		              break;
		           default:
		              break;
				  }
	             bit[scale]=temp&1;
                 tempkey=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey>>1));
	             PseudoRandomSequence[i]=tempkey&1;
				 if ((PseudoRandomSequence[i]!=0)&&(PseudoRandomSequence[i]!=1))
                    AfxMessageBox("Pseudo Random Sequence Error!");
			}          

           for (i=0;i<288;i++)
               for (j=0;j<352;j++)
				 { 
					if (Watermark[i*352+j]==1)
						  temp_conversion=-1;//1 -> -1
					else
					      temp_conversion=1;//0 -> 1
                    if (PseudoRandomSequence[i*352+j]==1)
						  temp_conversion=temp_conversion*(-1);
					else
					      temp_conversion=temp_conversion*1;
                    if (temp_conversion==-1)
						  PseudoRandomSequence[i*352+j]=1;
					else
                          PseudoRandomSequence[i*352+j]=0;
				}


		     if (BitplanePositionSequence[k]==0)
			 {
				flag_0=flag_0+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {   
					     m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   		    FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);    //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				
                         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 } 
			 }	 


            if (BitplanePositionSequence[k]==1)
			{
				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                             FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}

			 if (BitplanePositionSequence[k]==2)
			 {
                flag_2=flag_2+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   	         FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				    DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			 }

			 if (BitplanePositionSequence[k]==3)
			 {
				flag_3=flag_3+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		                     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   // 		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				
				    DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			 }

			if (BitplanePositionSequence[k]==4)
			{
//				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   		     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //	 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}


			if (BitplanePositionSequence[k]==5)
			{
//				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}

            if (BitplanePositionSequence[k]==6)
			{
//				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}


            if (BitplanePositionSequence[k]==7)
			{
//				flag_1=flag_1+1;
				for (i=0;i<288;i++)
               	 for (j=0;j<352;j++)
				 {
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//暂存

                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&254)+(PseudoRandomSequence[i*352+j]*1);   //1111 1110
                   //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&253)+(PseudoRandomSequence[i*352+j]*2);   //1111 1101
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&251)+(PseudoRandomSequence[i*352+j]*4);   //1111 1011
		           //        FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&247)+(PseudoRandomSequence[i*352+j]*8);   //1111 0111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&239)+(PseudoRandomSequence[i*352+j]*16);  //1110 1111
                   //		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&223)+(PseudoRandomSequence[i*352+j]*32);  //1101 1111		
                   //	     FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&191)+(PseudoRandomSequence[i*352+j]*64);  //1011 1111
                    		 FrameBytes_Embedding[i*352+j]=(FrameBytes_Embedding[i*352+j]&127)+(PseudoRandomSequence[i*352+j]*128); //0111 1111
				 
				         DifferenceSum=DifferenceSum+(m_FrameByte-FrameBytes_Embedding[i*352+j])*(m_FrameByte-FrameBytes_Embedding[i*352+j]);
				 }
			}

   
			fwrite(FrameHeader,8,1,fp);
			fwrite(FrameBytes_Embedding,101376,1,fp);
			
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);
		  }
          


		  //AVI文件后面的INDEX部分
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  else
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//利用FrameBytes_Embedding数组
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  	char temp_flag[10];
			sprintf(temp_flag,"%d",flag_0);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"%d",flag_1);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"%d",flag_2);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"%d",flag_3);
            MessageBox(NULL,temp_flag,"",MB_OK);
 
            fclose(fp);
	        sprintf(temp0,"AVI文件已成功保存");
	        MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //彩色
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/
        
       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//嵌入水印的序列对应的位置
       FrameNumber_Watermarked=0;//首先将文件的位置初始化
	  
}

void CREADAVIDoc::PseudoRandomSequence_Generator(unsigned char *sequence, int number, int seed)
{
  int bit[32];
  int i,j,scale,temp;
  int tempkey;
 

//  int number=65536*2;
  if (number<4||number>(65536*2)) return;

    //取得m序列的阶数
	scale=0;//scale级反馈移位寄存器--用于产生长为2^scale-1的m序列     
    temp=number;
    while(temp!=1)//求阶数
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
   tempkey=seed;//置随机序列的种子

   for (i=0;i<101376;i++) //周期为352*288=101376,产生101376个伪随机值（种子给定）
	{
      for(j=0;j<scale;j++) //取各位0/1值-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//作模2加法
	  {
	    case 2:
		  temp=bit[0]+bit[1];
		break;
		case 3:
          temp=bit[0]+bit[2];
		break;
		case 4:
		  temp=bit[0]+bit[3];
		break;
		case 5:
		  temp=bit[0]+bit[3];
		break;
		case 6:
		  temp=bit[0]+bit[5];
		break;
		case 7:
		  temp=bit[0]+bit[4];
		break;
		case 8:
		  temp=bit[0]+bit[4]+bit[5]+bit[6];
		break;
		case 9:
		  temp=bit[0]+bit[5];
		break;
		case 10:
		  temp=bit[0]+bit[7];
		break;
		case 11:
		  temp=bit[0]+bit[9];
		break;
		case 12:
		  temp=bit[0]+bit[6]+bit[8]+bit[11];
		break;
		case 13:
		  temp=bit[0]+bit[9]+bit[10]+bit[12];
		break;
		case 14:
          temp=bit[0]+bit[4]+bit[8]+bit[13];
	    break;
		case 15:
		  temp=bit[0]+bit[14];
		break;
		case 16:
		  temp=bit[0]+bit[4]+bit[13]+bit[15];
		break;
		case 17:
		  temp=bit[0]+bit[14];
		break;
		default:
		break;
	  }
	  bit[scale]=temp&1;
      tempkey=(int)(bit[scale]*(pow(2,(scale-1)))+(tempkey>>1));
	  sequence[i]=tempkey&1;
  }
}

void CREADAVIDoc::OnExtract_CurrentWatermark()
{

//  CFile WatermarkedFile;
//	WatermarkedFile.Open(FileName,CFile::modeRead);
    int i,j,temp_conversion;
	PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//65536*2
	
	
		if (BitplanePositionSequence[FrameNumber]==0)
		{
			for (i=0;i<101376;i++)
			{
			   if ((FrameBytes_Embedding[i]&1)==1)    //1111 1110
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}
		if (BitplanePositionSequence[FrameNumber]==1)
		{
            for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&2)>>1)==1)    //1111 1101
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}
       if (BitplanePositionSequence[FrameNumber]==2)
	   {
		   for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&4)>>2)==1)    //1111 1011
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
	   }
        if (BitplanePositionSequence[FrameNumber]==3)
		{
		    for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&8)>>3)==1)    //1111 0111
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}   

		if (BitplanePositionSequence[FrameNumber]==4)
		{
		    for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&16)>>4)==1)    //1101 1111
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}   


		if (BitplanePositionSequence[FrameNumber]==5)
		{
		    for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&32)>>5)==1)    //1101 1111
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}   

       
        if (BitplanePositionSequence[FrameNumber]==6)
		{
		    for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&64)>>6)==1)    //1011 1111
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}   


		if (BitplanePositionSequence[FrameNumber]==7)
		{
		    for (i=0;i<101376;i++)
			{
			   if (((FrameBytes_Embedding[i]&128)>>7)==1)    //0111 1111
				   temp_conversion=-1; //1 -> -1
			   else
				   temp_conversion=1;  //0 -> 1
               if  (PseudoRandomSequence[i]==1)
				   temp_conversion=temp_conversion*(-1);
			   else
                   temp_conversion=temp_conversion*1;
			   if (temp_conversion==1)
				   Watermark_Extracted[i]=0;
			   else
                   Watermark_Extracted[i]=1;
			}
		}   



		//byte -> bit 
		for (i=0;i<12672;i++)
		{
			m_ExtractedWatermarkBytes[i]=0;//将数组清空
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}

		//计算NC值
		int watermark_sum_up=0;
		double watermark_sum_down=101376.0;
		int watermark_temp;
		for (i=0;i<101376;i++)
		{
			if (Watermark[i]==1)
				watermark_temp=-1;
			if (Watermark[i]==0)
				watermark_temp=1;
			if (Watermark_Extracted[i]==1)
				watermark_sum_up=watermark_temp*(-1)+watermark_sum_up;
			if ((Watermark_Extracted[i]==0))
				watermark_sum_up=watermark_temp*1+watermark_sum_up;
		}
		NC_Value=watermark_sum_up/101376.0;
    
	    CurrentWatermark_Extracted=1;
		m_save_currentwatermark=TRUE;

}

void CREADAVIDoc::OnSave_CurrentWatermark()
{
  //Save the watermark file
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_Watermark[_MAX_PATH],FileTitle_Watermark[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_Watermark[0]='\0';
   FileTitle_Watermark[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_Watermark; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_Watermark;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.bmp文件");
	   strcpy(FileName_Watermark,temp0);
	   strcat(FileName_Watermark,"\\temp.bmp");
	   strcpy(FileTitle_Watermark,"temp.avi");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_Watermark,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_Watermark)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_Watermark);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_Watermark,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_Watermark,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Watermark File Error");
	  return;
   }
	 
   fwrite(m_OriginalWatermarkBytes,62,1,fp); 
   fwrite(m_ExtractedWatermarkBytes,12672,1,fp);

   fclose(fp);
}

void CREADAVIDoc::OnOpenAttackedAVIFile()
{
   OPENFILENAME ofn;
  
// char FileName_Attacked[_MAX_PATH],FileTitle_Attacked[_MAX_PATH];//The Filename and FileTitle 
  
   FileName_Attacked[0]='\0';  //NULL string
   FileTitle_Attacked[0]='\0'; //NULL string


   ofn.lStructSize=sizeof(OPENFILENAME);
   ofn.hwndOwner=NULL;
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI文件 *.avi\0*.avi\0All File *.*\0*.*\0\0"); 
   ofn.lpstrCustomFilter=NULL; 
   ofn.nMaxCustFilter=0; 
   
   ofn.nFilterIndex=1; //Select the item 'AVI文件 *.avi'
   ofn.lpstrFile=FileName_Attacked; 
   ofn.nMaxFile=_MAX_PATH; 
   
   ofn.lpstrFileTitle=FileTitle_Attacked; 
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;
   ofn.lpstrTitle="请打开经过攻击后的视频文件";
   ofn.Flags=OFN_FILEMUSTEXIST;
   ofn.nFileOffset;
   ofn.nFileExtension;
   ofn.lpstrDefExt="AVI";
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL;
   ofn.lpTemplateName=NULL; 


//   char temp0[100];
   if(GetOpenFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
//	   strcpy(FileName_Attacked,temp0);
//	   strcat(FileName_Attacked,"\\temp.bmp");
//	   strcpy(FileTitle_Attacked,"temp.avi");
	   return;
   }

   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
   }

//计算PSNR
   int i,j,k;
   int temp_difference;
   double DifferenceSum_Attacked;

   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//注意：这里的调色板顺序

   fseek(fp,4096,0);//找到数据的位置

   CFile OriginalFile;
   OriginalFile.Open(FilePath,CFile::modeRead);

   OriginalFile.Seek(4096L,CFile::begin);

   for (k=0;k<TotalFrameNumber;k++)
   {
       DifferenceSum_Attacked=0.0;//针对每一帧都清零 
	   OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
       OriginalFile.Read(Watermark_Attacked,101376);//借用这个数组Watermark_Attacked

	   fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
       fread(FrameBytes_Attacked,101376,1,fp);

	   for (i=0;i<288;i++)
		  for (j=0;j<352;j++)
		  {  
		       temp_difference=(int)FrameBytes_Attacked[i*352+j]-(int)Watermark_Attacked[i*352+j];
			   DifferenceSum_Attacked=DifferenceSum_Attacked+temp_difference*temp_difference;
		   
		   }

       PSNR_Attacked[k]=10*log10(255.0*255.0*352.0*288.0/DifferenceSum_Attacked);
   }


//显示第一帧画面
// for (i=0;i<TotalFrameNumber;i++)
   {
      fseek(fp,4096,0);
	  fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
      fread(FrameBytes_Attacked,101376,1,fp);

/*	   for (tx=240;tx<280;tx++)
	   {
		   FrameBytes_Attacked[101376-(60+1)*352+tx]=255;
		   FrameBytes_Attacked[101376-(120+1)*352+tx]=255;
	   }
       for (ty=60;ty<120;ty++)
	   {
		   FrameBytes_Attacked[101376-(ty+1)*352+240]=255;
		   FrameBytes_Attacked[101376-(ty+1)*352+280]=255;
	   }
*/
     
   }

//   unsigned char temp_attacked;
//   for (i=0;i<101376;i++)
//   {
//      temp_attacked=(FrameBytes_Attacked[i]&254);
//	    FrameBytes_Attacked[i]=temp_attacked;
//   }

   PositionFlag_AttackedVideo=1;
   PositionFlag_AttackedWatermark=0;
   AttackedFile_Opened=TRUE;
   FrameNumber_Attacked=0;
}

void CREADAVIDoc::OnExtract_WatermarkAttacked()
{
   int i,j,temp_bit,temp_conversion;

   //申请内存
   unsigned char *current_watermark;//The pointer of the current watermark data
   void *mem;
   
   mem=GlobalAlloc(GMEM_FIXED,101376);

   //If memory is not enough, return directly
   if(mem==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem);
	  return;
   }
   
   current_watermark=(unsigned char *)mem; 

   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
   }


   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//文件的调色板信息

   fseek(fp,4096,0);

//   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//解扩
   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//解扩

  for (i=0;i<101376;i++)
   Watermark_Attacked[i]=0;

//在连续15帧中提取水印
  for (j=0;j<150;j++)  
  {
   fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
   fread(FrameBytes_Attacked_Watermark,101376,1,fp);
   
   if (BitplanePositionSequence[j]==0)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=(FrameBytes_Attacked_Watermark[i]&1);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			 temp_conversion=temp_bit*(-1);
		  else
             temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }
	   }
   }

   if (BitplanePositionSequence[j]==1)
   {
	   for (i=0;i<101376;i++)
	   {
          temp_bit=((FrameBytes_Attacked_Watermark[i]&2)>>1);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==2)
   {
	   for (i=0;i<101376;i++)
	   { 
		  temp_bit=((FrameBytes_Attacked_Watermark[i]&4)>>2);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }    
	   }
   }

   if (BitplanePositionSequence[j]==3)
   {
	   for (i=0;i<101376;i++)
	   { 	     
          temp_bit=((FrameBytes_Attacked_Watermark[i]&8)>>3);//0 - 1  1 - 0
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }          
	   }
   }

  if (BitplanePositionSequence[j]==4)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&16)>>4);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			 current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==5)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&32)>>5);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==6)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&64)>>6);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			 current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }
	   }
   }

   if (BitplanePositionSequence[j]==7)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&128)>>7);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }
}

//   for (i=0;i<101376;i++)
//	  Watermark_Attacked[i]=current_watermark[i];

 for (i=0;i<101376;i++)
   {
	   if (Watermark_Attacked[i]>=75)
		    Watermark_Attacked[i]=1;
	   else
		    Watermark_Attacked[i]=0;
   } 


   //计算NC值
   int watermark_attacked_sum_up=0;
   double watermark_attacked_sum_down=101376.0;
   int watermark_attacked_temp;
   for (i=0;i<101376;i++)
   {
	 if (Watermark[i]==1)
		watermark_attacked_temp=-1;
	 if (Watermark[i]==0)
		watermark_attacked_temp=1;
	 if (Watermark_Attacked[i]==1)
		watermark_attacked_sum_up=watermark_attacked_temp*(-1)+watermark_attacked_sum_up;
	 if (Watermark_Attacked[i]==0)
		watermark_attacked_sum_up=watermark_attacked_temp*1+watermark_attacked_sum_up;
	}
	NC_Attacked_Value=watermark_attacked_sum_up/101376.0;


   	//byte -> bit 
	for (i=0;i<12672;i++)
	{
		m_AttackedWatermarkBytes[i]=0;//将数组清空
		for (j=0;j<8;j++)
		{
		   if (Watermark_Attacked[i*8+j]==1)
               m_AttackedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_AttackedWatermarkBytes[i];
	
		}
	}
	PositionFlag_AttackedWatermark=1;

	GlobalFree(mem);//释放内存
}

void CREADAVIDoc::OnAttackedReport() 
{
   // TODO: Add your command handler code here
   //Save the attacked report
   char FileName_TXT[_MAX_PATH],FileTitle_TXT[_MAX_PATH];//The Filename and FileTitle
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName_TXT[0]='\0';
   FileTitle_TXT[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL;//The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("TXT Files (*.txt)\0*.txt\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'TXT文件 *.txt'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_TXT; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_TXT;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存测试报告";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="TXT";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.txt文件");
	   strcpy(FileName_TXT,temp0);
	   strcat(FileName_TXT,"\\temp.txt");
	   strcpy(FileTitle_TXT,"temp.txt");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_TXT,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_TXT)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_TXT);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_TXT,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

    FILE *fp;
    fp=fopen(FileName_TXT,"w");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Open File Error");
		return ;
	}
	fprintf(fp,"受到攻击后的视频序列所对应的PSNR");

	int i;
	for (i=0;i<TotalFrameNumber;i++)
       fprintf(fp,"\n第 %3d 帧  PSNR：  %f  dB",i,PSNR_Attacked[i]);
	fclose(fp);	
}

void CREADAVIDoc::FramePlay_Attacked()
{  
	int i;
	FrameNumber_Attacked=FrameNumber_Attacked+1;

	FILE *fp;
    fp=fopen(FileName_Attacked,"rb");
	if(fp==NULL)
	{
	   MessageBeep(-1);
	   AfxMessageBox("File");
	   return;
	}

	if (FrameColorInfo[0]==8)//灰度
	{
	   fseek(fp,4104L,0);

	   for (i=0;i<FrameNumber_Attacked;i++)
	   {
	      fseek(fp,101384L,1);//每一帧的数据长度（8+101376）bytes
	   }
          fread(FrameBytes_Attacked,101376,1,fp);

/*	   for (tx=240;tx<280;tx++)
	   {
		   FrameBytes_Attacked[101376-(60+1)*352+tx]=255;
		   FrameBytes_Attacked[101376-(120+1)*352+tx]=255;
	   }
       for (ty=60;ty<120;ty++)
	   {
		   FrameBytes_Attacked[101376-(ty+1)*352+240]=255;
		   FrameBytes_Attacked[101376-(ty+1)*352+280]=255;
	   }
*/
	   }
	   else//彩色
	   {   
/*		   fseek(fp,2056L,0);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,304136L,1);//每一帧的数据长度（8＋304128）bytes
		   }
           fread(FrameBytes,304128,1,fp);   */
	   }   
	   fclose(fp);  


}

void CREADAVIDoc::OnExtractCurrentWatermarkattacked()
{   
   int i,j,temp_bit,temp_conversion;

   //申请内存
   unsigned char *current_watermark;//The pointer of the current watermark data
   void *mem;
   
   mem=GlobalAlloc(GMEM_FIXED,101376);

   //If memory is not enough, return directly
   if(mem==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem);
	  return;
   }
   
   current_watermark=(unsigned char *)mem; 

   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
   }


   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//文件的调色板信息

   fseek(fp,4096,0);

//   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//解扩
   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//解扩

  for (i=0;i<101376;i++)
   Watermark_Attacked[i]=0;

//提取当前帧水印
  for (j=0;j<(FrameNumber_Attacked+1);j++)  
  {
   fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
   fread(FrameBytes_Attacked_Watermark,101376,1,fp);
   
   if (BitplanePositionSequence[j]==0)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=(FrameBytes_Attacked_Watermark[i]&1);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			 temp_conversion=temp_bit*(-1);
		  else
             temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }
	   }
   }

   if (BitplanePositionSequence[j]==1)
   {
	   for (i=0;i<101376;i++)
	   {
          temp_bit=((FrameBytes_Attacked_Watermark[i]&2)>>1);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==2)
   {
	   for (i=0;i<101376;i++)
	   { 
		  temp_bit=((FrameBytes_Attacked_Watermark[i]&4)>>2);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }    
	   }
   }

   if (BitplanePositionSequence[j]==3)
   {
	   for (i=0;i<101376;i++)
	   { 	     
          temp_bit=((FrameBytes_Attacked_Watermark[i]&8)>>3);//0 - 1  1 - 0
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }          
	   }
   }

  if (BitplanePositionSequence[j]==4)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&16)>>4);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			 current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==5)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&32)>>5);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }

   if (BitplanePositionSequence[j]==6)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&64)>>6);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			 current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  }
	   }
   }

   if (BitplanePositionSequence[j]==7)
   {
	   for (i=0;i<101376;i++)
	   { 
          temp_bit=((FrameBytes_Attacked_Watermark[i]&128)>>7);
		  if (temp_bit==1)
			 temp_bit=-1;
		  else
             temp_bit=1;
		  if (PseudoRandomSequence[i]==1)
			  temp_conversion=temp_bit*(-1);
		  else
              temp_conversion=temp_bit*1;
		  if  (temp_conversion==1)
		  {
			  current_watermark[i]=0;
			  Watermark_Attacked[i]=0+Watermark_Attacked[i];
		  }
		  else
		  {   
			  current_watermark[i]=1;
			  Watermark_Attacked[i]=1+Watermark_Attacked[i];
		  } 
	   }
   }
}

   for (i=0;i<101376;i++)
	  Watermark_Attacked[i]=current_watermark[i];

/* for (i=0;i<101376;i++)
   {
	   if (Watermark_Attacked[i]>=16)
		    Watermark_Attacked[i]=1;
	   else
		    Watermark_Attacked[i]=0;
   }
*/

   //计算NC值
   int watermark_attacked_sum_up=0;
   double watermark_attacked_sum_down=101376.0;
   int watermark_attacked_temp;
   for (i=0;i<101376;i++)
   {
	 if (Watermark[i]==1)
		watermark_attacked_temp=-1;
	 if (Watermark[i]==0)
		watermark_attacked_temp=1;
	 if (Watermark_Attacked[i]==1)
		watermark_attacked_sum_up=watermark_attacked_temp*(-1)+watermark_attacked_sum_up;
	 if (Watermark_Attacked[i]==0)
		watermark_attacked_sum_up=watermark_attacked_temp*1+watermark_attacked_sum_up;
	}
	NC_Attacked_Value=watermark_attacked_sum_up/101376.0;


   	//byte -> bit 
	for (i=0;i<12672;i++)
	{
		m_AttackedWatermarkBytes[i]=0;//将数组清空
		for (j=0;j<8;j++)
		{
		   if (Watermark_Attacked[i*8+j]==1)
               m_AttackedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_AttackedWatermarkBytes[i];
	
		}
	}
	PositionFlag_AttackedWatermark=1;

	GlobalFree(mem);//释放内存

}

void CREADAVIDoc::OnSave_CurrentWatermark_Attacked()
{
   //Save the attacked_watermark file  
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_Watermark[_MAX_PATH],FileTitle_Watermark[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_Watermark[0]='\0';
   FileTitle_Watermark[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_Watermark; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_Watermark;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("由于您取消了存储，系统自动保存为temp.bmp文件");
	   strcpy(FileName_Watermark,temp0);
	   strcat(FileName_Watermark,"\\temp.bmp");
	   strcpy(FileTitle_Watermark,"temp.avi");
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_Watermark,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_Watermark)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_Watermark);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_Watermark,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_Watermark,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Watermark File Error");
	  return;
   }
	 
   fwrite(m_OriginalWatermarkBytes,62,1,fp); 
   
   fwrite(m_AttackedWatermarkBytes,12672,1,fp);

   fclose(fp);
}

void CREADAVIDoc::OnTimeSpatialEmbedding_Improved()
{
   //Save the watermarked avi file
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName[0]='\0';
   FileTitle[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI文件 *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存AVI文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作");
	   return;
   }

   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   
    int i,j,k,l,m,n;
 
    int flag_0=0,flag_1=0,flag_2=0,flag_3=0;

    FILE *fp;
    fp=fopen(FileName,"wb");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("File Error");
		return;
	}
	 
	fwrite(AVIFileInfo,4096,1,fp);         

//	int m_FrameByte;//用来暂存原始数据
	double DifferenceSum;//象素差
	int OriginalPixelValueSum;//原始象素的和，用来计算SNR
	int temp_conversion;
//	unsigned char temp_line[352];//用来将图像数据倒过来
	unsigned char Frame_Block[1584][8][8];//分块后的图像数据

	double BlockDCT_Coefficient[1584][8][8];//分块DCT系数

	double *BlockDCT_Coefficient_Saved[1584][8];//前一帧的分块DCT系数
	double *temp_DC_Coefficient_Difference;//前后帧相应的DCT直流系数之差
    double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double *temp_DCT_Edge_Detection;//在DCT域判断是否有边缘存在
	double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值
	int flag_DC_Coefficient,flag_AC_Coefficient;	 

	//分配内存
	void *mem;
	void *mem_DC_Difference;
    void *mem_DCT_Variance;
    void *mem_AC_Energy;
    void *mem_DCT_Edge_Detection;
	
	mem=GlobalAlloc(GMEM_FIXED,101376*8);
    mem_DC_Difference=GlobalAlloc(GMEM_FIXED,1584*8);	
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);
	mem_DCT_Edge_Detection=GlobalAlloc(GMEM_FIXED,1584*8);

	if(mem==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem);
	  return;
	}
    
    if(mem_DC_Difference==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DC_Difference);
	  return;
	}
   
	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	if(mem_DCT_Edge_Detection==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Edge_Detection);
	  return;
	}

	for (k=0;k<1584;k++)
      for (j=0;j<8;j++)
	  {
	   	 BlockDCT_Coefficient_Saved[k][j]=(double *)mem+k*64+j*8;
	  }

    temp_DC_Coefficient_Difference=(double *)mem_DC_Difference;
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;
    temp_DCT_Edge_Detection=(double *)mem_DCT_Edge_Detection;
	
    //对64位的水印进行置乱（扩频）
    //产生伪随机序列
    PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //利用伪随机序列对水印进行调制  
    for (i=0;i<64;i++)
	{ 
	  if (m_OriginalWatermarkBytes[i]==1)
		  temp_conversion=-1;//1 -> -1
	  else
          temp_conversion=1;//0 -> 1
      if (PseudoRandomSequence[i]==1)
	      temp_conversion=temp_conversion*(-1);
	  else
		  temp_conversion=temp_conversion*1;
      if (temp_conversion==-1)
		  PseudoRandomSequence[i]=1;
	  else
          PseudoRandomSequence[i]=0;
	}

	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);	

	for (l=0;l<1584;l++)
	{
	  temp_DC_Coefficient_Difference[l]=0.0;
      temp_DCT_Coefficient_Variance[l]=0.0;
	  temp_DCT_Edge_Detection[l]=0.0;
	}

	//计算水印嵌入的时间
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//灰度
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
//		  for (k=0;k<10;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//显示DC系数的信息
			 flag_AC_Coefficient=0;//显示AC系数的信息
//			 DC_Coefficient_Sum=0.0;
			 DifferenceSum=0.0;//针对每一帧都清零
			 OriginalPixelValueSum=0;//针对每一帧都清零
	         OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
             
             OriginalFile.Read(FrameBytes_Embedding,101376);

		//	 OriginalFile.Seek(101376L,CFile::current);
			 for (i=0;i<288;i++)//将图像数据倒过来
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//对当前帧进行8*8分块
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
			
                DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
// 				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//原图像进行DCT变换 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换

                if (k>=1)//比较前后两帧，这里不会出现误差累计的情况，因为前一帧的信息存储在DCT系数矩阵里
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//前后帧对应DCT系数的差值
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
                 
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			
			//	DC_Coefficient_Sum=DC_Coefficient_Sum+BlockDCT_Coefficient[l][0][0];//对直流系数进行求和
			 }

/*          if (k>=1)
			{
				char temp_DC_flag[10];
			    sprintf(temp_DC_flag,"%d   第%d帧",flag_DC_Coefficient,k-1);
                MessageBox(NULL,temp_DC_flag,"前后帧DC系数之差超过100",MB_OK); 
			}

			char temp_AC_flag[10];
			sprintf(temp_AC_flag,"%d   第%d帧",flag_AC_Coefficient,k);
            MessageBox(NULL,temp_AC_flag,"AC系数的方差超过1000",MB_OK); 
*/
//          DC_Coefficient_Sum=DC_Coefficient_Sum/1584.0;

//*****************************************************************

						 
//		    int temp;
			int r=0; 
			//经过判断后在相应的块中嵌水印
			
			  if (k==0)//第一帧单独处理,不参考邻近的帧
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				{ 
				   //if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//纹理复杂块 for table tennis improved2.avi 
				   if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//纹理复杂块 
				   {
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;

					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//标记出8*8方块的边界
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==7)
					    //Frame_Block[l][m][n]=255;//标记出这样的块
                      
                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(0*8));//1111 0111
                        
					    //A bitplane position controller is defined.
					  //    if (!fmod(l,2))
                              Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequence[m*8+n]*8));//1111 0111
                      //    else
			          //        Frame_Block[l][m][n]=((Frame_Block[l][m][n]&239)+(PseudoRandomSequence[m*8+n]*16));//1110 1111

                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&223)+(PseudoRandomSequence[m*8+n]*32));//1101 1111
					}
				   } 
                
				}
			  }
              else
			  {
                r=0;  
                //srand((unsigned)time(NULL));
				srand(6);//8,9
                for (l=0;l<1584;l++)
				{
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//既是运动块也是纹理复杂块 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//是运动块 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//是纹理复杂块  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//交流分量的能量总和
               
	//	        if ((temp_DC_Coefficient_Difference[l]>=120)&&(temp_AC_Coefficient_Energy[l]>=16000)&&(fabs(BlockDCT_Coefficient[l][0][0])>930))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块  for table tennis improved2.avi 
                if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12800))//&&(fabs(BlockDCT_Coefficient[l][0][0])>900))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块  for table tennis improved2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=25)&&(temp_AC_Coefficient_Energy[l]>=6000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>600))//既是运动块也是纹理复杂块  for miss america_0.avi 
                // if ((temp_DC_Coefficient_Difference[l]>=100)&&(temp_AC_Coefficient_Energy[l]>=5000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>600))//既是运动块也是纹理复杂块
    //          if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
//				if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))

				{  
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;
        
	                //define a bitplane position controller            
					int flag=0;
					int random;

		
					
                    random=rand();
					if (random>=12000)
					   flag=1;
				    if ((random<12000)&&(random>=10000))
                       flag=2;
					if (random<10000) 
                       flag=0; 

			
						
          
					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//标记出8*8方块的边界
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==7)
						//    Frame_Block[l][m][n]=255;//标记出这样的块
               

                          
                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(0*8));//1111 0111
                        
						//A bitplane position controller is defined.
		        //	  if (flag==1)
                            Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequence[m*8+n]*8));//1111 0111
                /*     else
						{
							if (flag==2)
			                   Frame_Block[l][m][n]=((Frame_Block[l][m][n]&239)+(PseudoRandomSequence[m*8+n]*16));//1110 1111
							 //   Frame_Block[l][m][n]=((Frame_Block[l][m][n]&253)+(PseudoRandomSequence[m*8+n]*2));//1111 1101
						    else
                               Frame_Block[l][m][n]=((Frame_Block[l][m][n]&251)+(PseudoRandomSequence[m*8+n]*4));//1111 1011
						}
                */  
					//Frame_Block[l][m][n]=((Frame_Block[l][m][n]&223)+(PseudoRandomSequence[m*8+n]*32));//1101 1111

						//if ((PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=0)&&(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=1))
						//{
						//	AfxMessageBox("Watermark Message Error!");
						//	return;
						//}
						//Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]*8));//1111 0111
					    //temp=Frame_Block[l][m][n]-temp;
					}
				}
			}
		}		
		
        //    Frame_Block[1500][0][0]=255;   //沿时间轴嵌入水印

            EmbeddingRegionSumSequence[k]=r; 
/*
//**********************************************************************
          //再次进行验证，判断经过改动后是否还能找到原来的区域
          for (l=0;l<1584;l++)
			 {	
                temp_DCT_Coefficient_Mean=0.0;
			    flag_DC_Coefficient=0;//显示DC系数的信息
			    flag_AC_Coefficient=0;//显示AC系数的信息
				temp_DCT_Coefficient_Variance[l]=0.0; 
				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//原图像进行DCT变换 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换

                if (k>=1)//比较前后两帧
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//前后帧对应DCT系数的差值
				   if (temp_DC_Coefficient_Difference[l]>=100)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
                 
				if (temp_DCT_Coefficient_Variance[l]>=1000)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			
			//	DC_Coefficient_Sum=DC_Coefficient_Sum+BlockDCT_Coefficient[l][0][0];//对直流系数进行求和
			 }


			//经过判断后在相应的块中嵌水印
			for (l=0;l<1584;l++)
            {
				if (temp_DCT_Coefficient_Variance[l]>=1000)//既是运动块也是纹理块 
				{  
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   Frame_Block[l][m][n]=255;//标记出这样的块  
					}
				}
			}
//********************************************************************************		
*/            
      
			//将分块后的数据还原为整幅数据
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //计算相应的PSNR(大约耗时7秒钟)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//用来计算PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);

//			fwrite(FrameBytes_Embedding,101376,1,fp);
	
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//将数据按倒序存储
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("水印嵌入过程结束!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"水印嵌入过程：%lf秒",m_time);
          MessageBox(NULL,temp_embeddingtime,"水印嵌入完成!",MB_OK);

		  //AVI文件后面的INDEX部分
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//利用FrameBytes_Embedding数组
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI文件已成功保存");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //彩色
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//嵌入水印的序列对应的位置
       FrameNumber_Watermarked=0;//首先将文件的位置初始化

	   GlobalFree(mem);
	   GlobalFree(mem_DC_Difference);
	   GlobalFree(mem_DCT_Variance);
	   GlobalFree(mem_AC_Energy);
	   GlobalFree(mem_DCT_Edge_Detection);
}

void CREADAVIDoc::DCT(unsigned char inf1[][8], double DCTout[][8])
{
  //DCT变换
  int x,y,u,v;
  double mf=0,nf=0;
  double a[8];
  a[0]=sqrt(1.0/8.0);
	for(x=1;x<8;x++)
    	a[x]=sqrt(2.0/8.0);	
 	for(u=0;u<8;u++)
		for(v=0;v<8;v++)
		{	mf=0;
			for(x=0;x<8;x++)
			   for(y=0;y<8;y++)
				mf+=inf1[x][y]*cos((2.0*y+1.0)*v*PI/16.0)*cos((2.0*x+1.0)*u*PI/16.0);
				DCTout[u][v]=mf*a[u]*a[v];
		}

}

void CREADAVIDoc::IDCT(double inf2[][8], unsigned char IDCTout[][8])
{
  //逆DCT变换
  int x,y,u,v;
  double mf=0;
  double a[8];
  a[0]=sqrt(1.0/8.0);
	for(x=1;x<8;x++)
    	a[x]=sqrt(2.0/8.0);	
	for(x=0;x<8;x++)
	 for(y=0;y<8;y++)
		{mf=0;
		for(u=0;u<8;u++)
			for(v=0;v<8;v++)
				mf+=a[u]*a[v]*inf2[u][v]*cos((2.0*y+1.0)*v*PI/16.0)*cos((2.0*x+1.0)*u*PI/16.0);
			if(mf>=254.5)
			{
				IDCTout[x][y]=255;
			}
			else 
			{
				if(mf<0.5)
				{
					IDCTout[x][y]=0;
				}
				else
				{
					IDCTout[x][y]=(unsigned char)(mf+0.5);
				}
			}
		}	


}

void CREADAVIDoc::OnWatermark_Region_Detection()
{
   OPENFILENAME ofn;
  
//   char FileName_Detection[_MAX_PATH],FileTitle_Detection[_MAX_PATH];//The Filename and FileTitle 
  
   FileName_Attacked[0]='\0';  //NULL string
   FileTitle_Attacked[0]='\0'; //NULL string


   ofn.lStructSize=sizeof(OPENFILENAME);
   ofn.hwndOwner=NULL;
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI文件 *.avi\0*.avi\0All File *.*\0*.*\0\0"); 
   ofn.lpstrCustomFilter=NULL; 
   ofn.nMaxCustFilter=0; 
   
   ofn.nFilterIndex=1; //Select the item 'AVI文件 *.avi'
   ofn.lpstrFile=FileName_Attacked; 
   ofn.nMaxFile=_MAX_PATH; 
   
   ofn.lpstrFileTitle=FileTitle_Attacked; 
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;
   ofn.lpstrTitle="请打开视频文件";
   ofn.Flags=OFN_FILEMUSTEXIST;
   ofn.nFileOffset;
   ofn.nFileExtension;
   ofn.lpstrDefExt="AVI";
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL;
   ofn.lpTemplateName=NULL; 

   if(GetOpenFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
	   return;
   }

   int k;

   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Detection File Error");
	  return;
   }
   
   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//注意：这里的调色板顺序

   for (k=0;k<TotalFrameNumber;k++)
   {
	   fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
       fread(FrameBytes_Attacked,101376,1,fp);
   }

// 显示第一帧画面
// for (i=0;i<TotalFrameNumber;i++)
   {
      fseek(fp,4096,0);
	  fseek(fp,8L,1);//每一帧的数据长度（8+101376）bytes
      fread(FrameBytes_Attacked,101376,1,fp);
   }

   PositionFlag_AttackedVideo=1;
   PositionFlag_AttackedWatermark=0;
   AttackedFile_Opened=TRUE;
   FrameNumber_Attacked=0;

}




void CREADAVIDoc::OnDetection_CurrentWatermarkSequence()
{
    int i,j,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值

    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

	PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

    for (l=0;l<1584;l++)
	{
		temp_DCT_Coefficient_Variance[l]=0.0;
	    temp_AC_Coefficient_Energy[l]=0.0;
	} 

	         for (l=0;l<1584;l++)
			 {	
                temp_DCT_Coefficient_Mean=0.0;	 
				temp_DCT_Coefficient_Variance[l]=0.0; 

				DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//原图像进行DCT变换 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换
			
            /*  for (m=0;m<8;m++)
				   for (n=0;n<8;n++)
					{
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				      
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值
            */ 
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					    temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
		        temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));
			 }

            int correlation_threshold;
			int existing_block_number=0;
        	int candidate_block_number=0;

			int watermark_extracted_sum[64];

			for (m=0;m<64;m++)
            {
              watermark_extracted_sum[m]=0;
			}

			//经过判断，确定候选块，然后确定水印存在块
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			//	if (temp_DCT_Coefficient_Variance[l]>=1000)//仅仅判断是否是纹理块 
			//  if ((temp_AC_Coefficient_Energy[l]>=28000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1080))
			//  if (temp_AC_Coefficient_Energy[l]>=35000)// for mobile & calendar
				if (temp_AC_Coefficient_Energy[l]>=10000)// for table tennis
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//标记出这样的块 
					   if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
					   //if (((Frame_Block[l][m][n]&16)>>4)==1)//1110 1111
				          temp_conversion=-1;//1 -> -1
			           else
				          temp_conversion=1;//0 -> 1
                       if  (PseudoRandomSequence[m*8+n]==1)
				          temp_conversion=temp_conversion*(-1);
			           else
                          temp_conversion=temp_conversion*1;
			           if (temp_conversion==1)
				          Watermark_Extracted[m*8+n]=0;
			           else
                          Watermark_Extracted[m*8+n]=1;
					}
				  
				 //再从中检测水印是否存在
				 for (m=0;m<64;m++)
				 { 
					 if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
                        correlation_threshold=correlation_threshold+1; 
				     if (Watermark_Extracted[m]!=m_OriginalWatermarkBytes[m])
						correlation_threshold=correlation_threshold-1;  
				 }

				 if (correlation_threshold==64)
				 {
					 existing_block_number=existing_block_number+1;
				     for (m=0;m<64;m++)
                       watermark_extracted_sum[m]=watermark_extracted_sum[m]+Watermark_Extracted[m];
				 }					
				}
			}

          if (existing_block_number>1)//最终确定水印信息
		   {
			   for (m=0;m<64;m++)
			   {
				if (watermark_extracted_sum[m]>=(existing_block_number/2.0))
//                if (watermark_extracted_sum[m]>=(candidate_block_number/2)) 
				   Watermark_Extracted[m]=1;
				else
                    Watermark_Extracted[m]=0;
			   }
		   }
//		   else
//		   {
//			   for (m=0;m<64;m++)
//				   Watermark_Extracted[m]=0;
//		   }
	
		   correct_bit_number=0;
		   correct_rate=0.0;
	       for (m=0;m<64;m++)
		   { 
			    if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
                   correct_bit_number=correct_bit_number+1; 
		   }
		   correct_rate=correct_bit_number/64.0*100.0;

/*		//byte -> bit 
		for (i=0;i<12672;i++)
		{
			m_ExtractedWatermarkBytes[i]=0;//将数组清空
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}
*/

		//计算NC值
		int watermark_sum_up=0;
		double watermark_sum_down=101376.0;
		int watermark_temp;
		for (i=0;i<101376;i++)
		{
			if (Watermark[i]==1)
				watermark_temp=-1;
			if (Watermark[i]==0)
				watermark_temp=1;
			if (Watermark_Extracted[i]==1)
				watermark_sum_up=watermark_temp*(-1)+watermark_sum_up;
			if ((Watermark_Extracted[i]==0))
				watermark_sum_up=watermark_temp*1+watermark_sum_up;
		}
		NC_Value=watermark_sum_up/101376.0;
    
		CurrentWatermarkSequence_Extracted=1;

		m_save_currentwatermark=TRUE;


		GlobalFree(mem_DCT_Variance);
		GlobalFree(mem_AC_Energy);
}

void CREADAVIDoc::OnExtract_CurrentWatermarksequenceattacked()
{
    int i,j,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
	double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值

    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

    if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
	temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

	PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

//  for (l=0;l<1584;l++)
//	{
//		temp_DCT_Coefficient_Variance[l]=0.0;
//        temp_AC_Coefficient_Energy[l]=0.0;
//	}

	    for (l=0;l<1584;l++)
			 {	
                temp_DCT_Coefficient_Mean=0.0;	 
				temp_DCT_Coefficient_Variance[l]=0.0; 
                temp_AC_Coefficient_Energy[l]=0.0;

                DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//原图像进行DCT变换 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换
			
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
					    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					    temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]); 
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
			 }

            int correlation_threshold;
			int existing_block_number=0;
			int candidate_block_number=0;
			int watermark_attacked_sum[64];

			for (m=0;m<64;m++)
            {
              watermark_attacked_sum[m]=0;
			}

			//经过判断，确定候选块，然后检测水印是否存在
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			//	if (temp_DCT_Coefficient_Variance[l]>=1000)//仅仅判断是否是纹理块 
			//  if (temp_AC_Coefficient_Energy[l]>=12000)//TD'<TD 12000<16000
			    if (temp_AC_Coefficient_Energy[l]>=40000)//TD'<TD 35000<40000
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//标记出这样的块 
					   if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
				          temp_conversion=-1;//1 -> -1
			           else
				          temp_conversion=1;//0 -> 1
                       if  (PseudoRandomSequence[m*8+n]==1)
				          temp_conversion=temp_conversion*(-1);
			           else
                          temp_conversion=temp_conversion*1;
			           if (temp_conversion==1)
				          Watermark_Attacked[m*8+n]=0;
			           else
                          Watermark_Attacked[m*8+n]=1;
					}
				  
				 //再从中检测水印是否存在
				 for (m=0;m<64;m++)
				 { 
					 if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
                        correlation_threshold=correlation_threshold+1; 
				     if (Watermark_Attacked[m]!=m_OriginalWatermarkBytes[m])
						correlation_threshold=correlation_threshold-1;  
				 }

				 if (correlation_threshold>=20)
				 {
					 existing_block_number=existing_block_number+1;
				     for (m=0;m<64;m++)
                       watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m];
				 }					
				}
			}

		 char temp_existing_block_number[100];
         sprintf(temp_existing_block_number,"检测出水印的块数 %d",existing_block_number);
         MessageBox(NULL,temp_existing_block_number,"检测出水印的块数",MB_OK);//Display the info

         if (existing_block_number>1)//最终确定水印信息
		   {
			   for (m=0;m<64;m++)
			   {
				if (watermark_attacked_sum[m]>=(existing_block_number/2.0))
//                if (watermark_attacked_sum[m]>=(candidate_block_number/2)) 
				   Watermark_Attacked[m]=1;
				else
                    Watermark_Attacked[m]=0;
			   }
		   }
//		   else
//		   {
//			   for (m=0;m<64;m++)
//				   Watermark_Extracted[m]=0;
//		   }
	
		   correct_bit_number_attacked=0;
		   correct_rate_attacked=0.0;
	       for (m=0;m<64;m++)
		   { 
			    if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
                   correct_bit_number_attacked=correct_bit_number_attacked+1; 
		   }
		   correct_rate_attacked=correct_bit_number_attacked/64.0*100.0;
	       
	    PositionFlag_AttackedWatermarkSequence=1;
}

void CREADAVIDoc::OnSaveCurrentEmbeddedFrame()
{
	//Save the watermarked file
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_CurrentEmbeddingFrame[_MAX_PATH],FileTitle_CurrentEmbeddingFrame[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_CurrentEmbeddingFrame[0]='\0';
   FileTitle_CurrentEmbeddingFrame[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentEmbeddingFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentEmbeddingFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
//	   strcpy(FileName_CurrentEmbeddingFrame,temp0);
//	   strcat(FileName_CurrentEmbeddingFrame,"\\temp.bmp");
//	   strcpy(FileTitle_CurrentEmbeddingFrame,"temp.avi");
	   return;
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_CurrentEmbeddingFrame,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_CurrentEmbeddingFrame)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_CurrentEmbeddingFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentEmbeddingFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_CurrentEmbeddingFrame,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Current Frame File Error");
	  return;
   }
	 
   	long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"标准[256GrayBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"读取标准[256GrayBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	}
    _close(fh);  

   fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
   
   fwrite(FrameBytes_Embedding,101376,1,fp);

   fclose(fp);

}

void CREADAVIDoc::OnExtract_GOPWatermarkSequenceAttacked()
{
 // int i,j,temp_bit,temp_conversion;

    int i,j,k,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
	double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值

    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if (mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}
   
    if (mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   

   temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
   temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

   PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2
   
   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if (fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
   }


   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//文件的调色板信息

   fseek(fp,4096,0);

   PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//解扩

   for (i=0;i<64;i++)
       Watermark_Attacked[i]=0;

   int correlation_threshold;
   int existing_block_number=0;
   int candidate_block_number=0;
   int watermark_attacked_sum[64];

   for (m=0;m<64;m++)
   {
       watermark_attacked_sum[m]=0;
	}

   //每一帧的数据长度（8＋101376）bytes
   //不考虑第一帧
   fseek(fp,8L,1);
   fseek(fp,101376L,1);

//在连续15帧中提取水印
  for (j=0;j<29;j++)  
  {
   fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
   fread(FrameBytes_Attacked_Watermark,101376,1,fp);

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (k=0;k<352;k++)
            FrameBytes_Inserting[287-i][k]=FrameBytes_Attacked_Watermark[i*352+k];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

    for (l=0;l<1584;l++)
	    temp_DCT_Coefficient_Variance[l]=0.0;

	    for (l=0;l<1584;l++)
			 {	
                temp_DCT_Coefficient_Mean=0.0;	 
				temp_DCT_Coefficient_Variance[l]=0.0; 
                temp_AC_Coefficient_Energy[l]=0.0;

				DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//原图像进行DCT变换 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换
			
                for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
	                  temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					  temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);  
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
			 }

			//经过判断，确定候选块，然后确定水印存在块
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			  //if (temp_DCT_Coefficient_Variance[l]>=1000)//仅仅判断是否是纹理块 
			  //if (temp_AC_Coefficient_Energy[l]>=15000)//TD'<TD 15000<16000
				if (temp_AC_Coefficient_Energy[l]>=35000)//TD'<TD 35000<40000
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//标记出这样的块 
					   if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
				          temp_conversion=-1;//1 -> -1
			           else
				          temp_conversion=1;//0 -> 1
                       if  (PseudoRandomSequence[m*8+n]==1)
				          temp_conversion=temp_conversion*(-1);
			           else
                          temp_conversion=temp_conversion*1;
			           if (temp_conversion==1)
				          Watermark_Attacked[m*8+n]=0;
			           else
                          Watermark_Attacked[m*8+n]=1;
					}
				  
		 		 //再从中确定水印存在块
//				 for (m=0;m<64;m++)
//				 { 
//					 if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
//                        correlation_threshold=correlation_threshold+1; 
//				     if (Watermark_Extracted[m]!=m_OriginalWatermarkBytes[m])
//						correlation_threshold=correlation_threshold-1;  
//				 }

//				 if (correlation_threshold==64)
				 {
//					 existing_block_number=existing_block_number+1;
				     for (m=0;m<64;m++)
                       watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m];
				 }					
				}
			}
}

//         if (existing_block_number>1)//最终确定水印信息
		   {
			   for (m=0;m<64;m++)
			   {
//				if (watermark_extracted_sum[m]>=(existing_block_number/2))
                if (watermark_attacked_sum[m]>=(candidate_block_number/2)) 
				   Watermark_Attacked[m]=1;
				else
                    Watermark_Attacked[m]=0;
			   }
		   }
//		   else
//		   {
//			   for (m=0;m<64;m++)
//				   Watermark_Extracted[m]=0;
//		   }
	
		   correct_bit_number_attacked=0;
		   correct_rate_attacked=0.0;
	       for (m=0;m<64;m++)
		   { 
			    if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
                   correct_bit_number_attacked=correct_bit_number_attacked+1; 
		   }
		   correct_rate_attacked=correct_bit_number_attacked/64.0*100.0;

           PositionFlag_AttackedWatermarkSequence=1;
}

void CREADAVIDoc::DCT_Program(unsigned char OriginalImageBlock[][8], double DCTBlock[][8], int blockheight, int blockwidth)
{
	int i;                        //Define the repeating variable
	int timerow,timecolumn;		  //Define the input interative variable(Time Domain)
	int	frerow,frecolumn;		  //Define the output interative variable(Frequency Domain)
	double row[8];	              //Define the parameter of the first parameter
	double column[8];             //Define the parameter of the second parameter
	double temp;	              //Define the temporary variable 
	double temp1;	              //Define the first temporary variable 
	double temp2;	              //Define the second temporary variable 
	
	//Initiaize the transform parameters
	row[0]=sqrt(1.0/blockheight);
	column[0]=sqrt(1.0/blockwidth);	
	for(i=1;i<blockheight;i++)
	{
	  row[i]=sqrt(2.0/blockheight);
	}
	for(i=1;i<blockwidth;i++)
	{
	  column[i]=sqrt(2.0/blockwidth);
	}
	
	//Calculate the DCT of blockheight*blockwidth points 
	for(frerow=0;frerow<blockheight;frerow++)
	{
		for(frecolumn=0;frecolumn<blockwidth;frecolumn++)
		{
			//Calculate the DCT for one point 
			temp1=0.0;
			for(timerow=0;timerow<blockheight;timerow++)
			{
				temp2=0;
				for(timecolumn=0;timecolumn<blockwidth;timecolumn++)
				{
					temp=(2.0*timecolumn+1.0)/(2.0*blockwidth)*frecolumn*PI;
					temp2=temp2+OriginalImageBlock[timerow][timecolumn]*cos(temp);
				}
				temp=(2.0*timerow+1.0)/(2.0*blockheight)*frerow*PI;
				temp1=temp1+temp2*cos(temp);
			}
			DCTBlock[frerow][frecolumn]=temp1*row[frerow]*column[frecolumn];
		}
	}

}

void CREADAVIDoc::OnNonBlindExtraction_CurrentWatermarkSequence()
{
    int i,j,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
   
    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

	PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

	    int existing_block_number=0;
     	int candidate_block_number=0;
		int watermark_extracted_sum[64];
		int r,temp_subscript;

		for (m=0;m<64;m++)
            watermark_extracted_sum[m]=0;
			
        for (r=0;r<EmbeddingRegionSumSequence[FrameNumber_Watermarked];r++)
            for (m=0;m<8;m++)
				for (n=0;n<8;n++)
				{   
					//源抽取，需要原始视频序列的参与
					temp_subscript=EmbeddingRegionSequence[FrameNumber_Watermarked][r];
					if (((Frame_Block[temp_subscript][m][n]&8)>>3)==1)//1111 0111
					//if (((Frame_Block[l][m][n]&16)>>4)==1)//1110 1111
				        temp_conversion=-1;//1 -> -1
			        else
				        temp_conversion=1;//0 -> 1
                    if  (PseudoRandomSequence[m*8+n]==1)
				        temp_conversion=temp_conversion*(-1);
			        else
                        temp_conversion=temp_conversion*1;
			        if  (temp_conversion==1)
				        Watermark_Extracted[m*8+n]=0;
			        else
                        Watermark_Extracted[m*8+n]=1;
				     watermark_extracted_sum[m*8+n]=watermark_extracted_sum[m*8+n]+Watermark_Extracted[m*8+n];
				}
		     
		for (m=0;m<64;m++)
		{
            if (watermark_extracted_sum[m]==EmbeddingRegionSumSequence[FrameNumber_Watermarked]) 
				 Watermark_Extracted[m]=1;
			else
                 Watermark_Extracted[m]=0;
		}
	

		correct_bit_number=0;//正确的位数
		correct_rate=0.0;//正确率
	    for (m=0;m<64;m++)
		{ 
			if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
                 correct_bit_number=correct_bit_number+1; 
		}
		correct_rate=correct_bit_number/64.0*100.0;

/*		//byte -> bit 
		for (i=0;i<12672;i++)
		{
			m_ExtractedWatermarkBytes[i]=0;//将数组清空
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}
*/

		//计算NC值
		int watermark_sum_up=0;
		double watermark_sum_down=101376.0;
		int watermark_temp;
		for (i=0;i<101376;i++)
		{
			if (Watermark[i]==1)
				watermark_temp=-1;
			if (Watermark[i]==0)
				watermark_temp=1;
			if (Watermark_Extracted[i]==1)
				watermark_sum_up=watermark_temp*(-1)+watermark_sum_up;
			if ((Watermark_Extracted[i]==0))
				watermark_sum_up=watermark_temp*1+watermark_sum_up;
		}
		NC_Value=watermark_sum_up/101376.0;
        CurrentWatermarkSequence_Extracted=1;
		m_save_currentwatermark=TRUE;

		GlobalFree(mem_DCT_Variance);
		GlobalFree(mem_AC_Energy);
}

void CREADAVIDoc::OnNoBlindExtractCurrentWatermarkattacked()
{
    int i,j,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据

	PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}
    //非盲提取（需要原始序列参与）
    int watermark_attacked_sum[64];
	int r,temp_subscript;
	int correlation_threshold=0;
	int existing_block_number=0;
	int bitplane_position=0;
	double denominator=0.0;

	for (m=0;m<64;m++)
        watermark_attacked_sum[m]=0;
			
    for (r=0;r<EmbeddingRegionSumSequence[FrameNumber_Attacked];r++)//对于每一帧记录的块数
	{
		correlation_threshold=0;
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
				{   
					//源抽取，需要原始视频序列的参与
					temp_subscript=EmbeddingRegionSequence[FrameNumber_Attacked][r];
					if (((Frame_Block[temp_subscript][m][n]&8)>>3)==1)//1111 0111
		            //if (((Frame_Block[temp_subscript][m][n]&16)>>4)==1)//1110 1111
				        temp_conversion=-1;//1 -> -1
			        else
				        temp_conversion=1;//0 -> 1
                    if  (PseudoRandomSequence[m*8+n]==1)
				        temp_conversion=temp_conversion*(-1);
			        else
                        temp_conversion=temp_conversion*1;
			        if  (temp_conversion==1)
				        Watermark_Attacked[m*8+n]=0;
			        else
                        Watermark_Attacked[m*8+n]=1;
		
				}
        
		//检测水印是否存在
		for (m=0;m<8;m++)
		    for (n=0;n<8;n++)
			{
				if (Watermark_Attacked[m*8+n]==m_OriginalWatermarkBytes[m*8+n]) 
                        correlation_threshold=correlation_threshold+1; 
				if (Watermark_Attacked[m*8+n]!=m_OriginalWatermarkBytes[m*8+n])
						correlation_threshold=correlation_threshold-1; 
			
			}
       
        bitplane_position=3;

/*      //检测其它位平面
        //1110 1111
	    if (correlation_threshold<10)
		{
			correlation_threshold=0;
		    for (m=0;m<8;m++)
			   for (n=0;n<8;n++)
				{   
					//源抽取，需要原始视频序列的参与
					temp_subscript=EmbeddingRegionSequence[FrameNumber_Attacked][r];
					if (((Frame_Block[temp_subscript][m][n]&16)>>4)==1)//1110 1111
		            //if (((Frame_Block[temp_subscript][m][n]&16)>>4)==1)//1110 1111
				        temp_conversion=-1;//1 -> -1
			        else
				        temp_conversion=1;//0 -> 1
                    if  (PseudoRandomSequence[m*8+n]==1)
				        temp_conversion=temp_conversion*(-1);
			        else
                        temp_conversion=temp_conversion*1;
			        if  (temp_conversion==1)
				        Watermark_Attacked[m*8+n]=0;
			        else
                        Watermark_Attacked[m*8+n]=1;
		
				}
            	//检测水印是否存在
		 for (m=0;m<8;m++)
		    for (n=0;n<8;n++)
			{
				if (Watermark_Attacked[m*8+n]==m_OriginalWatermarkBytes[m*8+n]) 
                        correlation_threshold=correlation_threshold+1; 
				if (Watermark_Attacked[m*8+n]!=m_OriginalWatermarkBytes[m*8+n])
						correlation_threshold=correlation_threshold-1; 
			
			}

		 bitplane_position=4;
		}	
      
          //1111 1011
	    if (correlation_threshold<10)
		{
			correlation_threshold=0;
		    for (m=0;m<8;m++)
			   for (n=0;n<8;n++)
				{   
					//源抽取，需要原始视频序列的参与
					temp_subscript=EmbeddingRegionSequence[FrameNumber_Attacked][r];
					if (((Frame_Block[temp_subscript][m][n]&4)>>2)==1)//1111 1011
				        temp_conversion=-1;//1 -> -1
			        else
				        temp_conversion=1;//0 -> 1
                    if  (PseudoRandomSequence[m*8+n]==1)
				        temp_conversion=temp_conversion*(-1);
			        else
                        temp_conversion=temp_conversion*1;
			        if  (temp_conversion==1)
				        Watermark_Attacked[m*8+n]=0;
			        else
                        Watermark_Attacked[m*8+n]=1;
		
				}
            	//检测水印是否存在
		 for (m=0;m<8;m++)
		    for (n=0;n<8;n++)
			{
				if (Watermark_Attacked[m*8+n]==m_OriginalWatermarkBytes[m*8+n]) 
                        correlation_threshold=correlation_threshold+1; 
				if (Watermark_Attacked[m*8+n]!=m_OriginalWatermarkBytes[m*8+n])
						correlation_threshold=correlation_threshold-1; 
			
			}

		 bitplane_position=2;
		}	
      
*/

      if (correlation_threshold>=16)
		{
				existing_block_number=existing_block_number+1;
				for (m=0;m<64;m++)
				{
				    watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m];
				  //  watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m]*pow(2,bitplane_position);
				}
				denominator=denominator+pow(2,bitplane_position);
	  }
	}

    char temp_existing_block_number[100];
    sprintf(temp_existing_block_number,"检测出水印的块数 %d",existing_block_number);
    MessageBox(NULL,temp_existing_block_number,"检测出水印的块数",MB_OK);//Display the info

	if (existing_block_number>1)//最终确定水印信息	     
	{
	  for (m=0;m<64;m++)
	  {
        //if (watermark_attacked_sum[m]>=(EmbeddingRegionSumSequence[FrameNumber_Attacked]/2.0+2.5)) for table tennis 4Mbps
		
/*		//进行加权平均
		if ((watermark_attacked_sum[m]/denominator)>=0.5) 
		    Watermark_Attacked[m]=1;
		else
            Watermark_Attacked[m]=0;
*/        

		if (watermark_attacked_sum[m]>=(existing_block_number/2.0)) 
		    Watermark_Attacked[m]=1;
		else
            Watermark_Attacked[m]=0;
        


	  }
	} 
/*  int existing_block_number=0;
	int candidate_block_number=0;
    int watermark_attacked_sum[64];

	for (m=0;m<64;m++)
    {
        watermark_attacked_sum[m]=0;
	}

    //经过判断，确定候选块，然后确定水印存在块
	for (l=0;l<1584;l++)
    {
	
		if (temp_DCT_Coefficient_Variance[l]>=1000)//仅仅判断是否是纹理块 
		{  
           candidate_block_number=candidate_block_number+1;
		   for (m=0;m<8;m++)
			  for (n=0;n<8;n++)
				{
             	   //Frame_Block[l][m][n]=255;//标记出这样的块 
				   if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
				      temp_conversion=-1;//1 -> -1
			       else
				      temp_conversion=1;//0 -> 1
                   if  (PseudoRandomSequence[m*8+n]==1)
				      temp_conversion=temp_conversion*(-1);
			       else
                      temp_conversion=temp_conversion*1;
			       if (temp_conversion==1)
				      Watermark_Attacked[m*8+n]=0;
			       else
                      Watermark_Attacked[m*8+n]=1;
				}
				  
		  for (m=0;m<64;m++)
              watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m];
		}					
	}
		

		   
			   for (m=0;m<64;m++)
			   {
//				if (watermark_extracted_sum[m]>=(existing_block_number/2))
                if (watermark_attacked_sum[m]>=(candidate_block_number/2)) 
				   Watermark_Attacked[m]=1;
				else
                    Watermark_Attacked[m]=0;
			   }
*/		 
		   correct_bit_number_attacked=0;
		   correct_rate_attacked=0.0;
	       for (m=0;m<64;m++)
		   { 
			  if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
                 correct_bit_number_attacked=correct_bit_number_attacked+1; 
		   }
		   correct_rate_attacked=correct_bit_number_attacked/64.0*100.0;
	       PositionFlag_AttackedWatermarkSequence=1;
}

void CREADAVIDoc::OnNoblindExtractGopWmarksequenceattacked()
{
    int f,i,j,l,m,n,temp_conversion;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
   
    FILE *fp;
    fp=fopen(FileName_Attacked,"rb");
    if (fp==NULL)
    {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
    }

    fseek(fp,172,0);
    fread(FrameInfoAttacked,1064,1,fp);//文件的调色板信息
    fseek(fp,4096,0);

    PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//解扩

    for (i=0;i<64;i++)
       Watermark_Attacked[i]=0;

    int watermark_attacked_sum[64];
    int r,temp_subscript,EmbeddingRegionTotalSum=0;

    for (m=0;m<64;m++)
       watermark_attacked_sum[m]=0;

//   //每一帧的数据长度（8＋101376）bytes
//   //不考虑第一帧
//   fseek(fp,8L,1);
//   fseek(fp,101376L,1);

//在连续15帧中提取水印
  for (f=0;f<15;f++)  
  {
    fseek(fp,8L,1);//每一帧的数据长度（8＋101376）bytes
    fread(FrameBytes_Attacked_Watermark,101376,1,fp);

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked_Watermark[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

     for (r=0;r<EmbeddingRegionSumSequence[f];r++)
            for (m=0;m<8;m++)//从第0位到第63位
				for (n=0;n<8;n++)
				{   
					//源抽取，需要原始视频序列的参与
					temp_subscript=EmbeddingRegionSequence[f][r];
					if (((Frame_Block[temp_subscript][m][n]&8)>>3)==1)//1111 0111
				    //if (((Frame_Block[temp_subscript][m][n]&16)>>4)==1)//1110 1111    
					    temp_conversion=-1;//1 -> -1
			        else
				        temp_conversion=1;//0 -> 1
                    if  (PseudoRandomSequence[m*8+n]==1)
				        temp_conversion=temp_conversion*(-1);
			        else
                        temp_conversion=temp_conversion*1;
			        if (temp_conversion==1)
				        Watermark_Attacked[m*8+n]=0;
			        else
                        Watermark_Attacked[m*8+n]=1;
					watermark_attacked_sum[m*8+n]=watermark_attacked_sum[m*8+n]+Watermark_Attacked[m*8+n];
				}
	
				 
			//	for (m=0;m<64;m++)
            //        watermark_attacked_sum[m]=watermark_attacked_sum[m]+Watermark_Attacked[m];
				EmbeddingRegionTotalSum=EmbeddingRegionTotalSum+EmbeddingRegionSumSequence[f];					
							
  }

			   for (m=0;m<64;m++)
			   {
                if (watermark_attacked_sum[m]>=(EmbeddingRegionTotalSum/2)) 
				    Watermark_Attacked[m]=1;
				else
                    Watermark_Attacked[m]=0;
			   }
	
		   correct_bit_number_attacked=0;
		   correct_rate_attacked=0.0;
	       for (m=0;m<64;m++)
		   { 
			    if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
                   correct_bit_number_attacked=correct_bit_number_attacked+1; 
		   }
		   correct_rate_attacked=correct_bit_number_attacked/64.0*100.0;

           PositionFlag_AttackedWatermarkSequence=1;

}

void CREADAVIDoc::OnImageEdgeEnhancingFunction()
{
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;

   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }
   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;

   int i,j,temp_result;
   if (PositionFlag==1)//原始视频区域
	{
	   //四周的象素保持不动
	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
		  {
			   temp_result=(10*FrameBytes[101376-(i+1)*352+j]
			                -FrameBytes[101376-(i+1)*352+j+1]-FrameBytes[101376-(i+1)*352+j-1]
							-FrameBytes[101376-i*352+j]-FrameBytes[101376-i*352+j-1]-FrameBytes[101376-i*352+j+1]
							-FrameBytes[101376-(i+2)*352+j]-FrameBytes[101376-(i+2)*352+j-1]
							-FrameBytes[101376-(i+2)*352+j+1])/2;
		                                        //  -1 -1 -1
		                                        //  -1 10 -1  /2
		                                        //  -1 -1 -1
			   
			   if (temp_result>255)
				   temp_result=255;
               if (temp_result<0)
                   temp_result=0;
			   temp_FrameBytes[101376-(i+1)*352+j]=temp_result;
		  }
	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
	        FrameBytes[101376-(i+1)*352+j]=temp_FrameBytes[101376-(i+1)*352+j];
       
	    
	}
   GlobalFree(mem_temp_FrameBytes);
}

void CREADAVIDoc::OnImageBlurFunction()
{
   unsigned char *temp_FrameBytes,*temp_FrameBytes_Attacked;
   void *mem_temp_FrameBytes,*mem_temp_FrameBytes_Attacked;

   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);
   mem_temp_FrameBytes_Attacked=GlobalAlloc(GMEM_FIXED,101376);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }
   
   if(mem_temp_FrameBytes_Attacked==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes_Attacked);
	  return;
   }
   
   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   temp_FrameBytes_Attacked=(unsigned char *)mem_temp_FrameBytes_Attacked;  

   int i,j;
   if (PositionFlag==1)//原始视频区域
	{
	   //四周的象素保持不动
	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
		  {
			  temp_FrameBytes[101376-(i+1)*352+j]=(FrameBytes[101376-(i+1)*352+j]
			                                +FrameBytes[101376-(i+1)*352+j+1]+FrameBytes[101376-(i+1)*352+j-1]
											+FrameBytes[101376-i*352+j]+FrameBytes[101376-i*352+j-1]+FrameBytes[101376-i*352+j+1]
											+FrameBytes[101376-(i+2)*352+j]+FrameBytes[101376-(i+2)*352+j-1]
											+FrameBytes[101376-(i+2)*352+j+1])/9;
		                                        //  1 1 1
		                                        //  1 1 1  /9
	                                            //  1 1 1 
              if (temp_FrameBytes[101376-(i+1)*352+j]>255)
				 temp_FrameBytes[101376-(i+1)*352+j]=255;
			  if (temp_FrameBytes[101376-(i+1)*352+j]<0)
				 temp_FrameBytes[101376-(i+1)*352+j]=0;
		  }

	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
	        FrameBytes[101376-(i+1)*352+j]=temp_FrameBytes[101376-(i+1)*352+j];
       
	    
   }
   
   if (PositionFlag_AttackedVideo==1)
   {
       //四周的象素保持不动
	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
		  {
		    /* temp_FrameBytes_Attacked[101376-(i+1)*352+j]=(FrameBytes_Attacked[101376-(i+1)*352+j]
			                                +FrameBytes_Attacked[101376-(i+1)*352+j+1]+FrameBytes_Attacked[101376-(i+1)*352+j-1]
											+FrameBytes_Attacked[101376-i*352+j]+FrameBytes_Attacked[101376-i*352+j-1]+FrameBytes_Attacked[101376-i*352+j+1]
											+FrameBytes_Attacked[101376-(i+2)*352+j]+FrameBytes_Attacked[101376-(i+2)*352+j-1]
											+FrameBytes_Attacked[101376-(i+2)*352+j+1])/9;
		                                        //  1 1 1
		                                        //  1 1 1  /9
	                                            //  1 1 1 
	       */
            temp_FrameBytes_Attacked[101376-(i+1)*352+j]=(4*FrameBytes_Attacked[101376-(i+1)*352+j]
			                                +2*FrameBytes_Attacked[101376-(i+1)*352+j+1]+2*FrameBytes_Attacked[101376-(i+1)*352+j-1]
											+2*FrameBytes_Attacked[101376-i*352+j]+FrameBytes_Attacked[101376-i*352+j-1]+FrameBytes_Attacked[101376-i*352+j+1]
											+2*FrameBytes_Attacked[101376-(i+2)*352+j]+FrameBytes_Attacked[101376-(i+2)*352+j-1]
											+FrameBytes_Attacked[101376-(i+2)*352+j+1])/16;
		                                        //  1 2 1
		                                        //  2 4 2  /16
	                                            //  1 2 1 
		  }
		 for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
	        FrameBytes_Attacked[101376-(i+1)*352+j]=temp_FrameBytes_Attacked[101376-(i+1)*352+j];
   }

   GlobalFree(mem_temp_FrameBytes); 
   GlobalFree(mem_temp_FrameBytes_Attacked); 
}

void CREADAVIDoc::OnImageEdgeDetection()
{
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;

   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }
   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   
   int i,j,Gx,Gy;
   double temp;
   if (PositionFlag==1)//原始视频区域
   {
	   //四周的象素保持不动
	   for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
		  {
			Gy=(2*FrameBytes[101376-(i+1)*352+j+1])-(2*FrameBytes[101376-(i+1)*352+j-1])
											-FrameBytes[101376-i*352+j-1]+FrameBytes[101376-i*352+j+1]
											-FrameBytes[101376-(i+2)*352+j-1]
											+FrameBytes[101376-(i+2)*352+j+1];
		                                        //  -1 0 1
		                                        //  -2 0 2  
		                                        //  -1 0 1  
            Gx=(2*FrameBytes[101376-i*352+j])+FrameBytes[101376-i*352+j-1]+FrameBytes[101376-i*352+j+1]
											-(2*FrameBytes[101376-(i+2)*352+j])-FrameBytes[101376-(i+2)*352+j-1]
											-FrameBytes[101376-(i+2)*352+j+1];
		                                        //   1  2  1
		                                        //   0  0  0  
		                                        //  -1 -2 -1
			temp=(Gx*Gx)+(Gy*Gy);
		    temp_FrameBytes[101376-(i+1)*352+j]=(unsigned char)sqrt(temp);
			if (temp_FrameBytes[101376-(i+1)*352+j]>255)
				temp_FrameBytes[101376-(i+1)*352+j]=255;
			if (temp_FrameBytes[101376-(i+1)*352+j]<0)
				temp_FrameBytes[101376-(i+1)*352+j]=0;
		  }

      for (i=1;i<287;i++)
		  for (j=1;j<351;j++) 
	        FrameBytes[101376-(i+1)*352+j]=temp_FrameBytes[101376-(i+1)*352+j];
   } 
   GlobalFree(mem_temp_FrameBytes); 
}

void CREADAVIDoc::OnFrameDifferenceFunction()
{
     int i;
	  
	 if ((PositionFlag==1)&&(PositionFlag_Watermarked==1))
	 {  
	    for (i=0;i<101376;i++)
          //FrameBytes_Difference[i]=(unsigned char)(abs(FrameBytes[i]-FrameBytes_Embedding[i])*15);
		{ 
          if (FrameBytes[i]!=FrameBytes_Embedding[i]) 
		      FrameBytes_Difference[i]=255;
		  else
              FrameBytes_Difference[i]=0;
		}   
		PositionFlag_Difference=1;
	 }
      
	 if ((PositionFlag==1)&&(PositionFlag_AttackedVideo==1))
	 {  
	    for (i=0;i<101376;i++)
		  // FrameBytes_Difference2[i]=(unsigned char)(abs(FrameBytes[i]-FrameBytes_Attacked[i]));
		{
			if (FrameBytes[i]!=FrameBytes_Attacked[i])
			    FrameBytes_Difference2[i]=255;
            else
                FrameBytes_Difference2[i]=0;
		}
		PositionFlag_Difference2=1;
	 }

}

void CREADAVIDoc::OnSave_CurrentOriginalFrame()
{
   	//Save the current original frame 
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_CurrentOriginalFrame[_MAX_PATH],FileTitle_CurrentOriginalFrame[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_CurrentOriginalFrame[0]='\0';
   FileTitle_CurrentOriginalFrame[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentOriginalFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentOriginalFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
	   return;
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_CurrentOriginalFrame,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_CurrentOriginalFrame)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_CurrentOriginalFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentOriginalFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_CurrentOriginalFrame,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Current Frame File Error");
	  return;
   }
	 
   	long int fh;
	if (FrameSizeInfo[0]==96) //352*288
    {
	   //open the standard header file 注意这里只考虑了灰度图象的情况
	   fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	   if(fh==-1)
	   {
		 MessageBox(NULL,"标准[256GrayBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);		
	   }
       //Read file
       if (_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
	   {
	      _close(fh);   
          MessageBox(NULL,"读取标准[256GrayBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	   }
       _close(fh);  
       fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
       fwrite(FrameBytes,101376,1,fp);
	}

	if (FrameSizeInfo[0]==208) //720*576
    {
	   //open the standard header file 注意这里只考虑了灰度图象的情况
	   fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader720x576.bmp",_O_RDONLY|_O_BINARY);
	   if(fh==-1)
	   {
		 MessageBox(NULL,"标准[256GrayBmpHeader720x576.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);		
	   }
       //Read file
       if (_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
	   {
	      _close(fh);   
          MessageBox(NULL,"读取标准[256GrayBmpHeader720x576.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	   }
       _close(fh);  
       fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
       fwrite(FrameBytes,414720,1,fp);
	}

   fclose(fp);
}

void CREADAVIDoc::OnImagePredictionFilter()
{
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;

   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }
   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   
   int pixel_value;//注意是整型数据 

   int i,j;
   if (PositionFlag==1)//原始视频区域
   {
	   //四周的象素保持不动
	   for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
		  {
		
		    //temp_FrameBytes[101376-(i+1)*352+j]//
			  pixel_value=((12*FrameBytes[101376-(i+1)*352+j])-FrameBytes[101376-(i+1)*352+j-1]
				                            -FrameBytes[101376-(i+1)*352+j-2]-FrameBytes[101376-(i+1)*352+j-3]
											-FrameBytes[101376-(i+1)*352+j+1]-FrameBytes[101376-(i+1)*352+j+2]
											-FrameBytes[101376-(i+1)*352+j+3]-FrameBytes[101376-(i+2)*352+j]
											-FrameBytes[101376-(i+3)*352+j]-FrameBytes[101376-(i+4)*352+j] 
											-FrameBytes[101376-i*352+j]-FrameBytes[101376-(i-1)*352+j]
											-FrameBytes[101376-(i-2)*352+j])/12;
										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
		  

		 /* pixel_value=((4*FrameBytes[101376-(i+1)*352+j])-(2*FrameBytes[101376-(i+1)*352+j-1])
					  -(2*FrameBytes[101376-(i+1)*352+j+1])-(2*FrameBytes[101376-(i+2)*352+j])
					  -(2*FrameBytes[101376-i*352+j])+FrameBytes[101376-i*352+j+1]
					  +FrameBytes[101376-i*352+j-1]+FrameBytes[101376-(i+2)*352+j+1]
					  +FrameBytes[101376-(i+2)*352+j-1])/4;
										 
		                                    
		                                     //         1  -2  1 
			                                 //(1/4)   -2   4 -2 
			                                 //         1  -2  1 
		 */                                   

		  pixel_value=pixel_value+128;
			if (pixel_value<0)
               pixel_value=0;
			if (pixel_value>255)
               pixel_value=255;
  
			temp_FrameBytes[101376-(i+1)*352+j]=(unsigned char)pixel_value;
			
		  }

      for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
	        FrameBytes[101376-(i+1)*352+j]=temp_FrameBytes[101376-(i+1)*352+j];
   } 
   GlobalFree(mem_temp_FrameBytes); 

}

void CREADAVIDoc::OnSaveDifferenceimage() 
{
	// TODO: Add your command handler code here
   //Save the differenceimage file
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_DifferenceImage[_MAX_PATH],FileTitle_DifferenceImage[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_DifferenceImage[0]='\0';
   FileTitle_DifferenceImage[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_DifferenceImage; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_DifferenceImage;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
//	   strcpy(FileName_CurrentEmbeddingFrame,temp0);
//	   strcat(FileName_CurrentEmbeddingFrame,"\\temp.bmp");
//	   strcpy(FileTitle_CurrentEmbeddingFrame,"temp.avi");
	   return;
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_DifferenceImage,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_DifferenceImage)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_DifferenceImage);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_DifferenceImage,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_DifferenceImage,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Current Frame File Error");
	  return;
   }
	 
   	long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"标准[256GrayBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"读取标准[256GrayBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	}
    _close(fh);  

   fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
   
   fwrite(FrameBytes_Difference,101376,1,fp);

   fclose(fp);

}

void CREADAVIDoc::OnTimeAxisWatermarkEmbedding()
{
   //Save the watermarked avi file
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName[0]='\0';
   FileTitle[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI文件 *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存AVI文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作!");
	   return;
   }

   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   
    int i,j,k,l,m,n;
    int flag_0=0,flag_1=0,flag_2=0,flag_3=0;

    FILE *fp;
    fp=fopen(FileName,"wb");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("File Error");
		return;
	}
	 
    fwrite(AVIFileInfo,4096,1,fp);

	double DifferenceSum;//象素差
	int OriginalPixelValueSum;//原始象素的和，用来计算SNR
//	int temp_conversion;
	unsigned char Frame_Block[1584][8][8];//分块后的图像数据
	double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *BlockDCT_Coefficient_Saved[1584][8];//前一帧的分块DCT系数
	double *temp_DC_Coefficient_Difference;//前后帧相应的DCT直流系数之差
    double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double *temp_DCT_Edge_Detection;//在DCT域判断是否有边缘存在
	double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值
	int flag_DC_Coefficient,flag_AC_Coefficient;	 

	//分配内存
	void *mem;
	void *mem_DC_Difference;
    void *mem_DCT_Variance;
    void *mem_AC_Energy;
    void *mem_DCT_Edge_Detection;
	
	mem=GlobalAlloc(GMEM_FIXED,101376*8);
    mem_DC_Difference=GlobalAlloc(GMEM_FIXED,1584*8);	
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);
	mem_DCT_Edge_Detection=GlobalAlloc(GMEM_FIXED,1584*8);

	if(mem==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem);
	  return;
	}
    
    if(mem_DC_Difference==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DC_Difference);
	  return;
	}
   
	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	if(mem_DCT_Edge_Detection==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Edge_Detection);
	  return;
	}

	for (k=0;k<1584;k++)
      for (j=0;j<8;j++)
	  {
	   	 BlockDCT_Coefficient_Saved[k][j]=(double *)mem+k*64+j*8;
	  }

    temp_DC_Coefficient_Difference=(double *)mem_DC_Difference;
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;
    temp_DCT_Edge_Detection=(double *)mem_DCT_Edge_Detection;
	
	for (i=0;i<66;i++)
		m_HammingCodedOriginalWatermarkInformationSequence[i]=m_OriginalWatermarkInformationSequence[i];
	//对水印信息进行汉明编码 66bits->90bits
    //m_OriginalWatermarkInformationSequence->m_HammingCodedOriginalWatermarkInformationSequence
    //先进行分组，信息位11位，加上4位监督位
/*    unsigned char temp_hammingcode[15];
	for (i=0;i<6;i++)
	{
		for (j=0;j<11;j++)
           temp_hammingcode[j]=m_OriginalWatermarkInformationSequence[i*11+j];
	    
		//监督矩阵
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        temp_hammingcode[11]=(temp_hammingcode[0]+temp_hammingcode[1]
			                 +temp_hammingcode[2]+temp_hammingcode[3]
							 +temp_hammingcode[5]+temp_hammingcode[7]
							 +temp_hammingcode[8])%2;//模2加
        temp_hammingcode[12]=(temp_hammingcode[1]+temp_hammingcode[2]
			                 +temp_hammingcode[3]+temp_hammingcode[4]
							 +temp_hammingcode[6]+temp_hammingcode[8]
							 +temp_hammingcode[9])%2;//模2加
        temp_hammingcode[13]=(temp_hammingcode[2]+temp_hammingcode[3]
			                 +temp_hammingcode[4]+temp_hammingcode[5]
							 +temp_hammingcode[7]+temp_hammingcode[9]
							 +temp_hammingcode[10])%2;//模2加
        temp_hammingcode[14]=(temp_hammingcode[0]+temp_hammingcode[1]
			                 +temp_hammingcode[2]+temp_hammingcode[4]
							 +temp_hammingcode[6]+temp_hammingcode[7]
	                         +temp_hammingcode[10])%2;//模2加   
	    //编完码后进行存储
		for (j=0;j<15;j++)
           m_HammingCodedOriginalWatermarkInformationSequence[i*15+j]=temp_hammingcode[j];

	}
*/

	//用于检验经过滤波后相关的结果
//  m_HammingCodedOriginalWatermarkInformationSequence[84]=1;
//  m_HammingCodedOriginalWatermarkInformationSequence[85]=1;
//  m_HammingCodedOriginalWatermarkInformationSequence[86]=1;

/*	for (i=3;i<45;i++) 
	    m_HammingCodedOriginalWatermarkInformationSequence[i]=1;//相当于双极性的-1

	for (i=45;i<90;i++) 
	    m_HammingCodedOriginalWatermarkInformationSequence[i]=0;//相当于双极性的1
*/
	
	for (j=0;j<66;j++)
	{
        if ((m_HammingCodedOriginalWatermarkInformationSequence[j]!=1)&&
			(m_HammingCodedOriginalWatermarkInformationSequence[j]!=0))
			AfxMessageBox("水印位出错！");
	}

	
	//对90位的水印进行扩谱
    //产生伪随机序列

    int seed;//用来确定水印信息位的顺序


	for (i=0;i<22;i++)
	{
      seed=i+120;
      //seed=i+200;
      //PseudoRandomSequence_Generator(PseudoRandomSequence,1024,120);//31<32
      PseudoRandomSequence_Generator(PseudoRandomSequence,1024,seed);
	  //利用伪随机序列对水印进行调制  
      for (j=0;j<3;j++)
	  { 
        if (m_HammingCodedOriginalWatermarkInformationSequence[i*3+j]==1)//双极性的-1,伪随机序列反向
		{
		  for (k=(j*10);k<((j+1)*10);k++)
		  {
			  if (PseudoRandomSequence[k]==1)//相当于双极性的-1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=-1;//双极性的-1
			      m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=1;//相当于双极性的1
			  }
			  else //PseudoRandomSequence[k]==0，相当于双极性的1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=1;//双极性的1
				  m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=-1;//相当于双极性的-1
			  }
		  }
		}
		else//双极性的1,伪随机序列不变
		{
          for (k=(j*10);k<((j+1)*10);k++)
		  {
			  if (PseudoRandomSequence[k]==1)//相当于双极性的-1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=-1;//双极性的-1
				  m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=-1;//双极性的-1
			 
			  }
			  else //PseudoRandomSequence[k]==0，双极性的1
              {
				  m_SpreadSpectrumWatermarkSequence[i][k]=1;//双极性的1
			      m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=1;//双极性的1
			  }  
		  }
		}

/*	  if (m_OriginalWatermarkBytes[i]==1)
		  temp_conversion=-1;//1 -> -1
	  else
          temp_conversion=1;//0 -> 1
      if (PseudoRandomSequence[i]==1)
	      temp_conversion=temp_conversion*(-1);
	  else
		  temp_conversion=temp_conversion*1;
      if (temp_conversion==-1)
		  PseudoRandomSequence[i]=1;
	  else
          PseudoRandomSequence[i]=0;
*/
	  }
	}


	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);	

	for (l=0;l<1584;l++)
	{
	  temp_DC_Coefficient_Difference[l]=0.0;
      temp_DCT_Coefficient_Variance[l]=0.0;
	  temp_DCT_Edge_Detection[l]=0.0;
	}

	//计算水印嵌入的时间
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//灰度
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//显示DC系数的信息
			 flag_AC_Coefficient=0;//显示AC系数的信息
			 DifferenceSum=0.0;//针对每一帧都清零
			 OriginalPixelValueSum=0;//针对每一帧都清零
	         OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);

		
			 for (i=0;i<288;i++)//将图像数据倒过来
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//对当前帧进行8*8分块
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
       //       DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);

                if (k>=1)//比较前后两帧，这里不会出现误差累计的情况，因为前一帧的信息存储在DCT系数矩阵里
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//前后帧对应DCT系数的差值
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			 }

//*****************************************************************

			  int r=0; 
			  //经过判断后在相应的块中嵌水印
			
			  if (k==0)//第一帧单独处理,不参考邻近的帧
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				{ 
				   //if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//纹理复杂块 for table tennis improved2.avi 
				   if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//纹理复杂块 
				   {
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;

					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//标记出8*8方块的边界
					//	if (m==0)
					//		Frame_Block[l][m][n]=255;//标记出这样的块
                    //   if (m==7)
					//	    Frame_Block[l][m][n]=255;//标记出这样的块
                    //    if (n==0)
					//		Frame_Block[l][m][n]=255;//标记出这样的块
                    //    if (n==7)
					//    Frame_Block[l][m][n]=255;//标记出这样的块
               

                          
                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(0*8));//1111 0111
                        
					    //A bitplane position controller is defined.
					  //    if (!fmod(l,2))
                 //             Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequence[m*8+n]*8));//1111 0111
                      //    else
			          //        Frame_Block[l][m][n]=((Frame_Block[l][m][n]&239)+(PseudoRandomSequence[m*8+n]*16));//1110 1111

                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&223)+(PseudoRandomSequence[m*8+n]*32));//1101 1111
					}
				   } 
                
				}
			  }
              else
			  {
                r=0;  
                srand((unsigned)time(NULL));
                for (l=0;l<1584;l++)
				{
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//既是运动块也是纹理复杂块 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//是运动块 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//是纹理复杂块  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//交流分量的能量总和
               
	//	        if ((temp_DC_Coefficient_Difference[l]>=120)&&(temp_AC_Coefficient_Energy[l]>=16000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块  for table tennis improved2.avi 
                if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
//				if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1000))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1160)) 
				{  
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;
        
					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//标记出8*8方块的边界
					//    if (m==0)
					//		Frame_Block[l][m][n]=255;//标记出这样的块
                    //    if (m==7)
					//	    Frame_Block[l][m][n]=255;//标记出这样的块
                    //    if (n==0)
					//		Frame_Block[l][m][n]=255;//标记出这样的块
                    //    if (n==7)
					//	    Frame_Block[l][m][n]=255;//标记出这样的块
         
                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequence[m*8+n]*8));//1111 0111
          
						//if ((PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=0)&&(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=1))
						//{
						//	AfxMessageBox("Watermark Message Error!");
						//	return;
						//}
						//Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]*8));//1111 0111
					    //temp=Frame_Block[l][m][n]-temp;
					}
				}
			}
		}		
		
        //char temp_originalembeddingpoint[100];
		//sprintf(temp_originalembeddingpoint,"嵌入点原始象素值%d",Frame_Block[518+3*22][0][0]);
        //MessageBox(NULL,temp_originalembeddingpoint,"水印嵌入点原始象素值",MB_ICONSTOP|MB_OK);//Display the info

        //int temp_result;
        //temp_result=255;  //沿时间轴嵌入水印

		
		short int x,y;
		long int s1,s2;
		s1=13579;
		s2=139;
	

		for (i=0;i<22;i++)
		{
			if (m_ModulatedSpreadSpectrumWatermarkSequence[i][k]==1)//1-> 0
			{   
				for (j=0;j<3;j++)
				{
					x=(int)uniform(0.0,10.0,&s1);//产生在[0.0,10.0]区间内均匀分布的随机数
				    y=(int)uniform(0.0,8.0,&s2);//产生在[0.0,10.0]区间内均匀分布的随机数
					Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]=((Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]&223)+(0*32));   //沿时间轴嵌入水印,注意必须加括号
				}
			}
			if (m_ModulatedSpreadSpectrumWatermarkSequence[i][k]==-1)//-1-> 1
			{
				for (j=0;j<3;j++)
				{
					x=(int)uniform(0.0,10.0,&s1);//产生在[0.0,10.0]区间内均匀分布的随机数
				    y=(int)uniform(0.0,8.0,&s2);//产生在[0.0,10.0]区间内均匀分布的随机数
					Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]=((Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]&223)+(1*32));   //沿时间轴嵌入水印,注意必须加括号
				}
			}
		/*   if (i==0)
		   {
			 for (j=-1;j<2;j++)
			{
			 temp_result=Frame_Block[473+(i/2*44)+j*2][i%2*4][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
     
             if (temp_result<0)
		         Frame_Block[473+(i/2*44)+j*2][i%2*4][0]=0;
		     else
			 {
		         if (temp_result>255)
                     Frame_Block[473+(i/2*44)+j*2][i%2*4][0]=255;
	             else
		             Frame_Block[473+(i/2*44)+j*2][i%2*4][0]=temp_result;
			 }
			}

		   }
           else
		   {
		    for (j=-1;j<2;j++)
			{
			 temp_result=Frame_Block[473+(i/2*44)+j][i%2*4][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
     
             if (temp_result<0)
		         Frame_Block[473+(i/2*44)+j][i%2*4][0]=0;
		     else
			 {
		         if (temp_result>255)
                     Frame_Block[473+(i/2*44)+j][i%2*4][0]=255;
	             else
		             Frame_Block[473+(i/2*44)+j][i%2*4][0]=temp_result;
			 }
			}
           } */       
          //判断是否位于图象的中间区域 200x160
          //if (((518+i*5)/44*8)<76)||((518+i*5)/44*8)>275)||
		  //  ((518+i*5)l%44*8)<64)||((518+i*5)l%44*8)>223))
          
         //if ((i!=3)&&(i!=5)&&(i!=23))	// for table_g1_frames_2.avi 	
     /*      if ((i!=1)&&(i!=3)&&(i!=5)&&(i!=23)&&(i!=7)&&(i!=9))	// for table_g1_frames_2.avi 	
          {
		     //temp_result=255;  //沿时间轴嵌入水印 
		     temp_result=Frame_Block[518+i*22][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
             // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
             if (temp_result<0)
               //Frame_Block[1500][0][0]=0;
		         Frame_Block[518+i*22][0][0]=0;
		     else
			 {
		         if (temp_result>255)
                     Frame_Block[518+i*22][0][0]=255;
	             else
		             Frame_Block[518+i*22][0][0]=temp_result;
			 }
		 }
		 else
		 {	 
		
			    //temp_result=255;  //沿时间轴嵌入水印 
		        temp_result=Frame_Block[518+i*22+6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
                if (temp_result<0)
                //Frame_Block[1500][0][0]=0;
		           Frame_Block[518+i*22+6][0][0]=0;
		        else
				{
		           if (temp_result>255)
                       Frame_Block[518+i*22+6][0][0]=255;
	               else
		               Frame_Block[518+i*22+6][0][0]=temp_result;
				}
		}  
	*/	 
          

     /*    if ((i!=3)&&(i!=5)&&(i!=19)&&(i!=21)&&(i!=23)&&(i!=25)&&(i!=27)&&(i!=28)&&(i!=29))	
         {
		  //temp_result=255;  //沿时间轴嵌入水印 
		  temp_result=Frame_Block[518+i*22][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
          // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
          if (temp_result<0)
          //Frame_Block[1500][0][0]=0;
		    Frame_Block[518+i*22][0][0]=0;
		  else
		  {
		    if (temp_result>255)
              Frame_Block[518+i*22][0][0]=255;
	        else
		      Frame_Block[518+i*22][0][0]=temp_result;
		  }
		 }
		 else
		 {
			 if(i==28)
			 {
                	 
		        //temp_result=255;  //沿时间轴嵌入水印 
		        temp_result=Frame_Block[518+i*22-6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
                if (temp_result<0)
                //Frame_Block[1500][0][0]=0;
		        Frame_Block[518+i*22-6][0][0]=0;
		        else
				{
		          if (temp_result>255)
                      Frame_Block[518+i*22-6][0][0]=255;
	              else
		              Frame_Block[518+i*22-6][0][0]=temp_result;
				}

			 }
			 if(i==23)
			 {
                	 
		        //temp_result=255;  //沿时间轴嵌入水印 
		        temp_result=Frame_Block[518+i*22+12][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
                if (temp_result<0)
                //Frame_Block[1500][0][0]=0;
		        Frame_Block[518+i*22+12][0][0]=0;
		        else
				{
		          if (temp_result>255)
                      Frame_Block[518+i*22+12][0][0]=255;
	              else
		              Frame_Block[518+i*22+12][0][0]=temp_result;
				}

			 }
			 if(i==25)
			 {
                	 
		           //temp_result=255;  //沿时间轴嵌入水印 
		           temp_result=Frame_Block[518+i*22+19][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
                   // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
                   if (temp_result<0)
                   //Frame_Block[1500][0][0]=0;
		           Frame_Block[518+i*22+19][0][0]=0;
		           else
				   {
		             if (temp_result>255)
                         Frame_Block[518+i*22+19][0][0]=255;
	                 else
		                 Frame_Block[518+i*22+19][0][0]=temp_result;
				   }
			}
			if ((i==3)||(i==5)||(i==19)||(i==21)||(i==27)||(i==29))
			{
		           //temp_result=255;  //沿时间轴嵌入水印 
		           temp_result=Frame_Block[518+i*22+6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //沿时间轴嵌入水印
                   // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //沿时间轴嵌入水印
		
                   if (temp_result<0)
                   //Frame_Block[1500][0][0]=0;
		               Frame_Block[518+i*22+6][0][0]=0;
		           else
				   {
		               if (temp_result>255)
                           Frame_Block[518+i*22+6][0][0]=255;
	                   else
		                   Frame_Block[518+i*22+6][0][0]=temp_result;
				   }
			}	 
		 }
*/
}
		//char temp_embeddingpoint[100];
		//sprintf(temp_embeddingpoint,"嵌入点象素值%d",Frame_Block[518+3*22][0][0]);
        //MessageBox(NULL,temp_embeddingpoint,"水印嵌入点",MB_ICONSTOP|MB_OK);//Display the info
		

        EmbeddingRegionSumSequence[k]=r; 

     	//将分块后的数据还原为整幅数据
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //计算相应的PSNR(大约耗时7秒钟)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//用来计算PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);
	
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//将数据按倒序存储
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("水印嵌入过程结束!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"水印嵌入过程：%lf秒",m_time);
          MessageBox(NULL,temp_embeddingtime,"水印嵌入完成!",MB_OK);

		  //AVI文件后面的INDEX部分
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//利用FrameBytes_Embedding数组
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI文件已成功保存");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //彩色
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//嵌入水印的序列对应的位置
       FrameNumber_Watermarked=0;//首先将文件的位置初始化

	   GlobalFree(mem);
	   GlobalFree(mem_DC_Difference);
	   GlobalFree(mem_DCT_Variance);
	   GlobalFree(mem_AC_Energy);
	   GlobalFree(mem_DCT_Edge_Detection);
}

void CREADAVIDoc::OnExtract_WatermarkSequenceAlongTimeAxis()
{
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;
   void *mem_temp_ConvolutionResult;
   short int Extracted_WatermarkSequenceAlongTimeAxis[30];
// short int Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[30];
// short int Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[30];
   short int *temp_ConvolutionResult;//经过和高通滤波器卷积后，所得的结果有正有负
   int correlationresult_threshold;
   short int correlationresult,correlationresult0,correlationresult1,correlationresult2;
  
   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);
   mem_temp_ConvolutionResult=GlobalAlloc(GMEM_FIXED,101376*2);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }

   if(mem_temp_ConvolutionResult==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_ConvolutionResult);
	  return;
   }

   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   temp_ConvolutionResult=(short int *)mem_temp_ConvolutionResult;

   FILE *fp_txt;
   fp_txt=fopen("D:\\CorrelationDetectionResult.txt","w");
   if(fp_txt==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Write Record File Error");
		return ;
	}
	fprintf(fp_txt,"不同位置对应的相关检测结果");
   
// unsigned char Frame_Block[1584][8][8];//分块后的图像数据
   int i,j,k,tx,ty,pn_number;
   FILE *fp;
   fp=fopen(FileName,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Watermarked File Error");
	  return;
   }
 
   
   
   for (i=0;i<90;i++)
       m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i]=0;
   for (i=0;i<66;i++)
       m_ExtractedOriginalWatermarkInformationSequence[i]=0;
   
   correlationresult_threshold=0;
   
 //   for (ty=3;ty<285;ty++)
 //     for (tx=3;tx<349;tx++)
	   
//	for (ty=64;ty<224;ty++)
//     for (tx=76;tx<276;tx++)

//    for (ty=104;ty<105;ty++)
//     for (tx=96;tx<97;tx++)


//     for (ty=200;ty<273;ty++)
//       for (tx=3;tx<50;tx++)

   //PN1
   // for (ty=88;ty<89;ty++)
   //   for (tx=272;tx<273;tx++) 
 
   //PN2
   //for (ty=96;ty<97;ty++)
   //   for (tx=96;tx<97;tx++) 

   //PN3
   //for (ty=96;ty<97;ty++)
   //   for (tx=272;tx<273;tx++)    
   
   //PN4
   //PN5  
   

//  for (pn_number=0;pn_number<30;pn_number++)
   //计算水印检测与提取的时间
   time_t ltime1,ltime2;
   time(&ltime1);
   short int temp_bitplane;

   pn_number=19;
//   int temp_ty=0,temp_tx=0;
//   temp_ty=(180+pn_number*44)/44*8;
//   temp_tx=(180+pn_number*44)%44*8;

//   for (ty=temp_ty;ty<(temp_ty+1);ty++)
//       for (tx=temp_tx;tx<(temp_tx+1);tx++)  
//   for (ty=20;ty<224;ty++)
//     for (tx=76;tx<276;tx++)  
	 
   for (ty=100;ty<120;ty++)
     for (tx=20;tx<320;tx++)  
	 {
		 fseek(fp,4096L,0);
		 correlationresult=0;
         correlationresult0=0;
         correlationresult1=0;
		 correlationresult2=0;
	

		 for (k=0;k<30;k++)
		 { 
			 //pixelvaluesum=0;
			 fseek(fp,8L,1);//每一帧的数据长度（8+101376）bytes
             fread(FrameBytes_ForWatermarkExtraction,101376,1,fp);

	/*		 for (i=0;i<288;i++)//将图像数据倒过来
			 {
				 fseek(fp,-352L,1);
                 fread(FrameBytes_Inserting[i],352,1,fp);
				 fseek(fp,-352L,1);
			 }
			   
			 fseek(fp,101376L,1);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//对当前帧进行8*8分块
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}
			 }
   */
    //char temp_originalembeddingpoint[100];
    //sprintf(temp_originalembeddingpoint,"检测点象素值%d",FrameBytes_ForWatermarkExtraction[(287-temp_ty)*352+temp_tx]);
    //MessageBox(NULL,temp_originalembeddingpoint,"水印检测（未经过高通滤波前）",MB_OK);//Display the info
	
	//先对temp_ConvolutionResult数组赋初值 
  //  for (i=0;i<288;i++)
  //	for (j=0;j<352;j++) 
  //         temp_ConvolutionResult[101376-(i+1)*352+j]=FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j];
	
	//先进行高通滤波的处理
     
    //四周的象素保持不动
/*	   for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
		  {
			//temp_FrameBytes[101376-(i+1)*352+j]//
			  temp_ConvolutionResult[101376-(i+1)*352+j]=((12*FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j])-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-1]
				                                         -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-2]-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-3]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+1]-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+2]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+3]-FrameBytes_ForWatermarkExtraction[101376-(i+2)*352+j]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+3)*352+j]-FrameBytes_ForWatermarkExtraction[101376-(i+4)*352+j] 
											             -FrameBytes_ForWatermarkExtraction[101376-i*352+j]-FrameBytes_ForWatermarkExtraction[101376-(i-1)*352+j]
											             -FrameBytes_ForWatermarkExtraction[101376-(i-2)*352+j])/12;
										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
		 // if ((temp_ConvolutionResult[101376-(i+1)*352+j]<-60)||(temp_ConvolutionResult[101376-(i+1)*352+j]>60))
		 //	  pixelvaluesum=pixelvaluesum+1;
		 

		     //  if (pixel_value<0)
             //   pixel_value=0;
		     //if (pixel_value>255)
             //   pixel_value=255;
  
			 //temp_FrameBytes[101376-(i+1)*352+j]=(unsigned char)pixel_value;
          

		  }
*/
//      Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]=FrameBytes_ForWatermarkExtraction[101024-ty*352+tx];//准备沿时间轴进行滤波

// 7x7 
/*	  Extracted_WatermarkSequenceAlongTimeAxis[k]=((12*FrameBytes_ForWatermarkExtraction[101024-ty*352+tx])-FrameBytes_ForWatermarkExtraction[101023-ty*352+tx]
				                                         -FrameBytes_ForWatermarkExtraction[101022-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101021-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[101025-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101026-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[101027-ty*352+tx]-FrameBytes_ForWatermarkExtraction[100672-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[100320-ty*352+tx]-FrameBytes_ForWatermarkExtraction[99968-ty*352+tx] 
											             -FrameBytes_ForWatermarkExtraction[101376-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101728-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[102080-ty*352+tx])/12;
*/										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 


// 3x3


/*	Extracted_WatermarkSequenceAlongTimeAxis[k]=((4*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx])-(2*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx-1])
					  -(2*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx+1])-(2*FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx])
					  -(2*FrameBytes_ForWatermarkExtraction[101376-ty*352+tx])+FrameBytes_ForWatermarkExtraction[101376-ty*352+tx+1]
					  +FrameBytes_ForWatermarkExtraction[101376-ty*352+tx-1]+FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx+1]
					  +FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx-1])/4;
										 
		                                    
		                                     //         1  -2  1 
			                                 //(1/4)   -2   4 -2 
			                                 //         1  -2  1 
        
	  if (Extracted_WatermarkSequenceAlongTimeAxis[k]<-20)
		  Extracted_WatermarkSequenceAlongTimeAxis[k]=-20;
      if (Extracted_WatermarkSequenceAlongTimeAxis[k]>20)
		  Extracted_WatermarkSequenceAlongTimeAxis[k]=20;


	  if (k<10)	  
	     correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult2;
	  }
*/
    temp_bitplane=((FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx]&32)>>5);
    if ((temp_bitplane)==0)
       Extracted_WatermarkSequenceAlongTimeAxis[k]=1;
    if ((temp_bitplane)==1)
       Extracted_WatermarkSequenceAlongTimeAxis[k]=-1;

	 if (k<10)	  
	     correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult2;
	  } 
  
    //找到当前帧嵌入的点
    //[l/44*8+m][l%44*8+n]->[1500/44*8+0][1500%44*8+0]->[272][32]
    //Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-272)*352+32];

//    Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-ty)*352+tx];
  
	//char temp_embeddingpoint[100];
    //sprintf(temp_embeddingpoint,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis[k]);
    //MessageBox(NULL,temp_embeddingpoint,"水印检测（经过高通滤波后）",MB_ICONSTOP|MB_OK);//Display the info
 

   }
   //考虑进行一维预测滤波
/* //[-1 -1 -1 6 -1 -1 -1]/6
   //序列的两边进行零阶扩展0,1,2,27,28,29
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[0]=(3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3])/6; 
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[1]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4])/6;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[2]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/6;

   for(k=3;k<27;k++)
   { 
      Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-1]
	                                                             -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-3]
														         -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+2]
	           											         -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+3])/6;
   }

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[27]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24])/6;

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[28]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25])/6;
   
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[29]=(3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26])/6; 


   //[-1 -1 -1 -1 8 -1 -1 -1 -1]/8
   //序列的两边进行零阶扩展0,1,2,3,26,27,28,29
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[0]=(4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4])/8; 
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[1]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/8;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[2]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/8;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[3]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[6]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[7])/8;


   for(k=4;k<26;k++)
   { 
      Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-1]
	                                                             -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-3]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+1]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+3]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+4])/8;
   }

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[26]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[22])/8;


   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[27]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23])/8;

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[28]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24])/8;
   
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[29]=(4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25])/8; 

     //[1 -1 2 -4 8 -4 2 -1 1]/16
   //序列的两边进行零阶扩展0,1,2,3,26,27,28,29
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[0]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]
	                                                          +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
															  +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4])/16; 
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[1]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]+Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/16;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[2]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/16;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[3]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
	                                                          +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]
														      -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[6]+Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[7])/16;


   for(k=4;k<26;k++)
   { 
      Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-1]
	                                                             +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-3]
																 +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-4]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+1]
																 +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+3]
																 +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+4])/16;
   }

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[26]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
	                                                           +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]
														       -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23]+Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[22])/16;


   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[27]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23])/16;

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[28]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]+2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]+Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24])/16;
   
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[29]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]
	                                                           +2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
															   +Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25])/16; 

  
   correlationresult=0;
   correlationresult0=0;
   correlationresult1=0;
   correlationresult2=0;
   
   for (k=0;k<30;k++)
   {
      if (Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]<-20)
		  Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=-20;
      if (Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]>20)
		  Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=20;

	  //char temp_embeddingpoint_1D[100];
      //sprintf(temp_embeddingpoint_1D,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]);
      //MessageBox(NULL,temp_embeddingpoint_1D,"水印检测（未经过一维预测滤波前）",MB_ICONSTOP|MB_OK);//Display the info
      //sprintf(temp_embeddingpoint_1D,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]);
      //MessageBox(NULL,temp_embeddingpoint_1D,"水印检测（经过一维预测滤波后）",MB_ICONSTOP|MB_OK);//Display the info
  

      if (k<10)	  
	     correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult2;
	  }
   }
*/
     

   
/*    correlationresult=abs(correlationresult0)+abs(correlationresult1)+abs(correlationresult2); 
    correlationresult=correlationresult/10;
	if (correlationresult>=48)
	{
    if (correlationresult0>0)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=0;
    if (correlationresult0<0)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=1;

    if (correlationresult1>0)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=0;
    if (correlationresult1<0)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=1;

    if (correlationresult2>0)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=0;
    if (correlationresult2<0)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=1;

	correlationresult_threshold=correlationresult_threshold+1;
	pn_number=pn_number+1;
	}
*/

    correlationresult=abs(correlationresult0)+abs(correlationresult1)+abs(correlationresult2); 
	if (correlationresult==30)
	{
    if (correlationresult0==10)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=0;
    if (correlationresult0==-10)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=1;

    if (correlationresult1==10)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=0;
    if (correlationresult1==-10)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=1;

    if (correlationresult2==10)//双极性的1-单极性的0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=0;
    if (correlationresult2==-10)//双极性的-1-单极性的1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=1;

	correlationresult_threshold=correlationresult_threshold+1;
	pn_number=pn_number+1;	
	}

	if (pn_number>22)//避免数组超界（共22条伪随机序列）
        pn_number=22;
   	  //char temp_correlationresult[80];
	  //sprintf(temp_correlationresult,"相关结果%d",correlationresult);
      //MessageBox(NULL,temp_correlationresult,"",MB_OK)

     fprintf(fp_txt,"\n%3d     ",correlationresult);
//	 if (correlationresult>30)
//		 AfxMessageBox("Error!");
	}

    for (i=0;i<66;i++)
	  m_ExtractedOriginalWatermarkInformationSequence[i]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i];
	//检验汉明码的纠错能力
	//可以纠一位错，出现两位错的时候误码率上升
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]=0;
	//m_ExtractedHammingCodedOriginalWatermarkInformationSequence[2]=0;

	//汉明编码对应的纠错过程
    //对提取的信息进行纠错 90bits->66bits
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence->m_ExtractedOriginalWatermarkInformationSequence
    //先进行分组，信息位11位，加上4位监督位 90位分成6组
/*    unsigned char temp_hammingcode[15];
	int s1,s2,s3,s4;
	for (i=0;i<6;i++)
	{
		for (j=0;j<15;j++)
           temp_hammingcode[j]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i*15+j];
	    
		//监督矩阵
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        s1=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[5]+temp_hammingcode[7]
		    +temp_hammingcode[8]+temp_hammingcode[11])%2;//模2加
        s2=(temp_hammingcode[1]+temp_hammingcode[2]
			+temp_hammingcode[3]+temp_hammingcode[4]
	        +temp_hammingcode[6]+temp_hammingcode[8]
	        +temp_hammingcode[9]+temp_hammingcode[12])%2;//模2加
        s3=(temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[4]+temp_hammingcode[5]
            +temp_hammingcode[7]+temp_hammingcode[9]
		    +temp_hammingcode[10]+temp_hammingcode[13])%2;//模2加
        s4=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[4]
            +temp_hammingcode[6]+temp_hammingcode[7]
	        +temp_hammingcode[10]+temp_hammingcode[14])%2;//模2加   
	    
		//针对11位信息位
		//利用校验子纠错 错误图样
        if ((s1==1)&&(s2==0)&&(s3==0)&&(s4==1))
		    temp_hammingcode[0]=1-temp_hammingcode[0];//0->1  1->0
        if ((s1==1)&&(s2==1)&&(s3==0)&&(s4==1))	
            temp_hammingcode[1]=1-temp_hammingcode[1];//0->1  1->0
		if ((s1==1)&&(s2==1)&&(s3==1)&&(s4==1))
			temp_hammingcode[2]=1-temp_hammingcode[2];//0->1  1->0
        if ((s1==1)&&(s2==1)&&(s3==1)&&(s4==0))	
		    temp_hammingcode[3]=1-temp_hammingcode[3];//0->1  1->0	    
	    if ((s1==0)&&(s2==1)&&(s3==1)&&(s4==1))
			temp_hammingcode[4]=1-temp_hammingcode[4];//0->1  1->0
		if ((s1==1)&&(s2==0)&&(s3==1)&&(s4==0))
            temp_hammingcode[5]=1-temp_hammingcode[5];//0->1  1->0  
	    if ((s1==0)&&(s2==1)&&(s3==0)&&(s4==1))
            temp_hammingcode[6]=1-temp_hammingcode[6];//0->1  1->0  
		if ((s1==1)&&(s2==0)&&(s3==1)&&(s4==1))	
		    temp_hammingcode[7]=1-temp_hammingcode[7];//0->1  1->0	    
	    if ((s1==1)&&(s2==1)&&(s3==0)&&(s4==0))
			temp_hammingcode[8]=1-temp_hammingcode[8];//0->1  1->0
		if ((s1==0)&&(s2==1)&&(s3==1)&&(s4==0))
            temp_hammingcode[9]=1-temp_hammingcode[9];//0->1  1->0  
	    if ((s1==0)&&(s2==0)&&(s3==1)&&(s4==1))
            temp_hammingcode[10]=1-temp_hammingcode[10];//0->1  1->0  
		
		for (j=0;j<11;j++)
           m_ExtractedOriginalWatermarkInformationSequence[i*11+j]=temp_hammingcode[j];		
	}
*/
	time(&ltime2);
	double m_time;
	m_time=ltime2-ltime1;
    AfxMessageBox("水印检测过程结束!"); 
	char temp_embeddingtime[100];
    sprintf(temp_embeddingtime,"水印检测过程：%lf秒",m_time);
    MessageBox(NULL,temp_embeddingtime,"水印检测完成!",MB_OK);

    fprintf(fp_txt,"\n\n\n%3d     ",correlationresult_threshold);
	fclose(fp_txt);	
	fclose(fp);	
    GlobalFree(mem_temp_FrameBytes);
	GlobalFree(mem_temp_ConvolutionResult);

	correct_bit_number=0;//正确的位数
    correct_rate=0.0;    //正确率
	for (j=0;j<66;j++)
	{ 
	    if (m_OriginalWatermarkInformationSequence[j]==m_ExtractedOriginalWatermarkInformationSequence[j]) 
           correct_bit_number=correct_bit_number+1; 
	}
	correct_rate=correct_bit_number/66.0*100.0;

	CurrentWatermarkSequenceAlongTimeAxis_Extracted=1;
	
}

void CREADAVIDoc::OnExtract_Watermarksequencealongtimeaxisattacked()
{
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;
   void *mem_temp_ConvolutionResult;
   short int Extracted_WatermarkSequenceAlongTimeAxis[30];
// short int Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[30];
// short int Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[30];
   short int *temp_ConvolutionResult;//经过和高通滤波器卷积后，所得的结果有正有负
   int correlationresult_threshold;
   short int correlationresult,correlationresult0,correlationresult1,correlationresult2;
   
   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);
   mem_temp_ConvolutionResult=GlobalAlloc(GMEM_FIXED,101376*2);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }

   if(mem_temp_ConvolutionResult==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_ConvolutionResult);
	  return;
   }

   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   temp_ConvolutionResult=(short int *)mem_temp_ConvolutionResult;

   FILE *fp_txt;
   fp_txt=fopen("D:\\CorrelationDetectionResult.txt","w");
   if(fp_txt==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Write Record File Error");
		return ;
	}
	fprintf(fp_txt,"不同位置对应的相关检测结果");
   
//   unsigned char Frame_Block[1584][8][8];//分块后的图像数据
   int i,j,k,tx,ty,pn_number,temp_bitplane,temp_correlationresult;
   FILE *fp;
   fp=fopen(FileName_Attacked,"rb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Watermarked File Error");
	  return;
   }
   
   correlationresult_threshold=0;
   correlationresult=0;
   int ty_low=0,ty_high=0;

   for (i=0;i<100;i++)
   {
	   m_ExtractedOriginalWatermarkInformationSequence_Attacked[i]=0;
	   m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[i]=0;
   }
   
   //计算水印检测与提取的时间
   time_t ltime1,ltime2;
   time(&ltime1);

//   for (ty=64;ty<224;ty++)
//     for (tx=76;tx<276;tx++)  
   
   
   for (pn_number=0;pn_number<22;pn_number++)
   {
     temp_correlationresult=0;
     correlationresult=0; 
	 if (pn_number<11)
     {
		 ty_low=60;
         ty_high=180;
	 }
	 else
     {
		 ty_low=160;
         ty_high=280;
	 }		 
	 for (ty=ty_low;ty<ty_high;ty++)
      for (tx=250;tx<276;tx++)
	  {
		//if (correlationresult<35)
		{
		 fseek(fp,4096L,0);
		 correlationresult=0;
         correlationresult0=0;
         correlationresult1=0;
		 correlationresult2=0;
		 for (k=0;k<30;k++)
		 { 
	       //pixelvaluesum=0;
			 fseek(fp,8L,1);//每一帧的数据长度（8+101376）bytes
             fread(FrameBytes_ForWatermarkExtraction,101376,1,fp);

//    char temp_originalembeddingpoint[100];
//    sprintf(temp_originalembeddingpoint,"检测点象素值%d",FrameBytes_ForWatermarkExtraction[(287-88)*352+272]);
//    MessageBox(NULL,temp_originalembeddingpoint,"水印检测（未经过高通滤波前）",MB_OK);//Display the info
	
	//先对temp_ConvolutionResult数组赋初值 
  //  for (i=0;i<288;i++)
  //	for (j=0;j<352;j++) 
  //         temp_ConvolutionResult[101376-(i+1)*352+j]=FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j];
	
	//先进行高通滤波的处理
     
    //四周的象素保持不动
/*	   for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
		  {
			//temp_FrameBytes[101376-(i+1)*352+j]//
			  temp_ConvolutionResult[101376-(i+1)*352+j]=((12*FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j])-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-1]
				                                         -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-2]-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j-3]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+1]-FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+2]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j+3]-FrameBytes_ForWatermarkExtraction[101376-(i+2)*352+j]
											             -FrameBytes_ForWatermarkExtraction[101376-(i+3)*352+j]-FrameBytes_ForWatermarkExtraction[101376-(i+4)*352+j] 
											             -FrameBytes_ForWatermarkExtraction[101376-i*352+j]-FrameBytes_ForWatermarkExtraction[101376-(i-1)*352+j]
											             -FrameBytes_ForWatermarkExtraction[101376-(i-2)*352+j])/12;
										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
		 // if ((temp_ConvolutionResult[101376-(i+1)*352+j]<-60)||(temp_ConvolutionResult[101376-(i+1)*352+j]>60))
		 //	  pixelvaluesum=pixelvaluesum+1;
		 

		     //  if (pixel_value<0)
             //   pixel_value=0;
		     //if (pixel_value>255)
             //   pixel_value=255;
  
			 //temp_FrameBytes[101376-(i+1)*352+j]=(unsigned char)pixel_value;
          

		  }
*/
//      Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]=FrameBytes_ForWatermarkExtraction[101024-ty*352+tx];//准备沿时间轴进行滤波


/*		Extracted_WatermarkSequenceAlongTimeAxis[k]=((12*FrameBytes_ForWatermarkExtraction[101024-ty*352+tx])-FrameBytes_ForWatermarkExtraction[101023-ty*352+tx]
				                                         -FrameBytes_ForWatermarkExtraction[101022-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101021-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[101025-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101026-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[101027-ty*352+tx]-FrameBytes_ForWatermarkExtraction[100672-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[100320-ty*352+tx]-FrameBytes_ForWatermarkExtraction[99968-ty*352+tx] 
											             -FrameBytes_ForWatermarkExtraction[101376-ty*352+tx]-FrameBytes_ForWatermarkExtraction[101728-ty*352+tx]
											             -FrameBytes_ForWatermarkExtraction[102080-ty*352+tx])/12;
*/										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 

/*	  Extracted_WatermarkSequenceAlongTimeAxis[k]=((4*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx])-(2*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx-1])
					  -(2*FrameBytes_ForWatermarkExtraction[101376-(ty+1)*352+tx+1])-(2*FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx])
					  -(2*FrameBytes_ForWatermarkExtraction[101376-ty*352+tx])+FrameBytes_ForWatermarkExtraction[101376-ty*352+tx+1]
					  +FrameBytes_ForWatermarkExtraction[101376-ty*352+tx-1]+FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx+1]
					  +FrameBytes_ForWatermarkExtraction[101376-(ty+2)*352+tx-1])/4;
										 
*/		                                    
		                                     //         1  -2  1 
			                                 //(1/4)   -2   4 -2 
	                                         //         1  -2  1
			 
      
      temp_bitplane=((FrameBytes_ForWatermarkExtraction[101024-ty*352+tx]&32)>>5);
      if (temp_bitplane==0)
         Extracted_WatermarkSequenceAlongTimeAxis[k]=1;
      if ((temp_bitplane)==1)
         Extracted_WatermarkSequenceAlongTimeAxis[k]=-1;

	  if (k<10)	  
	      correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult2;
	  }


/*	  if (Extracted_WatermarkSequenceAlongTimeAxis[k]<-20)
		  Extracted_WatermarkSequenceAlongTimeAxis[k]=-20;
      if (Extracted_WatermarkSequenceAlongTimeAxis[k]>20)
		  Extracted_WatermarkSequenceAlongTimeAxis[k]=20;


	  if (k<10)	  
	      correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis[k]*m_SpreadSpectrumWatermarkSequence[pn_number][k]+correlationresult2;
	  }
*/
    //char temp_pixelvaluerange[300];
    //sprintf(temp_pixelvaluerange,"滤波后结果大于60或小于-60的象素数： %d",pixelvaluesum);
    //MessageBox(NULL,temp_pixelvaluerange,"滤波后的结果",MB_OK);//Display the info

     
  
    //找到当前帧嵌入的点
    //[l/44*8+m][l%44*8+n]->[1500/44*8+0][1500%44*8+0]->[272][32]
    //Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-272)*352+32];

//    Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-ty)*352+tx];
  
//	  char temp_embeddingpoint[100];
//    sprintf(temp_embeddingpoint,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis[k]);
//    MessageBox(NULL,temp_embeddingpoint,"水印检测（经过高通滤波后）",MB_ICONSTOP|MB_OK);//Display the info
  
   }

   //考虑进行一维预测滤波
/* //[-1 -1 -1 6 -1 -1 -1]/6
   //序列的两边进行零阶扩展0,1,2,27,28,29
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[0]=(3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3])/6; 
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[1]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4])/6;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[2]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/6;

   for(k=3;k<27;k++)
   { 
      Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-1]
	                                                             -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-3]
														         -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+2]
	           											         -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+3])/6;
   }

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[27]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24])/6;

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[28]=(6*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25])/6;
   
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[29]=(3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26])/6; 


   //[-1 -1 -1 -1 8 -1 -1 -1 -1]/8
   //序列的两边进行零阶扩展0,1,2,3,26,27,28,29
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[0]=(4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4])/8; 
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[1]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/8;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[2]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5])/8;
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[3]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[3]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[0]
	                                                          -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[1]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[2]
														      -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[5]
															  -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[6]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[7])/8;


   for(k=4;k<26;k++)
   { 
      Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-1]
	                                                             -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-3]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k-4]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+1]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+2]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+3]
																 -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k+4])/8;
   }

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[26]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]-2*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[22])/8;


   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[27]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-3*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[23])/8;

   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[28]=(8*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]-4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
														       -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[24])/8;
   
   Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[29]=(4*Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[29]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[28]
	                                                           -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[27]-Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[26]
															   -Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[25])/8; 

   correlationresult=0;
   correlationresult0=0;
   correlationresult1=0;
   correlationresult2=0;
   
   for (k=0;k<30;k++)
   {
      if (Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]<-20)
		  Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=-20;
      if (Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]>20)
		  Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]=20;

	  char temp_embeddingpoint_1D[100];
      sprintf(temp_embeddingpoint_1D,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]);
      MessageBox(NULL,temp_embeddingpoint_1D,"水印检测（未经过一维预测滤波前）",MB_ICONSTOP|MB_OK);//Display the info
      sprintf(temp_embeddingpoint_1D,"检测点象素值%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]);
      MessageBox(NULL,temp_embeddingpoint_1D,"水印检测（经过一维预测滤波后）",MB_ICONSTOP|MB_OK);//Display the info
  

      if (k<10)	  
	     correlationresult0=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[0][k]+correlationresult0;
	  else
	  {
		  if (k<20)	  
			  correlationresult1=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[0][k]+correlationresult1;
	      else
			  correlationresult2=Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]*m_SpreadSpectrumWatermarkSequence[0][k]+correlationresult2;
	  }
   }
*/
    correlationresult=abs(correlationresult0)+abs(correlationresult1)+abs(correlationresult2); 
    //correlationresult=correlationresult/10;
	//if (correlationresult>=35)
     if (correlationresult>temp_correlationresult)
	 {
      temp_correlationresult=correlationresult;
	  if (correlationresult0>0)//双极性的1-单极性的0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3]=0;
      if (correlationresult0<0)//双极性的-1-单极性的1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3]=1;
      if (correlationresult1>0)//双极性的1-单极性的0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+1]=0;
      if (correlationresult1<0)//双极性的-1-单极性的1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+1]=1;

      if (correlationresult2>0)//双极性的1-单极性的0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+2]=0;
      if (correlationresult2<0)//双极性的-1-单极性的1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+2]=1;
	}

	  if (correlationresult==30)
	     correlationresult_threshold=correlationresult_threshold+1;
 
   	  //char temp_correlationresult[80];
	  //sprintf(temp_correlationresult,"相关结果%d",correlationresult);
      //MessageBox(NULL,temp_correlationresult,"",MB_OK)

      fprintf(fp_txt,"\n%3d     ",correlationresult);
	}
}
}	
    
    for (i=0;i<66;i++)
		m_ExtractedOriginalWatermarkInformationSequence_Attacked[i]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[i];
    //检验汉明码的纠错能力
	//可以纠一位错，出现两位错的时候误码率上升
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]=0;
	//m_ExtractedHammingCodedOriginalWatermarkInformationSequence[2]=0;

	//汉明编码对应的纠错过程
    //对提取的信息进行纠错 90bits->66bits
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked->m_ExtractedOriginalWatermarkInformationSequence_Attacked
    //先进行分组，信息位11位，加上4位监督位 90位分成6组
/*    unsigned char temp_hammingcode[15];
	int s1,s2,s3,s4;
	for (i=0;i<6;i++)
	{
		for (j=0;j<15;j++)
           temp_hammingcode[j]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[i*15+j];
	    
		//监督矩阵
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        s1=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[5]+temp_hammingcode[7]
		    +temp_hammingcode[8]+temp_hammingcode[11])%2;//模2加
        s2=(temp_hammingcode[1]+temp_hammingcode[2]
			+temp_hammingcode[3]+temp_hammingcode[4]
	        +temp_hammingcode[6]+temp_hammingcode[8]
	        +temp_hammingcode[9]+temp_hammingcode[12])%2;//模2加
        s3=(temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[4]+temp_hammingcode[5]
            +temp_hammingcode[7]+temp_hammingcode[9]
		    +temp_hammingcode[10]+temp_hammingcode[13])%2;//模2加
        s4=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[4]
            +temp_hammingcode[6]+temp_hammingcode[7]
	        +temp_hammingcode[10]+temp_hammingcode[14])%2;//模2加   
	    
		//针对11位信息位
		//利用校验子纠错 错误图样
        if ((s1==1)&&(s2==0)&&(s3==0)&&(s4==1))
		    temp_hammingcode[0]=1-temp_hammingcode[0];//0->1  1->0
        if ((s1==1)&&(s2==1)&&(s3==0)&&(s4==1))	
            temp_hammingcode[1]=1-temp_hammingcode[1];//0->1  1->0
		if ((s1==1)&&(s2==1)&&(s3==1)&&(s4==1))
			temp_hammingcode[2]=1-temp_hammingcode[2];//0->1  1->0
        if ((s1==1)&&(s2==1)&&(s3==1)&&(s4==0))	
		    temp_hammingcode[3]=1-temp_hammingcode[3];//0->1  1->0	    
	    if ((s1==0)&&(s2==1)&&(s3==1)&&(s4==1))
			temp_hammingcode[4]=1-temp_hammingcode[4];//0->1  1->0
		if ((s1==1)&&(s2==0)&&(s3==1)&&(s4==0))
            temp_hammingcode[5]=1-temp_hammingcode[5];//0->1  1->0  
	    if ((s1==0)&&(s2==1)&&(s3==0)&&(s4==1))
            temp_hammingcode[6]=1-temp_hammingcode[6];//0->1  1->0  
		if ((s1==1)&&(s2==0)&&(s3==1)&&(s4==1))	
		    temp_hammingcode[7]=1-temp_hammingcode[7];//0->1  1->0	    
	    if ((s1==1)&&(s2==1)&&(s3==0)&&(s4==0))
			temp_hammingcode[8]=1-temp_hammingcode[8];//0->1  1->0
		if ((s1==0)&&(s2==1)&&(s3==1)&&(s4==0))
            temp_hammingcode[9]=1-temp_hammingcode[9];//0->1  1->0  
	    if ((s1==0)&&(s2==0)&&(s3==1)&&(s4==1))
            temp_hammingcode[10]=1-temp_hammingcode[10];//0->1  1->0  
		
		for (j=0;j<11;j++)
           m_ExtractedOriginalWatermarkInformationSequence_Attacked[i*11+j]=temp_hammingcode[j];
	}
*/	
	time(&ltime2);
	double m_time;
	m_time=ltime2-ltime1;
    AfxMessageBox("水印检测过程结束!"); 
	char temp_detectiontime[200];
    sprintf(temp_detectiontime,"水印检测过程：%lf秒",m_time);
    MessageBox(NULL,temp_detectiontime,"水印检测完成!",MB_OK);

    fprintf(fp_txt,"\n\n\n%3d     ",correlationresult_threshold);
	fclose(fp_txt);	
	fclose(fp);	
    GlobalFree(mem_temp_FrameBytes);
	GlobalFree(mem_temp_ConvolutionResult); 

	correct_bit_number_attacked=0;//正确的位数
    correct_rate_attacked=0.0;    //正确率
	for (j=0;j<66;j++)
	{ 
	    if (m_OriginalWatermarkInformationSequence[j]==m_ExtractedOriginalWatermarkInformationSequence_Attacked[j]) 
           correct_bit_number_attacked=correct_bit_number_attacked+1; 
	}
	correct_rate_attacked=correct_bit_number_attacked/66.0*100.0;

	AttackedCurrentWatermarkSequenceAlongTimeAxis_Extracted=1;
}

void CREADAVIDoc::OnSaveCurrentAttackedFrame()
{
   //Save the watermarked frame
   OPENFILENAME ofn;//The structure to open a FileDialog
   char FileName_CurrentAttackedFrame[_MAX_PATH],FileTitle_CurrentAttackedFrame[_MAX_PATH];//The Filename and FileTitle 
   
   FileName_CurrentAttackedFrame[0]='\0';
   FileTitle_CurrentAttackedFrame[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP文件 *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentAttackedFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentAttackedFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存BMP文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作！");
	   return;
   }


   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName_CurrentAttackedFrame,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName_CurrentAttackedFrame)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName_CurrentAttackedFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentAttackedFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

   FILE *fp;
   fp=fopen(FileName_CurrentAttackedFrame,"wb");
   if(fp==NULL)
   {
	  MessageBeep(-1);
	  AfxMessageBox("Current Frame File Error");
	  return;
   }
	 
   	long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"标准[256GrayBmpHeader.bmp]找不到或打不开!","程序初始化",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"读取标准[256GrayBmpHeader.bmp]失败!","程序初始化",MB_ICONSTOP|MB_OK);
	}
    _close(fh);  

   fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
   
   fwrite(FrameBytes_Attacked,101376,1,fp);

   fclose(fp);

}

void CREADAVIDoc::OnTimeSpatialEmbedding_SpreadSpectrum()
{
   //Save the watermarked avi file
   OPENFILENAME ofn;//The structure to open a FileDialog
  
   FileName[0]='\0';
   FileTitle[0]='\0';

   //Initialize the FileSaveDialog structure begins
   ofn.lStructSize=sizeof(OPENFILENAME);//The structure size 
   ofn.hwndOwner=NULL; //The Owner window handle 
   ofn.hInstance=NULL; 
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0所有文件 (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI文件 *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="保存AVI文件";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("您取消了本次操作");
	   return;
   }

   //Search the filename
   CFileFind cff;
   if(cff.FindFile(FileName,0))
   {
     cff.FindNextFile();
     if(cff.GetFilePath().CompareNoCase(FileName)==0)
	 { 
	     if(cff.IsReadOnly())//Judge if the file attribute is read only
		 { 
		    sprintf(temp0,"文件[%s]是只读文件,确实要覆盖吗？",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("存储失败!");
			   cff.Close();
			   return;
			}
		 }
	 }
   }

    int i,j,k,l,m,n;
 
    int flag_0=0,flag_1=0,flag_2=0,flag_3=0;

    FILE *fp;
    fp=fopen(FileName,"wb");
	if(fp==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("File Error");
		return;
	}
	 
	fwrite(AVIFileInfo,4096,1,fp);         

	double DifferenceSum;//象素差
	int OriginalPixelValueSum;//原始象素的和，用来计算SNR
	unsigned char Frame_Block[1584][8][8];//分块后的图像数据
	double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *BlockDCT_Coefficient_Saved[1584][8];//前一帧的分块DCT系数
	double *temp_DC_Coefficient_Difference;//前后帧相应的DCT直流系数之差
    double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double *temp_DCT_Edge_Detection;//在DCT域判断是否有边缘存在
	double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值
	int flag_DC_Coefficient,flag_AC_Coefficient;	 


	//分配内存
	void *mem;
	void *mem_DC_Difference;
    void *mem_DCT_Variance;
    void *mem_AC_Energy;
    void *mem_DCT_Edge_Detection;
	
	mem=GlobalAlloc(GMEM_FIXED,101376*8);
    mem_DC_Difference=GlobalAlloc(GMEM_FIXED,1584*8);	
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);
	mem_DCT_Edge_Detection=GlobalAlloc(GMEM_FIXED,1584*8);

	if(mem==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem);
	  return;
	}
    
    if(mem_DC_Difference==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DC_Difference);
	  return;
	}
   
	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	if(mem_DCT_Edge_Detection==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Edge_Detection);
	  return;
	}

	for (k=0;k<1584;k++)
      for (j=0;j<8;j++)
	  {
	   	 BlockDCT_Coefficient_Saved[k][j]=(double *)mem+k*64+j*8;
	  }

    temp_DC_Coefficient_Difference=(double *)mem_DC_Difference;
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;
    temp_DCT_Edge_Detection=(double *)mem_DCT_Edge_Detection;
	
    //对64位的水印进行扩谱，进一步考虑Gold序列
	//每一个block中放1位
    //产生伪随机序列
	//冗余嵌入
    
	int seed;
    seed=12;

    for (i=0;i<64;i++)
	{
	    seed=seed+1;
        PseudoRandomSequence_Generator(PseudoRandomSequence,512,seed);//64*2
	    for (j=0;j<64;j++)
		{
			if (PseudoRandomSequence[j]==0)//单极性0 -> 双极性1
		        m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j]=1;//调制序列
            else//单极性1 -> 双极性-1
				m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j]=-1;//调制序列
		}
	
        //利用伪随机序列对水印进行调制  
        for (j=0;j<1;j++)
		{ 
	      if (m_OriginalWatermarkBytes[i+j]==0)//0 -> 1(双极性)
		  {
			  for (k=0;k<64;k++)//经过水印信息调制的序列
			      m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j*64+k]=m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j*64+k];//调制序列保持不变
		  }
          else//1 -> -1(双极性)
          {
			  for (k=0;k<64;k++)//经过水印信息调制的序列
			      m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j*64+k]=-m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j*64+k];//调制序列反向		  
		  }
		}
	}
	
	for (i=0;i<64;i++)
	   for (j=0;j<64;j++)
	   {
		   if ((m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j]!=1)&&
			     (m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j]!=-1))
           AfxMessageBox("Watermark Information Error!"); 
	   }
  
	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);	

	for (l=0;l<1584;l++)
	{
	  temp_DC_Coefficient_Difference[l]=0.0;
      temp_DCT_Coefficient_Variance[l]=0.0;
	  temp_DCT_Edge_Detection[l]=0.0;
	}

	//计算水印嵌入的时间
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//灰度
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
//		  for (k=0;k<10;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//显示DC系数的信息
			 flag_AC_Coefficient=0;//显示AC系数的信息
			 DifferenceSum=0.0;//针对每一帧都清零
			 OriginalPixelValueSum=0;//针对每一帧都清零
	         OriginalFile.Seek(8L,CFile::current);//每一帧的数据长度（8＋101376）bytes
             
             OriginalFile.Read(FrameBytes_Embedding,101376);

			 for (i=0;i<288;i++)//将图像数据倒过来
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//对当前帧进行8*8分块
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
			
                DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//原图像进行逆DCT变换

                if (k>=1)//比较前后两帧，这里不会出现误差累计的情况，因为前一帧的信息存储在DCT系数矩阵里
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//前后帧对应DCT系数的差值
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//相应块交流系数的均值

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
                 
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			 }

//*****************************************************************

			  int r=0; 
		      int temp_result;
			  //经过判断后在相应的块中嵌水印
			
			  if (k==0)//第一帧单独处理,不参考邻近的帧
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				//for (l=300;l<1584;l++)
				{ 
				   if (temp_AC_Coefficient_Energy[l]>=12000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>880))//纹理复杂块 
				   {
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
					if (r<128)
					{
					 for (m=0;m<8;m++)
				      for (n=0;n<8;n++)
					  { 		
						//标记出8*8方块的边界
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==7)
					    //    Frame_Block[l][m][n]=255;//标记出这样的块

					 //temp_result=(int)(Frame_Block[l][m][n]+(PseudoRandomSequence[m*8+n]*10));
                     
					 if (m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[r/2][m*8+n]==1)
						temp_result=0;
                     if (m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[r/2][m*8+n]==-1)
                        temp_result=1;

                     Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(temp_result*8));//1111 0111
					 //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&239)+(temp_result*16));//1110 1111
					 
                     //Frame_Block[l][m][n]=temp_result; 
                     //if (temp_result<0)
					 //	 Frame_Block[l][m][n]=0;
                     //if (temp_result>255)
					 //	 Frame_Block[l][m][n]=255;
               
					}
				   } 
				   r=r+1;
				  }            
				}
				if (r<128)
					AfxMessageBox("所选择的块数不够！");
			  }
              else
			  {
                r=0;  
                for (l=0;l<1584;l++)
				{
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//既是运动块也是纹理复杂块 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//是运动块 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//是纹理复杂块  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//交流分量的能量总和
               
		        if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>680))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块  for table tennis 2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12800)&&(fabs(BlockDCT_Coefficient[l][0][0])>900))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块  for table tennis 2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//既是运动块也是纹理复杂块   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
    //			if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1000))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1160)) 
				{  
					//记录下嵌入水印的位置
				    EmbeddingRegionSequence[k][r]=l;
			        
       
					if (r<128)
					{
					 for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					 { 
                        //temp=Frame_Block[l][m][n];
						
						//标记出8*8方块的边界
					    //if (m==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//标记出这样的块
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//标记出这样的块
                        // if (n==7)
					    //    Frame_Block[l][m][n]=255;//标记出这样的块
               		 
                     //temp_result=(int)(Frame_Block[l][m][n]+(PseudoRandomSequence[m*8+n]*10));
					  if (m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[r/2][m*8+n]==1)
						 temp_result=0;
                      if (m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[r/2][m*8+n]==-1)
                         temp_result=1;

                      Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(temp_result*8));//1111 0111
                      //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&239)+(temp_result*16));//1110 1111

                     //Frame_Block[l][m][n]=temp_result; 
                     //if (temp_result<0)
					 //	 Frame_Block[l][m][n]=0;
                     //if (temp_result>255)
					 //	 Frame_Block[l][m][n]=255;
                                      
    
                        //Frame_Block[l][m][n]=((Frame_Block[l][m][n]&223)+(PseudoRandomSequence[m*8+n]*32));//1101 1111

						//if ((PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=0)&&(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]!=1))
						//{
						//	AfxMessageBox("Watermark Message Error!");
						//	return;
						//}
						//Frame_Block[l][m][n]=((Frame_Block[l][m][n]&247)+(PseudoRandomSequenceBytes[watermark_block_size*64+m*8+n]*8));//1111 0111
					    //temp=Frame_Block[l][m][n]-temp;
					 }
					}
				   r=r+1;
				}
			}
            if (r<128)
			   AfxMessageBox("所选择的块数不够！");			  
		}		
		
          //Frame_Block[1500][0][0]=255;   //沿时间轴嵌入水印

            EmbeddingRegionSumSequence[k]=r; 
                 
			//将分块后的数据还原为整幅数据
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //计算相应的PSNR(大约耗时7秒钟)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//用来计算PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//将数据按倒序存储
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("水印嵌入过程结束!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"水印嵌入过程：%lf秒",m_time);
          MessageBox(NULL,temp_embeddingtime,"水印嵌入完成!",MB_OK);

		  //AVI文件后面的INDEX部分
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//利用FrameBytes_Embedding数组
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//利用FrameBytes_Embedding数组
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI文件已成功保存");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //彩色
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//每一帧的数据长度（8＋304128）bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//嵌入水印的序列对应的位置
       FrameNumber_Watermarked=0;//首先将文件的位置初始化

	   GlobalFree(mem);
	   GlobalFree(mem_DC_Difference);
	   GlobalFree(mem_DCT_Variance);
	   GlobalFree(mem_AC_Energy);
	   GlobalFree(mem_DCT_Edge_Detection);

}

void CREADAVIDoc::OnImagePredictionFilter3x3(short int a0,
		                                     short int a11, short int a12, short int a13,
		                                     short int a21, short int a22, short int a23,
									         short int a31, short int a32, short int a33,
											 short int offset)
{  
   unsigned char *temp_FrameBytes;
   void *mem_temp_FrameBytes;

   mem_temp_FrameBytes=GlobalAlloc(GMEM_FIXED,101376);

   if(mem_temp_FrameBytes==NULL)
   {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_temp_FrameBytes);
	  return;
   }
   temp_FrameBytes=(unsigned char *)mem_temp_FrameBytes;
   
   int pixel_value;//注意是整型数据 

   int i,j;
   if (PositionFlag==1)//原始视频区域
   {
	   //四周的象素保持不动
	   for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
		  {
		
		  /*  //temp_FrameBytes[101376-(i+1)*352+j]//
			  pixel_value=((12*FrameBytes[101376-(i+1)*352+j])-FrameBytes[101376-(i+1)*352+j-1]
				                            -FrameBytes[101376-(i+1)*352+j-2]-FrameBytes[101376-(i+1)*352+j-3]
											-FrameBytes[101376-(i+1)*352+j+1]-FrameBytes[101376-(i+1)*352+j+2]
											-FrameBytes[101376-(i+1)*352+j+3]-FrameBytes[101376-(i+2)*352+j]
											-FrameBytes[101376-(i+3)*352+j]-FrameBytes[101376-(i+4)*352+j] 
											-FrameBytes[101376-i*352+j]-FrameBytes[101376-(i-1)*352+j]
											-FrameBytes[101376-(i-2)*352+j])/12;
										
		                                     //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
			                                 //(1/12) -1 -1 -1 12 -1 -1 -1
			                                 //        0  0  0 -1  0  0  0 
		                                     //        0  0  0 -1  0  0  0   
		                                     //        0  0  0 -1  0  0  0 
		  */

		  pixel_value=((a22*FrameBytes[101376-(i+1)*352+j])+(a21*FrameBytes[101376-(i+1)*352+j-1])
					  +(a23*FrameBytes[101376-(i+1)*352+j+1])+(a32*FrameBytes[101376-(i+2)*352+j])
					  +(a31*FrameBytes[101376-(i+2)*352+j-1])+(a33*FrameBytes[101376-(i+2)*352+j+1])
					  +(a12*FrameBytes[101376-i*352+j])+(a11*FrameBytes[101376-i*352+j-1])+(a13*FrameBytes[101376-i*352+j+1]))/a0;
										 
		                                    
		                                     //         1  -2  1            a11 a12 a13
			                                 //(1/4)   -2   4 -2    (1/a0)  a21 a22 a23 
			                                 //         1  -2  1            a31 a32 a33
		                                    

		  pixel_value=pixel_value+offset;
			if (pixel_value<0)
               pixel_value=0;
			if (pixel_value>255)
               pixel_value=255;
  
			temp_FrameBytes[101376-(i+1)*352+j]=(unsigned char)pixel_value;
			
		  }

      for (i=3;i<285;i++)
		  for (j=3;j<349;j++) 
	        FrameBytes[101376-(i+1)*352+j]=temp_FrameBytes[101376-(i+1)*352+j];
   } 
   GlobalFree(mem_temp_FrameBytes); 

}

void CREADAVIDoc::OnExtraction_CurrentFrameSSsequence()
{
    int i,j,l,m,n,pn_number;
	short int temp_conversion[64],correlation_result,correlation_result0;//correlation_result1;
		                          //correlation_result2,correlation_result3;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值

    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

	FILE *fp_txt;
    fp_txt=fopen("D:\\SSCorrelationDetectionResult.txt","w");
    if(fp_txt==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Write Record File Error");
		return ;
	}
	fprintf(fp_txt,"不同位置对应的相关检测结果");

    //对当前帧数据进行处理
	//将FrameBytes_Embedding中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

    for (l=0;l<1584;l++)
	{
		temp_DCT_Coefficient_Variance[l]=0.0;
	    temp_AC_Coefficient_Energy[l]=0.0;
	} 

    for (l=0;l<1584;l++)
	{	
       temp_DCT_Coefficient_Mean=0.0;	 
       temp_DCT_Coefficient_Variance[l]=0.0; 
	   DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
	
	   for (m=0;m<8;m++)
		   for (n=0;n<8;n++)
		   {
               temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
			   temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
		   }

	  temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
      temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));
	 }
    
	 for (m=0;m<64;m++)
	    Watermark_Extracted[m]=0;

     for (pn_number=0;pn_number<64;pn_number++) 
	 {
	  //经过判断，确定水印是否存在
	  for (l=0;l<1584;l++)
      //for (l=300;l<1584;l++)
	 {
		//	if (temp_DCT_Coefficient_Variance[l]>=1000)//仅仅判断是否是纹理块 
		//  if ((temp_AC_Coefficient_Energy[l]>=28000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1080))
		//  if (temp_AC_Coefficient_Energy[l]>=35000)// for mobile & calendar
		if (temp_AC_Coefficient_Energy[l]>=10000)// for table tennis 2.avi
		{  
		    for (m=0;m<8;m++)
				for (n=0;n<8;n++)
				{
             		//Frame_Block[l][m][n]=255;//标记出这样的块 
					//if (((Frame_Block[l][m][n]&16)>>4)==1)//1110 1111
					if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
					    temp_conversion[m*8+n]=-1;//1 -> -1
			        if (((Frame_Block[l][m][n]&8)>>3)==0)//1111 0111
				        temp_conversion[m*8+n]=1;//0 -> 1
				}

		    correlation_result0=0;
		    //correlation_result1=0;
		    //correlation_result2=0;
		    //correlation_result3=0;
				 
			for (n=0;n<64;n++)
			{
				correlation_result0=correlation_result0+(temp_conversion[n]*
					                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][n]);
          
			    //correlation_result1=correlation_result1+(temp_conversion[32+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][32+n]);
    
			    //correlation_result2=correlation_result2+(temp_conversion[32+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][32+n]);
     
			    //correlation_result3=correlation_result3+(temp_conversion[48+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][48+n]);
            }
			correlation_result=abs(correlation_result0);//+abs(correlation_result1);
			//+abs(correlation_result2+abs(correlation_result3);
                    
			if (correlation_result==64)
			{
			   if (correlation_result0==64)//双极性的1
                  Watermark_Extracted[pn_number]=0;
               if (correlation_result0==-64)//双极性的-1
                  Watermark_Extracted[pn_number]=1;

			   //if (correlation_result1==32)//双极性的1
               //   Watermark_Extracted[pn_number*2+1]=0;
               //if (correlation_result1==-32)//双极性的-1
               //   Watermark_Extracted[pn_number*2+1]=1;

			   //if (correlation_result2==16)//双极性的1
               //   Watermark_Extracted[pn_number*4+2]=0;
               //if (correlation_result2==-16)//双极性的-1
               //   Watermark_Extracted[pn_number*4+2]=1;

			   //if (correlation_result3==16)//双极性的1
               //   Watermark_Extracted[pn_number*4+3]=0;
               //if (correlation_result3==-16)//双极性的-1
               //   Watermark_Extracted[pn_number*4+3]=1;
			}
		   fprintf(fp_txt,"\n%3d     ",correlation_result);
		}	 
	  }
	 }	
	    correct_bit_number=0;
		correct_rate=0.0;
	    for (m=0;m<64;m++)
		{ 
			if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
               correct_bit_number=correct_bit_number+1; 
		}
		correct_rate=correct_bit_number/64.0*100.0;

		CurrentSSWatermarkSequence_Extracted=1;
		m_save_currentwatermark=TRUE;
		fclose(fp_txt);
		GlobalFree(mem_DCT_Variance);
		GlobalFree(mem_AC_Energy);
}

void CREADAVIDoc::OnExtraction_CurrentFrameSSsequence_Attacked()
{ 
	int i,j,l,m,n,pn_number;
	short int temp_conversion[64],correlation_result,correlation_result_temp,correlation_result0;
		                          //correlation_result1,correlation_result2,correlation_result3;
    unsigned char Frame_Block[1584][8][8];//分块后的图像数据
    double BlockDCT_Coefficient[1584][8][8];//分块DCT系数
	double *temp_DCT_Coefficient_Variance;//分块DCT交流系数的方差
    double *temp_AC_Coefficient_Energy;//分块DCT交流系数的能量
    double temp_DCT_Coefficient_Mean;//暂存分块DCT交流系数的均值

    //分配内存
    void *mem_DCT_Variance;
	void *mem_AC_Energy;
		
    mem_DCT_Variance=GlobalAlloc(GMEM_FIXED,1584*8);	
	mem_AC_Energy=GlobalAlloc(GMEM_FIXED,1584*8);	

	if(mem_DCT_Variance==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_DCT_Variance);
	  return;
	}

	if(mem_AC_Energy==NULL)
    {
	  AfxMessageBox("Sorry,memory is not enough!");
	  GlobalFree(mem_AC_Energy);
	  return;
	}
   
	temp_DCT_Coefficient_Variance=(double *)mem_DCT_Variance;
    temp_AC_Coefficient_Energy=(double *)mem_AC_Energy;

	FILE *fp_txt;
    fp_txt=fopen("D:\\SSCorrelationDetectionResult.txt","w");
    if(fp_txt==NULL)
	{
	    MessageBeep(-1);
		AfxMessageBox("Write Record File Error");
		return ;
	}
	fprintf(fp_txt,"不同位置对应的相关检测结果");

    //对当前帧数据进行处理
	//将FrameBytes_Attacked中的数据按正常顺序读出来
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //将图象数据分块
    //对当前帧进行8*8分块
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

    for (l=0;l<1584;l++)
	{
		temp_DCT_Coefficient_Variance[l]=0.0;
	    temp_AC_Coefficient_Energy[l]=0.0;
	} 

	for (l=0;l<100;l++)
		Watermark_Attacked[l]=0;

	//计算水印检测与提取的时间
    time_t ltime1,ltime2;
    time(&ltime1);

    for (l=0;l<1584;l++)
	{	
       temp_DCT_Coefficient_Mean=0.0;	 
       temp_DCT_Coefficient_Variance[l]=0.0; 
	   DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
	
	   for (m=0;m<8;m++)
		   for (n=0;n<8;n++)
		   {
               temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
			   temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
		   }

	  temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT块交流系数的方差
      temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));
	 }

     for (pn_number=0;pn_number<64;pn_number++) 
	 {
       correlation_result_temp=0; 
	   //经过判断，确定水印是否存在
	   //for (l=0;l<1584;l++)
	   for (l=0;l<1584;l++)
	   {
		//  if ((temp_AC_Coefficient_Energy[l]>=28000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1080))
		//  if (temp_AC_Coefficient_Energy[l]>=35000)// for mobile & calendar
		if (temp_AC_Coefficient_Energy[l]>=10000)// for table tennis 2.avi
		{  
		    for (m=0;m<8;m++)
				for (n=0;n<8;n++)
				{
             		//Frame_Block[l][m][n]=255;//标记出这样的块 
					
					//if (((Frame_Block[l][m][n]&16)>>4)==1)//1110 1111
				    if (((Frame_Block[l][m][n]&8)>>3)==1)//1111 0111
						temp_conversion[m*8+n]=-1;//1 -> -1
			        if (((Frame_Block[l][m][n]&8)>>3)==0)//1111 0111
				        temp_conversion[m*8+n]=1;//0 -> 1
				}

		    correlation_result0=0;
		    //correlation_result1=0;
		    //correlation_result2=0;
		    //correlation_result3=0;
				 
			for (n=0;n<64;n++)
			{
				correlation_result0=correlation_result0+(temp_conversion[n]*
					                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][n]);
          
			    //correlation_result1=correlation_result1+(temp_conversion[32+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][32+n]);
    
			    //correlation_result2=correlation_result2+(temp_conversion[32+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][32+n]);
     
			    //correlation_result3=correlation_result3+(temp_conversion[48+n]*
				//	                      m_TimeEmbeddingSpreadSpectrumWatermarkSequence[pn_number][48+n]);
            }
			correlation_result=abs(correlation_result0);//+abs(correlation_result1);
				//+abs(correlation_result2+abs(correlation_result3);
                    
			if (correlation_result>correlation_result_temp)
			{
			   if (correlation_result0>=0)//双极性的1
                  Watermark_Attacked[pn_number]=0;
               if (correlation_result0<0)//双极性的-1
                  Watermark_Attacked[pn_number]=1;

			   //if (correlation_result1>=0)//双极性的1
               //   Watermark_Attacked[pn_number*2+1]=0;
               //if (correlation_result1<0)//双极性的-1
               //   Watermark_Attacked[pn_number*2+1]=1;

			 //if (correlation_result2>=0)//双极性的1
             //  Watermark_Attacked[pn_number*4+2]=0;
             //if (correlation_result2<0)//双极性的-1
             //  Watermark_Attacked[pn_number*4+2]=1;

			 //if (correlation_result3>=0)//双极性的1
             //  Watermark_Attacked[pn_number*4+3]=0;
             //if (correlation_result3<0)//双极性的-1
             //     Watermark_Attacked[pn_number*4+3]=1;

			   correlation_result_temp=correlation_result;
			}
		    fprintf(fp_txt,"\n%3d     ",correlation_result);
		}
	   }
	}	
	 
	    time(&ltime2);
	    double m_time;
	    m_time=ltime2-ltime1;
        AfxMessageBox("水印检测与提取过程结束!"); 
	    char temp_detectiontime[150];
        sprintf(temp_detectiontime,"水印检测与提取过程：%lf秒",m_time);
        MessageBox(NULL,temp_detectiontime,"水印提取过程完成!",MB_OK);

	    correct_bit_number_attacked=0;
		correct_rate_attacked=0.0;
	    for (m=0;m<64;m++)
		{ 
			if (Watermark_Attacked[m]==m_OriginalWatermarkBytes[m]) 
               correct_bit_number_attacked=correct_bit_number_attacked+1; 
		}
		correct_rate_attacked=correct_bit_number_attacked/64.0*100.0;

		AttackedCurrentSSWatermarkSequence_Extracted=1;
		m_save_currentwatermark=TRUE;
		fclose(fp_txt);
		GlobalFree(mem_DCT_Variance);
		GlobalFree(mem_AC_Energy);
}

double CREADAVIDoc::uniform(double a, double b, long int *seed)
{
	double t;
	*seed=2045*(*seed)+1;
	*seed=*seed-(*seed/1048576)*1048576;
	t=(*seed)/1048576.0;
	t=a+(b-a)*t;
	return(t);
}
