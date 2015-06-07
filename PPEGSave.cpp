
#include "PPEGsave.h"



//////////////////////////////////////////////////////////
//
//    PPEGSave
//
//////////////////////////////////////////////////////////

string PPEGSave::makeCommandString()
{
    return "q\xa";
}


string PPEGSave::xmlString(int level)
{
    string retstr;
    retstr = tapStr(level) + "<Element type='GSave'/>\xa";
    return retstr;
}