//
//  PPPage.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <sstream>
#include "PPPage.h"
#include "PPCoord.h"
#include "PPDocument.h"
#include "PPTStream.h"
#include "PPContext.h"
#include "PPTNumber.h"
#include "PPTArray.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"

string PPTabStr(int cnt);


///////////////////////////////////////////////////////////////////
// Constructors / Destructor
///////////////////////////////////////////////////////////////////
PPPage::PPPage(PPDocument *doc)  // this invokes PPFormBase() (default)constructor.
{
    _document = doc;
	_context = new PPContext;
}

PPPage::PPPage(PPPage *page)
{
	_document = page->_document;
	_formDict = (PPTDictionary *)page->_formDict->Copy();
}

// ????
void PPPage::StoreResources()
{
}

void PPPage::LoadDictionary(PPTDictionary *page_dict)
{
    _formDict = page_dict;
	_context->ptMatrix()->Rotate(rotate());

    _resourceDict = (PPTDictionary *)_formDict->valueObjectForKey("Resources");

	// 읽어들인 리소스들을 Document에 저장한다.
	StoreResources();// 아직 기능이 없음

    PPTDictionary *font_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("Font");
    if (font_dict) {
        map <string, PPToken *> &dict = font_dict->_dict;
       
        map <string, PPToken *> ::iterator it_token_objs;
        for(it_token_objs = dict.begin(); it_token_objs != dict.end(); it_token_objs++) {
            PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
            int obj_num = indir_ref->_objNum;
            PPTIndirectObj *font_obj = (PPTIndirectObj *)_document->_fonts[obj_num];
            if (font_obj == NULL) {
                font_obj = indir_ref->targetObject();
                _document->_fonts[obj_num] = font_obj;
            }
        }
    }

    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("XObject");
    if (xobject_dict) {
        map <string, PPToken *> &dict = xobject_dict->_dict;
        map <string, PPToken *> ::iterator it_token_objs;
        for(it_token_objs = dict.begin(); it_token_objs != dict.end(); it_token_objs++) {
            PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
            int obj_num = indir_ref->_objNum;
            PPTIndirectObj *indir_obj = (PPTIndirectObj *)_document->_xobjects[obj_num];
            if (indir_obj == NULL) {
                indir_obj = indir_ref->targetObject();
                _document->_xobjects[obj_num] = indir_obj;
                indir_obj = (PPTIndirectObj *)_document->_xobjects[obj_num];
            }
        }
    }

    size_t icnt = contentsCount();
    for (size_t i=0; i<icnt; i++) {
        PPTStream *stream = contentAt(i);
        if (!stream->_decodeFailed) {
            _graphicParser.ParseStream(*stream);
        }
    }
}

// Invoked by PPDocument::AddPage()
void PPPage::WriteDictionary(PPTDictionary *page_dict) // -> PreBuildPDF
{
	_formDict = page_dict;
	page_dict->SetTokenAndKey("Page", "Type");
	page_dict->SetTokenAndKey(0, "Rotate");
	
	if(_resourceDict == NULL) {
		_resourceDict = new PPTDictionary(_document);

		PPTArray *proset_list = new PPTArray(_document);

		PPTName *pname = new PPTName(_document, new string("PDF"));
		proset_list->AddToken(pname);
		pname = new PPTName(_document, new string("Test"));
		proset_list->AddToken(pname);
		proset_list->AddToken(new PPTName(_document, new string("ImageB")));
		proset_list->AddToken(new PPTName(_document, new string("ImageC")));
		proset_list->AddToken(new PPTName(_document, new string("ImageI")));
		_resourceDict->SetTokenAndKey(proset_list, "ProcSet");
	}
	PPTIndirectObj *rcs_obj = _document->SetRefTokenForKey(page_dict, _resourceDict, PPKN_RESOURCES);
}

// 페이지의 내용을 스트림으로 빌드하고 페이지 정보를 정리한다.
void PPPage::BuildPDF()
{
	PPTDictionary *stream_dict = new PPTDictionary(_document);

	PPTIndirectObj *stream_obj = _document->SetRefTokenForKey(_formDict, stream_dict, PPKN_CONTENTS);
	PPTStream *contents = BuildStream(); // 페이지의 drawing 코드들을 스트림으로 빌드한다.
	stream_dict->SetTokenAndKey(contents->_streamSize, "Length");
	stream_dict->SetTokenAndKey("FlateDecode", "Filter"); // Filter/FlateDecode
	contents->SetDictionary(stream_dict);
	contents->_decoded = true; 
	stream_obj->AddObj(contents);
}

PPTDictionary *PPPage::ResourcesDict()
{
	PPToken * rscs_tkn = _formDict->ValueObjectForKey(PPKN_RESOURCES);
	if(rscs_tkn) {
		return (PPTDictionary *)rscs_tkn;
	}
	return NULL;
}

PPTDictionary *PPPage::ResourceDictForType(string type)
{
	PPTDictionary *rscs_dict = ResourcesDict();
	if(rscs_dict) {
		PPTDictionary *rsc_dict = (PPTDictionary *)rscs_dict->ValueObjectForKey(type);
		return rsc_dict;
	}
	return NULL;
}

void PPPage::AddResource(PPToken *res, char *type, char *key)
{

}

