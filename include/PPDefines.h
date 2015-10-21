//
//  PPDefines.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 12..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef PDFPlusLib_PPDefines_h
#define PDFPlusLib_PPDefines_h

#include <string.h>

using namespace std;

struct PPData{
    size_t _length;
    char *_data;

};

#ifndef byte
typedef unsigned char  byte;
#endif

#ifndef word
typedef unsigned short  word;
#endif


string tabStr(int cnt);
string toHexStr(string str);

#endif
