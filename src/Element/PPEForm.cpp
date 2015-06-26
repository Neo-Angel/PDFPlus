
#include <sstream>
#include "PPEForm.h"
#include "PPTName.h"
#include "PPTDictionary.h"

//
//  Form
//
///////////////////////////////////////////////////////
PPEForm::PPEForm(PPPage *page, PPDocument *doc)
{
	_name = doc->AddFormObject(page);
	_dict = NULL;
}
PPEForm::PPEForm(PPTName *name, PPContext *gcontext) : PPElement(gcontext)
{
	_name = name;
	_dict = NULL;
}

PPEForm::PPEForm(PPTDictionary *dict, PPContext *gcontext) : PPElement(gcontext)
{
	_name = NULL;
    _dict = dict;
//    _stream = NULL;
/*
	PPToken *token = _dict->objectForKey("Resources");
    if (token->classType() == PPTN_DICTIONARY) {
        _resourceDict = (PPTDictionary *)token;
    }
    else {
        PPTIndirectRef *indirref = (PPTIndirectRef *)token;
        _resourceDict = (PPTDictionary *)indirref->valueObject();
    }
	*/
}


void PPEForm::CopyMembers(PPBase *obj)
{
	PPElement::CopyMembers(obj);
	PPEForm *tar_obj = (PPEForm *)obj;
	tar_obj->_name = (PPTName *)_name->Copy();
	tar_obj->_dict = (PPTDictionary *)_dict->Copy();
}

void PPEForm::SetParser(PPParser *parser)
{
	if(_name)
		_name->_parser = parser;
	if(_dict)
		_dict->_parser = parser;
}
/*
void PPEForm::setStream(PPTStream *stream)
{
    
    if (stream) {
        _stream = stream;
        if (!stream->_decodeFailed) {
            _graphicParser.parseStream(*stream);
        }
    }
}
*/
string PPEForm::makeCommandString()
{
    string retstr;
    retstr = _name->pdfString() + " Do\xa";
    return retstr;
}

string PPEForm::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Form'>" << PP_ENDL;
    ostr << _gstate->xmlString(level+1);
    ostr << _dict->xmlString(level+1);
	/*
    ostr << tapStr(level+1) << "<SubElement>" << PP_ENDL;
    size_t i, icnt = _elements.size();
    for (i=0; i<icnt; i++) {
        PPElement *el =  _elements.at(i);
        ostr << el->xmlString(level+2);
    }
    ostr << tapStr(level+1) << "</SubElement>" << PP_ENDL;
   */
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

PPTIndirectObj *PPEForm::GetXObject()
{
	PPTIndirectObj *indir_obj = NULL;
	int xobj_num = _parentForm->GetXObjNumOf(*_name->_name);
	if(xobj_num > 0) {
		PPDocument *doc = _parentForm->_document;
		indir_obj = (PPTIndirectObj *)doc->_xobjects[xobj_num];
	}
	return indir_obj;
}

PPFormBase *PPEForm::GetFormObj()  // Not Form Element
{
	PPFormBase *ret_form = NULL;
	PPTIndirectObj *indir_obj = GetXObject();
	if(indir_obj) {
//		PPTDictionary *dict = indir_obj->firstDictionary();
		ret_form = new PPFormBase(indir_obj);
		if (!ret_form->HasElements()) {
			delete ret_form;
			return NULL;
		}
	}
	return ret_form;
}
