//
//  PPDocument.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//



//#include "StdAfx.h"


#include <fstream>
#include <sstream>

#include "PPDocument.h"
#include "PPToken.h"
#include "PPTStream.h"
#include "PPTNumber.h"
#include "PPTString.h"
#include "PPTName.h"
#include "PPTArray.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"
#include "PPTXRef.h"
#include "PPTTrailer.h"
#include "PPTComment.h"
#include "PPPage.h"
#include "PPLayer.h"


static unsigned int DOC_COUNT = 0;

///////////////////////////////////////////////////////////////////
// Constructors / Destructor
///////////////////////////////////////////////////////////////////
PPDocument::PPDocument(string filepath)
{
	// 도큐먼트가 오픈 될 때마다 1씩 증가시켜서 ID를 만듦
	_docID = ++DOC_COUNT;

	// Initialize member variables
	_objNumber = 0;
	_xobjNumber = 0;
    _version = NULL;
    _pageLayout = NULL;
    _pageMode = NULL;
    _trailer = NULL;
    _xref = NULL;

    _author =  NULL;
    _creator =  NULL;
    _creationDate = NULL;
    _modDate = NULL;
    _keywords = NULL;
    _producer = NULL;
    _subject = NULL;
    _title =  NULL;
	_state = PPDS_None;
	_OCProperties = NULL;
	_parser._document = this;
	_layerOrders = NULL;
	_OCGs = NULL;
	_layersOn = NULL;

	// open 단계에서는 저수준의 오브젝트(Token)들을 읽어들인다.
	// 주로 PPToken 의 서브 클래스 들이다.
	open(filepath);

	// 읽어들인 토큰들을 가지고 도큐먼트를 구성한다.
	// 도큐먼트는 Document > Page(s) > Stream 구조가 된다. 
    buildDocument();

	// 페이지의 내용들을 element 형태로 변환한다.
	buildElements();
}

// 신규 도큐먼트(PDF)를 만들기 위해 빈 PDF 도큐먼트를 생성한다.
PPDocument::PPDocument()
{
	_docID = ++DOC_COUNT;
	_objNumber = 0;
    _version = NULL;
    _pageLayout = NULL;
    _pageMode = NULL;
    _trailer = NULL;
    _xref = NULL;

    _author =  NULL;
    _creator =  NULL;
    _creationDate = NULL;
    _modDate = NULL;
    _keywords = NULL;
    _producer = NULL;
    _subject = NULL;
    _title =  NULL;
	_state = PPDS_None;
	_OCProperties = NULL;
	_parser._document = this;
	_layerOrders = NULL;
	_OCGs = NULL;
	_layersOn = NULL;

	// 빈 도큐먼트에 기본적인(Default) 셋팅을 한다.
    preBuildPDF();
}

PPDocument::~PPDocument()
{
    _file.close();
    int i, icnt = (int)_tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(i);
        delete token;
    }
    cout << "Document destructed..." << PP_ENDL;
    
}
/////////////////////////////////////////////////////////////////
// Build Tokens
/////////////////////////////////////////////////////////////////

// 기존 PDF 를 읽어서 읽어들인 토큰들로 도큐먼트를 구성함
// 도큐먼트는 Document > Page(s) > Stream 구조가 된다. 
/////////////////////////////////////////////////////////////////
int PPDocument::buildDocument()
{
    if (IsBuiltDocument()) { // 이미 빌드가 되어 있으면 무시한다.
        return 0;
    }
    
	// first trailer를 찾고 다른 트레일러들과의 연결 고리를 찾아줌
    PPTTrailer *first_trailer = NULL;
    size_t i, icnt = _tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(icnt-i-1);
        if (token->ClassType() == PPTN_TRAILER) {
            PPTTrailer *trailer = (PPTTrailer *)token;//현재 트레일러
            PPTDictionary *trailer_dict = trailer->getDictionary();
            if (trailer_dict) {
                PPTNumber *prev_num = (PPTNumber *)trailer_dict->objectForKey("Prev");
                if (prev_num != NULL) { // 이전 트레일러가 있으면
                    PPToken *token = _filePtDict[prev_num->intValue()];
                    if (token == NULL) {
                        // error!
                        return -4;
                    }
                    else if( token->ClassType() == PPTN_TRAILER) {
                        PPTTrailer *prev_trailer = (PPTTrailer *)token;
                        prev_trailer->_next = trailer; // 이전 트레일러의 '다음'트레일러를 현재 트레일러를
                    }
                    else if( token->ClassType() == PPTN_XREF) {
                        PPTXRef *xref = (PPTXRef *)token;
                        xref->_trailerObj->_next = trailer;
                    }
                    else {
                        return -5; // Previous XRef not found.
                    }
                }
                else {
                    first_trailer = trailer;
                }
            }
        }
    }
    if (first_trailer == NULL) {
        return -1;
    }

    // 마지막 트레일러를 찾아 _trailer 에 지정함.
    PPTTrailer *last_trailer = first_trailer;
    while (last_trailer != NULL) {
        _trailer = last_trailer;
        last_trailer = last_trailer->_next;
    }
    
	//메타데이터 읽어오기
    PPTIndirectObj *info_obj = (PPTIndirectObj *)_trailer->infoObject();
    if (info_obj != NULL) {
        PPTDictionary *info_dict = info_obj->firstDictionary();
        if (info_dict != NULL) {
            _author =  (PPTString *)info_dict->valueObjectForKey("Author");
            _creator =  (PPTString *)info_dict->valueObjectForKey("Creator");
            _creationDate =  (PPTString *)info_dict->valueObjectForKey("CreationDate");
            _modDate =  (PPTString *)info_dict->valueObjectForKey("ModDate");
            _keywords =  (PPTString *)info_dict->valueObjectForKey("Keywords");
            _producer =  (PPTString *)info_dict->valueObjectForKey("Producer");
            _subject =  (PPTString *)info_dict->valueObjectForKey("Subject");
            _title =  (PPTString *)info_dict->valueObjectForKey("Title");
        }
    }
    
	// root_obj (Type:Catalog)
    PPTIndirectObj *root_obj = (PPTIndirectObj *)_trailer->rootObject();
    if (root_obj == NULL) {
        return -2; // error: Cannot find Root Object;
    }
    PPTDictionary *root_dict = root_obj->firstDictionary();
    if (root_dict == NULL) {
        return -3;
    }

    _version = (PPTName *)root_dict->valueObjectForKey("Version");
    _pageLayout = (PPTName *)root_dict->valueObjectForKey("PageLayout");
    _pageMode = (PPTName *)root_dict->valueObjectForKey("PageMode");

	// Optional Contens 관련 데이터들을 읽어옴
    _OCProperties = (PPTDictionary *)root_dict->valueObjectForKey("OCProperties");
    PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->IndirectObjectForKey("Pages");
    PPTDictionary *pages_dict = pages->firstDictionary();
	if(_OCProperties) {
		PPTDictionary *d_dict = (PPTDictionary *)_OCProperties->valueObjectForKey("D");
		if(d_dict) {
			_layerOrders = (PPTArray *)d_dict->valueObjectForKey("Order");
			_layersOn = (PPTArray *)d_dict->valueObjectForKey("ON");
		}
		_OCGs = (PPTArray *)_OCProperties->valueObjectForKey("OCGs");
	}

	// 페이지들을 읽어들임
    collectPages(pages_dict);
    _state = PPDS_Built_Document;
    return 0;
}

