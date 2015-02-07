
int * subm(int p, int p1)
{   
	unsigned int t=01;
	int i,j;
	unsigned int f1,f2;
    unsigned int u=01;
	int matrix[100][127];
    int *c1,*c2, *c3;
    c1=(int *)malloc(sizeof(int)*p1);
	c2=(int *)malloc(sizeof(int)*p1);
	c3=(int *)malloc(sizeof(int)*p*p1);
	switch(p1)
	{
    case 31:
		f1=051;
		f2=075;
		break;
	case 63:
		f1=0103;
		f2=0147;
		break;
	case 127:
	    f1=0211;
		f2=0277;
        break;
	case 511:
        f1=01021;
		f2=01333;
        break;
	case 1023:
		f1=02011;
		f2=02415;
        break;
	default:
		printf("can't generate gold sequenses.");
	}
     for(i=0;i<p1;i++)
	 {
		if(t&f1&u==u)
		{
			*(c1+i)=1;
			t=t^f1;
			t=t>>1;
		}
		else
		{
			*(c1+i)=0;
			t=t>>1;
		}
     }
  unsigned int m=01;
  for(i=0;i<p1;i++)
	{
		if(m&f2&u==u)
		{
			*(c2+i)=1;
			m=m^f2;
			m=m>>1;
		}
		else
		{
			*(c2+i)=0;
		     m=m>>1;
		}
     }
    for(i=0;i<p;i++)
	{
    	for(j=0;j<p1;j++)
		{
		   matrix[i][j]=(*(c1+j))^(*(c2+j));
		}
       int k, temp;
	   temp=*(c1+p1-1);
	   for(k=p1-1;k>0;k--)
	   {
		   *(c1+k)=*(c1+k-1);
	   }
	   *c1=temp;
	} 
    for(i=0;i<p;i++)
	{
	    for(j=0;j<p1;j++)
		{
	    	*(c3+i*p1+j)=1-2*matrix[i][j];
		}
	}
return c3;
}
int * cdmawater(int *c3, int p1, int p,int q, int p2, int *watermessage)
{
	int *c4;
    int i,j,t;
	c4=NULL;
   c4=(int *)malloc(sizeof(int)*p1*p);
	switch(q)
	{
	   case 1:
	        for(i=0;i<p2;i++)
			{
		     	for(j=0;j<p1;j++)
				{
				    if(*(watermessage+i)==0)
				    	*(c4+p1*i+j)=*(c3+p1*i+j);
			         if(*(watermessage+i)==1)
				    	*(c4+p1*i+j)=(*(c3+p1*i+j))*(-1);
				}
			}
	    	break;
	   case 2:	
		         i=0;
	            while(i<p2)
				{
			         for(j=0;j<(p1/2);j++)
					 {
				         if(*(watermessage+i)==0)
			                *(c4+(i/2)*p1+j)=*(c3+(i/2)*p1+j);
				         if(*(watermessage+i)==1)
				            *(c4+(i/2)*p1+j)=(-1)*(*(c3+(i/2)*p1+j));
					 }
			         for(j=(p1/2);j<p1;j++)
					 {
				        if(*(watermessage+i+1)==0)
				            *(c4+(i/2)*p1+j)=*(c3+(i/2)*p1+j);
				         if(*(watermessage+i+1)==1) 
				        	*(c4+(i/2)*p1+j)=(*(c3+(i/2)*p1+j))*(-1);
					 }
			      i=i+2;
				}
	       break;
	   case 3:
		         i=0;
         	 while(i<p2)
			 {
	             for(t=0;t<2;t++)
				 {
			    	for(j=0;j<(p1/3);j++)
					{
				     	if(*(watermessage+i+t)==0)
					     	*(c4+(i/3)*p1+(p1/3)*t+j)=*(c3+(i/3)*p1+(p1/3)*t+j);
				         if(*(watermessage+i+t)==1)
						    *(c4+(i/3)*p1+(p1/3)*t+j)=(*(c3+(i/3)*p1+(p1/3)*t+j))*(-1);
					}
				 }
			     for(j=0;j<(p1-(p1/3)*2);j++)
				 {
				    if(*(watermessage+i+2)==0)
				        *(c4+(i/3)*p1+2*(p1/3)+j)=*(c3+(i/3)*p1+(p1/3)*2+j);
                    if(*(watermessage+i+2)==1)
					    *(c4+(i/3)*p1+(p1/3)*2+j)=(*(c3+(i/3)*p1+(p1/3)*2+j))*(-1);
				 }
	               i=i+3;
			 }
          break;
	   case 4:
		   i=0;
	          while(i<p2)
			  {
	              for(t=0;t<4;t++)
				  {
				      for(j=0;j<(p1/4);j++)
					  {
				     	 if(*(watermessage+i+t)==0)
					     	*(c4+(i/4)*p1+(p1/4)*t+j)=*(c3+(i/4)*p1+(p1/4)*t+j);
				         if(*(watermessage+i+t)==1)
					     	*(c4+(i/4)*p1+(p1/4)*t+j)=(*(c3+(i/4)*p1+(p1/4)*t+j))*(-1);
					  }
				  }
		       	 for(j=0;j<(p1-(p1/4)*3);j++)
				 {
				    if(*(watermessage+i+3)==0)
				       *(c4+(i/4)*p1+(p1/4)*3+j)=*(c3+(i/4)*p1+(p1/4)*3+j);
                    if(*(watermessage+i+3)==1)
					   *(c4+(i/4)*p1+(p1/4)*3+j)=(*(c3+(i/4)*p1+(p1/4)*3+j))*(-1);
				 }
                   i=i+4;
			  }
	              break;
	   case  5:
			i=0;
	      while(i<p2)
		  {
	          for(t=0;t<5;t++)
			  {
				 for(j=0;j<(p1/5);j++)
				 {
					if(*(watermessage+i+t)==0)
						*(c4+(i/5)*p1+(p1/5)*t+j)=*(c3+(i/5)*p1+(p1/5)*t+j);
				    if(*(watermessage+i+t)==1)
						*(c4+(i/5)*p1+(p1/5)*t+j)=(*(c3+(i/5)*p1+(p1/5)*t+j))*(-1);
				 }
			  }
			 for(j=0;j<(p1-(p1/5)*4);j++)
			 {
				if(*(watermessage+i+4)==0)
				   *(c4+(i/5)*p1+(p1/5)*4+j)=*(c3+(i/5)*p1+(p1/5)*4+j);
                if(*(watermessage+i+4)==1)
					*(c4+(i/5)*p1+(p1/5)*4+j)=(*(c3+(i/5)*p1+(p1/5)*4+j))*(-1);
			 }
	          i=i+5;
		  }
	   break;
	   }
	return c4;
}
 unsigned int main(unsigned int p1,int q,int p2)
{

 unsigned int counter=0;
   switch(q)
   {
   case 1:
	   counter=p1*p2;
	   break;
   case 2:
	   counter=(p2/2)*p1+(p2%2)*(p1/2);
	   break;
   case 3:
	   counter=(p2/3)*p1+(p2%3)*(p1/3);
	   break;
   case 4:
	   counter=(p2/4)*p1+(p2%4)*(p1/4);
	   break;
   case 5:
	   counter=(p2/5)*p1+(p2%5)*(p1/5);
	   break;
   }
	   return counter;
 }

