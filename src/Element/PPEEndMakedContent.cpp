

#include <sstream>
#include "PPEEndMarkedContent.h"





//  End Mark Content
//
///////////////////////////////////////////////////////
string PPEEndMarkedContent::makeCommandString()
{
    string retstr;
    retstr = "EMC\xa";
    return retstr;
}

string PPEEndMarkedContent::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tabStr(level) << "<Element type='EndMarkedContent'/>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
