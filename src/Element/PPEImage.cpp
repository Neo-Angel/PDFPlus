
#include <sstream>
#include "PPEImage.h"
#include "PPTStream.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"
//  Image
//
///////////////////////////////////////////////////////

PPEImage::PPEImage(PPTDictionary *dict, PPContext *gcontext) : PPElement(gcontext)
{
	_name = NULL;
//	_dict = dict;
//	_stream = NULL;
}

PPEImage::PPEImage(PPTName *name, PPContext *gcontext) : PPElement(gcontext)
{
	_name = name;
	_xobj = NULL;
}

PPEImage::PPEImage()
{
	_name = NULL;
//	_dict = NULL;
//	_stream = NULL;
}

void PPEImage::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEImage *tar_obj = (PPEImage *)obj;

	if(_name)
		tar_obj->_name = (PPTName *)_name->Copy();
	if(_xobj)
		tar_obj->_xobj = (PPTIndirectObj *)_xobj->Copy();
//	if(_dict)
//		tar_obj->_dict = (PPTDictionary *)_dict->Copy();
//	if(_stream)
//		tar_obj->_stream = (PPTStream *)_stream->Copy();
}

void PPEImage::SetParser(PPParser *parser) 
{
	if(_name)
		_name->_parser = parser;
	if(_xobj)
		_xobj->_parser = parser;
//	if(_stream)
//		_stream->_parser = parser;
}


string PPEImage::makeCommandString()
{
    string retstr;
	retstr = PPElement::makeCommandString();
	if(_name)
		retstr += _name->pdfString() + " Do\xa";
    return retstr;
}

string PPEImage::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Image'>" << PP_ENDL;
    ostr << _gstate->xmlString(level+1);
    ostr << _xobj->xmlString(level+1);
    
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}


void PPEImage::willAddToParent(PPFormBase *form)
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

bool PPEImage::HasResource()
{
	return true;
}

string PPEImage::ResourceType()
{
	return "XObject"; //_subtype;
}
string PPEImage::ResourceKey()
{
	return *_name->_name;
}


PPToken *PPEImage::GetResource()
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
vector <const char *> PPEImage::ResourceList()
{
	vector <const char *> rsc_list = PPElement::ResourceList();
	rsc_list.push_back(PPRT_XOBJECT);
	return rsc_list;
}

string PPEImage::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}
	return *_name->_name;
}


PPToken *PPEImage::GetResource(const char *rsc_type)
{
	if(rsc_type == PPRT_XOBJECT) {
		if(_xobj)
			return _xobj;
	}

	return PPElement::GetResource();
}
