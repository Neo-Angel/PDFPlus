
#include "PPEGRestore.h"



//////////////////////////////////////////////////////////
//
//    PPEGRestore
//
//////////////////////////////////////////////////////////

string PPEGRestore::makeCommandString()
{
    return "Q\xa";
}
string PPEGRestore::xmlString(int level)
{
    string retstr;
    retstr = tapStr(level) + "<Element type='GRestore'/>\xa";

    return retstr;
}

