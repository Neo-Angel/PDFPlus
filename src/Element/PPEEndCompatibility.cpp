
#include <sstream>
#include "PPEEndCompatibility.h"






//  End Compatibility
//
///////////////////////////////////////////////////////
string PPEEndCompatibility::makeCommandString()
{
    string retstr;
    retstr = "EX\xa";
    return retstr;
}

string PPEEndCompatibility::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='EndCompatibility'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}