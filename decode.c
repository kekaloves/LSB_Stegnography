#include "decode.h"//user define header file
#include <stdio.h>
Status decode_magic_string(EncodeInfo *decInfo,char *key)//for decoded magic string
{
	fseek(decInfo->fptr_stego_image,54,0);//set stego image pointer offset to 54bytes
	char ch,size_char,magic1=0;//define character variables
	int i;
	while(*key)//to enter magic key
	{
		for(i=0;i<8;i++)//start for loop to enter character 8bit
		{
			fread(&ch,sizeof(char),1,decInfo->fptr_stego_image);//read one byte

			magic1=magic1 | ((ch&1)<<(7-i));//decoded magic key lsb to msb format
		}
//		printf("%c",*key);
		if(magic1 != *key)//if condition to check enter key is not magic
		{
			fprintf(stdout,"Please enter the correct magic string yaar ☜ :\n");
			return e_failure;
		}
		magic1=0;//reset magic1 to 0
		key++;//magic key increment
	}

	return e_success;

}
Status decode_file_size(EncodeInfo *decInfo)//for decoded file size
{
	char ch;
	int i=31,size=0;
	while(fread(&ch, sizeof(ch),1, decInfo->fptr_stego_image))//read each byte of stego image file
	{
		size=size | ((ch&1)<<i);//decoded file size
		if(ftell(decInfo->fptr_stego_image)==102)//ckeck if decode is complete
		{
			break;
		}
		i--;//decrement i
	}
	decInfo->size_secret_file=size;//assign file size to structure variable
	printf("%d",size);//print size
	return e_success;
}
Status decode_secret_file(EncodeInfo *decInfo)//for decode secret file
{
	long int size=decInfo->size_secret_file;

	char ch;
	char str=0;
	int i,j;
		for(i=0;i<size;i++)//start for loop for total no of character
		{
		str=0;
			for(j=0;j<8;j++)//start for loop for each character
			{
				fread(&ch,sizeof(char),1,decInfo->fptr_stego_image);//read each byte of stego image file
				str=str | ((ch&1)<<(7-j));//decode secret file
			}
	//		printf("%c",str);
			fwrite(&str,sizeof(char),1,decInfo->fptr_secret);//write each byte 
		}	
	return e_success;
}
Status do_decoding(char *argv[],EncodeInfo *decInfo,char *key)//perform decoding
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");//open stego image file in read mode
	if(decInfo->fptr_stego_image == NULL)
	{
		fprintf(stdout,"error,no such file %s:\n",decInfo->stego_image_fname);
		return e_failure;
	}
	decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");//open in write mode
	if(decInfo->fptr_stego_image == NULL)
	{
		fprintf(stdout,"error,file  is not opening%s:\n",decInfo->secret_fname);
		return e_failure;
	}

	if( decode_magic_string(decInfo,key) == e_success)//if condition to check e_success
	{
		if( decode_file_size(decInfo) == e_success)//if condition to check e_success
		{
			if( decode_secret_file(decInfo) == e_success)//if condition to check e_success
			{
				return e_success;
			}
		}
	}
	return e_failure;
}