PPRect PPPage::rectForKey(string key)
{
    PPRect rect;
    PPTArray *num_list = (PPTArray *)_formDict->objectForKey(key);
    if (num_list) {
        return rectFromArray(num_list);
    }
    return rect;
}

void PPPage::setRectForKey(PPRect rect, string key)
{
	PPTArray *num_list = new PPTArray(_document);
	SetRectToArray(rect, num_list);
	_formDict->setTokenAndKey(num_list, key);
}

int PPPage::intValueForKey(string key)
{
    int ret = 0;
    PPTNumber *num = (PPTNumber *)_formDict->objectForKey(key);
    if (num) {
        return num->intValue();
    }
    return ret;
}

float PPPage::floatValueForKey(string key)
{
    float ret = 0;
    PPTNumber *num = (PPTNumber *)_formDict->objectForKey(key);
    if (num) {
        return num->floatValue();
    }
    return ret;
}

bool PPPage::hasValueWithKey(string key)
{
    if(_formDict->objectForKey(key))
        return true;
    return false;
}

PPTStream *PPPage::contentAt(size_t i)
{
    PPTStream *ret_stream = NULL;
    
    PPToken *contents = _formDict->objectForKey(PPKN_CONTENTS);
    if (contents->ClassType() == PPTN_ARRAY) {
        PPTArray *array = (PPTArray *)contents;
        if (array->size() > i) {
            PPTIndirectRef *ref = (PPTIndirectRef *)array->objectAtIndex(i);
            PPTIndirectObj *indirect = ref->targetObject();
            ret_stream = indirect->stream();
        }
    }
    else if (contents->ClassType() == PPTN_INDIRECTREF){
        PPTIndirectObj *indirect = ((PPTIndirectRef *)contents)->targetObject();
        ret_stream = indirect->stream();
    }
    return ret_stream;
}

size_t PPPage::contentsCount()
{
    PPToken *contents = _formDict->objectForKey(PPKN_CONTENTS);
    if (contents->ClassType() == PPTN_ARRAY) {
        PPTArray *array = (PPTArray *)contents;
        return array->size();
    }
    else if (contents->ClassType() == PPTN_INDIRECTREF){
        return 1;
    }
    return 0;
}

//
// public methods
//
//////////////////////////////////////////////////////////////////////
PPRect PPPage::getMediaBox()
{
    return rectForKey("MediaBox");
}

PPRect PPPage::getCropBox()
{
    return rectForKey("CropBox");
}

PPRect PPPage::getBleedBox()
{
    return rectForKey("BleedBox");
}

PPRect PPPage::getTrimBox()
{
    return rectForKey("TrimBox");
}

PPRect PPPage::getArtBox()
{
    return rectForKey("ArtBox");
}

void PPPage::SetMediaBox(PPRect rect)
{
	setRectForKey(rect, "MediaBox");
}
void PPPage::SetCropBox(PPRect rect)
{
	setRectForKey(rect, "CropBox");
}

void PPPage::SetBleedBox(PPRect rect)
{
	setRectForKey(rect, "BleedBox");
}
void PPPage::SetTrimBox(PPRect rect)
{
	setRectForKey(rect, "TrimBox");
}

void PPPage::SetArtBox(PPRect rect)
{
	setRectForKey(rect, "ArtBox");
}


float PPPage::rotate()
{
    return floatValueForKey("Rotate");
}

void PPPage::appendRectXmlString(ostringstream &ostr, string keyname, int level)
{
    if (hasValueWithKey(keyname)) {
        PPRect rect = rectForKey(keyname);
        ostr << PPTabStr(level) << "<" << keyname <<" X='" << rect._origin._x
        << "' Y='" <<  rect._origin._x
        << "' Width='" << rect._size._width
        << "' Height='" << rect._size._height << "'/>\xa";
    }
    
}

string PPPage::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Page>\xa";
    appendRectXmlString(ostr, "MediaBox", level+1);
    appendRectXmlString(ostr, "CropBox", level+1);
    appendRectXmlString(ostr, "BleedBox", level+1);
    appendRectXmlString(ostr, "TrimBox", level+1);
    appendRectXmlString(ostr, "ArtBox", level+1);
    if (hasValueWithKey("Rotate")) {
        ostr << PPTabStr(level+1) << "<Rotate angle='" << rotate() << "'/>\xa";
    }
    ostr << PPTabStr(level+1) << "<Contents>\xa";
    size_t icnt = _commands.size();
    for (size_t i=0; i<icnt; i++) {
        PPToken *token = _commands.at(i);
        ostr << token->XMLString(level + 2);
    }
    ostr << PPTabStr(level+1) << "</Contents>\xa";
    ostr << PPTabStr(level) << "</Page>\xa";
    
    retstr = ostr.str();
    return retstr;
}

string PPPage::elementXmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Page>\xa";
    size_t i, icnt = numberOfElements();
    for (i= 0; i<icnt; i++) {
        PPElement *element = elementAtIndex(i);
        ostr << element->XMLString(level+1);
    }
    ostr << PPTabStr(level) << "</Page>\xa";
    retstr = ostr.str();
    return retstr;
}


PPMatrix *PPPage::GetDefaultMatrix()
{
	return &_context->matrix();
}