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

class PPPage : public PPFormBase {
public:

/*	PageDict Example
		<Dict>
			<Key>Contents</Key>
			<Ref ObjID='35' Gen='0'/>
			<Key>MediaBox</Key>
			<Array>
				<Number>0</Number>
				<Number>0</Number>
				<Number>6803.15</Number>
				<Number>3401.57</Number>
			</Array>
			<Key>Parent</Key>
			<Ref ObjID='56' Gen='0'/>
			<Key>Resources</Key>
			<Dict>
				<Key>ProcSet</Key>
				<Array>
					<Name>PDF</Name>
					<Name>Text</Name>
					<Name>ImageB</Name>
					<Name>ImageC</Name>
					<Name>ImageI</Name>
				</Array>
				<Key>XObject</Key>
				<Ref ObjID='54' Gen='0'/>
			</Dict>
			<Key>Type</Key>
			<Name>Page</Name>
		</Dict>
*/
    PPTDictionary *_pageDict;

protected:
    
    PPRect rectForKey(string key);
    int intValueForKey(string key);
	float floatValueForKey(string key);
    bool hasValueWithKey(string key);
    void appendRectXmlString(ostringstream &ostr, string keyname, int level);
    size_t contentsCount();
    PPTStream *contentAt(size_t i);
	void setRectForKey(PPRect rect, string key);

public:
	PPPage(PPPage *page);
    PPPage(PPDocument *doc);
    void loadDictionary(PPTDictionary *page_dict);
	void WriteDictionary(PPTDictionary *page_dict);
    
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
    
    string xmlString(int level);
    string elementXmlString(int level);
	PPMatrix *GetDefaultMatrix();
};

#endif /* defined(__PDFPlusLib__PPPage__) */
