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

class PPLayer;


/* Document의 현재 상태를 나타내는 enum 목록들 */
typedef enum {
    PPDS_None,
    PPDS_Cannot_Open,
    PPDS_Parsing_Error,
    PPDS_Opened,
    PPDS_Built_Document,
    PPDS_Built_Elements
} PPDocumentState;

class PPParser;
class PPTTrailer;
class PPTXRef;

class PPDocument : public PPParserSource { // PPParserSource 는 virtual class로 PPParser.h에 정의 되어 있음
    
public:  //protected:
	int										_docID; // 여러개의 도큐먼트가 열렸을 때 각 도큐먼트들을 구분하기위한 ID
    int										_objNumber;  // 마지막에 사용된 오브젝 넘버. 오브젝이 생길 때마다 1씩 증가하며 번호가 할당된다.
	int										_xobjNumber; 
    PPDocumentState							_state;

    ifstream								_file; // PDF 도큐먼트 파일
	streampos								_foffset;  // for eof() bug in MS VS 2010
	streampos								_fsize;  // for eof() bug in MS VS 2010
    
    PPParser								_parser; // PDF 파일의 내용을 파싱해서 _tokens에 기본 객체들을 저장한다.
    
    vector <PPPage *>						_pages;

	// _parser에 의해 채워지는 데이터들
	//////////////////////////////////////////////////////

	// PDF+ 의 핵심 자료구조 
	//////////////////////////////////////////////////////
    vector <PPToken *> _tokens; // Core Raw Data of PDF+

	//////////////////////////////////////////////////////
	// Parsing Results
    unsigned int							_last_obj_idx;  //  사용되어진 오브젝 넘버들 중 가징 큰 수

	// 파싱하면서 추출되어진 토큰 객체들을 용도별로 모아둠
    map <int, PPTIndirectObj *>				_objDict; // 오브젝 넘버를 키값으로 PPTIndirectObj 를 가져올 수 있는 hash map
    map <unsigned long long, PPToken *>		_filePtDict; // IndirectObj, Trailer, XRef... 파일 위치를 키로 하는 토큰 객체 리스트
    vector <PPTIndirectRef *>				_ref_list; // 파싱후 IndirectRef들만 모아놓은 리스트
	vector <PPToken *>						_stream_list; // 파싱후 스트림들만 모아놓은 리스트
	//////////////////////////////////////////////////////

	// 파서 밖에서 _tokens 분석을 통해 알아낸 객체들
    PPTTrailer *							_trailer; // 메인 트레일러
    PPTXRef *								_xref; // 

	// OCG : Optional Contents Group
	PPTDictionary *							_OCProperties;
	PPTArray *								_layerOrders;
	PPTArray *								_OCGs;
	PPTArray *								_layersOn;
    
    // Metadata
    PPTString *								_author;
    PPTString *								_creator;
    PPTString *								_creationDate;
    PPTString *								_modDate;
    PPTString *								_keywords;
    PPTString *								_producer;
    PPTString *								_subject;
    PPTString *								_title;
    
    // Root
    PPTName *								_version;
    PPTName *								_pageLayout;
    PPTName *								_pageMode;
    
    map <int, PPToken *>					_fonts;  // PPToken => PPTIndirectObj
    map <int, PPToken *>					_xobjects; // images : PPToken => PPTIndirectObj
	map <int, PPTIndirectObj *>				_srcIndirectObjs;
	map <string, PPTIndirectObj *>			_images;

private:
	void									setPageCount(int cnt);

public:
	void									makePageWith(PPTDictionary *page_dict);
    void									CollectPages(PPTDictionary *pages);
	void									WriteLoadedPages();
    string									xobjectsXMLString(int level);
    string									fontsXMLString(int level);
    

	PPToken *								ResourceForExtObjNum(int num);
// 	PPToken *								ResourceForKey(int key);
//	PPTIndirectObj *						AddResource(PPToken *rcs, string type, string key);
	PPTIndirectObj *						AddResource(PPToken *rcs,  int num) ;

	// return copied resource.
	PPTIndirectObj *						WriteResource(PPToken *rsc, int obj_num);
public:
    
											PPDocument();
											PPDocument(string filepath);
    										~PPDocument();
    int 									buildDocument();
    int 									buildElements();

	int 									PreBuildPDF();
	int 									PostBuildPDF();

	bool 									open(string filepath);
    bool 									isOpened();
    bool 									isBuiltDocument();
    bool 									isBuiltElements();
    
    string 									XMLString(int level);
    string 									elementXmlString(int level);

    string 									rawDataXML();
    string 									documentXML();
    string 									elementsXML();

    inline vector <PPToken *> &				tokenList() {return _tokens;}
    void 									saveXObjectsToFolder(const char *folder); // Currently just Images
    void 									saveFontsToFolder(const char *folder);
    PPToken *								xobjectForKey(int key);
    
    void 									resortObjNum();
    int 									write(char *out_path);
	int 									Write(char *out_path) {return write(out_path);}
	int 									Save(char *out_path) {return write(out_path);}

    // overriding methods of PPParserSource
    bool 									canParseString(string str);
    PPToken *								parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void 									get(char &ch);
    bool 									eof();
    size_t 									tellg();
    void 									seekg(size_t pos);
    void 									read(char *buf, size_t size);
    void 									getline(char *buf, size_t size);
    unsigned long long						writeXRefs(std::ostream &os);

	PPPage *								newPage(PPRect &rect);

	PPPage *								PageCreate(PPRect &rect){return newPage(rect);}
	PPPage *								AddNewPage(PPRect &rect){return newPage(rect);}
	int										numberOfPages() {return _pages.size();}
	size_t									GetPageCount() {return _pages.size();}
	PPPage *								pageAtIndex(int idx){return _pages.at(idx);}
	PPPage *								GetPage(int page_no) {return _pages.at(page_no-1);}
	int										NewObjNum();
	PPTName *								AddFormObject(PPPage *page);  // convert PPPage to PPFormBase and add to document

	// Utils
	PPTIndirectObj *						SetRefTokenForKey(PPTDictionary *dict, PPToken *token, string key);
	PPTDictionary *							RootDict();

	// Page Handling
	PPTDictionary *							PagesDictionary();
	PPTArray *								PageArray();
	void									AddPage(PPPage *page);

	void									PushObj(PPTIndirectObj *obj, int obj_num);
	void									PushObj(PPTIndirectObj *obj);

	// Layer(OCG) Related Methods
	int										NumberOfLayers();
	PPTDictionary *							LayerInfoAtIndex(int idx);
	PPTIndirectObj *						LayerObjAtIndex(int idx);
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

	void									ApplyTokenToList(PPToken *token);

	PPTIndirectObj *						ImageFromPath(string path);

	void									RemoveRelatedObjects(PPTIndirectRef *ref);

	// Query Parsing Results
    map <int, PPTIndirectObj *> &			ObjectsDictionary(); // return _objDict
	PPToken *								ObjectForNumber(int num); // _objDict를 이용한 함수.
    PPToken *								ObjectAtFilePosition(unsigned long long pos);//_filePtDict를 이용한 함수
	void									DecodeStreams(vector<PPToken *> &token_list);


};

#endif /* defined(__PDFPlusLib__PPDocument__) */
