

#include <sstream>
#include "PPEEndMarkedContent.h"





//  End Mark Content
//
///////////////////////////////////////////////////////
string PPEEndMarkedContent::MakeCommandString()
{
    string retstr;
    retstr = "EMC\xa";
    return retstr;
}

string PPEEndMarkedContent::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='EndMarkedContent'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
