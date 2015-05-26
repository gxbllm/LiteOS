#include "vectorflash.h"
#include "io_eeprom.h"
#include "fs_config.h"


static char vectorflash[32];


static int getbit(char *set, int number)
{
        set += number / 8;
        return (*set & (1 << (number % 8))) != 0;       /* 0 or 1       */
}


static void setbit(char *set, int number, int value)
{
        set += number / 8;
        if (value)
                *set |= 1 << (number % 8);              /* set bit      */
        else    *set &= ~(1 << (number % 8));           /* clear bit    */
}


void initVectorFlash()
{
  int i;
  for (i=0;i<32;i++)
   vectorflash[i] = 0;	
}



int  getFlashPage()
{
  int num;
  for (num=0;num<256;num++)
    {
      if (getbit(vectorflash, num)==0)
      	 {
      	   setbit(vectorflash, num, 1);
      	   return num+1;      	 	
      	 }	
    }	
	return -1;
}



void scanVectorFlash()
{
	 int num, addr;
	 uint8_t valid;
	 uint8_t type;
	 uint8_t readpage;
	 
   for (num=0;num<96;num++)
    { 
    	addr = num + 1;
    	valid = read8uint(addr, VALIDOFFSET);
    	type = read8uint(addr, TYPEOFFSET);
    	if ((valid == 1)&&(type == FILENODE))
    		{
    		  	char i;
    		  	for (i=0;i<8;i++)
    		  	 {
    		  	   readpage = read8uint(addr, FILE_ADDRPAGEOFFSET+i);
    		  	   if (readpage > 0)
    		  	   	 setbit(vectorflash, readpage-1, 1);	
    		  	 }
    			   			
    		}
   }
}



void releaseFlashPage( int num )
{
   setbit(vectorflash, num-1, 0);	
	
}

int countVectorFlash()
{
  int count, num;
  count = 0; 
  
	for (num=0;num<256;num++)
    {
      if (getbit(vectorflash, num)==1)
      	count++;
    }
  return 256-count;           	
	
}




void printVectorFlash()
{
  int data[256];
  int i;
  for (i=0;i<256;i++)
  	{
  	     if (getbit(vectorflash, i)==0)
      	 {
      	   data[i]=0;
      	 }
      	 else
      	 	 data[i] = 1;		
  		
  	}
}