int *getmark(int p1,int q,int p2,int *C4, int *C3,int sum[])
{
	int m[10];
	int i,j,t;


	
       switch(q)
		{   
	      case 1: 
		   for(i=0;i<p2;i++)
		   {
	  		 sum[i]=0;
	         for(j=0;j<p1;j++)
			 {
		         sum[i]=sum[i]+((*(C4+i*p1+j))*(*(C3+i*p1+j)));
			 } 
			 if(sum[i]>0)
			   m[i]=0;
	     	 if(sum[i]<0)
		      m[i]=1;
		   }
		  break;
		  case 2:
             i=0;
	        while(i<p2)
			{   
			     sum=0;
			    for(j=0;j<(p1/2);j++)
				{
			    	sum=sum+((*(C4+(i/2)*p1+j))*(*(C3+(i/2)*p1+j)));
				}
			    if(sum>0)
				    m[i]=0;
		     	if(sum<0)
			     	m[i]=1;
			    sum=0;
			   for(j=(p1/2);j<p1;j++)
			   {
				   sum=sum+((*(C4+(i/2)*p1+j))*(*(C3+(i/2)*p1+j)));
			   }
               if(sum>0)
			    	m[i+1]=0;
			   if(sum<0)
				    m[i+1]=1;
			    i=i+2;
			}
	         break;
		  case 3:
               i=0;
         	 while(i<p2)
			 {
	             for(t=0;t<2;t++)
				 {    
					 sum=0;
			    	for(j=0;j<(p1/3);j++)
					{
				     	sum=sum+((*(C4+(i/3)*p1+t*(p1/3)+j))*(*(C3+(i/3)*p1+t*(p1/3)+j)));
					}
					if(sum>0)
						m[i+t]=0;
					if(sum<0)
						m[i+t]=1;

				 }
				    sum=0;
                    for(j=0;j<(p1-(p1/3)*2);j++)
					{
				       sum=sum+((*(C4+(i/3)*p1+2*(p1/3)+j))*(*(C3+(i/3)*p1+2*(p1/3)+j)));
					}
			            if(sum>0)
						m[i+2]=0;
				    	if(sum<0)
						m[i+2]=1;
	                i=i+3;
			 }
             break;
          case 4:
                  i=0;
	          while(i<p2)
			  {
	              for(t=0;t<3;t++)
				  {   
					  sum=0;
				      for(j=0;j<(p1/4);j++)
					  {
	                     sum=sum+((*(C4+(i/4)*p1+t*(p1/4)+j))*(*(C3+(i/4)*p1+t*(p1/4)+j)));
					  }
				    if(sum>0)
						m[i+t]=0;
					if(sum<0)
						m[i+t]=1;
				  }
					sum=0;
		        	 for(j=0;j<(p1-(p1/4)*3);j++)
					 {
		                 sum=sum+((*(C4+(i/4)*p1+3*(p1/4)+j))*(*(C3+(i/4)*p1+3*(p1/4)+j)));
					 }
			            if(sum>0)
						m[i+3]=0;
				    	if(sum<0)
						m[i+3]=1;
	                i=i+4;
			  }
	              break;
		  case 5:
			  i=0;
	          while(i<p2)
			  {
	              for(t=0;t<4;t++)
				  {    sum=0;
				      for(j=0;j<(p1/5);j++)
					  {
	                     sum=sum+((*(C4+(i/5)*p1+t*(p1/5)+j))*(*(C3+(i/5)*p1+t*(p1/5)+j)));
					  }
				    if(sum>0)
						m[i+t]=0;
					if(sum<0)
						m[i+t]=1;
				  }
					sum=0;
		        	 for(j=0;j<(p1-(p1/5)*4);j++)
					 {
		                 sum=sum+((*(C4+(i/5)*p1+4*(p1/5)+j))*(*(C3+(i/5)*p1+4*(p1/5)+j)));
					 }
			            if(sum>0)
						m[i+4]=0;
				    	if(sum<0)
						m[i+4]=1;
	                i=i+5;
			  }
	              break;
		}

		return m;
}
		
			  
					  

void getmark1(int *c3,int *c4,int sum[10][10])
{	

	
    int i,j,t;

	
for(i=0;i<10;i++)
{
	for(j=0;j<10;j++)
	{  
		sum[i][j]=0;
		for(t=0;t<31;t++)
		{
			sum[i][j]=sum[i][j]+((*(c3+31*i+t))*(*(c4+31*j+t)));
		}
	}
}
}
