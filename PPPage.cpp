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
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"

string tapStr(int cnt);


PPPage::PPPage(PPDocument *doc)  // this invokes PPFormBase() (default)constructor.
{
    _document = doc;
	_context = new PPContext;
}

PPPage::PPPage(PPPage *page)
{
	_document = page->_document;
	_pageDict = (PPTDictionary *)page->_pageDict->Copy();
}

void PPPage::loadDictionary(PPTDictionary *page_dict)
{
    _pageDict = page_dict;
	_context->ptMatrix()->rotate(rotate());

//    PPTIndirectObj *rcs_indir = (PPTIndirectObj *)_pageDict->indirectObjectForKey("Resources");
//    _resourceDict = rcs_indir->firstDictionary();
    _resourceDict = (PPTDictionary *)_pageDict->valueObjectForKey("Resources");
    
    PPTDictionary *font_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("Font");
    if (font_dict) {
        map <string, PPToken *> &dict = font_dict->_dict;
       
        map <string, PPToken *> ::iterator it_token_objs;
        for(it_token_objs = dict.begin(); it_token_objs != dict.end(); it_token_objs++) {
//            string name = it_token_objs->first;
            PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
            int obj_num = indir_ref->_objNum;
            PPTIndirectObj *font_obj = (PPTIndirectObj *)_document->_fonts[obj_num];
            if (font_obj == NULL) {
//                PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
                font_obj = indir_ref->targetObject();
                _document->_fonts[obj_num] = font_obj;
//                PPTDictionary *obj_dict = font_obj->firstDictionary();
                
            }
        }
    }

    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("XObject");
    if (xobject_dict) {
        map <string, PPToken *> &dict = xobject_dict->_dict;
        
        map <string, PPToken *> ::iterator it_token_objs;
        for(it_token_objs = dict.begin(); it_token_objs != dict.end(); it_token_objs++) {
//            string name = it_token_objs->first;
            PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
            int obj_num = indir_ref->_objNum;
            PPTIndirectObj *indir_obj = (PPTIndirectObj *)_document->_xobjects[obj_num];
            if (indir_obj == NULL) {
//                PPTIndirectRef *indir_ref = (PPTIndirectRef *)it_token_objs->second;
                indir_obj = indir_ref->targetObject();
                _document->_xobjects[obj_num] = indir_obj;
                indir_obj = (PPTIndirectObj *)_document->_xobjects[obj_num];
//                PPTDictionary *obj_dict = indir_obj->firstDictionary();
            }
        }
    }
    

    size_t icnt = contentsCount();
    for (size_t i=0; i<icnt; i++) {
        PPTStream *stream = contentAt(i);
        if (!stream->_decodeFailed) {
            _graphicParser.parseStream(*stream);
        }
    }
}

void PPPage::WriteDictionary(PPTDictionary *page_dict)
{
	PPTDictionary *rcs_dict = new PPTDictionary(&_document->_parser);
	_document->SetRefTokenForKey(page_dict, rcs_dict, PPKN_RESOURCES);
	PPTDictionary *stream_dict = new PPTDictionary(&_document->_parser);
	PPTIndirectObj *stream_obj = _document->SetRefTokenForKey(stream_dict, rcs_dict, PPKN_CONTENTS);
	PPTStream *contents = new PPTStream(&_document->_parser);
	stream_obj->_array.push_back(contents);
}

PPRect PPPage::rectForKey(string key)
{
    PPRect rect;
    PPTArray *num_list = (PPTArray *)_pageDict->objectForKey(key);
    if (num_list) {
        return rectFromArray(num_list);
    }
    return rect;
}

void PPPage::setRectForKey(PPRect rect, string key)
{
	PPTArray *num_list = new PPTArray(&_document->_parser);
	setRectToArray(rect, num_list);
	_pageDict->setTokenAndKey(num_list, key);
}

int PPPage::intValueForKey(string key)
{
    int ret = 0;
    PPTNumber *num = (PPTNumber *)_pageDict->objectForKey(key);
    if (num) {
        return num->intValue();
    }
    return ret;
}

float PPPage::floatValueForKey(string key)
{
    float ret = 0;
    PPTNumber *num = (PPTNumber *)_pageDict->objectForKey(key);
    if (num) {
        return num->floatValue();
    }
    return ret;
}

bool PPPage::hasValueWithKey(string key)
{
    if(_pageDict->objectForKey(key))
        return true;
    return false;
}

PPTStream *PPPage::contentAt(size_t i)
{
    PPTStream *ret_stream = NULL;
    
    PPToken *contents = _pageDict->objectForKey(PPKN_CONTENTS);
    if (contents->classType() == PPTN_ARRAY) {
        PPTArray *array = (PPTArray *)contents;
        if (array->size() > i) {
            PPTIndirectRef *ref = (PPTIndirectRef *)array->objectAtIndex(i);
            PPTIndirectObj *indirect = ref->targetObject();
            ret_stream = indirect->stream();
        }
    }
    else if (contents->classType() == PPTN_INDIRECTREF){
        PPTIndirectObj *indirect = ((PPTIndirectRef *)contents)->targetObject();
        ret_stream = indirect->stream();
    }
    return ret_stream;
}

size_t PPPage::contentsCount()
{
    PPToken *contents = _pageDict->objectForKey(PPKN_CONTENTS);
    if (contents->classType() == PPTN_ARRAY) {
        PPTArray *array = (PPTArray *)contents;
        return array->size();
    }
    else if (contents->classType() == PPTN_INDIRECTREF){
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

}
void PPPage::SetCropBox(PPRect rect)
{

}

void PPPage::SetBleedBox(PPRect rect)
{

}
void PPPage::SetTrimBox(PPRect rect)
{

}

void PPPage::SetArtBox(PPRect rect)
{

}


float PPPage::rotate()
{
    return floatValueForKey("Rotate");
}

void PPPage::appendRectXmlString(ostringstream &ostr, string keyname, int level)
{
    if (hasValueWithKey(keyname)) {
        PPRect rect = rectForKey(keyname);
        ostr << tapStr(level) << "<" << keyname <<" X='" << rect._origin._x
        << "' Y='" <<  rect._origin._x
        << "' Width='" << rect._size._width
        << "' Height='" << rect._size._height << "'/>\xa";
    }
    
}

string PPPage::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Page>\xa";
    appendRectXmlString(ostr, "MediaBox", level+1);
    appendRectXmlString(ostr, "CropBox", level+1);
    appendRectXmlString(ostr, "BleedBox", level+1);
    appendRectXmlString(ostr, "TrimBox", level+1);
    appendRectXmlString(ostr, "ArtBox", level+1);
    if (hasValueWithKey("Rotate")) {
        ostr << tapStr(level+1) << "<Rotate angle='" << rotate() << "'/>\xa";
    }
    ostr << tapStr(level+1) << "<Contents>\xa";
    size_t icnt = _commands.size();
    for (size_t i=0; i<icnt; i++) {
        PPToken *token = _commands.at(i);
        ostr << token->xmlString(level + 2);
    }
    ostr << tapStr(level+1) << "</Contents>\xa";
    ostr << tapStr(level) << "</Page>\xa";
    
    retstr = ostr.str();
    return retstr;
}

string PPPage::elementXmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Page>\xa";
    size_t i, icnt = _elements.size();
    for (i= 0; i<icnt; i++) {
        PPElement *element = _elements.at(i);
        ostr << element->xmlString(level+1);
    }
    ostr << tapStr(level) << "</Page>\xa";
    retstr = ostr.str();
    return retstr;
}


PPMatrix *PPPage::GetDefaultMatrix()
{
	return &_context->matrix();
}