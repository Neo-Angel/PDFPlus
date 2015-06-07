//
//  PPDocument.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPDocument__
#define __PDFPlusLib__PPDocument__

#include <iostream>
#include <fstream>
#include <vector>

#include "PPBase.h"
#include "PPPage.h"
#include "PPParser.h"

using namespace std;

class PPPage;

typedef enum {
    PPDS_None,
    PPDS_Cannot_Open,
    PPDS_Parsing_Error,
    PPDS_Opened,
    PPDS_Built_Document,
    PPDS_Built_Elements
} PPDocumentState;

class PPParser;

class PPDocument : PPParserSource {
    
public:  //protected:
    int _objNumber;
	int _xobjNumber;

    ifstream _file;
	streampos _foffset;  // for eof() bug in MS VS 2010
	streampos _fsize;  // for eof() bug in MS VS 2010
    
    PPParser _parser;
    
    vector <PPPage *> _pages;
    vector <PPToken *> _tokens;
    PPDocumentState _state;
    PPTTrailer *_trailer;
    PPTXRef *_xref;
    
    // Info
    PPTString *_author;
    PPTString *_creator;
    PPTString *_creationDate;
    PPTString *_modDate;
    PPTString *_keywords;
    PPTString *_producer;
    PPTString *_subject;
    PPTString *_title;
    
    // Root
    PPTName *_version;
    PPTName *_pageLayout;
    PPTName *_pageMode;
    

    map <int, PPToken *> _fonts;  // PPToken => PPTIndirectObj
    map <int, PPToken *> _xobjects; // images : PPToken => PPTIndirectObj

public:
	void readPage(PPTDictionary *page_dict);
    void loadPages(PPTDictionary *pages);
	void WritePages(PPTArray *page_list);
    string xobjectsXMLString(int level);
    string fontsXMLString(int level);
    
    
public:
    
	PPDocument();
    PPDocument(string filepath);
    ~PPDocument();
    int buildDocument();
    int buildElements();

	int buildPDF();

	bool open(string filepath);
    bool isOpened();
    bool isBuiltDocument();
    bool isBuiltElements();
    
    string xmlString(int level);
    string elementXmlString(int level);

    string rawDataXML();
    string documentXML();
    string elementsXML();

    inline vector <PPToken *> &tokenList() {return _tokens;}
    void saveXObjectsToFolder(const char *folder); // Currently just Images
    void saveFontsToFolder(const char *folder);
    PPToken *xobjectForKey(int key);
    
    void resortObjNum();
    int write(char *out_path);
	int Write(char *out_path) {return write(out_path);}
	int Save(char *out_path) {return write(out_path);}

    // overriding methods of PPParserSource
    bool canParseString(string str);
    PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void get(char &ch);
    bool eof();
    size_t tellg();
    void seekg(size_t pos);
    void read(char *buf, size_t size);
    void getline(char *buf, size_t size);
    unsigned long long writeXRefs(std::ostream &os);

	PPPage *newPage(PPRect &rect);

	PPPage *PageCreate(PPRect &rect){return newPage(rect);}
	PPPage *AddNewPage(PPRect &rect){return newPage(rect);}
	int numberOfPages() {return _pages.size();}
	size_t GetPageCount() {return _pages.size();}
	PPPage *pageAtIndex(int idx){return _pages.at(idx);}
	PPPage *GetPage(int page_no) {return _pages.at(page_no-1);}
	int NewObjNum();
	PPTName *AddFormObject(PPPage *page);  // convert PPPage to PPFormBase and add to document

	// Utils
	PPTIndirectObj *SetRefTokenForKey(PPTDictionary *dict, PPToken *token, string key);
};

#endif /* defined(__PDFPlusLib__PPDocument__) */
