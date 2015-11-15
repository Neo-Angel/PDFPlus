
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

PPEBeginMarkedContent::PPEBeginMarkedContent(PPParser *parser, string properties, PPContext *gcontext) : PPElement(gcontext)
{
	string *tag_str = new string("OC");
	_tag = new PPTName(parser, tag_str);
	string *new_properties = new string(properties);
	_properties = new PPTName(parser, new_properties);
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

void PPEBeginMarkedContent::SetParser(PPParser *parser)
{
	if(_properties)
		_properties->_parser = parser;
	if(_tag)
		_tag->_parser = parser;
}

string PPEBeginMarkedContent::makeCommandString()
{
    string retstr;
    if (_properties) {
        retstr = _tag->pdfString() + " " + _properties->pdfString() + " BDC\xa";
    }
    else {
        retstr = _tag->pdfString() + " BMC\xa";
    }
    return retstr;
}

string PPEBeginMarkedContent::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tabStr(level) << "<Element type='BeginMarkedContent' tag='" << _tag->_name << "'>" << PP_ENDL;
    if (_properties) {
        ostr << _properties->xmlString(level+1);
    }
    ostr << tabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEBeginMarkedContent::willAddToParent(PPFormBase *form)
{
    PPElement::willAddToParent(form);
	_OCGInfo = (PPTIndirectObj *)PPElement::GetResource(PPRT_PROPERTIES);
    if (!_OCGInfo) {
        cout << "OCG Resource Object not found..." << PP_ENDL;
        return;
	}
}


/////////////////////////////////////////////////////  Multi Resource Handling
bool PPEBeginMarkedContent::HasResource()
{
	if(_properties && _properties->classType() == PPTN_NAME)
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


PPToken *PPEBeginMarkedContent::GetResource(const char *rsc_type)
{
	if(rsc_type == PPRT_PROPERTIES) {
		if(_OCGInfo)
			return _OCGInfo;
	}

	return PPElement::GetResource(rsc_type);
}
