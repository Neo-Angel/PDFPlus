
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
    retstr = tabStr(level) + "<Element type='GRestore'/>\xa";

    return retstr;
}