bool PPDocument::IsBuiltDocument()
{
    return (_state >= PPDS_Built_Document);
}


// 트리형식으로 분포된 모든 페이지들을 재귀호출 방식으로 읽어들임
void PPDocument::collectPages(PPTDictionary *pages_dict)
{
    PPTArray *page_list = (PPTArray *)pages_dict->objectForKey("Kids");
    size_t i, icnt = page_list->_array.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *ref_obj = (PPTIndirectRef *)page_list->_array.at(i);
        PPTIndirectObj *page_obj = ref_obj->targetObject();
        PPTDictionary *child_dict = page_obj->firstDictionary();
        PPTName *type = (PPTName *)child_dict->objectForKey("Type");
        if (type && *type->_name == "Page") {
            makePageWith(child_dict);
        }
        else {
			// 재귀호출
            collectPages(child_dict);
        }
    }
}


// 페이지 정보가 들어있는 딕셔너리로 페이지를 만들어 추가함.
void PPDocument::makePageWith(PPTDictionary *page_dict)
{
    PPPage *page = new PPPage(this);
    page->loadDictionary(page_dict);
    _pages.push_back(page);
}


///////////////////////////////////////////////////////////////////
// Build Elements
///////////////////////////////////////////////////////////////////


// 페이지별 드로윙 코드들을 Element 단위로 빌드한다.
// Element 는 서드파티 개발자들에게 PDF Handling을 쉽게 할 수 있도록 하는 
// Helper Class 들이다.
///////////////////////////////////////////////////////////////////
int PPDocument::buildElements()
{
	// Token 기반의 빌드가 완료되지 않았다면 Element 빌드를 할 수 없다.
    if (IsBuiltElements()) {
        return 0;
    }
   size_t i, icnt = _pages.size();
    for (i=0; i<icnt; i++) {
        PPPage *page = _pages[i];
		// 페이지 내의 드로윙 코드들을 정리해 Element 리스트로 만든다.
        page->buildElements();
    }

    _state = PPDS_Built_Elements;
    return 0;
}

bool PPDocument::IsBuiltElements()
{
    return (_state >= PPDS_Built_Elements);
}

// 
// 파싱된 스트림들 중 FlateDecode 방식들만 디코딩 함.
///////////////////////////////////////////////////////////////////
void PPDocument::decodeStreams(vector<PPToken *> &token_list)
{
	// _stream : 파싱중 발견된 스트림들만 모아놓음.	
	int i, icnt = _stream_list.size();
	for(i=0;i<icnt;i++) {
		PPTStream *stream = (PPTStream *)_stream_list[i];
		PPTDictionary *dict = stream->_dict;
		PPToken *val_obj = (PPToken *)dict->valueObjectForKey("Length");
		if (val_obj) {
            PPTNumber *len_obj = (PPTNumber *)val_obj;
            long length = len_obj->longValue();
            PPTName *filter = (PPTName *)dict->nameForKey("Filter");
			// FlateDecode 방식들만 디코딩 함.
            if (filter != NULL && *filter->_name == "FlateDecode") {
                stream->flateDecodeStream();
                PPTName *type = (PPTName *)dict->objectForKey("Type");
                if (type != NULL && *type->_name == "ObjStm") {
					// 디코딩 한 스트림의 타입이 '오브젝 스트림'이면 도큐먼트의 _parser를 이용해 파싱을 함 
					if(stream->parseObjStm(token_list, &_parser) == false) {
                        return;
                    }
                 }
			}
		}
	}

}

// 오픈시 기본 데이터(PPToken을 기반으로 하는)들을 구성한다.
//  - _tokens 를 채운다.
//  - FlateDecode Stream 들을 디코딩 한다. 그 중 ObjStm 들은 또 파싱한다.
//  - IndirectObj와 IndirectRef 들의 관계를 정리한다.
///////////////////////////////////////////////////////////////////
bool PPDocument::open(string filepath)
{
    _file.open(filepath, std::ios::binary);
	cout << "Openning path : " << filepath << PP_ENDL;
    if (!_file.is_open()) {
        _state = PPDS_Cannot_Open;
        return false;
    }
	
	// Because VC++ 2010 has bug that tellg() returns strange value. 

	// get size of file (_fsize)
    _fsize = 0;
    //_file( filePath, std::ios::binary );

    _foffset = _file.tellg();

    _file.seekg( 0, std::ios::end );
    _fsize = _file.tellg() - _foffset;

    //================================================

    _file.seekg(0L, ios::beg);
	_stream_list.clear();

	// document 자신을 ParserSource로 파라미터를 넘겨서 _parser를 통해 파싱을 하고 
	// _tokens에 자료구조(객체)들을 담아온다.
    if(!_parser.ParseSource(*this, _tokens)) {
        _state = PPDS_Parsing_Error;
        return false;
    }

	//파싱된 스트림들 중 FlateDecode 방식들만 디코딩 함
	decodeStreams(_tokens);

	// _ref_list : 파싱중 발견된 IndirectRef 들만 모은다.
    size_t i, icnt = _ref_list.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *indir_ref = _ref_list[i];
        int objnum = indir_ref->_objNum;

		// _objDict : 파싱중 발견된 IndirectObj 들만 모아놓음.
		// hash map으로 저장하여 object number 값으로 쉽게 찾을 수 있도록 함.
        PPTIndirectObj *indir_obj = _objDict[objnum]; 
        if(indir_obj)

			// indir_obj 에 자신을 참조한 ref 들을 모은다.
			// 이는 추후 indir_obj의 objNum이 바뀔경우 자신을 참조한 ref들의 objNum를 
			// 업데이트 해 주기 위해서 쓰인다.
            indir_obj->AddRefObj(indir_ref);
        else
            cout << "Cannot fount IndirectRef of " << objnum << PP_ENDL;
    }

	// 도큐먼트의 상태를 '오픈됨' 상태로
    _state = PPDS_Opened;
	return true;
}

