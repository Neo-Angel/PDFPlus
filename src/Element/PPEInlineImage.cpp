
#include <sstream>
#include "PPEInlineImage.h"




//
//  Inline Image
//
///////////////////////////////////////////////////////
void PPEInlineImage::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEInlineImage *tar_obj = (PPEInlineImage *)obj;
	//map <string, PPToken *> _dict
    map <string, PPToken *>::iterator it_dict;
	for(it_dict = _dict.begin(); it_dict != _dict.end(); it_dict++) {
		string key = it_dict->first;
        PPToken *src_token = it_dict->second;
		tar_obj->_dict[key] = (PPToken *) src_token->Copy();
    }
}

void PPEInlineImage::SetParser(PPParser *parser)
{
    map <string, PPToken *>::iterator it_dict;
	for(it_dict = _dict.begin(); it_dict != _dict.end(); it_dict++) {
        PPToken *token = it_dict->second;
		if(token)
			token->_parser = parser;
    }
}

string PPEInlineImage::makeCommandString()
{
    string retstr;
    ostringstream ostr;
    ostr << "BI\xa";
//    ostr << _dict << PP_ENDL;
    ostr << "ID\xa";
//    ostr << _data << PP_ENDL;
    ostr << "EI\xa";
    retstr = ostr.str();
    return retstr;
}

string PPEInlineImage::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='InlineImage'>" << PP_ENDL;
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
