
#include <sstream>
#include "PPEMarkedContent.h"
#include "PPTName.h"
#include "PPTDictionary.h"



//
//  PPEMarkedContent
//
///////////////////////////////////////////////////////
PPEMarkedContent::PPEMarkedContent(PPTName *tag, PPTDictionary *property, PPContext *gcontext) : PPElement(gcontext)
{
	_tag = (PPTName *)tag->Copy();
       _property = (PPTDictionary *)property->Copy();
}
PPEMarkedContent::PPEMarkedContent()
{
	_property = NULL; 
	_tag = NULL;
}


void PPEMarkedContent::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEMarkedContent *tar_obj = (PPEMarkedContent *)obj;

	if(_tag)
		tar_obj->_tag = (PPTName *)_tag->Copy();
	if(_property)
		tar_obj->_property = (PPTDictionary *)_property->Copy();
}

void PPEMarkedContent::SetDocument(PPDocument *doc)
{
	if(_property)
		_property->_document = doc;
	if(_tag)
		_tag->_document = doc;
}

string PPEMarkedContent::MakeCommandString()
{
    string retstr;
    if (_property) {
        retstr = _tag->PDFString() + " " + _property->PDFString() + " DP\xa";
    }
    else {
        retstr = _tag->PDFString() + " MP\xa";
    }
    return retstr;
}

string PPEMarkedContent::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='MarkedContent' tag='" << _tag->_name << "'>" << PP_ENDL;
    if (_property) {
        ostr << _property->XMLString(level+1);
    }
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