// 도큐먼트의 상태를 알아낸다.
///////////////////////////////////////////////////////////////////
bool PPDocument::IsOpened()
{
    return (_state >= PPDS_Opened);
}

// PDF 계층 구조에서 최상위 딕셔너리를 가져옴
///////////////////////////////////////////////////////////////////
PPTDictionary *PPDocument::RootDict()
{
	PPTIndirectObj *root_obj = (PPTIndirectObj *)_trailer->rootObject();
    if (root_obj == NULL) {
        return NULL; // error: Cannot find Root Object;
    }
    PPTDictionary *root_dict = root_obj->firstDictionary();
    if (root_dict == NULL) {
        return NULL;
    }
	return root_dict;
}


PPTDictionary *PPDocument::PagesDictionary()
{
	PPTDictionary *root_dict = RootDict();

    PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->IndirectObjectForKey("Pages");
    PPTDictionary *pages_dict = pages->firstDictionary();
	return pages_dict;
}

// 페이지 리스트를 리턴한다. 
// 각 페이지들은 주로 IndirectObj 가르키는IndirectRef 이다.(PPPage 가 아님) 
//////////////////////////////////////////////////////////////////////////////
PPTArray *PPDocument::PageArray()
{
    PPTDictionary *pages_dict = PagesDictionary();

    PPTArray *page_list = (PPTArray *)pages_dict->objectForKey("Kids");
	return page_list;
}

// Private Method :AddPage() 함수 안에서만 사용됨.
///////////////////////////////////////////////////////////////////
void PPDocument::setPageCount(int cnt)
{
	PPTDictionary *pages_dict = PagesDictionary();
	PPTNumber *number = new PPTNumber(this, cnt);

	pages_dict->SetTokenAndKey(number, "Count");
}

// PDF 내의 파리미터로 주어진 페이지를 추가함.
///////////////////////////////////////////////////////////////////
void PPDocument::AddPage(PPPage *page)
{
	PPTArray *page_array = PageArray();
	PPTIndirectRef *page_ref = new PPTIndirectRef(this, ++_objNumber, 0);
	page_array->AddToken(page_ref);

	setPageCount(page_array->_array.size());

	PPTIndirectObj *page_obj = new PPTIndirectObj(this, _objNumber, 0);
	page_obj->AddRefObj(page_ref);
	PushObj(page_obj, _objNumber);
	PPTDictionary *page_dict = new PPTDictionary(this);
	page_obj->AddObj(page_dict);

	PPTDictionary *root_dict = RootDict();
	PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->IndirectObjectForKey("Pages");
	PPTIndirectRef *parent_ref = new PPTIndirectRef(this, pages->_objNum, 0);
	pages->AddRefObj(parent_ref);
	page_dict->SetTokenAndKey(parent_ref, "Parent");

	page->WriteDictionary(page_dict);
}

// for generating
// 새로운 빈 페이지를 생성해서 추가함
///////////////////////////////////////////////////////////////////
PPPage *PPDocument::NewPage(PPRect &rect)
{
	PPPage *page = new PPPage(this);

	// 새로 생성된 빈 페이지를 추가한다.
	AddPage(page); // AddPage() 에서 page->_formDict가 지정된단. 
	               // 그래야 page->SetMediaBox() 가 제대로 동작한다.

	page->SetMediaBox(rect);
//	page->SetCropBox(rect);
//	page->SetArtBox(rect);
//	page->SetBleedBox(rect);
//	page->SetTrimBox(rect);
	_pages.push_back(page);

	return page;
}


void PPDocument::writeLoadedPages()
{
	size_t i, icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages.at(i);
		AddPage(page);
	}
}

// Indirect Object를 _tokens 에 넣을 때는 이 함수를 쓴다.
///////////////////////////////////////////////////////////////////
void PPDocument::PushObj(PPTIndirectObj *obj, int obj_num)
{
	if(_objDict[obj_num] != NULL) {
		_objDict.erase(obj_num);
		return;
	}
	_tokens.push_back(obj);
	obj->SetDocument(this);
	_objDict[obj_num] = obj;
	
}

void PPDocument::PushObj(PPTIndirectObj *obj)
{
	int obj_num = obj->_objNum;
	if(_objDict[obj_num] != NULL) {
		_objDict.erase(obj_num);
		return;
	}
	_tokens.push_back(obj);
	obj->SetDocument(this);
	_objDict[obj_num] = obj;
	
}

//////////////////////////////////////////////////////////////////////////////////////////
// 신규 PDF를 생성할 때 기본 요소들을 세팅함 (PPDocument::PPDocument()에서 호출함)
//////////////////////////////////////////////////////////////////////////////////////////
int PPDocument::preBuildPDF()
{
	_objNumber = 1;
	_tokens.clear();  // Core Raw Data of PDF 

	string *ver_str = new string("PDF-1.5");
	PPTComment *verstr = new PPTComment(this, ver_str);
	_tokens.push_back(verstr);

	string *test_tstr = new string("PDFPlus");
	PPTComment *teststr = new PPTComment(this, test_tstr);
	_tokens.push_back(teststr);

	_trailer = new PPTTrailer(this);

	PPTIndirectRef *root_ref = new PPTIndirectRef(this,++_objNumber, 0);
	_trailer->getDictionary()->setTokenAndKey(root_ref, PPKN_ROOT);
	PPTIndirectObj *root_obj = new PPTIndirectObj(this, _objNumber, 0);
	root_obj->AddRefObj(root_ref);
	PushObj(root_obj, _objNumber);
	// -- RootDict
		PPTDictionary *root_dict = new PPTDictionary(this);
		root_dict->SetTokenAndKey(PPVN_CATALOG, PPKN_TYPE);

		// Pages
		PPTIndirectRef *pages_ref = new PPTIndirectRef(this,++_objNumber, 0);
		root_dict->setTokenAndKey(pages_ref, PPKN_PAGES);
		PPTIndirectObj *pages_obj = new PPTIndirectObj(this, _objNumber, 0);
		pages_obj->AddRefObj(pages_ref);

		PushObj(pages_obj, _objNumber);
			PPTDictionary *pages_dict = new PPTDictionary(this);
			PPTArray *page_list = new PPTArray(this);
			pages_dict->SetTokenAndKey(page_list, PPKN_KIDS);
			writeLoadedPages();
			pages_dict->SetTokenAndKey("Pages", "Type");

			pages_obj->AddObj(pages_dict);
		root_obj->AddObj(root_dict);
		
	// End RootDict

	PPTIndirectRef *info_ref = new PPTIndirectRef(this,++_objNumber, 0);
	_trailer->getDictionary()->setTokenAndKey(info_ref, PPKN_INFO);
	PPTIndirectObj *info_obj = new PPTIndirectObj(this, _objNumber, 0);
	info_obj->AddRefObj(info_ref);
	PushObj(info_obj, _objNumber);

	//  --  InfoDict
		PPTDictionary *info_dict = new PPTDictionary(this);
		info_dict->SetStringAndKey(PPVN_CREATOR, PPKN_TITLE);
		info_dict->SetStringAndKey(PPVN_CREATOR, PPKN_AUTHOR);
		info_dict->SetStringAndKey(PPVN_CREATOR, PPKN_SUBJECT);
		info_dict->SetStringAndKey(PPVN_CREATOR, PPKN_PRODUCER);
		info_dict->SetStringAndKey(PPVN_CREATOR, PPKN_CREATOR);
		info_dict->SetStringAndKey("D:20150719033247Z00'00'", PPKN_CREATIONDATE);
		info_dict->SetStringAndKey("D:20150719033247Z00'00'", PPKN_MODDATE);
		info_dict->SetStringAndKey(PPVN_CREATOR, "Keywords");

		info_obj->AddObj(info_dict);
	//  End  InfoDict



	_tokens.push_back(_trailer);
	_state = PPDS_Built_Document;
	return 0;
}

