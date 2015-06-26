
#include <sstream>
#include "PPEMarkedContent.h"
#include "PPTName.h"
#include "PPTDictionary.h"



//
//  PPEMarkedContent
//
///////////////////////////////////////////////////////
void PPEMarkedContent::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPEMarkedContent *tar_obj = (PPEMarkedContent *)obj;

	tar_obj->_tag = (PPTName *)_tag->Copy();
	tar_obj->_property = (PPTDictionary *)_property->Copy();
}

void PPEMarkedContent::SetParser(PPParser *parser)
{
	if(_property)
		_property->_parser = parser;
	if(_tag)
		_tag->_parser = parser;
}

string PPEMarkedContent::makeCommandString()
{
    string retstr;
    if (_property) {
        retstr = _tag->pdfString() + " " + _property->pdfString() + " DP\xa";
    }
    else {
        retstr = _tag->pdfString() + " MP\xa";
    }
    return retstr;
}

string PPEMarkedContent::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='MarkedContent' tag='" << _tag->_name << "'>" << PP_ENDL;
    if (_property) {
        ostr << _property->xmlString(level+1);
    }
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
