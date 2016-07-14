
#include "PPEGRestore.h"



//////////////////////////////////////////////////////////
//
//    PPEGRestore
//
//////////////////////////////////////////////////////////

string PPEGRestore::MakeCommandString()
{
    return "Q\xa";
}
string PPEGRestore::XMLString(int level)
{
    string retstr;
    retstr = PPTabStr(level) + "<Element type='GRestore'/>\xa";

    return retstr;
}

