#include "encode.h"
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)//for copy_bmp_header
{
	char ch1;//define character variables
	rewind(fptr_src_image);	//moved the source file position indicator to the begning of the file
	printf("file offset is  --> %ld\n\n", ftell(fptr_dest_image));//print destnation image file pointer current position
	printf("file offset is  --> %ld\n\n", ftell(fptr_src_image));//print source image file pointer current position
	while(fread(&ch1, sizeof(ch1),1, fptr_src_image))//fread used to read each bytes of source image file
	{
		fwrite(&ch1, sizeof(ch1),1, fptr_dest_image);//fwrite used to write each byte of dest image
		if(ftell(fptr_dest_image)==54)//1to 54 bytes directly copy from source image to dest image
		{
			break;//after 54 bytes while loop is break
		}
	}
	return e_success;
}
Status encode_secret_file_size(unsigned int file_size, EncodeInfo *encInfo)//for encoded secret file size
{

	unsigned int hw=file_size;//define unsigned integer to take file size
	printf("%d",file_size);//print file size
	int i;
	char ch;//define character and integer variables
	for(i=31;i>=0;i--)//start for loop for 32 bits
	{

		ch=(char)(hw>>i&1);//extract each bit of file size
		fwrite(&ch,sizeof(char),1,encInfo->fptr_stego_image);//fwrite use to write each character by msb to lsb format
	}
	return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)//for encoded magic string
{
	unsigned char ch,ch1;
	while( *magic_string)//enter magic string
	{
		ch1=*magic_string;//get magic string value

		int i;
		for(i=7;i>=0;i--)//start for loop to enter each bit of character
		{
//			fread(&ch1, sizeof(char), 1,encInfo->fptr_src_image);
			ch=(char)(ch1>>i&1);//encoded each bit of magic string
			fwrite(&ch,sizeof(char),1,encInfo->fptr_stego_image);//write each bit in one byte format
		}
		magic_string++;//increment magic string pointer
	}


	return e_success;

}
Status encode_secret_file(EncodeInfo *encInfo)//for encoded secret file
{
	unsigned char ch,ch1;
	unsigned int size1= encInfo->size_secret_file;//define size to get secrete file size


	rewind(encInfo->fptr_secret);//moved the file position indicator to the begning of the file
	while(fread(&ch1, sizeof(char), 1, encInfo->fptr_secret))//used to read each byte character
	{
		int i;
		for(i=7;i>=0;i--)//start for loop for 8bit
		{
			ch=(char)(ch1>>i&1);//encoded each bit of secret key
			fwrite(&ch,sizeof(char),1,encInfo->fptr_stego_image);//write each bit in one byte format
		}
		if(size1<1)//if condition to check size1 is less than 1
			break;
		size1--;//decrement size1
	}
	return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)//for copy remaining bit
{
	unsigned char ch;
	
	fseek(fptr_src, ftell(fptr_dest), SEEK_SET);//used to  set fptr_src pointer pointing to 198 bytes 
	printf("file offset of is at --> %ld END\n\n", ftell(fptr_dest));
	int i;
	for(i=ftell(fptr_src);!feof(fptr_src);i++)//start for loop to enter each byte 198 to last
	{
		fread(&ch,sizeof(char),1,fptr_src);
		fwrite(&ch,sizeof(char),1,fptr_dest);
	}
	return e_success;
}
Status do_encoding(EncodeInfo *encInfo)//perform the encoding
{
	if ( open_files(encInfo) == e_success )//check if condition
	{
		if( copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success )//check if condition
		{
			if( encode_magic_string(MAGIC_STRING,encInfo) == e_success)//check if condition
			{
				if( encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success )//check if condition
				{
					if( encode_secret_file(encInfo) == e_success )//check if condition
					{
						if( copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success )//check if condition 
						{
							fcloseall();//close all files
							return e_success;
						}
					}
				}
			}
		}
	}
}


