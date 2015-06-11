
#include <sstream>
#include "PPEShading.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"



//
//  PPEShading def
//
///////////////////////////////////////////////////////
void PPEShading::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPEShading *tar_obj = (PPEShading *)obj;

	tar_obj->_name = (PPTName *)_name->Copy();
	tar_obj->_dict = (PPTDictionary *)_dict->Copy();
}

void PPEShading::SetParser(PPParser *parser)
{
	_name->_parser = parser;
	_dict->_parser = parser;
}

string PPEShading::makeCommandString()
{
    string retstr;
    retstr = _name->pdfString() + " sh\xa";
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
    PPTIndirectRef *sh_ref = (PPTIndirectRef *)_parentForm->_resourceDict->objectForKey("Shading");
    if (!sh_ref) {
        cout << "Shading IndirectRef not found..." << PP_ENDL;
        return;
    }
    PPTDictionary *sh_dict = (PPTDictionary *)sh_ref->valueObject();
    if (!sh_dict) {
        cout << "Shading Dictionary not found..." << PP_ENDL;
        return;
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



