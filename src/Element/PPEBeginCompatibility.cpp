
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

string PPEBeginCompatibility::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='BeginCompatibility'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}