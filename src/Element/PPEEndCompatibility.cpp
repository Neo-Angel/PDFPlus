
#include <sstream>
#include "PPEEndCompatibility.h"






//  End Compatibility
//
///////////////////////////////////////////////////////
string PPEEndCompatibility::MakeCommandString()
{
    string retstr;
    retstr = "EX\xa";
    return retstr;
}

string PPEEndCompatibility::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='EndCompatibility'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}