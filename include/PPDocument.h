﻿//
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

class PPLayer;
class PPImage;


/* Document의 현재 상태를 나타내는 enum 목록들 */
typedef enum {
    PPDS_None,
    PPDS_Cannot_Open,
    PPDS_Parsing_Error,
    PPDS_Opened,
    PPDS_Built_Document,
    PPDS_Built_Elements
} PPDocumentState;

// Page Label Contant
typedef enum {
    PPPL_Name,
    PPPL_String,
    PPPL_BaseNumber
} PPPageLabelType;

typedef enum {
    PPPLN_DecimalArabic,
    PPPLN_UppercaseRoman,
    PPPLN_LowercaseRoman,
	PPPLN_UppercaseLetters,
	PPPLN_LowercaseLetters
} PPPageLabelName;


class PPParser;
class PPTTrailer;
class PPTXRef;
class PPPage;
class PPDocument;

class PPDocument : public PPParserSource { /* PPParserSource 는 virtual class로 PPParser.h에 정의 되어 있음*/
    
public:  //protected:
	int										_docID; /* 여러개의 도큐먼트가 열렸을 때 각 도큐먼트들을 구분하기위한 ID */
    unsigned int										_objNumber;  /* 마지막에 사용된 오브젝 넘버. 오브젝이 생길 때마다 1씩 증가하며 번호가 할당된다.*/
	unsigned int										_xobjNumber; 
//    unsigned int							_last_obj_idx;  /*  사용되어진 오브젝 넘버들 중 가징 큰 수 */

    PPDocumentState							_state;

    ifstream								_file; /* PDF 도큐먼트 파일*/
	streampos								_foffset;  /* for eof() bug in MS VS 2010*/
	streampos								_fsize;  /* for eof() bug in MS VS 2010 */
    
    PPParser								_parser; /* PDF 파일의 내용을 파싱해서 _tokens에 기본 객체들을 저장한다.*/
    
	/* _parser에 의해 채워지는 데이터들  */
	//////////////////////////////////////////////////////

	/* 핵심 자료구조 */
	//////////////////////////////////////////////////////
    vector <PPToken *> _tokens; // Core Raw Data of PDF+

	/* Parsing Results */
	//////////////////////////////////////////////////////

    vector <PPPage *>						_pages;

	// 파싱하면서 추출되어진 토큰 객체들을 용도별로 모아둠
    map <int, PPTIndirectObj *>				_objDict; // 오브젝 넘버를 키값으로 PPTIndirectObj 를 가져올 수 있는 hash map
    map <unsigned long long, PPToken *>		_filePtDict; // IndirectObj, Trailer, XRef... 파일 위치를 키로 하는 토큰 객체 리스트
    vector <PPTIndirectRef *>				_ref_list; // 파싱후 IndirectRef들만 모아놓은 리스트
	vector <PPToken *>						_stream_list; // 파싱후 스트림들만 모아놓은 리스트

	// 리소스 모음
    map <int, PPToken *>					_fonts;  // PPToken => PPTIndirectObj
    map <int, PPToken *>					_xobjects; // images : PPToken => PPTIndirectObj
	map <int, PPTIndirectObj *>				_srcIndirectObjs;
	map <string, PPImage *>					_images;
	map <string, PPTIndirectObj *>			_imageObjects;

	// OCG(Layer)처리를 위한...
											
	map <string, PPTIndirectObj *>			_OCGObjects;  // PDF Merge 시 Layer 처리용. 같은 이름이
														  // 있을 경우 리네임하기 위해서 필요.

	vector <int>					_merged_docs;

	//////////////////////////////////////////////////////

	// 파서 밖에서 _tokens 분석을 통해 알아낸 객체들
	// 특별한 용도의 토큰들
    PPTTrailer *							_trailer; // 메인 트레일러
    PPTXRef *								_xref; // 

	// OCG : Optional Contents Group
	PPTDictionary *							_OCProperties;
	PPTArray *								_layerOrders;
	PPTArray *								_OCGs;
	PPTArray *								_layersOn;
	PPTArray *								_layersOff;

	// Page Label
	PPTArray *								_pageLabels;
    
    // Metadata
    PPTString *								_author;
    PPTString *								_creator;
    PPTString *								_creationDate;
    PPTString *								_modDate;
    PPTString *								_keywords;
    PPTString *								_producer;
    PPTString *								_subject;
    PPTString *								_title;
    
    // Metadata(root)
    PPTName *								_version;
    PPTName *								_pageLayout;
    PPTName *								_pageMode;
    
protected:
	// Internal Methods
	bool 									open(string filepath);
    
    int 									buildDocument();
    int 									buildElements();

	int 									preBuildPDF();
	int 									postBuildPDF();
	void									decodeStreams(vector<PPToken *> &token_list);
    void 									doReorderingObjectNumbers();
	void									makePageWith(PPTDictionary *page_dict);
    void									collectPages(PPTDictionary *pages);
	void									writeLoadedPages();
	void									setPageCount(uint cnt);

public: 
	// Constructors
	////////////////////////////////////////////////////////////////////////////////////////////////
											PPDocument();
											PPDocument(string filepath);
	// Destructor
	////////////////////////////////////////////////////////////////////////////////////////////////
    										~PPDocument();
	// Save PDF
	////////////////////////////////////////////////////////////////////////////////////////////////
	int 									Save(string out_path);

