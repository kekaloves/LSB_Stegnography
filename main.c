
// include header files
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "common.h"//user define header file
#include "encode.h"
#include "decode.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);

	// Return image capacity
	return width * height * 3;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	//Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		printf("file not exist");
		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	//Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		//perror("fopen");
		//fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
		printf("file not exist");
		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	//Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		//perror("fopen");
		//fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
		printf("file not exist");
		return e_failure;
	}

	// No failure return e_success
	return e_success;

}
OperationType check_operation_type(char *argv[])//to check operation type through command line argument
{
	if(!strcmp(argv[1],"-e"))//if condition to return encode
		return e_encode;
	else if(!strcmp(argv[1],"-d"))//if condition to return decode
		return e_decode;
	else
		return e_unsupported;
}
//check capacity
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"r");
	if(encInfo->fptr_secret == NULL)
	{
		fprintf(stdout,"No such files : %s\n",encInfo->secret_fname);
		return e_failure;
	}
	if(encInfo->fptr_src_image == NULL)
	{
		fprintf(stdout,"No such files : %s\n",encInfo->src_image_fname);
		return e_failure;
	}
	fseek(encInfo->fptr_secret,0,2);//get secret file size
	encInfo->size_secret_file = ftell(encInfo->fptr_secret);//get secret file size from the  current  position
	encInfo->image_capacity =  get_image_size_for_bmp(encInfo->fptr_src_image); //get image capacity of bmp image file
	if ( (encInfo->size_secret_file * 8 + 54 )  > encInfo->image_capacity )//check if capacity is less
	{
		fprintf(stdout,"image size is less %d\n",encInfo->image_capacity);//print msg
		return e_failure;
	}
	else
	{
		return e_success;
	}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)//function definition
{
	int i,j;
	for(i=0;argv[i];i++)//for no of arguments
	{
		for(j=0;argv[i][j];j++)//for each argument string
		{
			if(argv[i][j] == '-' )//check if argument strart with '-'
			{
				j++;
				switch(argv[i][j])//command type
				{
					case 'i':encInfo->src_image_fname = argv[i+1];//take next command line argument 
							 break;
					case 's':encInfo->secret_fname = argv[i+1];//take next command line argument
							 break;
					case 'o':encInfo->stego_image_fname = argv[i+1];//take next command line argument
							 break;
					case 'e':
							 break;
					default:fprintf(stdout,"Wrong command line input %s\n",argv[i]);
							return e_failure;
							break;
				}//switch end	

			}//if end
		}//end if
	}//end if
	return e_success;
}

Status read_and_validate_decode_args(char *argv[],EncodeInfo *decInfo)//function defination
{
	int i,j;
	for(i=0;argv[i];i++)//for no of arguments
	{
		for(j=0;argv[i][j];j++)//for each argument string
		{
			if(argv[i][j] == '-' )//check if argument start with '-'
			{
				j++;
				switch(argv[i][j])//command type
				{
					case 'd':break;
					case 'i':decInfo->stego_image_fname=argv[i+1];//take next command line argument
							 break;
					case 'o':decInfo->secret_fname=argv[i+1];// text next command line argument
							 break;
					default:fprintf(stdout,"Invalid command line input %s:\n",argv[i]);
							return e_failure;
				}
			}
		}
	}
	return e_success;
}

// main function call
int main(int argc,char *argv[])//command line argument
{
	OperationType type=check_operation_type(argv);//check operation type through command line argument
	if(type == e_encode )//check if type is correct
	{
		EncodeInfo infoencode;//define structure variable
		infoencode.src_image_fname=malloc(20*sizeof(char));//allocate size for image file name
		infoencode.secret_fname=malloc(20*sizeof(char));//allocate size for secret file name
		infoencode.stego_image_fname=malloc(20*sizeof(char));//allocate size for final image file name
		if( read_and_validate_encode_args(argv,&infoencode) == e_success)//check if argument encoding is correct
		{
			
			if( check_capacity(&infoencode) == e_success)//check if image capacity is correct
			{
				fcloseall();//close all files
				
				if ( do_encoding(&infoencode) == e_success )//check if encoding is success
				{
					printf("encoding completed\n");//print
				}//end if
			}//end if
		}//end if
	}//end if
	else if(type == e_decode )//check if correct
	{
		char key[10];//define character key
		printf("enter your secret Key \n");//enter key
		scanf("%s",key);
		EncodeInfo infodecode;//define structure variable
		infodecode.stego_image_fname=malloc(20*sizeof(char));//allocate size for image file name
		infodecode.secret_fname=malloc(20*sizeof(char));//allocate size for secret file name
		if( read_and_validate_decode_args(argv,&infodecode) == e_success)//check if argumenr decoding is correct
		{
			
			if( do_decoding(argv,&infodecode,key) ==e_success)//check if decoding is success
			{
				printf("decoding completed\n");//print
			}//end if
		}//end if
	}
	else
	{
		printf("invalid operation\n");//print
	}
	fcloseall();//all file close
	return 0;//return successfully
}

