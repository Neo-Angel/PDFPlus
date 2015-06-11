
#include <sstream>
#include "PPEImage.h"
#include "PPTStream.h"
#include "PPTName.h"
#include "PPTDictionary.h"

//  Image
//
///////////////////////////////////////////////////////
void PPEImage::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPEImage *tar_obj = (PPEImage *)obj;

	tar_obj->_name = (PPTName *)_name->Copy();
	tar_obj->_dict = (PPTDictionary *)_dict->Copy();
	tar_obj->_stream = (PPTStream *)_stream->Copy();
}

void PPEImage::SetParser(PPParser *parser) 
{
	_name->_parser = parser;
	_dict->_parser = parser;
	_stream->_parser = parser;
}


string PPEImage::makeCommandString()
{
    string retstr;
    retstr = _name->pdfString() + " Do\xa";
    return retstr;
}

string PPEImage::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Image'>" << PP_ENDL;
    ostr << _gstate->xmlString(level+1);
    ostr << _dict->xmlString(level+1);
    
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
