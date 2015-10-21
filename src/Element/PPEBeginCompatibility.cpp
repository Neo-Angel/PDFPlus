
#include <sstream>
#include "PPEBeginCompatibility.h"






//  Begin Compatibility
//
///////////////////////////////////////////////////////
string PPEBeginCompatibility::makeCommandString()
{
    string retstr;
    retstr = "BX\xa";
    return retstr;
}

string PPEBeginCompatibility::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tabStr(level) << "<Element type='BeginCompatibility'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}