// 페이지들을 다 생성하고 난 후 저장하기 전 단계에서 하는 작업
///////////////////////////////////////////////////////////////////
int PPDocument::postBuildPDF()
{
	int i, icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages[i];
		// 페이지의 내용을 스트림으로 빌드하고 페이지 정보를 정리한다
		page->BuildPDF();
	}
	return 0;
}

////////////////////////////////////////////////////////////////////
// PDF를 저장하기 위한 함수들.
////////////////////////////////////////////////////////////////////

// PDF를 저장하기 직전에 IndirectObj와 IndirectRef들을 재정리하는 함수

void PPDocument::doReorderingObjectNumbers()
{
    
    vector<PPTIndirectObj *> indir_objs; // empty list
    int new_idx = 0;
    
	// 모든 IndirectObj들을 indir_objs에 차례대로 저장
    map <int, PPTIndirectObj *> ::iterator it_indir_objs;
    for(it_indir_objs = _objDict.begin(); it_indir_objs != _objDict.end(); it_indir_objs++) {
        //        int obj_num = it_indir_objs->first;
        PPTIndirectObj *indir_obj = it_indir_objs->second;
        if (indir_obj) {
            indir_objs.push_back(indir_obj);
        }
    }
    
    size_t i, icnt = indir_objs.size();
    _objDict.clear(); // 멤버 인스턴스를 클리어
    for (i=0; i < icnt; i++) {
        PPTIndirectObj *indir_obj = indir_objs[i];
        new_idx = (int)i + 1;

		// 이 indir_obj 를 참조하고 있던 모든 IndirectRef들의 
		// Object Number값을 new_idx값으로 변경함
        indir_obj->setObjNum(new_idx); 
        
        _objDict[new_idx] = indir_obj;
    }
    _last_obj_idx = new_idx;
}

// PDF 구성 요소중에 하나인 'xref' 값들을 저장하는 함수
unsigned long long PPDocument::writeXRefs(std::ostream &os)
{
    size_t obj_cnt = _last_obj_idx+1; //objNumber starts from 1.
    unsigned long long ret_pos = os.tellp();
    os << "xref" << PP_ENDL;
    os << "0 " << obj_cnt << PP_ENDL;
    size_t i;
    for(i=0;i<obj_cnt;i++) {
        PPTIndirectObj *obj = _objDict[i];
        char buf[30];
        if(obj == NULL)
            _objDict.erase(i);
        if (i == 0) {
#ifdef _WIN32
            sprintf_s(buf, "%010d %05d %c",0, 65535, 'f' );
#else
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
#endif
        }
        else if (obj == NULL) {
            cout << "Unexpected Error : IndirectObj not exists at " << i << PP_ENDL;
#ifdef _WIN32
            sprintf_s(buf, "%010d %05d %c",0, 65535, 'f' );
#else
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
#endif
        }
        else {
#ifdef _WIN32
            sprintf_s(buf, "%010llu %05d %c",obj->_filepos, (unsigned int)obj->_genNum, 'n' );
#else
            sprintf(buf, "%010llu %05d %c",obj->_filepos, (unsigned int)obj->_genNum, 'n' );
#endif
        }
        os << buf << PP_ENDL;
    }
    return ret_pos;
}

// PPDocument 내용을 PDF로 저장하는 함수.
int PPDocument::Save(char *out_path)
{
    std::filebuf fb;

	// 저장하기 위해 파일 스트림 오픈 
	fb.open (out_path, std::ios::out | std::ios::binary);
    std::ostream os(&fb);

	// 페이지 내용들을 스트림으로 저장한다. 
	postBuildPDF();

	// IndirectObj들의 넘버링일 다시한다.
    doReorderingObjectNumbers();

    _filePtDict.clear();
    vector<PPTTrailer *> trailers;
    PPTTrailer *master_trailer = new PPTTrailer(this);
    
    size_t i, icnt = _tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens[i];
        if (token->ClassType() == PPTN_TRAILER) {// trailer 는 하나로 모아서 별도로 처리
            trailers.push_back((PPTTrailer *)token);
        }
        else if (token->ClassType() != PPTN_XREF) {
			// IndirectObj의 내용들 중에 master_trailer 에 들어갈 내용들을 별도로 처리한다.
            if (token->ClassType() == PPTN_INDIRECTOBJ) {
                PPTIndirectObj *indir_obj = (PPTIndirectObj *)token;
                PPTDictionary *dict = indir_obj->FirstDictionary();
                if (dict) {
                    PPTName *type = (PPTName *)dict->ObjectForKey("Type");
                    if(type && *type->_name == "Catalog") {
                            PPTIndirectRef *indir_ref = new PPTIndirectRef(this, indir_obj->_objNum, indir_obj->_genNum);
							// master_trailer에 RootObject 를 지정.
                            master_trailer->SetRootObject(indir_ref);
							indir_obj->AddRefObj(indir_ref);
                    }
                    else {
                        PPTString *cdate = (PPTString *)dict->objectForKey("CreationDate");
                        PPTString *mdate = (PPTString *)dict->objectForKey("ModDate");
                        PPTString *producer = (PPTString *)dict->objectForKey("Producer");
						PPTString *creator = (PPTString *)dict->objectForKey("Creator");
                        if(cdate || mdate || producer || creator) {
                            PPTIndirectRef *indir_ref = new PPTIndirectRef(this, indir_obj->_objNum, indir_obj->_genNum);
							// master_trailer에 InfoObject 를 지정.
                            master_trailer->SetInfoObject(indir_ref);
							indir_obj->AddRefObj(indir_ref);
                        }
                    }
                }
                
            }
            token->Write(os);
        }
    }
    unsigned long long xref_pos = writeXRefs(os);

    master_trailer->Build();
    master_trailer->_startxref = xref_pos;
    master_trailer->Write(os);
    fb.close();
    cout << out_path << " generated!!!!\xa";
    return 0;
}

