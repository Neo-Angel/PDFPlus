
#include <sstream>
#include "PPEImage.h"
#include "PPTStream.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"
#include "PPImage.h"

//  Image
//
///////////////////////////////////////////////////////
PPEImage::PPEImage(PPTName *name, PPContext *gcontext) : PPElement(gcontext)
{
	_name = *name->_name;
	_image = NULL;
//	_image_path = NULL;
//	_dict = dict;
//	_stream = NULL;
}

PPEImage::PPEImage(PPTDictionary *dict, PPContext *gcontext) : PPElement(gcontext)
{
//	_name = NULL;
	_image = NULL;
//	_image_path = NULL;
//	_dict = dict;
//	_stream = NULL;
}

PPEImage::PPEImage(string image_path, PPContext *gcontext) : PPElement(gcontext)
{
//	_name = name;
	_xobj = NULL;
	_image = NULL;
	this->SetImagePath(image_path);
}


PPEImage::PPEImage(char *image_path, PPContext *gcontext) : PPElement(gcontext)
{
//	_name = NULL;
	_image = NULL;
	_xobj = NULL;
	_image_path = image_path;
	this->SetImagePath(image_path);
}

PPEImage::PPEImage(string image_path, PPFormBase *form) : PPElement(form->ContextRef())
{
//	_name = name;
    _parentForm = form;
	_xobj = NULL;
	_image = NULL;
//	_image_path = image_path;
	this->SetImagePath(image_path);
}


PPEImage::PPEImage()
{
//	_name = NULL;
//	_dict = NULL;
//	_stream = NULL;
}

void PPEImage::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEImage *tar_obj = (PPEImage *)obj;


	tar_obj->_name = _name;
	if(_xobj)
		tar_obj->_xobj = (PPTIndirectObj *)_xobj->Copy();
//	if(_dict)
//		tar_obj->_dict = (PPTDictionary *)_dict->Copy();
//	if(_stream)
//		tar_obj->_stream = (PPTStream *)_stream->Copy();
}

void PPEImage::SetDocument(PPDocument *doc) 
{
	if(_xobj)
		_xobj->_document = doc;
}


string PPEImage::MakeCommandString()
{
    string retstr;
	retstr = PPElement::MakeCommandString();
	if(_name.length())
		retstr += "/"+_name + " Do\xa";
    return retstr;
}

string PPEImage::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='Image'>" << PP_ENDL;
    ostr << _gstate->XMLString(level+1);
    ostr << _xobj->XMLString(level+1);
    
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEImage::SetImagePath(string new_path)
{

	if (new_path.length()) {
		PPTIndirectRef *prev_img_ref = NULL;
		PPDocument *doc = _parentForm->_document;
		// _name : image resource key
		if(_name.length() > 0) {
			// _name이 있다면 이 _name을 키로하는 ref가 
			// XObject 딕셔너리에 포함되어 있다는 얘기임.
			PPTDictionary *dict = _parentForm->ResourceDictForKey("XObject");
			if(dict) {
				prev_img_ref = (PPTIndirectRef *)dict->ObjectForKey(_name);
			}
		}
		if(_xobj && prev_img_ref) {
			// _xobj 의 여러 ref들 중에 prev_img_ref를 찾아 지운다.
			int icnt = _xobj->_ref_list.size();
			for(int i=0;i<icnt;i++) {
				PPTIndirectRef *ref = _xobj->_ref_list[i];
				if(ref == prev_img_ref) {
					_xobj->_ref_list.erase(_xobj->_ref_list.begin() + i);
				}
			}
			// _xobj를 참조하는 ref가 prev_img_ref 하나밖에 없었다면
			// 기존의 _xobj 를 지운다.
			// 아울러 도큐먼트의 _tokens에 저장되어 있는 오브젝트도 찾아서 지운다.
			// 그렇지 않으면 PDF에 사용되어지지 않은 이미지가 포함된다.
			if(_xobj->_ref_list.size() == 0) {
				doc->RemoveObj(_xobj);
				delete _xobj;
			}
		}

		//해당 경로에 PPImage가 없으면 생성한 후 doc에 추가하고 PPIndirObj를 리턴한다.
		_xobj = doc->ImageObjFromPath(new_path); 
		if(_xobj) {
			_name = _parentForm->NameFromResourceObj(_xobj, "XObject");
			
			_subtype = "Image";

			_image = doc->ImageFromPath(new_path);
		}
		_image_path = "";
		return;
	}
	_xobj = NULL;
}

void PPEImage::WillAddToParent(PPFormBase *form)
{
    PPElement::WillAddToParent(form);
	if (_image_path.length()) {
		PPDocument *doc = form->_document;

		//해당 경로에 PPImage가 없으면 생성한 후 doc에 추가하고 PPIndirObj를 리턴한다.
		_xobj = doc->ImageObjFromPath(_image_path); 
		if(_xobj) {
			_name = form->NameFromResourceObj(_xobj, "XObject");
			
			_subtype = "Image";
			_image = doc->ImageFromPath(_image_path);
		}
		_image_path = "";
		return;
	}

    PPTIndirectRef *xobj_ref = (PPTIndirectRef *)_parentForm->ResourceForKey("XObject");
    if (!xobj_ref) {
        cout << "XObje IndirectRef not found..." << PP_ENDL;
        return;
    }
	PPTDictionary *xobj_dict = NULL;
	if(xobj_ref->ClassType() == PPTN_INDIRECTREF) {
		xobj_dict = (PPTDictionary *)xobj_ref->ValueObject();
		if (!xobj_dict) {
			cout << "Shading Dictionary not found..." << PP_ENDL;
			return;
		}
	}
	else if(xobj_ref->ClassType() == PPTN_DICTIONARY) {
		xobj_dict = (PPTDictionary *)xobj_ref;
	}
    xobj_ref = (PPTIndirectRef *)xobj_dict->ObjectForKey(_name);
    if (!xobj_ref) {
        cout << "Shading IndirectRef not found..." << PP_ENDL;
        return;
    }
	_xobj = (PPTIndirectObj *)xobj_ref->TargetObject(); //  <==
    if (!_xobj) {
        cout << "Shading Resource Object not found..." << PP_ENDL;
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

bool PPEImage::HasResource()
{
	return true;
}


/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPEImage::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
	rsc_list.push_back(PPRT_XOBJECT);
	return rsc_list;
}

string PPEImage::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}
	return _name;
}


PPToken *PPEImage::ResourceObjectFor(const char *rsc_type)
{
	if(rsc_type == PPRT_XOBJECT) {
		if(_xobj)
			return _xobj;
	}

	return PPElement::ResourceObjectFor(rsc_type);
}
