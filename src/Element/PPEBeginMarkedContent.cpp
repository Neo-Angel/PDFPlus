
#include <sstream>
#include "PPEBeginMarkedContent.h"
#include "PPTDictionary.h"
#include "PPTName.h"



//  Begin Mark Content
//
///////////////////////////////////////////////////////

PPEBeginMarkedContent::PPEBeginMarkedContent(PPTName *tag, PPTDictionary *property, PPContext *gcontext) : PPElement(gcontext){
		_tag = (PPTName *)tag->Copy();
		_property = (PPTDictionary *)property->Copy();
    }
PPEBeginMarkedContent::PPEBeginMarkedContent(){
        _tag = NULL;
        _property = NULL;
	}

void PPEBeginMarkedContent::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEBeginMarkedContent *tar_obj = (PPEBeginMarkedContent *)obj;

	if(_property)
		tar_obj->_property = (PPTDictionary *)_property->Copy();
	if(_tag)
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