//////////////////////////////////////////////////////////////////
// PPParserSource 의 상속 함수들 
// Overriding Part for Parsing
//////////////////////////////////////////////////////////////////
bool PPDocument::canParseString(string str)
{
	// 'R'인 단어가 나타나면 파싱을 한다.
    if (str.length() == 1 && str == "R") {
        return true;
    }
    return false;
}

bool isKindOfNumber(PPToken *token);

PPToken *PPDocument::parseString(string str, vector <PPToken *> &tokens, PPParser *parser )
{
    // 'R'은 두개의 넘버를 가지고 있는 IndirectRef 이다.
    if (str.length() == 1 && str == "R") {
        int cnt = (int)tokens.size();
        PPToken *token1 = tokens[cnt-2];
        PPToken *token2 = tokens[cnt-1];
        //            if (token1->isKindOfClass<PPTNumber *>() && token2->isKindOfClass<PPTNumber *>()) {
        if (isKindOfNumber((PPTNumber *)token1) && isKindOfNumber((PPTNumber *)token2)) {
            PPTNumber *num1 = (PPTNumber *)token1;
            PPTNumber *num2 = (PPTNumber *)token2;
            PPToken *token_obj = (PPToken *)new PPTIndirectRef(this, num1->intValue(), num2->intValue());
            if(token_obj) {
                tokens.erase(tokens.begin()+(cnt-1));
                tokens.erase(tokens.begin()+(cnt-2));
                tokens.push_back(token_obj);
                return token_obj;
            }
        }
    }
    
    return NULL;
}

void PPDocument::get(char &ch)
{
    _file.get(ch);
}

bool PPDocument::eof()
{
	size_t curoff = tellg();
	if(curoff >= _fsize) {
		return true;
	}
	return false;
   // return _file.eof();
}

size_t PPDocument::tellg()
{
//	cout << "Doc Point : " << _file.tellg() << PP_ENDL;
	return (size_t)(_file.tellg() - _foffset);
}

void PPDocument::seekg(size_t pos)
{
    _file.seekg(pos);
}

void PPDocument::read(char *buf,size_t size)
{
     _file.read(buf, size);
}

void PPDocument::getline(char *buf, size_t size)
{
//    _file.getline(buf, size);
    
    
    size_t bufidx = 0;
    char ch;
    get(ch);
    while ((ch == 0x0d || ch == 0x0a) && !eof()) {
        get(ch);
    }
    while (ch != 0x0d && ch != 0x0a && !eof()) {
        buf[bufidx++] = ch;
        if (bufidx == size-1) {
            buf[bufidx] = 0;
            return;
        }
        get(ch);
    }
    buf[bufidx] = 0;
    while ((ch == 0x0d || ch == 0x0a)  && !eof()){
        get(ch);
    }
    size_t filept = tellg();
    seekg(filept-1);
}

////////////////////////////////////////////////////////////// End of PPParserSource
/////////////////////////////////////////////////////////////////////////////////////

int PPDocument::NewObjNum()
{
	return (++_objNumber);
}


PPTName *PPDocument::AddFormObject(PPPage *page)
{
	PPTName *ret_name;
	string name_str;
	_xobjNumber ++;
	name_str = "form" + _xobjNumber;

	vector <PPToken *> token_list;
	PPTDictionary *dict = new PPTDictionary(this);
	// BBox : pages MediaBox
	PPTArray *src_rect_arr = (PPTArray *)page->_formDict->objectForKey("MediaBox");
	if(src_rect_arr) {
		PPTArray *rect_array = (PPTArray *)src_rect_arr->Copy();
		dict->setTokenAndKey(rect_array, "BBox");
	}
	// Filter : FlateDecode
	PPTName *name = new PPTName(this, new string("FlateDecode"));
	dict->setTokenAndKey(name, "Filter");
	// Length : stream length <Number>
	// Matrix : pages matrix <Array<Number>>
	PPTArray *matrix = new PPTArray(this);
	matrix->AddToken(1);
	matrix->AddToken(0);
	matrix->AddToken(0);
	matrix->AddToken(1);
	matrix->AddToken(0);
	matrix->AddToken(0);
	dict->setTokenAndKey((PPToken *)matrix, "Matrix");
	// Resources : Ref (ColorSpace, ExtGState, Properties, Shading)
	PPToken *pg_rcs = page->_formDict->objectForKey("Resource");
	if(pg_rcs) {
		PPToken *resource = (PPToken *)pg_rcs->Copy();
		dict->setTokenAndKey(resource, "Resource");
	}
	// Subtype : Form <Name>
	dict->setTokenAndKey((PPToken *)new PPTName(this,new string("Form")), "Subtype");

	// Type : XObject <Name>
	dict->setTokenAndKey((PPToken *)new PPTName(this,new string("XObject")), "Type");

	++_objNumber;
	PPTIndirectObj *indirObj = new PPTIndirectObj(this, token_list, _objNumber, 0);
	PushObj(indirObj, _objNumber);
	_xobjects[_objNumber] = indirObj;
//	_tokens.push_back(indirObj);
	ret_name = new PPTName(this, new string(name_str));
	return ret_name;
}


PPTIndirectObj *PPDocument::SetRefTokenForKey(PPTDictionary *dict, PPToken *token, string key)
{
	PPTIndirectObj *obj = dict->SetRefTokenAndKey(token, key, ++_objNumber);  // return new PPTIndirectObj
	PushObj(obj, _objNumber);
//	_tokens.push_back(obj);
	return obj;
}

