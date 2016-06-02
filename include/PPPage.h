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
    
    bool hasValueWithKey(string key);
    void appendRectXmlString(ostringstream &ostr, string keyname, int level);
//	void StoreResources();

public:
	PPPage(PPPage *page);
    PPPage(PPDocument *doc);
    void LoadDictionary(PPTDictionary *page_dict);
	void WriteDictionary(PPTDictionary *page_dict);

	// 페이지에 사용되어지는 리소스들의 정보.
	// contents 내에서 사용되어지는 리소스 키는 
	PPTDictionary *ResourcesDict(); // 이 리소스 사전을 통해 원래 리소스로 접근할 수 있다.
	PPTDictionary *ResourceDictForType(string str);
	void AddResource(PPToken *res, char *type, char *key);
    
	// 하나의 페이지에는 다수의 contents(stream)가 존재할 수 있다.
    size_t ContentsCount();
    PPTStream *ContentAt(size_t i);
	void BuildContents(); // contents를 스트림으로 변환한다.

	// Getting Methods
    int IntValueForKey(string key);
	float FloatValueForKey(string key);
    PPRect RectForKey(string key);
	PPRect MediaBox();
	PPRect CropBox();
	PPRect BleedBox();
	PPRect TrimBox();
	PPRect ArtBox();
    float Rotate();
	PPMatrix *DefaultMatrix();
    
	// Setting Methods
    int SetIntValueForKey(string key);
	void SetFloatValueForKey(float value, string key);
	void SetRectForKey(PPRect rect, string key);
	void SetMediaBox(PPRect rect);
	void SetCropBox(PPRect rect);
	void SetBleedBox(PPRect rect);
	void SetTrimBox(PPRect rect);
	void SetArtBox(PPRect rect);
	void SetRotate(float value);

    string XMLString(int level);
    string ElementXmlString(int level);

};

#endif /* defined(__PDFPlusLib__PPPage__) */
