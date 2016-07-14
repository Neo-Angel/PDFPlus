
#include "PPEGsave.h"



//////////////////////////////////////////////////////////
//
//    PPEGSave
//
//////////////////////////////////////////////////////////

string PPEGSave::MakeCommandString()
{
    return "q\xa";
}


string PPEGSave::XMLString(int level)
{
    string retstr;
    retstr = PPTabStr(level) + "<Element type='GSave'/>\xa";
    return retstr;
}