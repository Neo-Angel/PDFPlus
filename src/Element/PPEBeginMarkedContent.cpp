
#include <sstream>
#include "PPEBeginMarkedContent.h"
#include "PPTDictionary.h"
#include "PPTName.h"



//  Begin Mark Content
//
///////////////////////////////////////////////////////
void PPEBeginMarkedContent::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPEBeginMarkedContent *tar_obj = (PPEBeginMarkedContent *)obj;

	tar_obj->_property = (PPTDictionary *)_property->Copy();
	tar_obj->_tag = (PPTName *)_tag->Copy();
}

void PPEBeginMarkedContent::SetParser(PPParser *parser)
{
	if(_property)
		_property->_parser = parser;
	if(_tag)
		_tag->_parser = parser;
}

string PPEBeginMarkedContent::makeCommandString()
{
    string retstr;
    if (_property) {
        retstr = _tag->pdfString() + " " + _property->pdfString() + " BDC\xa";
    }
    else {
        retstr = _tag->pdfString() + " BMC\xa";
    }
    return retstr;
}

string PPEBeginMarkedContent::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='BeginMarkedContent' tag='" << _tag->_name << "'>" << PP_ENDL;
    if (_property) {
        ostr << _property->xmlString(level+1);
    }
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