/////////////////////////////////////////////////////////////
//  Resource Related Methods
/////////////////////////////////////////////////////////////
PPTIndirectObj *PPDocument::AddResource(PPToken *rsc, int num)
{
	PPToken *already_have = ResourceForExtObjNum(num);
	if(already_have)
		return (PPTIndirectObj *)rsc;
	if(rsc->ClassType() == PPTN_INDIRECTREF) {
		PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc;
		rsc = rsc_ref->targetObject();
	}
	if(rsc->ClassType() != PPTN_INDIRECTOBJ) {
		PPTIndirectObj *container_obj = new PPTIndirectObj(this, 0, 0);
		container_obj->AddObj(rsc);
	//	PushObj(container_obj, _objNumber);
		rsc = (PPToken *)container_obj;
	}

	_srcIndirectObjs[num] = (PPTIndirectObj *)rsc; //_resources[num] = rsc;
	return (PPTIndirectObj *)rsc;
}

// 도큐먼트 외부에 있는 리소스를 이 도큐먼트에 저장함.
/////////////////////////////////////////////////////////////
PPTIndirectObj *PPDocument::WriteResource(PPToken *rsc, int obj_num)
{
	if(rsc->_document == this) {
		return NULL;
	}
	PPToken *new_rsc = (PPToken *)rsc->Copy();

	PPTIndirectObj *rsc_obj = AddResource(new_rsc, obj_num);
	
	rsc_obj->MoveInto(this);
	return rsc_obj;
}

PPToken *PPDocument::ResourceForExtObjNum(int num)
{
	PPToken *ret_token = _srcIndirectObjs[num]; //_resources[num];

	if(ret_token == NULL) {
		_srcIndirectObjs.erase(num);
	}
	return ret_token;
}

// Query Methods
///////////////////////////////////////////////////////////////////
PPToken *PPDocument::XObjectForKey(int key)
{
    PPToken *ret = _xobjects.at(key);
    if (ret == NULL) {
        _xobjects.erase(key);
    }
    return ret;
}

// XObject으로 지정되어 있는 이미지 스트림을 그대로 파일로 저장함.
// 필수 함수는 아님
////////////////////////////////////////////////////////////////////////////
void PPDocument::SaveXObjectsToFolder(const char *folder) // Currently just Images
{
    map <int, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _xobjects.begin(); it_token_objs != _xobjects.end(); it_token_objs++) {
        int obj_num = it_token_objs->first;
        PPTIndirectObj *indir_obj = (PPTIndirectObj *)it_token_objs->second;
        if (indir_obj) {
            PPTDictionary *obj_dict = indir_obj->firstDictionary();
            PPTName *filter = (PPTName *)obj_dict->objectForKey("Filter");
            cout << "Filter = " << *filter->_name << PP_ENDL;
            if (*filter->_name == "DCTDecode") {
                PPTStream *stream = indir_obj->stream();
                size_t folder_len = strlen(folder);
                char filename[7];
#ifdef _WIN32
                sprintf_s(filename, "%06d", obj_num);
#else
                sprintf(filename, "%06d", obj_num);
#endif
				int bufsize = folder_len + 6 + 6;
                char *tar_path = new char[bufsize]; // 6 : '/' + '.jpg' + NULL
#ifdef _WIN32
                sprintf_s(tar_path, bufsize, "%s\\%s.jpg",folder, filename);
#else
                sprintf(tar_path, "%s/%s.jpg",folder, filename);
#endif
				stream->writeTo((const char *)tar_path);
                delete [] tar_path;
            }
        }
    }
    
}

// 필수 함수는 아님
////////////////////////////////////////////////////////////////////////////
void PPDocument::SaveFontsToFolder(const char *folder)
{
    
}
//  Image Related Methods   
/////////////////////////////////////////////////////////////////////
#include "PPImage.h"

PPTIndirectObj *PPDocument::ImageFromPath(string path)
{
	PPTIndirectObj *ret_obj = _images[path];
	if(ret_obj == NULL) {
		PPImage *image = new PPImage(path, this);
		int obj_num = NewObjNum();
		ret_obj = image->MakeIndirectObj(obj_num);
		PushObj(ret_obj);
		_images[path] = ret_obj;
	}
	return ret_obj;
}


/////////////////////////////////////////////////////////////
//  Layer(OCG) Related Methods
/////////////////////////////////////////////////////////////

//#pragma mark -
//#pragma mark Sketch Button actions

int PPDocument::NumberOfLayers()
{
	if(_layerOrders)
		return _layerOrders->size();
	return 0;
}

PPTDictionary *PPDocument::LayerInfoAtIndex(int idx)
{
	PPTDictionary *ret_dict = NULL;
	PPToken *layer = _layerOrders->_array[idx];
	if(layer->TypeName() == PPTN_INDIRECTREF) {
		PPTIndirectRef *layer_ref = (PPTIndirectRef *)layer;
		ret_dict = (PPTDictionary *)layer_ref->valueObject();
	}
	else if(layer->TypeName() == PPTN_DICTIONARY) {
		ret_dict = (PPTDictionary *)layer;
	}
	return ret_dict;
}

PPTIndirectObj *PPDocument::LayerObjAtIndex(int idx)
{
	PPTIndirectObj *ret_obj = NULL;
	PPToken *layer = _layerOrders->_array[idx];
	if(layer->TypeName() == PPTN_INDIRECTREF) {
		PPTIndirectRef *layer_ref = (PPTIndirectRef *)layer;
		ret_obj = (PPTIndirectObj *)layer_ref->targetObject();
	}
	return ret_obj;
}

PPTDictionary *PPDocument::LayerDictForName(string name) 
{
	int i, icnt = this->NumberOfLayers();
	for(i=0;i<icnt;i++) {
		PPTDictionary *layer_dict = this->LayerInfoAtIndex(i);
		PPTString *str = (PPTString *)layer_dict->ValueObjectForKey("Name");
		if(*str->_string == name) {
			return layer_dict;
		}
	}
	return NULL;
}

PPTIndirectObj *PPDocument::LayerObjForName(string name) 
{
	int i, icnt = this->NumberOfLayers();
	for(i=0;i<icnt;i++) {
		PPTDictionary *layer_dict = this->LayerInfoAtIndex(i);
		PPTString *str = (PPTString *)layer_dict->ValueObjectForKey("Name");
		if(*str->_string == name) {
			PPTIndirectObj *ret_obj = this->LayerObjAtIndex(i);
			return ret_obj;
		}
	}
	return NULL;
}


PPLayer *PPDocument::NewLayerForName(string name)
{
	PPTDictionary *layer_dict = LayerDictForName(name);
	PPLayer *ret_layer = new PPLayer(layer_dict);
	return ret_layer;
}

