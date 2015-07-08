
#include <sstream>
#include "PPEShading.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"



//
//  PPEShading def
//
///////////////////////////////////////////////////////
PPEShading::PPEShading(PPContext *gcontext) : PPElement(gcontext)
{
	_dict = NULL;
	_name = NULL;
}

PPEShading::PPEShading()
{
	_dict = NULL;
	_name = NULL;
}

void PPEShading::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPEShading *tar_obj = (PPEShading *)obj;

	if(_name)
		tar_obj->_name = (PPTName *)_name->Copy();
	if(_dict)
		tar_obj->_dict = (PPTDictionary *)_dict->Copy();
}

void PPEShading::SetParser(PPParser *parser)
{
	if (_name)
		_name->_parser = parser;
	if (_dict)
		_dict->_parser = parser;
}

string PPEShading::makeCommandString()
{
    string retstr;
	if(_name) {
		retstr = _name->pdfString();
		retstr += " sh\xa";
	}
    return retstr;
}

string PPEShading::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Shading' name='" << *_name->_name << "'>" << PP_ENDL;
    if (_dict) {
        ostr << _dict->xmlString(level+1);
    }
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEShading::willAddToParent(PPFormBase *form)
{
    PPElement::willAddToParent(form);
    PPTIndirectRef *sh_ref = (PPTIndirectRef *)_parentForm->ResourceForKey("Shading");
    if (!sh_ref) {
        cout << "Shading IndirectRef not found..." << PP_ENDL;
        return;
    }
	PPTDictionary *sh_dict = NULL;
	if(sh_ref->classType() == PPTN_INDIRECTREF) {
		sh_dict = (PPTDictionary *)sh_ref->valueObject();
		if (!sh_dict) {
			cout << "Shading Dictionary not found..." << PP_ENDL;
			return;
		}
	}
	else if(sh_ref->classType() == PPTN_DICTIONARY) {
		sh_dict = (PPTDictionary *)sh_ref;
	}
    sh_ref = (PPTIndirectRef *)sh_dict->objectForKey(*_name->_name);
    if (!sh_ref) {
        cout << "Shading IndirectRef not found..." << PP_ENDL;
        return;
    }
    _dict = (PPTDictionary *)sh_ref->valueObject();
    if (!_dict) {
        cout << "Shading Dictionary not found..." << PP_ENDL;
        return;
    }
}



