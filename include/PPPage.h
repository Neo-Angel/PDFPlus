//
//  PPPage.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPPage__
#define __PDFPlusLib__PPPage__

#include <iostream>
#include <vector>

#include <PPBase.h>
#include <PPToken.h>

#include <PPElement.h>
#include <PPFormBase.h>

class PPDocument;
class PPRect;
class PPMatrix;


/* PPPage 클래스는 PPFormBase 의 서브클레스로서 기본적으로 Form의 특성을 갖는다.*/
class PPPage : public PPFormBase {

protected:
    
    PPRect rectForKey(string key);
    int intValueForKey(string key);
	float floatValueForKey(string key);
    bool hasValueWithKey(string key);
    void appendRectXmlString(ostringstream &ostr, string keyname, int level);
    size_t contentsCount();
    PPTStream *contentAt(size_t i);
	void setRectForKey(PPRect rect, string key);
	void StoreResources();

public:
	PPPage(PPPage *page);
    PPPage(PPDocument *doc);
    void LoadDictionary(PPTDictionary *page_dict);
	void WriteDictionary(PPTDictionary *page_dict);

	PPTDictionary *ResourcesDict();
	PPTDictionary *ResourceDictForType(string str);
	void AddResource(PPToken *res, char *type, char *key);
    
	PPRect getMediaBox();	PPRect GetMediaBox() {return getMediaBox();}
	PPRect getCropBox();	PPRect GetCropBox() {return getCropBox();}
	PPRect getBleedBox();	PPRect GetBleedBox() {return getBleedBox();}
	PPRect getTrimBox();	PPRect GetTrimBox() {return getTrimBox();}
	PPRect getArtBox();		PPRect GetArtBox() {return getArtBox();}

	void SetMediaBox(PPRect rect);
	void SetCropBox(PPRect rect);
	void SetBleedBox(PPRect rect);
	void SetTrimBox(PPRect rect);
	void SetArtBox(PPRect rect);

    float rotate();
    
    string XMLString(int level);
    string elementXmlString(int level);
	PPMatrix *GetDefaultMatrix();
	void BuildPDF();

};

#endif /* defined(__PDFPlusLib__PPPage__) */