void PPDocument::BuildOCProperties() {
    //_OCProperties = (PPTDictionary *)root_dict->valueObjectForKey("OCProperties");

	_OCProperties = new PPTDictionary();
	PPTDictionary *d_dict = new PPTDictionary();

	_layerOrders = new PPTArray();
	d_dict->SetTokenAndKey(_layerOrders, "Order");
	_layersOn = new PPTArray();
	d_dict->SetTokenAndKey(_layersOn, "ON");
	_OCProperties->SetTokenAndKey(d_dict, "D");

	_OCGs = new PPTArray();
	_OCProperties->SetTokenAndKey(_OCGs, "OCGs");

	PPTIndirectObj *root_obj = (PPTIndirectObj *)_trailer->rootObject();
    PPTDictionary *root_dict = root_obj->firstDictionary();
	root_dict->SetTokenAndKey(_OCProperties, "OCProperties");

}
bool PPDocument::AddLayer(string name) 
{
	if(_OCProperties == NULL) {
		this->BuildOCProperties();
	}
	PPTDictionary *layer_dict = new PPTDictionary(this);
	// 'Intent', 'Usage' are optional.
	layer_dict->SetStringAndKey(name, "Name");
	layer_dict->SetNameAndKey("OCG", "Type");
	++_objNumber;
	PPTIndirectRef *layer_ref = new PPTIndirectRef(this, _objNumber, 0);
	PPTIndirectObj *layer_obj = new PPTIndirectObj(this, _objNumber, 0);
	layer_obj->AddObj(layer_dict);

	_layerOrders->AddToken(layer_ref);
	layer_obj->AddRefObj(layer_ref);

	layer_ref = new PPTIndirectRef(this, _objNumber, 0);
	_layersOn->AddToken(layer_ref);
	layer_obj->AddRefObj(layer_ref);

	layer_ref = new PPTIndirectRef(this, _objNumber, 0);
	_OCGs->AddToken(layer_ref);
	layer_obj->AddRefObj(layer_ref);

	PushObj(layer_obj, _objNumber);
//	_tokens.insert(_tokens.begin(),layer_obj);
	return true;
}

bool PPDocument::RenameLayer(string org_name, string new_name)
{
	PPTDictionary *layer_dict = this->LayerDictForName(org_name);
	if(layer_dict) {
		PPTString *name_str = (PPTString *)layer_dict->ObjectForKey("Name");
		delete name_str->_string;
		name_str->_string = new string(new_name);
		return true;
	}
	return false;
}

void PPDocument::ReorderLayer(int from_idx, int to_idx)
{
	_layerOrders->Reorder(to_idx, from_idx);
	int i, icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages[i];
		page->ReorderLayer(to_idx, from_idx);
	}
}

void PPDocument::MergeLayer(PPTDictionary *layer_dict1, PPTDictionary *layer_dict2)
{

}

void PPDocument::RemoveRelatedObjects(PPTIndirectRef *ref)
{


}

void PPDocument::MergeLayer(string layer1, string layer2)
{
	//PPTArray *_layerOrders;
	PPTDictionary *layer_dict1 = NULL;
	int l1 = -1;
	int l2 = -1;
	PPTDictionary *layer_dict2 = NULL;
	int i, icnt = _layerOrders->_array.size();
	for(i=0;i<icnt;i++) {
		PPTIndirectRef *indir_ref = (PPTIndirectRef *)_layerOrders->_array[i];
		PPTIndirectObj *indir_obj = indir_ref->targetObject();
		PPTDictionary *layer_dict = indir_obj->FirstDictionary();
		PPTString *layer_name = (PPTString *)layer_dict->ObjectForKey("Name");
		if(*layer_name->_string == layer1) {
			layer_dict1 = layer_dict;
			l1 = i;
		}
		else if(*layer_name->_string == layer2) {
			layer_dict2 = layer_dict;
			l2 = i;
		}
	}
	if(layer_dict1 == NULL) {
		return;
	}
	if(layer_dict2 == NULL) {
		return;
	}
	PPTString *layer1_name = (PPTString *)layer_dict1->ObjectForKey("Name");
	PPTString *layer2_name = (PPTString *)layer_dict2->ObjectForKey("Name");
	layer1_name->AppendString(layer2_name);
	_layerOrders->RemoveAtIndex(l2);


	icnt = _OCGs->_array.size();
	for(i=0;i<icnt;i++) {
		PPTIndirectRef *indir_ref = (PPTIndirectRef *)_OCGs->_array[i];
		PPTIndirectObj *indir_obj = indir_ref->targetObject();
		PPTDictionary *layer_dict = indir_obj->FirstDictionary();
		PPTString *layer_name = (PPTString *)layer_dict->ObjectForKey("Name");
		if(*layer_name->_string == layer2) {
			_OCGs->RemoveAtIndex(i);
			// remove related IndirectObjs
			break;
		}
	}

	icnt = _layersOn->_array.size();
	for(i=0;i<icnt;i++) {
		PPTIndirectRef *indir_ref = (PPTIndirectRef *)_layersOn->_array[i];
		PPTIndirectObj *indir_obj = indir_ref->targetObject();
		PPTDictionary *layer_dict = indir_obj->FirstDictionary();
		PPTString *layer_name = (PPTString *)layer_dict->ObjectForKey("Name");
		if(*layer_name->_string == layer2) {
			_layersOn->RemoveAtIndex(i);
			break;
		}
	}

	icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages[i];
		page->MergeLayer(*layer1_name->_string, layer2);
	}
}


void PPDocument::WriteOCProperties(PPTDictionary *properties)
{
	// root_obj (Type:Catalog)
    PPTIndirectObj *root_obj = (PPTIndirectObj *)_trailer->rootObject();
    PPTDictionary *root_dict = root_obj->firstDictionary();

	_OCProperties = (PPTDictionary *)properties->Copy();
	root_dict->SetTokenAndKey(_OCProperties, "OCProperties");
	
	_OCProperties->MoveInto(this);
}
////////////////////////////////// End of Layer(OC) Related Methods
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//  Query Methods
/////////////////////////////////////////////////////////////

map <int, PPTIndirectObj *> &PPDocument::ObjectsDictionary()
{
    return _objDict;
}

// Query Information
PPToken *PPDocument::ObjectForNumber(int obj_num)
{
    PPToken *ret =_objDict[obj_num];
    if (ret == NULL)
        _objDict.erase(obj_num);
    return ret;
}

