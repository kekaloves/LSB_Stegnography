#ifndef DECODE_H
#define DECODE_H

#include "types.h"//user define function
#include "encode.h"
//for decoded magic string
Status decode_magic_string(EncodeInfo *decInfo,char *key);
//for decoded file size
Status decode_file_size(EncodeInfo *decInfo);
//for decoded secret file
Status decode_secret_file(EncodeInfo *decInfo);
//perform the decoding
Status do_decoding(char *argv[],EncodeInfo *decInfo,char *key);


#endif