	// Page Handling
	////////////////////////////////////////////////////////////////////////////////////////////////
	PPPage *								NewPage(PPRect &rect);
	PPPage *								PageCreate(PPRect &rect){return NewPage(rect);}
	PPPage *								AddNewPage(PPRect &rect){return NewPage(rect);}
	PPPage *								AddPage(PPPage *page);
	void									AddInternalPage(PPPage *page);
	PPPage *								AddExternalPage(PPPage *ext_page);
	void									PushObj(PPTIndirectObj *obj, int obj_num);
	void									PushObj(PPTIndirectObj *obj);
	void									RemoveObj(PPTIndirectObj *obj);

	// Utils
	////////////////////////////////////////////////////////////////////////////////////////////////
	int										NewObjNum();
	PPTName *								AddFormObject(PPPage *page);  // convert PPPage to PPFormBase and add to document
    inline vector <PPToken *> &				TokenList() {return _tokens;}
    PPToken *								XObjectForKey(int key);
	PPTIndirectObj *						SetRefTokenForKey(PPTDictionary *dict, PPToken *token, string key);

	// Layer(OCG) Related Methods
	////////////////////////////////////////////////////////////////////////////////////////////////
	int										NumberOfLayers();
	PPTDictionary *							LayerInfoAtIndex(int idx);
	PPTIndirectObj *						LayerObjAtIndex(int idx);
	string									LayerNameAtIndex(int idx);
	PPTDictionary *							LayerDictForName(string name);
	PPTIndirectObj *						LayerObjForName(string name);
	PPLayer *								NewLayerForName(string name);
	bool 									AddLayer(string name);
	bool 									RenameLayer(string org_name, string new_name);
	void 									ReorderLayer(int from_idx, int to_idx);
	void 									MergeLayer(string layer1, string layer2);
	void 									MergeLayer(PPTDictionary *layer_dict1, PPTDictionary *layer_dict2);
	void 									WriteOCProperties(PPTDictionary *properties);
	void 									BuildOCProperties();
	bool									IsMergedDoc(PPDocument *doc);
	void									ImportOCGsFrom(PPDocument *doc);
	void									CopyLayerToDocument(string layer_name, PPDocument *doc);
	void									CopyAndAddLayerToDocument(string layer_name, uint idx, uint req_cnt, PPDocument *doc);

	// Query methods
	////////////////////////////////////////////////////////////////////////////////////////////////
    bool 									IsOpened();
    bool 									IsBuiltDocument();
    bool 									IsBuiltElements();
    map <int, PPTIndirectObj *> &			ObjectsDictionary(); // return _objDict
	PPToken *								ObjectForNumber(int num); // _objDict를 이용한 함수.
	int										NextObjectNumber();
    PPToken *								ObjectAtFilePosition(unsigned long long pos);//_filePtDict를 이용한 함수
	size_t									NumberOfPages() {return _pages.size();}
	size_t									GetPageCount() {return _pages.size();}
	PPPage *								PageAtIndex(int idx){return _pages.at(idx);}
	PPPage *								GetPage(int page_no);// {return _pages.at(page_no-1);}
	PPTDictionary *							PagesDictionary();
	PPTArray *								PageArray();
	PPTDictionary *							RootDict();

    // overriding methods of PPParserSource
	////////////////////////////////////////////////////////////////////////////////////////////////
    bool 									canParseString(string str);
    PPToken *								parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void 									get(char &ch);
    bool 									eof();
    size_t 									tellg();
    void 									seekg(size_t pos);
    void 									read(char *buf, size_t size);
    void 									getline(char *buf, size_t size);
    unsigned long long						writeXRefs(std::ostream &os);
	////////////////////////////////////////////////////////////////////////////////////////////////

	// XML Output
	////////////////////////////////////////////////////////////////////////////////////////////////
    string 									XMLString(int level);
    string 									ElementXmlString(int level);
    string 									RawDataXML();
    string 									DocumentXML();
    string 									ElementsXML();
    string									XObjectsXMLString(int level);
    string									fontsXMLString(int level);
    

	// Resource Related Methods
	PPToken *								ResourceForExtObjNum(int num);
	PPTIndirectObj *						AddResource(PPToken *rcs,  int num) ;
	PPTIndirectObj *						WriteResource(PPToken *rsc, int obj_num); // return copied resource.
	PPTIndirectObj *						MoveObjFrom(PPTIndirectObj *src_obj, PPDocument *src_doc);

	//  Image Related Methods   
	PPTIndirectObj *						ImageObjFromPath(string path);
	PPImage *								ImageFromPath(string path);
	PPTIndirectObj *						AddImage(PPImage *image);

	// Utility Methods
	void									ApplyTokenToList(PPToken *token);
	void									RemoveRelatedObjects(PPTIndirectRef *ref);
    void 									SaveXObjectsToFolder(const char *folder); // Currently just Images
    void 									SaveFontsToFolder(const char *folder);

	// Add PageLabel
	void									CreatePageLabel();
	PPTArray *								PageLabel();
	void									ClearPageLabel();
	void									AddPageLabel(uint start, PPPageLabelType type, PPToken *token);
	void									AddPageLabel(uint start, PPPageLabelName name);
	void									AddPageLabel(uint start, string string);
	void									AddPageLabel(uint start, uint num);

};

#endif /* defined(__PDFPlusLib__PPDocument__) */