// 현재 딱 한 군데서 쓰임 : PPTDictionary *PPTTrailer::getDictionary()
PPToken *PPDocument::ObjectAtFilePosition(unsigned long long pos)
{
    PPToken *ret = _filePtDict[pos];
    return ret;
}


//////////////////////////////////////////////////////////////////////////////
// XML 만들기 관련 Methods. 편의를 위해 만든 함수들로 필수는 아님
//////////////////////////////////////////////////////////////////////////////

// 페이지 순서대로 Token 기반의 XML 스트링을 만든다.
//////////////////////////////////////////////////////////////////////////////
string PPDocument::xobjectsXMLString(int level)
{
    string retstr;
    ostringstream ostr;
   
    map <int, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _xobjects.begin(); it_token_objs != _xobjects.end(); it_token_objs++) {
        int obj_num = it_token_objs->first;
        ostr << PPTabStr(level) << "<XObject id ='" << obj_num << "'>\xa";
        PPTIndirectObj *indir_obj = (PPTIndirectObj *)it_token_objs->second;
        PPTDictionary *obj_dict = indir_obj->firstDictionary();
        ostr << obj_dict->XMLString(level+1);
        ostr << PPTabStr(level) << "</XObject>\xa";
    }
    
    retstr = ostr.str();
    return retstr;
}

string PPDocument::fontsXMLString(int level)
{
    string retstr;
    ostringstream ostr;
    
    map <int, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _fonts.begin(); it_token_objs != _fonts.end(); it_token_objs++) {
        int obj_num = it_token_objs->first;
        ostr << PPTabStr(level) << "<Font name='" << obj_num << "'>\xa";
        PPTIndirectObj *indir_obj = (PPTIndirectObj *)it_token_objs->second;
        PPTDictionary *obj_dict = indir_obj->firstDictionary();
        ostr << obj_dict->XMLString(level+1);
        ostr << PPTabStr(level) << "</Font>\xa";
    }
    
    retstr = ostr.str();
    return retstr;
}

string PPDocument::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
   if (_version) {
        ostr << PPTabStr(level) << "<Version>" << *_version->_name << "</Version>\xa";
    }
    if (_pageLayout) {
        ostr << PPTabStr(level) << "<PageLayout>" << *_pageLayout->_name << "</PageLayout>\xa";
    }
    if (_pageMode) {
        ostr << PPTabStr(level) << "<PageMode>" << *_pageMode->_name << "</PageMode>\xa";
    }
    
    if (_title) {
        ostr << PPTabStr(level) << "<Title><![CDATA[" << _title->utf8String() << "]]></Title>\xa";
    }
    if (_subject) {
        ostr << PPTabStr(level) << "<Subject><![CDATA[" << _subject->utf8String() << "]]></Subject>\xa";
    }
    if (_producer) {
 //       ostr << PPTabStr(level) << "<Producer>" << *_producer->_string << "</Producer>\xa";
        ostr << PPTabStr(level) << "<Producer><![CDATA[" << _producer->utf8String() << "]]></Producer>\xa";
    }
    if (_author) {
        ostr << PPTabStr(level) << "<Author><![CDATA[" << _author->utf8String() << "]]></Author>\xa";
    }
    if (_creator) {
        ostr << PPTabStr(level) << "<Creator><![CDATA[" << _creator->utf8String() << "]]></Creator>\xa";
    }
    if (_creationDate) {
        ostr << PPTabStr(level) << "<CreationDate>" << *_creationDate->_string << "</CreationDate>\xa";
    }
    if (_modDate) {
        ostr << PPTabStr(level) << "<ModDate>" << *_modDate->_string << "</ModDate>\xa";
    }
    if (_keywords) {
        ostr << PPTabStr(level) << "<Keywords><![CDATA[" << _keywords->utf8String() << "]]></Keywords>\xa";
    }
    
    ostr << PPTabStr(level) << "<NumberOfPages>" <<_pages.size() << "</NumberOfPages>\xa";
    ostr << PPTabStr(level) << "<Pages>\xa";
    size_t icnt = _pages.size();
    for (size_t i=0; i<icnt; i++) {
        PPPage *page = _pages.at(i);
        ostr << page->XMLString(level+1);
    }
    ostr << PPTabStr(level) << "</Pages>\xa";

    ostr << PPTabStr(level) << "<XObjects>\xa";
    ostr << xobjectsXMLString(level+1);
    ostr << PPTabStr(level) << "</XObjects>\xa";

    ostr << PPTabStr(level) << "<Fonts>\xa";
    ostr << fontsXMLString(level+1);
    ostr << PPTabStr(level) << "</Fonts>\xa";
    retstr = ostr.str();
    
    return retstr;
    
}

string PPDocument::DocumentXML()
{
    string xml_str;
    if(buildDocument() == 0) {
        cout << "Number of pages : " << _pages.size() << PP_ENDL;
        xml_str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\xa";
        xml_str += "<pdf-doc>\xa";
        xml_str += XMLString(1);
        xml_str += "</pdf-doc>\xa";
    }
    return xml_str;
}

// 페이지 순서대로 Element 기반의 XML 스트링을 만든다.
//////////////////////////////////////////////////////////////////////////////

string PPDocument::ElementXmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<NumberOfPages>" <<_pages.size() << "</NumberOfPages>\xa";
    ostr << PPTabStr(level) << "<Pages>\xa";
    size_t icnt = _pages.size();
    for (size_t i=0; i<icnt; i++) {
        PPPage *page = _pages.at(i);
        ostr << page->elementXmlString(level+1);
    }
    ostr << PPTabStr(level) << "</Pages>\xa";
    retstr = ostr.str();
    
    return retstr;
}

string PPDocument::ElementsXML()
{
    string xml_str;
    if(buildElements() == 0) {
        cout << "Number of pages : " << _pages.size() << PP_ENDL;
        xml_str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\xa";
        xml_str += "<pdf-elements>\xa";
        xml_str += ElementXmlString(1);
        xml_str += "</pdf-elements>\xa";
    }
    return xml_str;
}

// 페이지 순서 상관없이 토큰 기반의 XML 스트링을 토큰 원래의 순서대로 만든다.
//////////////////////////////////////////////////////////////////////////////
string PPDocument::RawDataXML()
{
    string xml_str;
    xml_str += "<?xml version=\"1.0\" encoding=\"ASCII\"?>\xa";
    xml_str += "<pdf-raw>\xa";
    int i, icnt = (int)_tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(i);
        xml_str += token->XMLString(1);
    }
    xml_str += "</pdf-raw>\xa";
    return xml_str;
}
