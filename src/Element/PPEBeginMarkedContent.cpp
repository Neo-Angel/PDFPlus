
#include <sstream>
#include "PPEBeginMarkedContent.h"
#include "PPTDictionary.h"
#include "PPTName.h"



//  Begin Mark Content
//
///////////////////////////////////////////////////////

PPEBeginMarkedContent::PPEBeginMarkedContent(PPTName *tag, PPToken *properties, PPContext *gcontext) : PPElement(gcontext){
		_tag = (PPTName *)tag->Copy();
		if(properties)
			_properties = (PPToken *)properties->Copy();
		_OCGInfo = NULL;
}

PPEBeginMarkedContent::PPEBeginMarkedContent(PPDocument *doc, string properties, PPContext *gcontext) : PPElement(gcontext)
{
	string *tag_str = new string("OC");
	_tag = new PPTName(doc, tag_str);
	string *new_properties = new string(properties);
	_properties = new PPTName(doc, new_properties);
	_OCGInfo = NULL;
}
PPEBeginMarkedContent::PPEBeginMarkedContent(){
        _tag = NULL;
        _properties = NULL;
	_OCGInfo = NULL;
}

PPEBeginMarkedContent::~PPEBeginMarkedContent(){
        if(_tag)
			delete _tag;
		if(_properties)
			delete _properties;
}

void PPEBeginMarkedContent::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEBeginMarkedContent *tar_obj = (PPEBeginMarkedContent *)obj;

	if(_properties)
		tar_obj->_properties = (PPToken *)_properties->Copy();
	if(_tag)
		tar_obj->_tag = (PPTName *)_tag->Copy();
}

void PPEBeginMarkedContent::SetDocument(PPDocument *doc)
{
	if(_properties)
		_properties->_document = doc;
	if(_tag)
		_tag->_document = doc;
}

string PPEBeginMarkedContent::MakeCommandString()
{
    string retstr;
    if (_properties) {
        retstr = _tag->PDFString() + " " + _properties->PDFString() + " BDC\xa";
    }
    else {
        retstr = _tag->PDFString() + " BMC\xa";
    }
    return retstr;
}

string PPEBeginMarkedContent::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='BeginMarkedContent' tag='" << _tag->_name << "'>" << PP_ENDL;
    if (_properties) {
        ostr << _properties->XMLString(level+1);
    }
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEBeginMarkedContent::WillAddToParent(PPFormBase *form)
{
    PPElement::WillAddToParent(form);
	_OCGInfo = (PPTIndirectObj *)PPElement::ResourceObjectFor(PPRT_PROPERTIES);
    if (!_OCGInfo) {
        cout << "OCG Resource Object not found..." << PP_ENDL;
        return;
	}
}


/////////////////////////////////////////////////////  Multi Resource Handling
bool PPEBeginMarkedContent::HasResource()
{
	if(_properties && _properties->ClassType() == PPTN_NAME)
		return true;
	return false;
}


vector <const char *> PPEBeginMarkedContent::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
	rsc_list.push_back(PPRT_PROPERTIES);
	return rsc_list;
}


string PPEBeginMarkedContent::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}

	return *((PPTName *)_properties)->_name;
}


PPToken *PPEBeginMarkedContent::ResourceObjectFor(const char *rsc_type)
{
	if(rsc_type == PPRT_PROPERTIES) {
		if(_OCGInfo)
			return _OCGInfo;
	}

	return PPElement::ResourceObjectFor(rsc_type);
}
