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
		MessageBox(NULL,temp,"�ļ���С",MB_OK);
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
						  //����220֡�����
                          if (TotalFrameNumberInfo[0]==220)
			                  TotalFrameNumber=220;
					      else
						  {
						      MessageBox(NULL,"������һ�������ϱ�����淶����Ч�ļ�!","�ļ���",MB_OK|MB_ICONSTOP);
					          return;
						  }
					  }
				  }
				}
			}
		}

        //��ȡÿһ֡�Ĵ�С
        fp->Seek(64L,CFile::begin);
		fp->Read(FrameSizeInfo,4);

        fp->Seek(186L,CFile::begin);
		fp->Read(FrameColorInfo,2);

		if ((FrameSizeInfo[0]==96)&&(FrameSizeInfo[1]==1)
			&&(FrameSizeInfo[2]==0)&&(FrameSizeInfo[3]==0))//352*288=101376 ʮ������HEX 18C00
		{
		 if (FrameColorInfo[0]==8)//�Ҷ�
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
			&&(FrameSizeInfo[2]==0)&&(FrameSizeInfo[3]==0))//720*576=414720 ʮ������HEX 65400
		{
         if (FrameColorInfo[0]==8)//�Ҷ�
		 {
		   fp->SeekToBegin();
		   fp->Read(AVIFileInfo,4096);
		   fp->Read(FrameHeader,8);
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,1064);
		   fp->Seek(4104L,CFile::begin);
		   fp->Read(FrameBytes,414720);
		}
		else//��ɫ
		{
		   fp->Seek(172L,CFile::begin);
		   fp->Read(FrameInfo,40);
		   fp->Seek(2056L,CFile::begin);
		   fp->Read(FrameBytes,1244160);
		}


		}

		FrameNumber=0;
		PositionFlag=1;//��ʾλ�� 1:ԭʼ����
		PositionFlag_Watermark=0;//����ʾˮӡ
		PositionFlag_Watermarked=0;//����ʾǶ��ˮӡ����Ƶ
		m_button_play=TRUE;//�����š���ťʹ��
		m_button_begin=FALSE;//����һ֡����ť�ر�
        m_button_end=TRUE;//�����һ֡����ť�ر�
		m_button_forward=TRUE;//��֡������ťʹ��
		m_button_reset=FALSE;//��ֹͣ����ť�ر�
		m_button_back=FALSE;//��֡�ˡ���ť�ر�
		m_button_reset=FALSE;//����λ����ť�ر�
		m_button_save_current_original_frame=TRUE;//�����浱ǰ֡����ťʹ��
        m_pseudorandom_generator=TRUE;//��α������С��˵�ʹ��
		m_binary_watermark=TRUE;//��α������С��˵�ʹ��
		m_timespatialembedding=FALSE;//���Ρ�ʱ������Ƕ�롱�˵�
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
		 if (FrameColorInfo[0]==8)//�Ҷ�
		 {
		   ThisFile.Seek(4104L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(101384L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
		   }
           ThisFile.Read(FrameBytes,101376);
		 }
	     else
		 {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           ThisFile.Read(FrameBytes,304128);
		 }
	   }

	   if (FrameSizeInfo[0]==208)//720*576
	   {
		 if (FrameColorInfo[0]==8)//�Ҷ�
		 {
		   ThisFile.Seek(4104L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(414728L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
		   }
           ThisFile.Read(FrameBytes,414720);
		 }
	     else//��ɫ
		 {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber;i++)
		   {
	         ThisFile.Seek(1244168L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��1244160��bytes
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

    //ȡ��m���еĽ���
	scale=0; //scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
    temp=number;
    while(temp!=1)//�����
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
   tempkey=15;//��������е�����

   for(i=0;i<101376;i++) //����Ϊ352*288=101376,����101376��α���ֵ�����Ӹ�����
	{
      for(j=0;j<scale;j++) //ȡ��λ0/1ֵ-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//��ģ2�ӷ�
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
		PseudoRandomSequenceBytes[i]=0;//���������
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
		MessageBox(NULL,"��׼[2BitBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard2BitBmpFileInfo,62)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"��ȡ��׼[2BitBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI�ļ� *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����AVI�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.avi�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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

  //ȡ��m���еĽ���
  scale=0; //scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
  temp=number;
  while(temp!=1)//�����
  {    
	temp=temp/2;
    scale=scale+1; 
  }
	
  tempkey=9;//��������е�����

  for (i=0;i<TotalFrameNumber;i++)
    BitplanePositionSequence[i]=0;

  for(k=0;k<3;k++)
  {
   for(i=0;i<TotalFrameNumber;i++) //������еĳ���Ϊ��Ƶ��֡����Ŀ����Ϊ�˿���Ƕ���λ��
	{
      for(j=0;j<scale;j++) //ȡ��λ0/1ֵ-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//��ģ2�ӷ�
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
	int m_FrameByte;//�����ݴ�ԭʼ����
	double DifferenceSum;//���ز�

	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);

	if (FrameColorInfo[0]==8)//�Ҷ�
	{
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             DifferenceSum=0.0;//���ÿһ֡������ 
	         OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);
		     if (BitplanePositionSequence[k]==0)
			 {
				flag_0=flag_0+1;
				for (i=0;i<288;i++)
            	 for (j=0;j<352;j++)
				 {   
					     m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
          
           
 

		  //AVI�ļ������INDEX����
/*		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//����FrameBytes_Embedding����
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }
   
*/		    
            

		  	char temp_flag[80];
			sprintf(temp_flag,"���λƽ�棺 %d",flag_0);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"��һλƽ�棺 %d",flag_1);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"�ڶ�λƽ�棺 %d",flag_2);
            MessageBox(NULL,temp_flag,"",MB_OK);
			sprintf(temp_flag,"����λƽ�棺 %d",flag_3);
            MessageBox(NULL,temp_flag,"",MB_OK);
              
            
		
           
            fclose(fp);
	        sprintf(temp0,"AVI�ļ��ѳɹ�����");
	        MessageBox(NULL,temp0,"",MB_OK);

            

/////////////////Save the avi file ends//////////

	   }

  /*	   else //��ɫ
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//Ƕ��ˮӡ�����ж�Ӧ��λ��
       FrameNumber_Watermarked=0;//���Ƚ��ļ���λ�ó�ʼ��
	   
	  
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

	    if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		   fseek(fp,4104L,0);

		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,101384L,1);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
		   }
           fread(FrameBytes_Embedding,101376,1,fp);
	   }
	   else//��ɫ
	   {   
/*		   fseek(fp,2056L,0);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,304136L,1);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           fread(FrameBytes,304128,1,fp);   */
	   }   
	   fclose(fp);  


/*	   CFile ThisFile;
	   ThisFile.Open(FilePath_Watermarked,CFile::modeRead);
		 
	   
	   
	   if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		   ThisFile.Seek(4104L,CFile::begin);

		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         ThisFile.Seek(101384L,CFile::current);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
		   }
           ThisFile.Read(FrameBytes_Embedding,101376);
	   }
	   else
	   {   
		   ThisFile.Seek(2056L,CFile::begin);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         ThisFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
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
  
    //ȡ��m���еĽ���
	scale=0;//scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
    temp=number;
    while(temp!=1)//�����
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
	//����һ��[0,2^scale-1]֮��������a  2^scale=number
	srand((unsigned)time(NULL));
    tt=1.0/32767.0*(number-1);
	a=(long int)((double)rand()*tt);
	key[a]=0;//��õ�ַ����Ϊ0
	
	b=0;
	while(b==0) b=(long int)((double)rand()*tt);

//	tempkey[0]=b; //��m���г�ֵΪ1��number-1�������
    
	tempkey[0]=9; //��m���г�ֵΪ1��number-1�������

    for(i=0;i<number-1;i++)//����Ϊnumber-1,������1��number-1��α���ֵ
	{
      for(j=0;j<scale;j++)//ȡ��λ0/1ֵ-->bit[i]
	  {
        temp=tempkey[i]>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//��ģ2�ӷ�
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
		  PseudoRandomSequence[i*8+7-j]=((m_OriginalWatermarkBytes[i+62]>>j)&1);//ע���Ǵ����λȡ�ߵ�,�Լ�����ķ�ʽ
		 
	 WatermarkPermuted(PseudoRandomSequence,PseudoRandomSequenceBytes,RandomKey,1024);//������Ǵ��Һ��ˮӡ

	 for (i=0;i<(32*32/8);i++)
	 {
		Watermark_Permuted[i]=0;//���������
	    for (j=0;j<8;j++)
		{
           if ((PseudoRandomSequenceBytes[i*8+j]!=1)&&(PseudoRandomSequenceBytes[i*8+j]!=0))//���Һ��ˮӡ 
		   {
			   AfxMessageBox("Watermark Permuted Error!");
               return;
		   }
           if (PseudoRandomSequenceBytes[i*8+j]==1)//���Һ��ˮӡ
              Watermark_Permuted[i]=(unsigned char)pow(2,(7-j))+Watermark_Permuted[i];
		}
	 }

	 WatermarkFlag_Permuted=1;
     
/*  long int fh;
    //open the standard header file
	fh=_open("D:\\VC AVI\\READ AVI\\2BitBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	if(fh==-1)
	{
		MessageBox(NULL,"��׼[2BitBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard2BitBmpFileInfo,62)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"��ȡ��׼[2BitBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
   ofn.lpstrFilter=TEXT("TXT Files (*.txt)\0*.txt\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'TXT�ļ� *.txt'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_TXT; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_TXT;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="������Ա���";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="TXT";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.txt�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_TXT);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_TXT,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
	fprintf(fp,"Ƕ��ˮӡ����Ƶ��������Ӧ��PSNR");

	int i;
	for (i=0;i<TotalFrameNumber;i++)
       fprintf(fp,"\n�� %3d ֡  PSNR��  %f  dB       SNR:    %f  dB",i,PSNR[i],SNR[i]);
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
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI�ļ� *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����AVI�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.avi�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
//����ֵˮӡת�����ֽ���ʽ
  
  for (i=0;i<12672;i++)
	for (j=0;j<8;j++)
	  {
		 Watermark[i*8+7-j]=(m_OriginalWatermarkBytes[i+62]>>j)&1;
         if ((Watermark[i*8+7-j]!=0)&&(Watermark[i*8+7-j]!=1))
			 AfxMessageBox("Watermark Message Error!");
	  }




//******************************************************
//����Ƕ��λƽ���λ��
  int number=512;
  if (number<4||number>512) return;

  //ȡ��m���еĽ���
  scale=0; //scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
  temp=number;
  while(temp!=1)//�����
  {    
	temp=temp/2;
    scale=scale+1; 
  }
	
  tempkey=11;//��������е�����

  for (i=0;i<TotalFrameNumber;i++)
  {
//   BitplanePositionSequence[i]=3; 
     BitplanePositionSequence[i]=2;
  }

 for(k=0;k<1;k++)
  {
   for(i=0;i<TotalFrameNumber;i++) //������еĳ���Ϊ��Ƶ��֡����Ŀ����Ϊ�˿���Ƕ���λ��
	{
      for(j=0;j<scale;j++) //ȡ��λ0/1ֵ-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//��ģ2�ӷ�
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

	int m_FrameByte;//�����ݴ�ԭʼ����
	double DifferenceSum;//���ز�
	int temp_conversion;
//	unsigned char temp_line[352];//������ͼ�����ݵ�����
//	unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������

	CFile OriginalFile;
	OriginalFile.Open(FilePath,CFile::modeRead);

	if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             DifferenceSum=0.0;//���ÿһ֡������ 
	         OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);
			
             //******************************************************
			 //��ˮӡ������Ƶ
            
              number=65536*2;
              if (number<4||number>(65536*2)) return;

              //ȡ��m���еĽ���
	          scale=0; //scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
              temp=number;
              while(temp!=1)//�����
			  {    
	             temp=temp/2;
                 scale=scale+1; 
			  }
	
              tempkey=15;//��������е�����

              for(i=0;i<101376;i++) //����Ϊ352*288=101376,����101376��α���ֵ�����Ӹ�����
			  {
                  for(j=0;j<scale;j++) //ȡ��λ0/1ֵ-->bit[i]
				  {
                        temp=tempkey>>j;
	                    bit[j]=temp&1;
				  }
	              switch(scale)//��ģ2�ӷ�
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
					     m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
					 m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
                          m_FrameByte=FrameBytes_Embedding[i*352+j];//�ݴ�

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
          


		  //AVI�ļ������INDEX����
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  else
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//����FrameBytes_Embedding����
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
	        sprintf(temp0,"AVI�ļ��ѳɹ�����");
	        MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //��ɫ
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/
        
       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//Ƕ��ˮӡ�����ж�Ӧ��λ��
       FrameNumber_Watermarked=0;//���Ƚ��ļ���λ�ó�ʼ��
	  
}

void CREADAVIDoc::PseudoRandomSequence_Generator(unsigned char *sequence, int number, int seed)
{
  int bit[32];
  int i,j,scale,temp;
  int tempkey;
 

//  int number=65536*2;
  if (number<4||number>(65536*2)) return;

    //ȡ��m���еĽ���
	scale=0;//scale��������λ�Ĵ���--���ڲ�����Ϊ2^scale-1��m����     
    temp=number;
    while(temp!=1)//�����
	{    
	  temp=temp/2;
      scale=scale+1; 
	}
	
   tempkey=seed;//��������е�����

   for (i=0;i<101376;i++) //����Ϊ352*288=101376,����101376��α���ֵ�����Ӹ�����
	{
      for(j=0;j<scale;j++) //ȡ��λ0/1ֵ-->bit[i]
	  {
        temp=tempkey>>j;
	    bit[j]=temp&1;
	  }
	  switch(scale)//��ģ2�ӷ�
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
			m_ExtractedWatermarkBytes[i]=0;//���������
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}

		//����NCֵ
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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_Watermark; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_Watermark;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.bmp�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_Watermark);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_Watermark,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
   ofn.lpstrFilter=TEXT("AVI�ļ� *.avi\0*.avi\0All File *.*\0*.*\0\0"); 
   ofn.lpstrCustomFilter=NULL; 
   ofn.nMaxCustFilter=0; 
   
   ofn.nFilterIndex=1; //Select the item 'AVI�ļ� *.avi'
   ofn.lpstrFile=FileName_Attacked; 
   ofn.nMaxFile=_MAX_PATH; 
   
   ofn.lpstrFileTitle=FileTitle_Attacked; 
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;
   ofn.lpstrTitle="��򿪾������������Ƶ�ļ�";
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
	   AfxMessageBox("��ȡ���˱��β�����");
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

//����PSNR
   int i,j,k;
   int temp_difference;
   double DifferenceSum_Attacked;

   fseek(fp,172,0);
   fread(FrameInfoAttacked,1064,1,fp);//ע�⣺����ĵ�ɫ��˳��

   fseek(fp,4096,0);//�ҵ����ݵ�λ��

   CFile OriginalFile;
   OriginalFile.Open(FilePath,CFile::modeRead);

   OriginalFile.Seek(4096L,CFile::begin);

   for (k=0;k<TotalFrameNumber;k++)
   {
       DifferenceSum_Attacked=0.0;//���ÿһ֡������ 
	   OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
       OriginalFile.Read(Watermark_Attacked,101376);//�����������Watermark_Attacked

	   fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
       fread(FrameBytes_Attacked,101376,1,fp);

	   for (i=0;i<288;i++)
		  for (j=0;j<352;j++)
		  {  
		       temp_difference=(int)FrameBytes_Attacked[i*352+j]-(int)Watermark_Attacked[i*352+j];
			   DifferenceSum_Attacked=DifferenceSum_Attacked+temp_difference*temp_difference;
		   
		   }

       PSNR_Attacked[k]=10*log10(255.0*255.0*352.0*288.0/DifferenceSum_Attacked);
   }


//��ʾ��һ֡����
// for (i=0;i<TotalFrameNumber;i++)
   {
      fseek(fp,4096,0);
	  fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
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

   //�����ڴ�
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
   fread(FrameInfoAttacked,1064,1,fp);//�ļ��ĵ�ɫ����Ϣ

   fseek(fp,4096,0);

//   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//����
   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//����

  for (i=0;i<101376;i++)
   Watermark_Attacked[i]=0;

//������15֡����ȡˮӡ
  for (j=0;j<150;j++)  
  {
   fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
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


   //����NCֵ
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
		m_AttackedWatermarkBytes[i]=0;//���������
		for (j=0;j<8;j++)
		{
		   if (Watermark_Attacked[i*8+j]==1)
               m_AttackedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_AttackedWatermarkBytes[i];
	
		}
	}
	PositionFlag_AttackedWatermark=1;

	GlobalFree(mem);//�ͷ��ڴ�
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
   ofn.lpstrFilter=TEXT("TXT Files (*.txt)\0*.txt\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'TXT�ļ� *.txt'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_TXT; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_TXT;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="������Ա���";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="TXT";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.txt�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_TXT);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_TXT,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
	fprintf(fp,"�ܵ����������Ƶ��������Ӧ��PSNR");

	int i;
	for (i=0;i<TotalFrameNumber;i++)
       fprintf(fp,"\n�� %3d ֡  PSNR��  %f  dB",i,PSNR_Attacked[i]);
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

	if (FrameColorInfo[0]==8)//�Ҷ�
	{
	   fseek(fp,4104L,0);

	   for (i=0;i<FrameNumber_Attacked;i++)
	   {
	      fseek(fp,101384L,1);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
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
	   else//��ɫ
	   {   
/*		   fseek(fp,2056L,0);
		   for (i=0;i<FrameNumber_Watermarked;i++)
		   {
	         fseek(fp,304136L,1);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           fread(FrameBytes,304128,1,fp);   */
	   }   
	   fclose(fp);  


}

void CREADAVIDoc::OnExtractCurrentWatermarkattacked()
{   
   int i,j,temp_bit,temp_conversion;

   //�����ڴ�
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
   fread(FrameInfoAttacked,1064,1,fp);//�ļ��ĵ�ɫ����Ϣ

   fseek(fp,4096,0);

//   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//����
   PseudoRandomSequence_Generator(PseudoRandomSequence,131072,15);//����

  for (i=0;i<101376;i++)
   Watermark_Attacked[i]=0;

//��ȡ��ǰ֡ˮӡ
  for (j=0;j<(FrameNumber_Attacked+1);j++)  
  {
   fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
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

   //����NCֵ
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
		m_AttackedWatermarkBytes[i]=0;//���������
		for (j=0;j<8;j++)
		{
		   if (Watermark_Attacked[i*8+j]==1)
               m_AttackedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_AttackedWatermarkBytes[i];
	
		}
	}
	PositionFlag_AttackedWatermark=1;

	GlobalFree(mem);//�ͷ��ڴ�

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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_Watermark; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_Watermark;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("������ȡ���˴洢��ϵͳ�Զ�����Ϊtemp.bmp�ļ�");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_Watermark);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_Watermark,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI�ļ� *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����AVI�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β���");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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

//	int m_FrameByte;//�����ݴ�ԭʼ����
	double DifferenceSum;//���ز�
	int OriginalPixelValueSum;//ԭʼ���صĺͣ���������SNR
	int temp_conversion;
//	unsigned char temp_line[352];//������ͼ�����ݵ�����
	unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������

	double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��

	double *BlockDCT_Coefficient_Saved[1584][8];//ǰһ֡�ķֿ�DCTϵ��
	double *temp_DC_Coefficient_Difference;//ǰ��֡��Ӧ��DCTֱ��ϵ��֮��
    double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double *temp_DCT_Edge_Detection;//��DCT���ж��Ƿ��б�Ե����
	double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ
	int flag_DC_Coefficient,flag_AC_Coefficient;	 

	//�����ڴ�
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
	
    //��64λ��ˮӡ�������ң���Ƶ��
    //����α�������
    PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //����α������ж�ˮӡ���е���  
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

	//����ˮӡǶ���ʱ��
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
//		  for (k=0;k<10;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//��ʾDCϵ������Ϣ
			 flag_AC_Coefficient=0;//��ʾACϵ������Ϣ
//			 DC_Coefficient_Sum=0.0;
			 DifferenceSum=0.0;//���ÿһ֡������
			 OriginalPixelValueSum=0;//���ÿһ֡������
	         OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
             
             OriginalFile.Read(FrameBytes_Embedding,101376);

		//	 OriginalFile.Seek(101376L,CFile::current);
			 for (i=0;i<288;i++)//��ͼ�����ݵ�����
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//�Ե�ǰ֡����8*8�ֿ�
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
			
                DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
// 				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//ԭͼ�����DCT�任 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任

                if (k>=1)//�Ƚ�ǰ����֡�����ﲻ���������ۼƵ��������Ϊǰһ֡����Ϣ�洢��DCTϵ��������
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//ǰ��֡��ӦDCTϵ���Ĳ�ֵ
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
                 
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			
			//	DC_Coefficient_Sum=DC_Coefficient_Sum+BlockDCT_Coefficient[l][0][0];//��ֱ��ϵ���������
			 }

/*          if (k>=1)
			{
				char temp_DC_flag[10];
			    sprintf(temp_DC_flag,"%d   ��%d֡",flag_DC_Coefficient,k-1);
                MessageBox(NULL,temp_DC_flag,"ǰ��֡DCϵ��֮���100",MB_OK); 
			}

			char temp_AC_flag[10];
			sprintf(temp_AC_flag,"%d   ��%d֡",flag_AC_Coefficient,k);
            MessageBox(NULL,temp_AC_flag,"ACϵ���ķ����1000",MB_OK); 
*/
//          DC_Coefficient_Sum=DC_Coefficient_Sum/1584.0;

//*****************************************************************

						 
//		    int temp;
			int r=0; 
			//�����жϺ�����Ӧ�Ŀ���Ƕˮӡ
			
			  if (k==0)//��һ֡��������,���ο��ڽ���֡
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				{ 
				   //if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����ӿ� for table tennis improved2.avi 
				   if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����ӿ� 
				   {
					//��¼��Ƕ��ˮӡ��λ��
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;

					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//��ǳ�8*8����ı߽�
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==7)
					    //Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                      
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
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//�����˶���Ҳ�������ӿ� 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//���˶��� 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//�������ӿ�  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//���������������ܺ�
               
	//	        if ((temp_DC_Coefficient_Difference[l]>=120)&&(temp_AC_Coefficient_Energy[l]>=16000)&&(fabs(BlockDCT_Coefficient[l][0][0])>930))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�  for table tennis improved2.avi 
                if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12800))//&&(fabs(BlockDCT_Coefficient[l][0][0])>900))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�  for table tennis improved2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=25)&&(temp_AC_Coefficient_Energy[l]>=6000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>600))//�����˶���Ҳ�������ӿ�  for miss america_0.avi 
                // if ((temp_DC_Coefficient_Difference[l]>=100)&&(temp_AC_Coefficient_Energy[l]>=5000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>600))//�����˶���Ҳ�������ӿ�
    //          if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
//				if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))

				{  
					//��¼��Ƕ��ˮӡ��λ��
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
						
						//��ǳ�8*8����ı߽�
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==7)
						//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
               

                          
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
		
        //    Frame_Block[1500][0][0]=255;   //��ʱ����Ƕ��ˮӡ

            EmbeddingRegionSumSequence[k]=r; 
/*
//**********************************************************************
          //�ٴν�����֤���жϾ����Ķ����Ƿ����ҵ�ԭ��������
          for (l=0;l<1584;l++)
			 {	
                temp_DCT_Coefficient_Mean=0.0;
			    flag_DC_Coefficient=0;//��ʾDCϵ������Ϣ
			    flag_AC_Coefficient=0;//��ʾACϵ������Ϣ
				temp_DCT_Coefficient_Variance[l]=0.0; 
				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//ԭͼ�����DCT�任 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任

                if (k>=1)//�Ƚ�ǰ����֡
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//ǰ��֡��ӦDCTϵ���Ĳ�ֵ
				   if (temp_DC_Coefficient_Difference[l]>=100)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
                 
				if (temp_DCT_Coefficient_Variance[l]>=1000)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			
			//	DC_Coefficient_Sum=DC_Coefficient_Sum+BlockDCT_Coefficient[l][0][0];//��ֱ��ϵ���������
			 }


			//�����жϺ�����Ӧ�Ŀ���Ƕˮӡ
			for (l=0;l<1584;l++)
            {
				if (temp_DCT_Coefficient_Variance[l]>=1000)//�����˶���Ҳ������� 
				{  
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   Frame_Block[l][m][n]=255;//��ǳ������Ŀ�  
					}
				}
			}
//********************************************************************************		
*/            
      
			//���ֿ������ݻ�ԭΪ��������
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //������Ӧ��PSNR(��Լ��ʱ7����)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//��������PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);

//			fwrite(FrameBytes_Embedding,101376,1,fp);
	
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//�����ݰ�����洢
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("ˮӡǶ����̽���!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"ˮӡǶ����̣�%lf��",m_time);
          MessageBox(NULL,temp_embeddingtime,"ˮӡǶ�����!",MB_OK);

		  //AVI�ļ������INDEX����
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//����FrameBytes_Embedding����
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI�ļ��ѳɹ�����");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //��ɫ
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//Ƕ��ˮӡ�����ж�Ӧ��λ��
       FrameNumber_Watermarked=0;//���Ƚ��ļ���λ�ó�ʼ��

	   GlobalFree(mem);
	   GlobalFree(mem_DC_Difference);
	   GlobalFree(mem_DCT_Variance);
	   GlobalFree(mem_AC_Energy);
	   GlobalFree(mem_DCT_Edge_Detection);
}

void CREADAVIDoc::DCT(unsigned char inf1[][8], double DCTout[][8])
{
  //DCT�任
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
  //��DCT�任
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
   ofn.lpstrFilter=TEXT("AVI�ļ� *.avi\0*.avi\0All File *.*\0*.*\0\0"); 
   ofn.lpstrCustomFilter=NULL; 
   ofn.nMaxCustFilter=0; 
   
   ofn.nFilterIndex=1; //Select the item 'AVI�ļ� *.avi'
   ofn.lpstrFile=FileName_Attacked; 
   ofn.nMaxFile=_MAX_PATH; 
   
   ofn.lpstrFileTitle=FileTitle_Attacked; 
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;
   ofn.lpstrTitle="�����Ƶ�ļ�";
   ofn.Flags=OFN_FILEMUSTEXIST;
   ofn.nFileOffset;
   ofn.nFileExtension;
   ofn.lpstrDefExt="AVI";
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL;
   ofn.lpTemplateName=NULL; 

   if(GetOpenFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β�����");
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
   fread(FrameInfoAttacked,1064,1,fp);//ע�⣺����ĵ�ɫ��˳��

   for (k=0;k<TotalFrameNumber;k++)
   {
	   fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
       fread(FrameBytes_Attacked,101376,1,fp);
   }

// ��ʾ��һ֡����
// for (i=0;i<TotalFrameNumber;i++)
   {
      fseek(fp,4096,0);
	  fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ

    //�����ڴ�
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

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//ԭͼ�����DCT�任 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任
			
            /*  for (m=0;m<8;m++)
				   for (n=0;n<8;n++)
					{
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				      
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ
            */ 
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					    temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
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

			//�����жϣ�ȷ����ѡ�飬Ȼ��ȷ��ˮӡ���ڿ�
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			//	if (temp_DCT_Coefficient_Variance[l]>=1000)//�����ж��Ƿ�������� 
			//  if ((temp_AC_Coefficient_Energy[l]>=28000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1080))
			//  if (temp_AC_Coefficient_Energy[l]>=35000)// for mobile & calendar
				if (temp_AC_Coefficient_Energy[l]>=10000)// for table tennis
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
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
				  
				 //�ٴ��м��ˮӡ�Ƿ����
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

          if (existing_block_number>1)//����ȷ��ˮӡ��Ϣ
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
			m_ExtractedWatermarkBytes[i]=0;//���������
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}
*/

		//����NCֵ
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
	double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ

    //�����ڴ�
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

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//ԭͼ�����DCT�任 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任
			
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
					    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					    temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]); 
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
			 }

            int correlation_threshold;
			int existing_block_number=0;
			int candidate_block_number=0;
			int watermark_attacked_sum[64];

			for (m=0;m<64;m++)
            {
              watermark_attacked_sum[m]=0;
			}

			//�����жϣ�ȷ����ѡ�飬Ȼ����ˮӡ�Ƿ����
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			//	if (temp_DCT_Coefficient_Variance[l]>=1000)//�����ж��Ƿ�������� 
			//  if (temp_AC_Coefficient_Energy[l]>=12000)//TD'<TD 12000<16000
			    if (temp_AC_Coefficient_Energy[l]>=40000)//TD'<TD 35000<40000
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
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
				  
				 //�ٴ��м��ˮӡ�Ƿ����
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
         sprintf(temp_existing_block_number,"����ˮӡ�Ŀ��� %d",existing_block_number);
         MessageBox(NULL,temp_existing_block_number,"����ˮӡ�Ŀ���",MB_OK);//Display the info

         if (existing_block_number>1)//����ȷ��ˮӡ��Ϣ
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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentEmbeddingFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentEmbeddingFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β�����");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_CurrentEmbeddingFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentEmbeddingFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
		MessageBox(NULL,"��׼[256GrayBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"��ȡ��׼[256GrayBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
	double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ

    //�����ڴ�
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
   fread(FrameInfoAttacked,1064,1,fp);//�ļ��ĵ�ɫ����Ϣ

   fseek(fp,4096,0);

   PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//����

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

   //ÿһ֡�����ݳ��ȣ�8��101376��bytes
   //�����ǵ�һ֡
   fseek(fp,8L,1);
   fseek(fp,101376L,1);

//������15֡����ȡˮӡ
  for (j=0;j<29;j++)  
  {
   fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
   fread(FrameBytes_Attacked_Watermark,101376,1,fp);

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (k=0;k<352;k++)
            FrameBytes_Inserting[287-i][k]=FrameBytes_Attacked_Watermark[i*352+k];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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
//				DCT(Frame_Block[l],BlockDCT_Coefficient[l]);//ԭͼ�����DCT�任 
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任
			
                for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
	                  temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					  temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);  
					}
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 }

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
			 }

			//�����жϣ�ȷ����ѡ�飬Ȼ��ȷ��ˮӡ���ڿ�
			for (l=0;l<1584;l++)
            {
				correlation_threshold=0;
			  //if (temp_DCT_Coefficient_Variance[l]>=1000)//�����ж��Ƿ�������� 
			  //if (temp_AC_Coefficient_Energy[l]>=15000)//TD'<TD 15000<16000
				if (temp_AC_Coefficient_Energy[l]>=35000)//TD'<TD 35000<40000
				{  
                  candidate_block_number=candidate_block_number+1;
				  for (m=0;m<8;m++)
				    for (n=0;n<8;n++)
					{
             		   //Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
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
				  
		 		 //�ٴ���ȷ��ˮӡ���ڿ�
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

//         if (existing_block_number>1)//����ȷ��ˮӡ��Ϣ
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
   
    //�����ڴ�
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

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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
					//Դ��ȡ����Ҫԭʼ��Ƶ���еĲ���
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
	

		correct_bit_number=0;//��ȷ��λ��
		correct_rate=0.0;//��ȷ��
	    for (m=0;m<64;m++)
		{ 
			if (Watermark_Extracted[m]==m_OriginalWatermarkBytes[m]) 
                 correct_bit_number=correct_bit_number+1; 
		}
		correct_rate=correct_bit_number/64.0*100.0;

/*		//byte -> bit 
		for (i=0;i<12672;i++)
		{
			m_ExtractedWatermarkBytes[i]=0;//���������
			for (j=0;j<8;j++)
			{
				if (Watermark_Extracted[i*8+j]==1)
                  m_ExtractedWatermarkBytes[i]=(unsigned char)pow(2,(7-j))+m_ExtractedWatermarkBytes[i];
	
			}
		}
*/

		//����NCֵ
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������

	PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//64*2

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}
    //��ä��ȡ����Ҫԭʼ���в��룩
    int watermark_attacked_sum[64];
	int r,temp_subscript;
	int correlation_threshold=0;
	int existing_block_number=0;
	int bitplane_position=0;
	double denominator=0.0;

	for (m=0;m<64;m++)
        watermark_attacked_sum[m]=0;
			
    for (r=0;r<EmbeddingRegionSumSequence[FrameNumber_Attacked];r++)//����ÿһ֡��¼�Ŀ���
	{
		correlation_threshold=0;
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
				{   
					//Դ��ȡ����Ҫԭʼ��Ƶ���еĲ���
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
        
		//���ˮӡ�Ƿ����
		for (m=0;m<8;m++)
		    for (n=0;n<8;n++)
			{
				if (Watermark_Attacked[m*8+n]==m_OriginalWatermarkBytes[m*8+n]) 
                        correlation_threshold=correlation_threshold+1; 
				if (Watermark_Attacked[m*8+n]!=m_OriginalWatermarkBytes[m*8+n])
						correlation_threshold=correlation_threshold-1; 
			
			}
       
        bitplane_position=3;

/*      //�������λƽ��
        //1110 1111
	    if (correlation_threshold<10)
		{
			correlation_threshold=0;
		    for (m=0;m<8;m++)
			   for (n=0;n<8;n++)
				{   
					//Դ��ȡ����Ҫԭʼ��Ƶ���еĲ���
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
            	//���ˮӡ�Ƿ����
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
					//Դ��ȡ����Ҫԭʼ��Ƶ���еĲ���
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
            	//���ˮӡ�Ƿ����
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
    sprintf(temp_existing_block_number,"����ˮӡ�Ŀ��� %d",existing_block_number);
    MessageBox(NULL,temp_existing_block_number,"����ˮӡ�Ŀ���",MB_OK);//Display the info

	if (existing_block_number>1)//����ȷ��ˮӡ��Ϣ	     
	{
	  for (m=0;m<64;m++)
	  {
        //if (watermark_attacked_sum[m]>=(EmbeddingRegionSumSequence[FrameNumber_Attacked]/2.0+2.5)) for table tennis 4Mbps
		
/*		//���м�Ȩƽ��
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

    //�����жϣ�ȷ����ѡ�飬Ȼ��ȷ��ˮӡ���ڿ�
	for (l=0;l<1584;l++)
    {
	
		if (temp_DCT_Coefficient_Variance[l]>=1000)//�����ж��Ƿ�������� 
		{  
           candidate_block_number=candidate_block_number+1;
		   for (m=0;m<8;m++)
			  for (n=0;n<8;n++)
				{
             	   //Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
   
    FILE *fp;
    fp=fopen(FileName_Attacked,"rb");
    if (fp==NULL)
    {
	  MessageBeep(-1);
	  AfxMessageBox("Attacked File Error");
	  return;
    }

    fseek(fp,172,0);
    fread(FrameInfoAttacked,1064,1,fp);//�ļ��ĵ�ɫ����Ϣ
    fseek(fp,4096,0);

    PseudoRandomSequence_Generator(PseudoRandomSequence,128,15);//����

    for (i=0;i<64;i++)
       Watermark_Attacked[i]=0;

    int watermark_attacked_sum[64];
    int r,temp_subscript,EmbeddingRegionTotalSum=0;

    for (m=0;m<64;m++)
       watermark_attacked_sum[m]=0;

//   //ÿһ֡�����ݳ��ȣ�8��101376��bytes
//   //�����ǵ�һ֡
//   fseek(fp,8L,1);
//   fseek(fp,101376L,1);

//������15֡����ȡˮӡ
  for (f=0;f<15;f++)  
  {
    fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
    fread(FrameBytes_Attacked_Watermark,101376,1,fp);

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked_Watermark[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
	for (l=0;l<1584;l++)
		for (m=0;m<8;m++)
			for (n=0;n<8;n++)
			{ 
				Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
			}

     for (r=0;r<EmbeddingRegionSumSequence[f];r++)
            for (m=0;m<8;m++)//�ӵ�0λ����63λ
				for (n=0;n<8;n++)
				{   
					//Դ��ȡ����Ҫԭʼ��Ƶ���еĲ���
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
   if (PositionFlag==1)//ԭʼ��Ƶ����
	{
	   //���ܵ����ر��ֲ���
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
   if (PositionFlag==1)//ԭʼ��Ƶ����
	{
	   //���ܵ����ر��ֲ���
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
       //���ܵ����ر��ֲ���
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
   if (PositionFlag==1)//ԭʼ��Ƶ����
   {
	   //���ܵ����ر��ֲ���
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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentOriginalFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentOriginalFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β�����");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_CurrentOriginalFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentOriginalFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
	   //open the standard header file ע������ֻ�����˻Ҷ�ͼ������
	   fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader.bmp",_O_RDONLY|_O_BINARY);
	   if(fh==-1)
	   {
		 MessageBox(NULL,"��׼[256GrayBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);		
	   }
       //Read file
       if (_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
	   {
	      _close(fh);   
          MessageBox(NULL,"��ȡ��׼[256GrayBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
	   }
       _close(fh);  
       fwrite(m_Standard256GrayBmpFileInfo,1078,1,fp);
       fwrite(FrameBytes,101376,1,fp);
	}

	if (FrameSizeInfo[0]==208) //720*576
    {
	   //open the standard header file ע������ֻ�����˻Ҷ�ͼ������
	   fh=_open("D:\\VC AVI\\READ AVI\\256GrayBmpHeader720x576.bmp",_O_RDONLY|_O_BINARY);
	   if(fh==-1)
	   {
		 MessageBox(NULL,"��׼[256GrayBmpHeader720x576.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);		
	   }
       //Read file
       if (_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
	   {
	      _close(fh);   
          MessageBox(NULL,"��ȡ��׼[256GrayBmpHeader720x576.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
   
   int pixel_value;//ע������������ 

   int i,j;
   if (PositionFlag==1)//ԭʼ��Ƶ����
   {
	   //���ܵ����ر��ֲ���
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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_DifferenceImage; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_DifferenceImage;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β�����");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_DifferenceImage);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_DifferenceImage,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
		MessageBox(NULL,"��׼[256GrayBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"��ȡ��׼[256GrayBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI�ļ� *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����AVI�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β���!");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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

	double DifferenceSum;//���ز�
	int OriginalPixelValueSum;//ԭʼ���صĺͣ���������SNR
//	int temp_conversion;
	unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
	double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *BlockDCT_Coefficient_Saved[1584][8];//ǰһ֡�ķֿ�DCTϵ��
	double *temp_DC_Coefficient_Difference;//ǰ��֡��Ӧ��DCTֱ��ϵ��֮��
    double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double *temp_DCT_Edge_Detection;//��DCT���ж��Ƿ��б�Ե����
	double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ
	int flag_DC_Coefficient,flag_AC_Coefficient;	 

	//�����ڴ�
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
	//��ˮӡ��Ϣ���к������� 66bits->90bits
    //m_OriginalWatermarkInformationSequence->m_HammingCodedOriginalWatermarkInformationSequence
    //�Ƚ��з��飬��Ϣλ11λ������4λ�ලλ
/*    unsigned char temp_hammingcode[15];
	for (i=0;i<6;i++)
	{
		for (j=0;j<11;j++)
           temp_hammingcode[j]=m_OriginalWatermarkInformationSequence[i*11+j];
	    
		//�ල����
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        temp_hammingcode[11]=(temp_hammingcode[0]+temp_hammingcode[1]
			                 +temp_hammingcode[2]+temp_hammingcode[3]
							 +temp_hammingcode[5]+temp_hammingcode[7]
							 +temp_hammingcode[8])%2;//ģ2��
        temp_hammingcode[12]=(temp_hammingcode[1]+temp_hammingcode[2]
			                 +temp_hammingcode[3]+temp_hammingcode[4]
							 +temp_hammingcode[6]+temp_hammingcode[8]
							 +temp_hammingcode[9])%2;//ģ2��
        temp_hammingcode[13]=(temp_hammingcode[2]+temp_hammingcode[3]
			                 +temp_hammingcode[4]+temp_hammingcode[5]
							 +temp_hammingcode[7]+temp_hammingcode[9]
							 +temp_hammingcode[10])%2;//ģ2��
        temp_hammingcode[14]=(temp_hammingcode[0]+temp_hammingcode[1]
			                 +temp_hammingcode[2]+temp_hammingcode[4]
							 +temp_hammingcode[6]+temp_hammingcode[7]
	                         +temp_hammingcode[10])%2;//ģ2��   
	    //���������д洢
		for (j=0;j<15;j++)
           m_HammingCodedOriginalWatermarkInformationSequence[i*15+j]=temp_hammingcode[j];

	}
*/

	//���ڼ��龭���˲�����صĽ��
//  m_HammingCodedOriginalWatermarkInformationSequence[84]=1;
//  m_HammingCodedOriginalWatermarkInformationSequence[85]=1;
//  m_HammingCodedOriginalWatermarkInformationSequence[86]=1;

/*	for (i=3;i<45;i++) 
	    m_HammingCodedOriginalWatermarkInformationSequence[i]=1;//�൱��˫���Ե�-1

	for (i=45;i<90;i++) 
	    m_HammingCodedOriginalWatermarkInformationSequence[i]=0;//�൱��˫���Ե�1
*/
	
	for (j=0;j<66;j++)
	{
        if ((m_HammingCodedOriginalWatermarkInformationSequence[j]!=1)&&
			(m_HammingCodedOriginalWatermarkInformationSequence[j]!=0))
			AfxMessageBox("ˮӡλ����");
	}

	
	//��90λ��ˮӡ��������
    //����α�������

    int seed;//����ȷ��ˮӡ��Ϣλ��˳��


	for (i=0;i<22;i++)
	{
      seed=i+120;
      //seed=i+200;
      //PseudoRandomSequence_Generator(PseudoRandomSequence,1024,120);//31<32
      PseudoRandomSequence_Generator(PseudoRandomSequence,1024,seed);
	  //����α������ж�ˮӡ���е���  
      for (j=0;j<3;j++)
	  { 
        if (m_HammingCodedOriginalWatermarkInformationSequence[i*3+j]==1)//˫���Ե�-1,α������з���
		{
		  for (k=(j*10);k<((j+1)*10);k++)
		  {
			  if (PseudoRandomSequence[k]==1)//�൱��˫���Ե�-1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=-1;//˫���Ե�-1
			      m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=1;//�൱��˫���Ե�1
			  }
			  else //PseudoRandomSequence[k]==0���൱��˫���Ե�1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=1;//˫���Ե�1
				  m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=-1;//�൱��˫���Ե�-1
			  }
		  }
		}
		else//˫���Ե�1,α������в���
		{
          for (k=(j*10);k<((j+1)*10);k++)
		  {
			  if (PseudoRandomSequence[k]==1)//�൱��˫���Ե�-1
			  {
				  m_SpreadSpectrumWatermarkSequence[i][k]=-1;//˫���Ե�-1
				  m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=-1;//˫���Ե�-1
			 
			  }
			  else //PseudoRandomSequence[k]==0��˫���Ե�1
              {
				  m_SpreadSpectrumWatermarkSequence[i][k]=1;//˫���Ե�1
			      m_ModulatedSpreadSpectrumWatermarkSequence[i][k]=1;//˫���Ե�1
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

	//����ˮӡǶ���ʱ��
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//��ʾDCϵ������Ϣ
			 flag_AC_Coefficient=0;//��ʾACϵ������Ϣ
			 DifferenceSum=0.0;//���ÿһ֡������
			 OriginalPixelValueSum=0;//���ÿһ֡������
	         OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
             OriginalFile.Read(FrameBytes_Embedding,101376);

		
			 for (i=0;i<288;i++)//��ͼ�����ݵ�����
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//�Ե�ǰ֡����8*8�ֿ�
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
       //       DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);

                if (k>=1)//�Ƚ�ǰ����֡�����ﲻ���������ۼƵ��������Ϊǰһ֡����Ϣ�洢��DCTϵ��������
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//ǰ��֡��ӦDCTϵ���Ĳ�ֵ
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			 }

//*****************************************************************

			  int r=0; 
			  //�����жϺ�����Ӧ�Ŀ���Ƕˮӡ
			
			  if (k==0)//��һ֡��������,���ο��ڽ���֡
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				{ 
				   //if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����ӿ� for table tennis improved2.avi 
				   if (temp_AC_Coefficient_Energy[l]>=80000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����ӿ� 
				   {
					//��¼��Ƕ��ˮӡ��λ��
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;

					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//��ǳ�8*8����ı߽�
					//	if (m==0)
					//		Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //   if (m==7)
					//	    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //    if (n==0)
					//		Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //    if (n==7)
					//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
               

                          
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
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//�����˶���Ҳ�������ӿ� 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//���˶��� 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//�������ӿ�  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//���������������ܺ�
               
	//	        if ((temp_DC_Coefficient_Difference[l]>=120)&&(temp_AC_Coefficient_Energy[l]>=16000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�  for table tennis improved2.avi 
                if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
//				if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1000))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1160)) 
				{  
					//��¼��Ƕ��ˮӡ��λ��
				    EmbeddingRegionSequence[k][r]=l;
			        r=r+1;
        
					for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					{ 
                        //temp=Frame_Block[l][m][n];
						
						//��ǳ�8*8����ı߽�
					//    if (m==0)
					//		Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //    if (m==7)
					//	    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //    if (n==0)
					//		Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                    //    if (n==7)
					//	    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
         
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
		//sprintf(temp_originalembeddingpoint,"Ƕ���ԭʼ����ֵ%d",Frame_Block[518+3*22][0][0]);
        //MessageBox(NULL,temp_originalembeddingpoint,"ˮӡǶ���ԭʼ����ֵ",MB_ICONSTOP|MB_OK);//Display the info

        //int temp_result;
        //temp_result=255;  //��ʱ����Ƕ��ˮӡ

		
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
					x=(int)uniform(0.0,10.0,&s1);//������[0.0,10.0]�����ھ��ȷֲ��������
				    y=(int)uniform(0.0,8.0,&s2);//������[0.0,10.0]�����ھ��ȷֲ��������
					Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]=((Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]&223)+(0*32));   //��ʱ����Ƕ��ˮӡ,ע����������
				}
			}
			if (m_ModulatedSpreadSpectrumWatermarkSequence[i][k]==-1)//-1-> 1
			{
				for (j=0;j<3;j++)
				{
					x=(int)uniform(0.0,10.0,&s1);//������[0.0,10.0]�����ھ��ȷֲ��������
				    y=(int)uniform(0.0,8.0,&s2);//������[0.0,10.0]�����ھ��ȷֲ��������
					Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]=((Frame_Block[180+(i/2*44)+((i%2)*22)+x][0+y][0]&223)+(1*32));   //��ʱ����Ƕ��ˮӡ,ע����������
				}
			}
		/*   if (i==0)
		   {
			 for (j=-1;j<2;j++)
			{
			 temp_result=Frame_Block[473+(i/2*44)+j*2][i%2*4][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
     
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
			 temp_result=Frame_Block[473+(i/2*44)+j][i%2*4][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
     
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
          //�ж��Ƿ�λ��ͼ����м����� 200x160
          //if (((518+i*5)/44*8)<76)||((518+i*5)/44*8)>275)||
		  //  ((518+i*5)l%44*8)<64)||((518+i*5)l%44*8)>223))
          
         //if ((i!=3)&&(i!=5)&&(i!=23))	// for table_g1_frames_2.avi 	
     /*      if ((i!=1)&&(i!=3)&&(i!=5)&&(i!=23)&&(i!=7)&&(i!=9))	// for table_g1_frames_2.avi 	
          {
		     //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		     temp_result=Frame_Block[518+i*22][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
             // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
		
			    //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		        temp_result=Frame_Block[518+i*22+6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
		  //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		  temp_result=Frame_Block[518+i*22][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
          // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
                	 
		        //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		        temp_result=Frame_Block[518+i*22-6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
                	 
		        //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		        temp_result=Frame_Block[518+i*22+12][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
                // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
                	 
		           //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		           temp_result=Frame_Block[518+i*22+19][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
                   // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
		           //temp_result=255;  //��ʱ����Ƕ��ˮӡ 
		           temp_result=Frame_Block[518+i*22+6][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[i][k];   //��ʱ����Ƕ��ˮӡ
                   // temp_result=Frame_Block[518][0][0]+20*m_ModulatedSpreadSpectrumWatermarkSequence[0][k];   //��ʱ����Ƕ��ˮӡ
		
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
		//sprintf(temp_embeddingpoint,"Ƕ�������ֵ%d",Frame_Block[518+3*22][0][0]);
        //MessageBox(NULL,temp_embeddingpoint,"ˮӡǶ���",MB_ICONSTOP|MB_OK);//Display the info
		

        EmbeddingRegionSumSequence[k]=r; 

     	//���ֿ������ݻ�ԭΪ��������
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //������Ӧ��PSNR(��Լ��ʱ7����)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//��������PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);
	
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//�����ݰ�����洢
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("ˮӡǶ����̽���!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"ˮӡǶ����̣�%lf��",m_time);
          MessageBox(NULL,temp_embeddingtime,"ˮӡǶ�����!",MB_OK);

		  //AVI�ļ������INDEX����
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//����FrameBytes_Embedding����
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI�ļ��ѳɹ�����");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //��ɫ
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//Ƕ��ˮӡ�����ж�Ӧ��λ��
       FrameNumber_Watermarked=0;//���Ƚ��ļ���λ�ó�ʼ��

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
   short int *temp_ConvolutionResult;//�����͸�ͨ�˲�����������õĽ�������и�
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
	fprintf(fp_txt,"��ͬλ�ö�Ӧ����ؼ����");
   
// unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
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
   //����ˮӡ�������ȡ��ʱ��
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
			 fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
             fread(FrameBytes_ForWatermarkExtraction,101376,1,fp);

	/*		 for (i=0;i<288;i++)//��ͼ�����ݵ�����
			 {
				 fseek(fp,-352L,1);
                 fread(FrameBytes_Inserting[i],352,1,fp);
				 fseek(fp,-352L,1);
			 }
			   
			 fseek(fp,101376L,1);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//�Ե�ǰ֡����8*8�ֿ�
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}
			 }
   */
    //char temp_originalembeddingpoint[100];
    //sprintf(temp_originalembeddingpoint,"��������ֵ%d",FrameBytes_ForWatermarkExtraction[(287-temp_ty)*352+temp_tx]);
    //MessageBox(NULL,temp_originalembeddingpoint,"ˮӡ��⣨δ������ͨ�˲�ǰ��",MB_OK);//Display the info
	
	//�ȶ�temp_ConvolutionResult���鸳��ֵ 
  //  for (i=0;i<288;i++)
  //	for (j=0;j<352;j++) 
  //         temp_ConvolutionResult[101376-(i+1)*352+j]=FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j];
	
	//�Ƚ��и�ͨ�˲��Ĵ���
     
    //���ܵ����ر��ֲ���
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
//      Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]=FrameBytes_ForWatermarkExtraction[101024-ty*352+tx];//׼����ʱ��������˲�

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
  
    //�ҵ���ǰ֡Ƕ��ĵ�
    //[l/44*8+m][l%44*8+n]->[1500/44*8+0][1500%44*8+0]->[272][32]
    //Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-272)*352+32];

//    Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-ty)*352+tx];
  
	//char temp_embeddingpoint[100];
    //sprintf(temp_embeddingpoint,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis[k]);
    //MessageBox(NULL,temp_embeddingpoint,"ˮӡ��⣨������ͨ�˲���",MB_ICONSTOP|MB_OK);//Display the info
 

   }
   //���ǽ���һάԤ���˲�
/* //[-1 -1 -1 6 -1 -1 -1]/6
   //���е����߽��������չ0,1,2,27,28,29
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
   //���е����߽��������չ0,1,2,3,26,27,28,29
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
   //���е����߽��������չ0,1,2,3,26,27,28,29
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
      //sprintf(temp_embeddingpoint_1D,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]);
      //MessageBox(NULL,temp_embeddingpoint_1D,"ˮӡ��⣨δ����һάԤ���˲�ǰ��",MB_ICONSTOP|MB_OK);//Display the info
      //sprintf(temp_embeddingpoint_1D,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]);
      //MessageBox(NULL,temp_embeddingpoint_1D,"ˮӡ��⣨����һάԤ���˲���",MB_ICONSTOP|MB_OK);//Display the info
  

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
    if (correlationresult0>0)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=0;
    if (correlationresult0<0)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=1;

    if (correlationresult1>0)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=0;
    if (correlationresult1<0)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=1;

    if (correlationresult2>0)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=0;
    if (correlationresult2<0)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=1;

	correlationresult_threshold=correlationresult_threshold+1;
	pn_number=pn_number+1;
	}
*/

    correlationresult=abs(correlationresult0)+abs(correlationresult1)+abs(correlationresult2); 
	if (correlationresult==30)
	{
    if (correlationresult0==10)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=0;
    if (correlationresult0==-10)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3]=1;

    if (correlationresult1==10)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=0;
    if (correlationresult1==-10)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+1]=1;

    if (correlationresult2==10)//˫���Ե�1-�����Ե�0
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=0;
    if (correlationresult2==-10)//˫���Ե�-1-�����Ե�1
        m_ExtractedHammingCodedOriginalWatermarkInformationSequence[pn_number*3+2]=1;

	correlationresult_threshold=correlationresult_threshold+1;
	pn_number=pn_number+1;	
	}

	if (pn_number>22)//�������鳬�磨��22��α������У�
        pn_number=22;
   	  //char temp_correlationresult[80];
	  //sprintf(temp_correlationresult,"��ؽ��%d",correlationresult);
      //MessageBox(NULL,temp_correlationresult,"",MB_OK)

     fprintf(fp_txt,"\n%3d     ",correlationresult);
//	 if (correlationresult>30)
//		 AfxMessageBox("Error!");
	}

    for (i=0;i<66;i++)
	  m_ExtractedOriginalWatermarkInformationSequence[i]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i];
	//���麺����ľ�������
	//���Ծ�һλ��������λ���ʱ������������
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]=0;
	//m_ExtractedHammingCodedOriginalWatermarkInformationSequence[2]=0;

	//���������Ӧ�ľ������
    //����ȡ����Ϣ���о��� 90bits->66bits
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence->m_ExtractedOriginalWatermarkInformationSequence
    //�Ƚ��з��飬��Ϣλ11λ������4λ�ලλ 90λ�ֳ�6��
/*    unsigned char temp_hammingcode[15];
	int s1,s2,s3,s4;
	for (i=0;i<6;i++)
	{
		for (j=0;j<15;j++)
           temp_hammingcode[j]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence[i*15+j];
	    
		//�ල����
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        s1=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[5]+temp_hammingcode[7]
		    +temp_hammingcode[8]+temp_hammingcode[11])%2;//ģ2��
        s2=(temp_hammingcode[1]+temp_hammingcode[2]
			+temp_hammingcode[3]+temp_hammingcode[4]
	        +temp_hammingcode[6]+temp_hammingcode[8]
	        +temp_hammingcode[9]+temp_hammingcode[12])%2;//ģ2��
        s3=(temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[4]+temp_hammingcode[5]
            +temp_hammingcode[7]+temp_hammingcode[9]
		    +temp_hammingcode[10]+temp_hammingcode[13])%2;//ģ2��
        s4=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[4]
            +temp_hammingcode[6]+temp_hammingcode[7]
	        +temp_hammingcode[10]+temp_hammingcode[14])%2;//ģ2��   
	    
		//���11λ��Ϣλ
		//����У���Ӿ��� ����ͼ��
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
    AfxMessageBox("ˮӡ�����̽���!"); 
	char temp_embeddingtime[100];
    sprintf(temp_embeddingtime,"ˮӡ�����̣�%lf��",m_time);
    MessageBox(NULL,temp_embeddingtime,"ˮӡ������!",MB_OK);

    fprintf(fp_txt,"\n\n\n%3d     ",correlationresult_threshold);
	fclose(fp_txt);	
	fclose(fp);	
    GlobalFree(mem_temp_FrameBytes);
	GlobalFree(mem_temp_ConvolutionResult);

	correct_bit_number=0;//��ȷ��λ��
    correct_rate=0.0;    //��ȷ��
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
   short int *temp_ConvolutionResult;//�����͸�ͨ�˲�����������õĽ�������и�
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
	fprintf(fp_txt,"��ͬλ�ö�Ӧ����ؼ����");
   
//   unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
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
   
   //����ˮӡ�������ȡ��ʱ��
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
			 fseek(fp,8L,1);//ÿһ֡�����ݳ��ȣ�8+101376��bytes
             fread(FrameBytes_ForWatermarkExtraction,101376,1,fp);

//    char temp_originalembeddingpoint[100];
//    sprintf(temp_originalembeddingpoint,"��������ֵ%d",FrameBytes_ForWatermarkExtraction[(287-88)*352+272]);
//    MessageBox(NULL,temp_originalembeddingpoint,"ˮӡ��⣨δ������ͨ�˲�ǰ��",MB_OK);//Display the info
	
	//�ȶ�temp_ConvolutionResult���鸳��ֵ 
  //  for (i=0;i<288;i++)
  //	for (j=0;j<352;j++) 
  //         temp_ConvolutionResult[101376-(i+1)*352+j]=FrameBytes_ForWatermarkExtraction[101376-(i+1)*352+j];
	
	//�Ƚ��и�ͨ�˲��Ĵ���
     
    //���ܵ����ر��ֲ���
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
//      Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]=FrameBytes_ForWatermarkExtraction[101024-ty*352+tx];//׼����ʱ��������˲�


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
    //sprintf(temp_pixelvaluerange,"�˲���������60��С��-60���������� %d",pixelvaluesum);
    //MessageBox(NULL,temp_pixelvaluerange,"�˲���Ľ��",MB_OK);//Display the info

     
  
    //�ҵ���ǰ֡Ƕ��ĵ�
    //[l/44*8+m][l%44*8+n]->[1500/44*8+0][1500%44*8+0]->[272][32]
    //Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-272)*352+32];

//    Extracted_WatermarkSequenceAlongTimeAxis[k]=temp_ConvolutionResult[(287-ty)*352+tx];
  
//	  char temp_embeddingpoint[100];
//    sprintf(temp_embeddingpoint,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis[k]);
//    MessageBox(NULL,temp_embeddingpoint,"ˮӡ��⣨������ͨ�˲���",MB_ICONSTOP|MB_OK);//Display the info
  
   }

   //���ǽ���һάԤ���˲�
/* //[-1 -1 -1 6 -1 -1 -1]/6
   //���е����߽��������չ0,1,2,27,28,29
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
   //���е����߽��������չ0,1,2,3,26,27,28,29
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
      sprintf(temp_embeddingpoint_1D,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilter[k]);
      MessageBox(NULL,temp_embeddingpoint_1D,"ˮӡ��⣨δ����һάԤ���˲�ǰ��",MB_ICONSTOP|MB_OK);//Display the info
      sprintf(temp_embeddingpoint_1D,"��������ֵ%d",Extracted_WatermarkSequenceAlongTimeAxis_1DFilterResult[k]);
      MessageBox(NULL,temp_embeddingpoint_1D,"ˮӡ��⣨����һάԤ���˲���",MB_ICONSTOP|MB_OK);//Display the info
  

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
	  if (correlationresult0>0)//˫���Ե�1-�����Ե�0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3]=0;
      if (correlationresult0<0)//˫���Ե�-1-�����Ե�1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3]=1;
      if (correlationresult1>0)//˫���Ե�1-�����Ե�0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+1]=0;
      if (correlationresult1<0)//˫���Ե�-1-�����Ե�1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+1]=1;

      if (correlationresult2>0)//˫���Ե�1-�����Ե�0
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+2]=0;
      if (correlationresult2<0)//˫���Ե�-1-�����Ե�1
         m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[pn_number*3+2]=1;
	}

	  if (correlationresult==30)
	     correlationresult_threshold=correlationresult_threshold+1;
 
   	  //char temp_correlationresult[80];
	  //sprintf(temp_correlationresult,"��ؽ��%d",correlationresult);
      //MessageBox(NULL,temp_correlationresult,"",MB_OK)

      fprintf(fp_txt,"\n%3d     ",correlationresult);
	}
}
}	
    
    for (i=0;i<66;i++)
		m_ExtractedOriginalWatermarkInformationSequence_Attacked[i]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[i];
    //���麺����ľ�������
	//���Ծ�һλ��������λ���ʱ������������
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence[0]=0;
	//m_ExtractedHammingCodedOriginalWatermarkInformationSequence[2]=0;

	//���������Ӧ�ľ������
    //����ȡ����Ϣ���о��� 90bits->66bits
    //m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked->m_ExtractedOriginalWatermarkInformationSequence_Attacked
    //�Ƚ��з��飬��Ϣλ11λ������4λ�ලλ 90λ�ֳ�6��
/*    unsigned char temp_hammingcode[15];
	int s1,s2,s3,s4;
	for (i=0;i<6;i++)
	{
		for (j=0;j<15;j++)
           temp_hammingcode[j]=m_ExtractedHammingCodedOriginalWatermarkInformationSequence_Attacked[i*15+j];
	    
		//�ල����
		// 1 1 1 1 0 1 0 1 1 0 0 : 1 0 0 0
		// 0 1 1 1 1 0 1 0 1 1 0 : 0 1 0 0
		// 0 0 1 1 1 1 0 1 0 1 1 : 0 0 1 0
		// 1 1 1 0 1 0 1 1 0 0 1 : 0 0 0 1
		

        s1=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[5]+temp_hammingcode[7]
		    +temp_hammingcode[8]+temp_hammingcode[11])%2;//ģ2��
        s2=(temp_hammingcode[1]+temp_hammingcode[2]
			+temp_hammingcode[3]+temp_hammingcode[4]
	        +temp_hammingcode[6]+temp_hammingcode[8]
	        +temp_hammingcode[9]+temp_hammingcode[12])%2;//ģ2��
        s3=(temp_hammingcode[2]+temp_hammingcode[3]
			+temp_hammingcode[4]+temp_hammingcode[5]
            +temp_hammingcode[7]+temp_hammingcode[9]
		    +temp_hammingcode[10]+temp_hammingcode[13])%2;//ģ2��
        s4=(temp_hammingcode[0]+temp_hammingcode[1]
			+temp_hammingcode[2]+temp_hammingcode[4]
            +temp_hammingcode[6]+temp_hammingcode[7]
	        +temp_hammingcode[10]+temp_hammingcode[14])%2;//ģ2��   
	    
		//���11λ��Ϣλ
		//����У���Ӿ��� ����ͼ��
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
    AfxMessageBox("ˮӡ�����̽���!"); 
	char temp_detectiontime[200];
    sprintf(temp_detectiontime,"ˮӡ�����̣�%lf��",m_time);
    MessageBox(NULL,temp_detectiontime,"ˮӡ������!",MB_OK);

    fprintf(fp_txt,"\n\n\n%3d     ",correlationresult_threshold);
	fclose(fp_txt);	
	fclose(fp);	
    GlobalFree(mem_temp_FrameBytes);
	GlobalFree(mem_temp_ConvolutionResult); 

	correct_bit_number_attacked=0;//��ȷ��λ��
    correct_rate_attacked=0.0;    //��ȷ��
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
   ofn.lpstrFilter=TEXT("BMP Files (*.bmp)\0*.bmp\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'BMP�ļ� *.bmp'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName_CurrentAttackedFrame; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle_CurrentAttackedFrame;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����BMP�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="BMP";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β�����");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName_CurrentAttackedFrame);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName_CurrentAttackedFrame,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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
		MessageBox(NULL,"��׼[256GrayBmpHeader.bmp]�Ҳ�����򲻿�!","�����ʼ��",MB_ICONSTOP|MB_OK);
		
	}
    //Read file
    if(_read(fh,m_Standard256GrayBmpFileInfo,1078)==-1)
    {
	  _close(fh);   
      MessageBox(NULL,"��ȡ��׼[256GrayBmpHeader.bmp]ʧ��!","�����ʼ��",MB_ICONSTOP|MB_OK);
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
   ofn.lpstrFilter=TEXT("AVI Files (*.avi)\0*.avi\0�����ļ� (*.*)\0*.*\0\0");//The filter 
   ofn.lpstrCustomFilter=NULL;//None user filter 
   ofn.nMaxCustFilter=0; 
   ofn.nFilterIndex=1;//Select the item 'AVI�ļ� *.bok'
   //Initial the full name without path
   //then save the selected filename with full path
   ofn.lpstrFile=FileName; 
   ofn.nMaxFile=_MAX_PATH;    
   ofn.lpstrFileTitle=FileTitle;//The filename without path
   ofn.nMaxFileTitle=_MAX_PATH; 
   ofn.lpstrInitialDir=NULL;//The initial directory is the next book directory of current directory
   ofn.lpstrTitle="����AVI�ļ�";//The title 
   ofn.Flags=OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;//The flags 
   ofn.lpstrDefExt="AVI";//Default extension 
   ofn.lCustData=NULL; 
   ofn.lpfnHook=NULL; 
   ofn.lpTemplateName=NULL;//User defined dialog 
   //Initialize the FileOpenDialog structure ends

   char temp0[100];
   if(GetSaveFileName(&ofn)==0)//If cancle or error,then return
   {
	   AfxMessageBox("��ȡ���˱��β���");
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
		    sprintf(temp0,"�ļ�[%s]��ֻ���ļ�,ȷʵҪ������",FileName);	  
		    if(MessageBox(NULL,temp0,"",MB_YESNO|MB_ICONQUESTION)==IDYES)//Confirm to overwrite
			   SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);  
		   
		    else //else return
			{
			   AfxMessageBox("�洢ʧ��!");
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

	double DifferenceSum;//���ز�
	int OriginalPixelValueSum;//ԭʼ���صĺͣ���������SNR
	unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
	double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *BlockDCT_Coefficient_Saved[1584][8];//ǰһ֡�ķֿ�DCTϵ��
	double *temp_DC_Coefficient_Difference;//ǰ��֡��Ӧ��DCTֱ��ϵ��֮��
    double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double *temp_DCT_Edge_Detection;//��DCT���ж��Ƿ��б�Ե����
	double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ
	int flag_DC_Coefficient,flag_AC_Coefficient;	 


	//�����ڴ�
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
	
    //��64λ��ˮӡ�������ף���һ������Gold����
	//ÿһ��block�з�1λ
    //����α�������
	//����Ƕ��
    
	int seed;
    seed=12;

    for (i=0;i<64;i++)
	{
	    seed=seed+1;
        PseudoRandomSequence_Generator(PseudoRandomSequence,512,seed);//64*2
	    for (j=0;j<64;j++)
		{
			if (PseudoRandomSequence[j]==0)//������0 -> ˫����1
		        m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j]=1;//��������
            else//������1 -> ˫����-1
				m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j]=-1;//��������
		}
	
        //����α������ж�ˮӡ���е���  
        for (j=0;j<1;j++)
		{ 
	      if (m_OriginalWatermarkBytes[i+j]==0)//0 -> 1(˫����)
		  {
			  for (k=0;k<64;k++)//����ˮӡ��Ϣ���Ƶ�����
			      m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j*64+k]=m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j*64+k];//�������б��ֲ���
		  }
          else//1 -> -1(˫����)
          {
			  for (k=0;k<64;k++)//����ˮӡ��Ϣ���Ƶ�����
			      m_TimeEmbeddingModulatedSpreadSpectrumWatermarkSequence[i][j*64+k]=-m_TimeEmbeddingSpreadSpectrumWatermarkSequence[i][j*64+k];//�������з���		  
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

	//����ˮӡǶ���ʱ��
    time_t ltime1,ltime2;
    time(&ltime1);

	if (FrameColorInfo[0]==8)//�Ҷ�
	   {
		  OriginalFile.Seek(4096L,CFile::begin);
		  
		  for (k=0;k<TotalFrameNumber;k++)
//		  for (k=0;k<10;k++)
		  {
             temp_DCT_Coefficient_Mean=0.0;
			 flag_DC_Coefficient=0;//��ʾDCϵ������Ϣ
			 flag_AC_Coefficient=0;//��ʾACϵ������Ϣ
			 DifferenceSum=0.0;//���ÿһ֡������
			 OriginalPixelValueSum=0;//���ÿһ֡������
	         OriginalFile.Seek(8L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��101376��bytes
             
             OriginalFile.Read(FrameBytes_Embedding,101376);

			 for (i=0;i<288;i++)//��ͼ�����ݵ�����
			 {
				 OriginalFile.Seek(-352L,CFile::current);
				 OriginalFile.Read(FrameBytes_Inserting[i],352);	 
				 OriginalFile.Seek(-352L,CFile::current);
			 }
			   
			 OriginalFile.Seek(101376L,CFile::current);
			 	 
			 for (l=0;l<1584;l++)
			 {	
				//�Ե�ǰ֡����8*8�ֿ�
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					    Frame_Block[l][m][n]=FrameBytes_Inserting[l/44*8+m][l%44*8+n];
					}

				temp_DCT_Coefficient_Variance[l]=0.0; 
				temp_AC_Coefficient_Energy[l]=0.0;
                temp_DCT_Edge_Detection[l]=0.0;
			
                DCT_Program(Frame_Block[l],BlockDCT_Coefficient[l],8,8);
//		        IDCT(BlockDCT_Coefficient[l],Frame_Block[l]);//ԭͼ�������DCT�任

                if (k>=1)//�Ƚ�ǰ����֡�����ﲻ���������ۼƵ��������Ϊǰһ֡����Ϣ�洢��DCTϵ��������
				{
				   temp_DC_Coefficient_Difference[l]=fabs(BlockDCT_Coefficient[l][0][0]-BlockDCT_Coefficient_Saved[l][0][0]);//ǰ��֡��ӦDCTϵ���Ĳ�ֵ
				   if (temp_DC_Coefficient_Difference[l]>=80)
                     flag_DC_Coefficient=flag_DC_Coefficient+1;
				}
				
                for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					 {
						BlockDCT_Coefficient_Saved[l][m][n]=BlockDCT_Coefficient[l][m][n];
	                    temp_DCT_Coefficient_Mean=BlockDCT_Coefficient[l][m][n]+temp_DCT_Coefficient_Mean;				    
					 }
            
			    temp_DCT_Coefficient_Mean=(temp_DCT_Coefficient_Mean-BlockDCT_Coefficient[l][0][0])/63.0;//��Ӧ�齻��ϵ���ľ�ֵ

				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{
                        temp_DCT_Coefficient_Variance[l]=temp_DCT_Coefficient_Variance[l]+(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][m][n]-temp_DCT_Coefficient_Mean);
					 
						temp_AC_Coefficient_Energy[l]=temp_AC_Coefficient_Energy[l]+(BlockDCT_Coefficient[l][m][n]*BlockDCT_Coefficient[l][m][n]);
					}

			    temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
                 
                temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));

                if ((fabs(BlockDCT_Coefficient[l][0][1])>40)||(fabs(BlockDCT_Coefficient[l][1][0])>40)||(fabs(BlockDCT_Coefficient[l][1][1])>40))
                   temp_DCT_Edge_Detection[l]=1.0; 

				if (temp_DCT_Coefficient_Variance[l]>=1200)
                   flag_AC_Coefficient=flag_AC_Coefficient+1;
			 }

//*****************************************************************

			  int r=0; 
		      int temp_result;
			  //�����жϺ�����Ӧ�Ŀ���Ƕˮӡ
			
			  if (k==0)//��һ֡��������,���ο��ڽ���֡
			  { 
				r=0;  
                for (l=0;l<1584;l++)
				//for (l=300;l<1584;l++)
				{ 
				   if (temp_AC_Coefficient_Energy[l]>=12000)//&&(fabs(BlockDCT_Coefficient[l][0][0])>880))//�����ӿ� 
				   {
					//��¼��Ƕ��ˮӡ��λ��
				    EmbeddingRegionSequence[k][r]=l;
					if (r<128)
					{
					 for (m=0;m<8;m++)
				      for (n=0;n<8;n++)
					  { 		
						//��ǳ�8*8����ı߽�
						//if (m==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==7)
					    //    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�

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
					AfxMessageBox("��ѡ��Ŀ���������");
			  }
              else
			  {
                r=0;  
                for (l=0;l<1584;l++)
				{
	//			if ((temp_DC_Coefficient_Difference[l]>=80)&&(temp_DCT_Coefficient_Variance[l]>=1000))//�����˶���Ҳ�������ӿ� 
	//			if (temp_DC_Coefficient_Difference[l]>=80)//���˶��� 
	//          if (temp_DCT_Coefficient_Variance[l]>=400)//�������ӿ�  
	//          if (temp_AC_Coefficient_Energy[l]>=28000)//���������������ܺ�
               
		        if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>680))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�  for table tennis 2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=96)&&(temp_AC_Coefficient_Energy[l]>=12800)&&(fabs(BlockDCT_Coefficient[l][0][0])>900))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�  for table tennis 2.avi 
    //          if ((temp_DC_Coefficient_Difference[l]>=10)&&(temp_AC_Coefficient_Energy[l]>=40000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   for mobile
    //          if ((temp_DC_Coefficient_Difference[l]>=60)&&(temp_AC_Coefficient_Energy[l]>=80000))//&&(fabs(BlockDCT_Coefficient[l][0][0])>980))//�����˶���Ҳ�������ӿ�   	
	//          if (temp_DCT_Edge_Detection[l]==1.0) 
    //			if ((temp_AC_Coefficient_Energy[l]>=28000)&&(temp_DCT_Edge_Detection[l]==1.0))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1000))
	//			if ((temp_AC_Coefficient_Energy[l]>=40000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1160)) 
				{  
					//��¼��Ƕ��ˮӡ��λ��
				    EmbeddingRegionSequence[k][r]=l;
			        
       
					if (r<128)
					{
					 for (m=0;m<8;m++)
				     for (n=0;n<8;n++)
					 { 
                        //temp=Frame_Block[l][m][n];
						
						//��ǳ�8*8����ı߽�
					    //if (m==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (m==7)
						//    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        //if (n==0)
						//	Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
                        // if (n==7)
					    //    Frame_Block[l][m][n]=255;//��ǳ������Ŀ�
               		 
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
			   AfxMessageBox("��ѡ��Ŀ���������");			  
		}		
		
          //Frame_Block[1500][0][0]=255;   //��ʱ����Ƕ��ˮӡ

            EmbeddingRegionSumSequence[k]=r; 
                 
			//���ֿ������ݻ�ԭΪ��������
			for (l=0;l<1584;l++)
				for (m=0;m<8;m++)
					for (n=0;n<8;n++)
					{ 
					  FrameBytes_Inserting[l/44*8+m][l%44*8+n]=Frame_Block[l][m][n];
					}

            //������Ӧ��PSNR(��Լ��ʱ7����)
            for (i=0;i<288;i++)
			   for (j=0;j<352;j++)
			   {
				  DifferenceSum=DifferenceSum+(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j])*(FrameBytes_Inserting[287-i][j]-FrameBytes_Embedding[i*352+j]);
				  OriginalPixelValueSum=OriginalPixelValueSum+(FrameBytes_Embedding[i*352+j]*FrameBytes_Embedding[i*352+j]);//��������PSNR
			   }
   
			fwrite(FrameHeader,8,1,fp);
			for (i=0;i<288;i++)
			{
				fwrite(FrameBytes_Inserting[287-i],352,1,fp);//�����ݰ�����洢
			}
            
			PSNR[k]=10*log10(255.0*255.0/DifferenceSum*352.0*288.0);//
            SNR[k]=10*log10(OriginalPixelValueSum/DifferenceSum);//
		  }
          
          time(&ltime2);
	      double m_time;
	      m_time=ltime2-ltime1;
          AfxMessageBox("ˮӡǶ����̽���!"); 
	      char temp_embeddingtime[100];
          sprintf(temp_embeddingtime,"ˮӡǶ����̣�%lf��",m_time);
          MessageBox(NULL,temp_embeddingtime,"ˮӡǶ�����!",MB_OK);

		  //AVI�ļ������INDEX����
		  if (TotalFrameNumber==30)
          {
			OriginalFile.Read(FrameBytes_Embedding,784);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,784,1,fp);
		  }
		  
		  if (TotalFrameNumber==150)
          {
			OriginalFile.Read(FrameBytes_Embedding,2896);//����FrameBytes_Embedding����
			fwrite(FrameBytes_Embedding,2896,1,fp);
		  }
		  if (TotalFrameNumber==300)
		  {
            OriginalFile.Read(FrameBytes_Embedding,5280);//����FrameBytes_Embedding����
          	fwrite(FrameBytes_Embedding,5280,1,fp);
		  }

		  fclose(fp);
 
		  char temp0[100];
	      sprintf(temp0,"AVI�ļ��ѳɹ�����");
	      MessageBox(NULL,temp0,"",MB_OK);

/////////////////Save the avi file ends//////////

	   }
/*	   else //��ɫ
	   {   
		   OriginalFile.Seek(2056L,CFile::begin);
		   for (i=0;i<TotalFrameNumber;i++)
		   {
	         OriginalFile.Seek(304136L,CFile::current);//ÿһ֡�����ݳ��ȣ�8��304128��bytes
		   }
           OriginalFile.Read(FrameBytes,304128);
	   }*/

       OriginalFile.Close();
    
	   fp=fopen(FileName,"rb");
	   fseek(fp,4104L,0);
	   fread(FrameBytes_Embedding,101376,1,fp);

	   PositionFlag_Watermarked=1;//Ƕ��ˮӡ�����ж�Ӧ��λ��
       FrameNumber_Watermarked=0;//���Ƚ��ļ���λ�ó�ʼ��

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
   
   int pixel_value;//ע������������ 

   int i,j;
   if (PositionFlag==1)//ԭʼ��Ƶ����
   {
	   //���ܵ����ر��ֲ���
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ

    //�����ڴ�
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
	fprintf(fp_txt,"��ͬλ�ö�Ӧ����ؼ����");

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Embedding�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Embedding[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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

	  temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
      temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));
	 }
    
	 for (m=0;m<64;m++)
	    Watermark_Extracted[m]=0;

     for (pn_number=0;pn_number<64;pn_number++) 
	 {
	  //�����жϣ�ȷ��ˮӡ�Ƿ����
	  for (l=0;l<1584;l++)
      //for (l=300;l<1584;l++)
	 {
		//	if (temp_DCT_Coefficient_Variance[l]>=1000)//�����ж��Ƿ�������� 
		//  if ((temp_AC_Coefficient_Energy[l]>=28000)&&(fabs(BlockDCT_Coefficient[l][0][0])>1080))
		//  if (temp_AC_Coefficient_Energy[l]>=35000)// for mobile & calendar
		if (temp_AC_Coefficient_Energy[l]>=10000)// for table tennis 2.avi
		{  
		    for (m=0;m<8;m++)
				for (n=0;n<8;n++)
				{
             		//Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
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
			   if (correlation_result0==64)//˫���Ե�1
                  Watermark_Extracted[pn_number]=0;
               if (correlation_result0==-64)//˫���Ե�-1
                  Watermark_Extracted[pn_number]=1;

			   //if (correlation_result1==32)//˫���Ե�1
               //   Watermark_Extracted[pn_number*2+1]=0;
               //if (correlation_result1==-32)//˫���Ե�-1
               //   Watermark_Extracted[pn_number*2+1]=1;

			   //if (correlation_result2==16)//˫���Ե�1
               //   Watermark_Extracted[pn_number*4+2]=0;
               //if (correlation_result2==-16)//˫���Ե�-1
               //   Watermark_Extracted[pn_number*4+2]=1;

			   //if (correlation_result3==16)//˫���Ե�1
               //   Watermark_Extracted[pn_number*4+3]=0;
               //if (correlation_result3==-16)//˫���Ե�-1
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
    unsigned char Frame_Block[1584][8][8];//�ֿ���ͼ������
    double BlockDCT_Coefficient[1584][8][8];//�ֿ�DCTϵ��
	double *temp_DCT_Coefficient_Variance;//�ֿ�DCT����ϵ���ķ���
    double *temp_AC_Coefficient_Energy;//�ֿ�DCT����ϵ��������
    double temp_DCT_Coefficient_Mean;//�ݴ�ֿ�DCT����ϵ���ľ�ֵ

    //�����ڴ�
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
	fprintf(fp_txt,"��ͬλ�ö�Ӧ����ؼ����");

    //�Ե�ǰ֡���ݽ��д���
	//��FrameBytes_Attacked�е����ݰ�����˳�������
	for (i=0;i<288;i++)
		for (j=0;j<352;j++)
            FrameBytes_Inserting[287-i][j]=FrameBytes_Attacked[i*352+j];

    //��ͼ�����ݷֿ�
    //�Ե�ǰ֡����8*8�ֿ�
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

	//����ˮӡ�������ȡ��ʱ��
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

	  temp_DCT_Coefficient_Variance[l]=(temp_DCT_Coefficient_Variance[l]-(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean)*(BlockDCT_Coefficient[l][0][0]-temp_DCT_Coefficient_Mean))/63.0;//DCT�齻��ϵ���ķ���
      temp_AC_Coefficient_Energy[l]=(temp_AC_Coefficient_Energy[l]-(BlockDCT_Coefficient[l][0][0]*BlockDCT_Coefficient[l][0][0]));
	 }

     for (pn_number=0;pn_number<64;pn_number++) 
	 {
       correlation_result_temp=0; 
	   //�����жϣ�ȷ��ˮӡ�Ƿ����
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
             		//Frame_Block[l][m][n]=255;//��ǳ������Ŀ� 
					
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
			   if (correlation_result0>=0)//˫���Ե�1
                  Watermark_Attacked[pn_number]=0;
               if (correlation_result0<0)//˫���Ե�-1
                  Watermark_Attacked[pn_number]=1;

			   //if (correlation_result1>=0)//˫���Ե�1
               //   Watermark_Attacked[pn_number*2+1]=0;
               //if (correlation_result1<0)//˫���Ե�-1
               //   Watermark_Attacked[pn_number*2+1]=1;

			 //if (correlation_result2>=0)//˫���Ե�1
             //  Watermark_Attacked[pn_number*4+2]=0;
             //if (correlation_result2<0)//˫���Ե�-1
             //  Watermark_Attacked[pn_number*4+2]=1;

			 //if (correlation_result3>=0)//˫���Ե�1
             //  Watermark_Attacked[pn_number*4+3]=0;
             //if (correlation_result3<0)//˫���Ե�-1
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
        AfxMessageBox("ˮӡ�������ȡ���̽���!"); 
	    char temp_detectiontime[150];
        sprintf(temp_detectiontime,"ˮӡ�������ȡ���̣�%lf��",m_time);
        MessageBox(NULL,temp_detectiontime,"ˮӡ��ȡ�������!",MB_OK);

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
