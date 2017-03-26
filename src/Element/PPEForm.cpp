
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
//	if(_xobj)
//		tar_obj->_xobj = (PPTIndirectObj *)_xobj->Copy();

	PPDocument *tar_doc = tar_obj->_parentForm->_document;
	PPDocument *this_doc = this->_parentForm->_document;
	if(tar_doc !=  this_doc && _name->_name->length() > 0) {
		PPTIndirectObj *rsc_obj = _parentForm->ResourceObjForName(*_name->_name, "XObject");
		if(rsc_obj) {
			// this->Document() 에 있는 rsc_obj를 tar_doc에 복사해 넣음
			// 복사할 때 rsc_obj의 objNum를 tar_doc에 맞춰서 변경함.
			PPTIndirectObj *new_obj = tar_doc->MoveObjFrom(rsc_obj, this->Document());
			PPTIndirectRef *new_ref = tar_obj->_parentForm->AddResourceRef(new_obj->_objNum, *_name->_name, "XObject");
			if(new_ref) {
				new_obj->AddRefObj(new_ref);
			}
			tar_obj->_xobj = new_obj;
		}
	}

}

void PPEForm::SetDocument(PPDocument *doc)
{
	if(_name)
		_name->_document = doc;
	if(_xobj)
		_xobj->_document = doc;
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
string PPEForm::MakeCommandString()
{
    string retstr;
	retstr = PPElement::MakeCommandString();
    retstr += _name->PDFString() + " Do\xa";
    return retstr;
}

string PPEForm::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='Form'>" << PP_ENDL;
    ostr << _gstate->XMLString(level+1);
    ostr << _xobj->XMLString(level+1);
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

PPTIndirectObj *PPEForm::XObject()
{
	PPTIndirectObj *indir_obj = NULL;
	int xobj_num = _parentForm->XObjectNumberFor(*_name->_name);
	if(xobj_num > 0) {
		PPDocument *doc = _parentForm->_document;
		indir_obj = (PPTIndirectObj *)doc->_xobjects[xobj_num];
	}
	return indir_obj;
}

PPFormBase *PPEForm::FormObj()  // Not Form Element
{
	PPFormBase *ret_form = NULL;
	PPTIndirectObj *indir_obj = XObject();
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

void PPEForm::WillAddToParent(PPFormBase *form)
{
    PPElement::WillAddToParent(form);
    PPTIndirectRef *xobj_ref = (PPTIndirectRef *)_parentForm->ResourceForKey("XObject");
    if (!xobj_ref) {
        cout << "XObje IndirectRef not found..." << PP_ENDL;
        return;
    }
	PPTDictionary *xobj_dict = NULL;
	if(xobj_ref->ClassType() == PPTN_INDIRECTREF) {
		xobj_dict = (PPTDictionary *)xobj_ref->ValueObject();
		if (!xobj_dict) {
			cout << "Form Dictionary not found..." << PP_ENDL;
			return;
		}
	}
	else if(xobj_ref->ClassType() == PPTN_DICTIONARY) {
		xobj_dict = (PPTDictionary *)xobj_ref;
	}
    xobj_ref = (PPTIndirectRef *)xobj_dict->ObjectForKey(*_name->_name);
    if (!xobj_ref) {
        cout << "Form IndirectRef not found..." << PP_ENDL;
        return;
    }
	_xobj = (PPTIndirectObj *)xobj_ref->TargetObject();
    if (!_xobj) {
        cout << "Form Resource Object not found..." << PP_ENDL;
        return;
	}
	xobj_dict = _xobj->FirstDictionary();
	if(xobj_dict == NULL) {
		return;
	}
	PPTName *subtype_name =  (PPTName *)xobj_dict->ObjectForKey("Subtype");
	if(subtype_name == NULL) {
		return;
	}
	_subtype = *subtype_name->_name;
}

bool PPEForm::HasResource()
{
	return true;
}

/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPEForm::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
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


PPToken *PPEForm::ResourceObjectFor(const char *rsc_type)
{
	if(rsc_type == PPRT_XOBJECT) {
		if(_xobj)
			return _xobj;
	}

	return PPElement::ResourceObjectFor(rsc_type);
}
