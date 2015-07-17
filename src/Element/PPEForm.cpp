
#include <sstream>
#include "PPEForm.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"

//
//  Form
//
///////////////////////////////////////////////////////
PPEForm::PPEForm(PPPage *page, PPDocument *doc)
{
	_name = doc->AddFormObject(page);
	_xobj = NULL;
}
PPEForm::PPEForm(PPTName *name, PPContext *gcontext) : PPElement(gcontext)
{
	_name = name;
	_xobj = NULL;
}

PPEForm::PPEForm()
{
	_name = NULL; 
	_xobj = NULL;
}

void PPEForm::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEForm *tar_obj = (PPEForm *)obj;
	if(_name)
		tar_obj->_name = (PPTName *)_name->Copy();
	if(_xobj)
		tar_obj->_xobj = (PPTIndirectObj *)_xobj->Copy();
}

void PPEForm::SetParser(PPParser *parser)
{
	if(_name)
		_name->_parser = parser;
	if(_xobj)
		_xobj->_parser = parser;
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
	retstr = PPElement::makeCommandString();
    retstr += _name->pdfString() + " Do\xa";
    return retstr;
}

string PPEForm::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Form'>" << PP_ENDL;
    ostr << _gstate->xmlString(level+1);
    ostr << _xobj->xmlString(level+1);
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
		PPDocument *doc = _parentForm->_document;
		ret_form = new PPFormBase(doc, indir_obj);
		if (!ret_form->HasElements()) {
			delete ret_form;
			return NULL;
		}
		ret_form->_form_key = (PPTName *)_name->Copy();
	}
	return ret_form;
}

void PPEForm::willAddToParent(PPFormBase *form)
{
    PPElement::willAddToParent(form);
    PPTIndirectRef *xobj_ref = (PPTIndirectRef *)_parentForm->ResourceForKey("XObject");
    if (!xobj_ref) {
        cout << "XObje IndirectRef not found..." << PP_ENDL;
        return;
    }
	PPTDictionary *xobj_dict = NULL;
	if(xobj_ref->classType() == PPTN_INDIRECTREF) {
		xobj_dict = (PPTDictionary *)xobj_ref->valueObject();
		if (!xobj_dict) {
			cout << "Shading Dictionary not found..." << PP_ENDL;
			return;
		}
	}
	else if(xobj_ref->classType() == PPTN_DICTIONARY) {
		xobj_dict = (PPTDictionary *)xobj_ref;
	}
    xobj_ref = (PPTIndirectRef *)xobj_dict->objectForKey(*_name->_name);
    if (!xobj_ref) {
        cout << "Shading IndirectRef not found..." << PP_ENDL;
        return;
    }
	_xobj = (PPTIndirectObj *)xobj_ref->targetObject();
    if (!_xobj) {
        cout << "Shading Resource Object not found..." << PP_ENDL;
        return;
	}
	xobj_dict = _xobj->firstDictionary();
	if(xobj_dict == NULL) {
		return;
	}
	PPTName *subtype_name =  (PPTName *)xobj_dict->ObjectForKey("Subtype");
	if(subtype_name == NULL) {
		return;
	}
	_subtype = *subtype_name->_name;
//	_dict = (PPTDictionary *)sh_ref->valueObject();
//    if (!_dict) {
//        cout << "Shading Dictionary not found..." << PP_ENDL;
//        return;
//    }
}

bool PPEForm::HasResource()
{
	return true;
}

string PPEForm::ResourceType()
{
	return "XObject"; //_subtype;
}
string PPEForm::ResourceKey()
{
	return *_name->_name;
}


PPToken *PPEForm::GetResource()
{
	if(_xobj)
		return _xobj;
//	if(_dict) {
//		return _dict;
//	}

	return PPElement::GetResource();
//	PPToken *rsc = _parentForm->_document->ResourceForKey(ResourceType(), ResourceKey());
//	return rsc;
}

/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPEForm::ResourceList()
{
	vector <const char *> rsc_list = PPElement::ResourceList();
	rsc_list.push_back(PPRT_XOBJECT);
	return rsc_list;
}

string PPEForm::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}
	return *_name->_name;
}


PPToken *PPEForm::GetResource(const char *rsc_type)
{
	if(rsc_type == PPRT_XOBJECT) {
		if(_xobj)
			return _xobj;
	}

	return PPElement::GetResource();
}
