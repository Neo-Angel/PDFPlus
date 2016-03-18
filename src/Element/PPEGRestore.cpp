
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
string PPEGRestore::XMLString(int level)
{
    string retstr;
    retstr = PPTabStr(level) + "<Element type='GRestore'/>\xa";

    return retstr;
}

