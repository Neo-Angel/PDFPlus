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


static unsigned int DOC_COUNT = 0;
PPDocument::PPDocument(string filepath)
{
	_docID = ++DOC_COUNT;
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

	// open 단계에서는 저수준의 오브젝트(Token)들을 읽어들인다.
	// 주로 PPToken 의 서브 클래스 들이다.
	open(filepath);

	// 읽어들인 토큰들을 가지고 도큐먼트를 구성한다.
	// 도큐먼트는 Document > Page(s) > Element(s) 구조가 된다. 
    buildDocument();
	buildElements();
}

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
    PreBuildPDF();
}
PPDocument::~PPDocument()
{
    _file.close();
    int i, icnt = (int)_tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(i);
        delete token;
    }
    cout << "PPParser destructed..." << PP_ENDL;
    
}

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
    //file.close();

    ///////////////////////////////////

    _file.seekg(0L, ios::beg);
	_parser.stream_list.clear();
    if(!_parser.parseSource(*this, _tokens)) {
        _state = PPDS_Parsing_Error;
        return false;
    }
	_parser.DecodeStreams(_tokens);
    size_t i, icnt = _parser._ref_list.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *indir_ref = _parser._ref_list[i];
        int objnum = indir_ref->_objNum;
        PPTIndirectObj *indir_obj = _parser._objDict[objnum];
        if(indir_obj)
			// indir_obj 에 자신을 참조한 ref 들을 모은다.
			// 이는 추후 indir_obj의 objNum이 바뀔경우 자신을 참조한 ref들의 objNum를 
			// 업데이트 해 주기 위해서 쓰인다.
            indir_obj->addRefObj(indir_ref);
        else
            cout << "Cannot fount IndirectRef of " << objnum << PP_ENDL;
    }

    _state = PPDS_Opened;
	return true;
}

bool PPDocument::isOpened()
{
    return (_state >= PPDS_Opened);
}

bool PPDocument::isBuiltDocument()
{
    return (_state >= PPDS_Built_Document);
}

bool PPDocument::isBuiltElements()
{
    return (_state >= PPDS_Built_Elements);
}


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

    PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->indirectObjectForKey("Pages");
    PPTDictionary *pages_dict = pages->firstDictionary();
	return pages_dict;
}

PPTArray *PPDocument::PageArray()
{
    PPTDictionary *pages_dict = PagesDictionary();

    PPTArray *page_list = (PPTArray *)pages_dict->objectForKey("Kids");
	return page_list;
}

void PPDocument::SetPageCount(int cnt)
{
	PPTDictionary *pages_dict = PagesDictionary();
	PPTNumber *number = new PPTNumber(&_parser, cnt);

	pages_dict->SetTokenAndKey(number, "Count");
}

// 실질적인 PDF 내의 페이지 구성
void PPDocument::AddPage(PPPage *page)
{
	PPTArray *page_array = PageArray();
	PPTIndirectRef *page_ref = new PPTIndirectRef(&_parser, ++_objNumber, 0);
	page_array->AddToken(page_ref);

	SetPageCount(page_array->_array.size());

	PPTIndirectObj *page_obj = new PPTIndirectObj(&_parser, _objNumber, 0);
	page_obj->addRefObj(page_ref);
	PushObj(page_obj, _objNumber);
	PPTDictionary *page_dict = new PPTDictionary(&_parser);
	page_obj->AddObj(page_dict);

	PPTDictionary *root_dict = RootDict();
	PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->indirectObjectForKey("Pages");
	PPTIndirectRef *parent_ref = new PPTIndirectRef(&_parser, pages->_objNum, 0);
	pages->addRefObj(parent_ref);
	page_dict->SetTokenAndKey(parent_ref, "Parent");

	page->WriteDictionary(page_dict);
}

// for generating
PPPage *PPDocument::newPage(PPRect &rect)
{
	PPPage *page = new PPPage(this);

	// AddPage() 에서 page->_formDict가 지정된단. 
	// 그래야 page->SetMediaBox() 가 제대로 동작한다.
	AddPage(page);

	page->SetMediaBox(rect);
	page->SetCropBox(rect);
	page->SetArtBox(rect);
	page->SetBleedBox(rect);
	page->SetTrimBox(rect);
	_pages.push_back(page);

	return page;
}


// for reading
void PPDocument::readPage(PPTDictionary *page_dict)
{
    PPPage *page = new PPPage(this);
    page->loadDictionary(page_dict);
    _pages.push_back(page);
}

void PPDocument::loadPages(PPTDictionary *pages_dict)
{
    PPTArray *page_list = (PPTArray *)pages_dict->objectForKey("Kids");
    size_t i, icnt = page_list->_array.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *ref_obj = (PPTIndirectRef *)page_list->_array.at(i);
        PPTIndirectObj *page_obj = ref_obj->targetObject();
        PPTDictionary *child_dict = page_obj->firstDictionary();
        PPTName *type = (PPTName *)child_dict->objectForKey("Type");
        if (type && *type->_name == "Page") {
            readPage(child_dict);
        }
        else {
            loadPages(child_dict);
        }
    }
}

void PPDocument::WriteLoadedPages()
{
	size_t i, icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages.at(i);
		AddPage(page);
	}
}

int PPDocument::buildDocument()
{
    if (isBuiltDocument()) {
        return 0;
    }
    
    PPTTrailer *first_trailer = NULL;
    size_t i, icnt = _tokens.size();
    
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(icnt-i-1);
        if (token->classType() == PPTN_TRAILER) {
            PPTTrailer *trailer = (PPTTrailer *)token;
            PPTDictionary *trailer_dict = trailer->getDictionary();
            if (trailer_dict) {
                PPTNumber *prev_num = (PPTNumber *)trailer_dict->objectForKey("Prev");
                if (prev_num != NULL) {
                    PPToken *token = _parser._filePtDict[prev_num->intValue()];
                    if (token == NULL) {
                        // error!
                        return -4;
                    }
                    else if( token->classType() == PPTN_TRAILER) {
                        PPTTrailer *prev_trailer = (PPTTrailer *)token;
                        prev_trailer->_next = trailer;

                    }
                    else if( token->classType() == PPTN_XREF) {
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
    
    PPTTrailer *lase_trailer = first_trailer;
    while (lase_trailer != NULL) {
        _trailer = lase_trailer;
        lase_trailer = lase_trailer->_next;
    }
    
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
    
    PPTIndirectObj *pages = (PPTIndirectObj *)root_dict->indirectObjectForKey("Pages");
    PPTDictionary *pages_dict = pages->firstDictionary();

    loadPages(pages_dict);
    _state = PPDS_Built_Document;
    return 0;
}

void PPDocument::PushObj(PPTIndirectObj *obj, int obj_num)
{
	if(_parser._objDict[obj_num] != NULL) {
		_parser._objDict.erase(obj_num);
		return;
	}
	_tokens.push_back(obj);
	obj->SetParser(&_parser);
	_parser._objDict[obj_num] = obj;
	
}

int PPDocument::PreBuildPDF()
{
	_objNumber = 1;
	_tokens.clear();

	string *ver_str = new string("PDF-1.4");
	PPTComment *verstr = new PPTComment(&_parser, ver_str);
	_tokens.push_back(verstr);

	string *test_tstr = new string("Test PDF");
	PPTComment *teststr = new PPTComment(&_parser, test_tstr);
	_tokens.push_back(teststr);

	_trailer = new PPTTrailer(&_parser);
//	PPTDictionary *dict = new PPTDictionary(&_parser);

	PPTIndirectRef *root_ref = new PPTIndirectRef(&_parser,++_objNumber, 0);
	_trailer->getDictionary()->setTokenAndKey(root_ref, PPKN_ROOT);
	PPTIndirectObj *root_obj = new PPTIndirectObj(&_parser, _objNumber, 0);
	root_obj->addRefObj(root_ref);
	PushObj(root_obj, _objNumber);
	// -- RootDict
		PPTDictionary *root_dict = new PPTDictionary(&_parser);
		root_dict->SetTokenAndKey(PPVN_CATALOG, PPKN_TYPE);

		// Pages
		PPTIndirectRef *pages_ref = new PPTIndirectRef(&_parser,++_objNumber, 0);
		root_dict->setTokenAndKey(pages_ref, PPKN_PAGES);
		PPTIndirectObj *pages_obj = new PPTIndirectObj(&_parser, _objNumber, 0);
		pages_obj->addRefObj(pages_ref);
		PushObj(pages_obj, _objNumber);
			PPTDictionary *pages_dict = new PPTDictionary(&_parser);
			PPTArray *page_list = new PPTArray(&_parser);
			pages_dict->SetTokenAndKey(page_list, PPKN_KIDS);
			WriteLoadedPages();
			pages_dict->SetTokenAndKey("Pages", "Type");

			pages_obj->AddObj(pages_dict);
		root_obj->AddObj(root_dict);
		
	// End RootDict

	PPTIndirectRef *info_ref = new PPTIndirectRef(&_parser,++_objNumber, 0);
	_trailer->getDictionary()->setTokenAndKey(info_ref, PPKN_INFO);
	PPTIndirectObj *info_obj = new PPTIndirectObj(&_parser, _objNumber, 0);
	info_obj->addRefObj(info_ref);
	PushObj(info_obj, _objNumber);

	//  --  InfoDict
		PPTDictionary *info_dict = new PPTDictionary(&_parser);
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

int PPDocument::PostBuildPDF()
{
	int i, icnt = _pages.size();
	for(i=0;i<icnt;i++) {
		PPPage *page = _pages[i];
		page->BuildPDF();
	}
	return 0;
}

PPToken *PPDocument::xobjectForKey(int key)
{
    PPToken *ret = _xobjects.at(key);
    if (ret == NULL) {
        _xobjects.erase(key);
    }
    return ret;
}

int PPDocument::buildElements()
{
    if (isBuiltElements()) {
        return 0;
    }
   size_t i, icnt = _pages.size();
    for (i=0; i<icnt; i++) {
        PPPage *page = _pages[i];
        page->buildElements();
    }

    _state = PPDS_Built_Elements;
    return 0;
}

string PPDocument::xobjectsXMLString(int level)
{
    string retstr;
    ostringstream ostr;
   
    map <int, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _xobjects.begin(); it_token_objs != _xobjects.end(); it_token_objs++) {
        int obj_num = it_token_objs->first;
        ostr << tapStr(level) << "<XObject id ='" << obj_num << "'>\xa";
        PPTIndirectObj *indir_obj = (PPTIndirectObj *)it_token_objs->second;
        PPTDictionary *obj_dict = indir_obj->firstDictionary();
        ostr << obj_dict->internalXmlString(level+1);
        ostr << tapStr(level) << "</XObject>\xa";
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
        ostr << tapStr(level) << "<Font name='" << obj_num << "'>\xa";
        PPTIndirectObj *indir_obj = (PPTIndirectObj *)it_token_objs->second;
        PPTDictionary *obj_dict = indir_obj->firstDictionary();
        ostr << obj_dict->internalXmlString(level+1);
        ostr << tapStr(level) << "</Font>\xa";
    }
    
    retstr = ostr.str();
    return retstr;
}

string PPDocument::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
   if (_version) {
        ostr << tapStr(level) << "<Version>" << *_version->_name << "</Version>\xa";
    }
    if (_pageLayout) {
        ostr << tapStr(level) << "<PageLayout>" << *_pageLayout->_name << "</PageLayout>\xa";
    }
    if (_pageMode) {
        ostr << tapStr(level) << "<PageMode>" << *_pageMode->_name << "</PageMode>\xa";
    }
    
    if (_title) {
        ostr << tapStr(level) << "<Title><![CDATA[" << _title->utf8String() << "]]></Title>\xa";
    }
    if (_subject) {
        ostr << tapStr(level) << "<Subject><![CDATA[" << _subject->utf8String() << "]]></Subject>\xa";
    }
    if (_producer) {
 //       ostr << tapStr(level) << "<Producer>" << *_producer->_string << "</Producer>\xa";
        ostr << tapStr(level) << "<Producer><![CDATA[" << _producer->utf8String() << "]]></Producer>\xa";
    }
    if (_author) {
        ostr << tapStr(level) << "<Author><![CDATA[" << _author->utf8String() << "]]></Author>\xa";
    }
    if (_creator) {
        ostr << tapStr(level) << "<Creator><![CDATA[" << _creator->utf8String() << "]]></Creator>\xa";
    }
    if (_creationDate) {
        ostr << tapStr(level) << "<CreationDate>" << *_creationDate->_string << "</CreationDate>\xa";
    }
    if (_modDate) {
        ostr << tapStr(level) << "<ModDate>" << *_modDate->_string << "</ModDate>\xa";
    }
    if (_keywords) {
        ostr << tapStr(level) << "<Keywords><![CDATA[" << _keywords->utf8String() << "]]></Keywords>\xa";
    }
    
    ostr << tapStr(level) << "<NumberOfPages>" <<_pages.size() << "</NumberOfPages>\xa";
    ostr << tapStr(level) << "<Pages>\xa";
    size_t icnt = _pages.size();
    for (size_t i=0; i<icnt; i++) {
        PPPage *page = _pages.at(i);
        ostr << page->xmlString(level+1);
    }
    ostr << tapStr(level) << "</Pages>\xa";

    ostr << tapStr(level) << "<XObjects>\xa";
    ostr << xobjectsXMLString(level+1);
    ostr << tapStr(level) << "</XObjects>\xa";

    ostr << tapStr(level) << "<Fonts>\xa";
    ostr << fontsXMLString(level+1);
    ostr << tapStr(level) << "</Fonts>\xa";
    retstr = ostr.str();
    
    return retstr;
    
}

string PPDocument::documentXML()
{
    string xml_str;
    if(buildDocument() == 0) {
        cout << "Number of pages : " << _pages.size() << PP_ENDL;
        xml_str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\xa";
        xml_str += "<pdf-doc>\xa";
        xml_str += xmlString(1);
        xml_str += "</pdf-doc>\xa";
    }
    return xml_str;
}
string PPDocument::elementXmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<NumberOfPages>" <<_pages.size() << "</NumberOfPages>\xa";
    ostr << tapStr(level) << "<Pages>\xa";
    size_t icnt = _pages.size();
    for (size_t i=0; i<icnt; i++) {
        PPPage *page = _pages.at(i);
        ostr << page->elementXmlString(level+1);
    }
    ostr << tapStr(level) << "</Pages>\xa";
    retstr = ostr.str();
    
    return retstr;
}
string PPDocument::elementsXML()
{
    string xml_str;
    if(buildElements() == 0) {
        cout << "Number of pages : " << _pages.size() << PP_ENDL;
        xml_str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\xa";
        xml_str += "<pdf-elements>\xa";
        xml_str += elementXmlString(1);
        xml_str += "</pdf-elements>\xa";
    }
    return xml_str;
}


string PPDocument::rawDataXML()
{
    string xml_str;
    xml_str += "<?xml version=\"1.0\" encoding=\"ASCII\"?>\xa";
    xml_str += "<pdf-raw>\xa";
    int i, icnt = (int)_tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens.at(i);
        xml_str += token->xmlString(1);
    }
    
    xml_str += "</pdf-raw>\xa";
    return xml_str;
}

void PPDocument::saveXObjectsToFolder(const char *folder) // Currently just Images
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

void PPDocument::saveFontsToFolder(const char *folder)
{
    
}
//
//void PPDocument::resortObjNum()
//{
//    
//    vector<PPTIndirectObj *> indir_objs;
//    int new_idx = 0;
//    
//    map <int, PPTIndirectObj *> ::iterator it_indir_objs;
//    for(it_indir_objs = _parser._objDict.begin(); it_indir_objs != _parser._objDict.end(); it_indir_objs++) {
////        int obj_num = it_indir_objs->first;
//        PPTIndirectObj *indir_obj = it_indir_objs->second;
//        if (indir_obj) {
//            indir_objs.push_back(indir_obj);
//        }
//    }
//    
//    size_t i, icnt = indir_objs.size();
//    _parser._objDict.clear();
//    for (i=0; i < icnt; i++) {
//        PPTIndirectObj *indir_obj = indir_objs[i];
//        new_idx = (int)i + 1;
//        indir_obj->setObjNum(new_idx);
//
//        _parser._objDict[new_idx] = indir_obj;
//    }
//    _parser._last_obj_idx = new_idx;
//}

void PPDocument::resortObjNum()
{
    
    vector<PPTIndirectObj *> indir_objs;
    int new_idx = 0;
    
    map <int, PPTIndirectObj *> ::iterator it_indir_objs;
    for(it_indir_objs = _parser._objDict.begin(); it_indir_objs != _parser._objDict.end(); it_indir_objs++) {
        //        int obj_num = it_indir_objs->first;
        PPTIndirectObj *indir_obj = it_indir_objs->second;
        if (indir_obj) {
            indir_objs.push_back(indir_obj);
        }
    }
    
    size_t i, icnt = indir_objs.size();
    _parser._objDict.clear();
    for (i=0; i < icnt; i++) {
        PPTIndirectObj *indir_obj = indir_objs[i];
        new_idx = (int)i + 1;
        indir_obj->setObjNum(new_idx);
        
        _parser._objDict[new_idx] = indir_obj;
    }
    _parser._last_obj_idx = new_idx;
}


/*
xref
0 190
0000000000 65535 f
0000731448 00000 n
0000732022 00000 n
0000736372 00000 n
0000738632 00000 n
0001132730 00000 n
0001134990 00000 n

0013838305 00000 n
0013878190 00000 n
trailer
<</Size 190>>
startxref
116
%%EOF

 */

unsigned long long PPDocument::writeXRefs(std::ostream &os)
{
    
    size_t obj_cnt = _parser._last_obj_idx+1; //_parser._objDict.size() + 1; // real count + 0 index : objNumber starts from 1.
//    _parser._filePtDict;
    unsigned long long ret_pos = os.tellp();
    os << "xref" << PP_ENDL;
    os << "0 " << obj_cnt << PP_ENDL;
    size_t i;
    for(i=0;i<obj_cnt;i++) {
        PPTIndirectObj *obj = _parser._objDict[i];
        char buf[30];
        if(obj == NULL)
            _parser._objDict.erase(i);
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

int PPDocument::write(char *out_path)
{
    std::filebuf fb;
	fb.open (out_path, std::ios::out | std::ios::binary);
    std::ostream os(&fb);
	PostBuildPDF();
    resortObjNum();
    _parser._objDict.clear(); // 
    _parser._filePtDict.clear();
    vector<PPTTrailer *> trailers;
    PPTTrailer *master_trailer = new PPTTrailer(&_parser);
    
    size_t i, icnt = _tokens.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _tokens[i];
        if (token->classType() == PPTN_TRAILER) {// trailer 는 하나로 모아서 별도로 처리
            trailers.push_back((PPTTrailer *)token);
        }
        else if (token->classType() != PPTN_XREF) {
            if (token->classType() == PPTN_INDIRECTOBJ) {
                PPTIndirectObj *indir_obj = (PPTIndirectObj *)token;
                PPTDictionary *dict = indir_obj->firstDictionary();
                if (dict) {
                    PPTName *type = (PPTName *)dict->objectForKey("Type");
                    if(type && *type->_name == "Catalog") {
                            PPTIndirectRef *indir_ref = new PPTIndirectRef(&_parser, indir_obj->_objNum, indir_obj->_genNum);
                            master_trailer->setRootObject(indir_ref);
							indir_obj->addRefObj(indir_ref);
                    }
                    else {
                        PPTString *cdate = (PPTString *)dict->objectForKey("CreationDate");
                        PPTString *mdate = (PPTString *)dict->objectForKey("ModDate");
                        PPTString *producer = (PPTString *)dict->objectForKey("Producer");
						PPTString *creator = (PPTString *)dict->objectForKey("Creator");
                        if(cdate || mdate || producer || creator) {
                            PPTIndirectRef *indir_ref = new PPTIndirectRef(&_parser, indir_obj->_objNum, indir_obj->_genNum);
                            master_trailer->setInfoObject(indir_ref);
							indir_obj->addRefObj(indir_ref);
                        }
                    }
                }
                
            }
            token->write(os);
        }
    }
    unsigned long long xref_pos = writeXRefs(os);

//    icnt = trailers.size();
//    for (i=0; i<icnt; i++) {
//        PPTTrailer *trailer = trailers[i];
//        master_trailer->merge(trailer);
//    }

    master_trailer->build();
    master_trailer->_startxref = xref_pos;
    master_trailer->write(os);
    fb.close();
    cout << out_path << " generated!!!!\xa";
    return 0;
}


// Overriding Part for Parsing
//////////////////////////////////////////////////////////////////
bool PPDocument::canParseString(string str)
{
    if (str.length() == 1 && str == "R") {
        return true;
    }
    return false;
}

bool isKindOfNumber(PPToken *token);

PPToken *PPDocument::parseString(string str, vector <PPToken *> &tokens, PPParser *parser )
{
    
    if (str.length() == 1 && str == "R") {
        int cnt = (int)tokens.size();
        PPToken *token1 = tokens[cnt-2];
        PPToken *token2 = tokens[cnt-1];
        //            if (token1->isKindOfClass<PPTNumber *>() && token2->isKindOfClass<PPTNumber *>()) {
        if (isKindOfNumber((PPTNumber *)token1) && isKindOfNumber((PPTNumber *)token2)) {
            PPTNumber *num1 = (PPTNumber *)token1;
            PPTNumber *num2 = (PPTNumber *)token2;
            PPToken *token_obj = (PPToken *)new PPTIndirectRef(&_parser, num1->intValue(), num2->intValue());
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
	PPTDictionary *dict = new PPTDictionary(&_parser);
	// BBox : pages MediaBox
	PPTArray *src_rect_arr = (PPTArray *)page->_formDict->objectForKey("MediaBox");
	if(src_rect_arr) {
		PPTArray *rect_array = (PPTArray *)src_rect_arr->Copy();
		dict->setTokenAndKey(rect_array, "BBox");
	}
	// Filter : FlateDecode
	PPTName *name = new PPTName(&_parser, new string("FlateDecode"));
	dict->setTokenAndKey(name, "Filter");
	// Length : stream length <Number>
	// Matrix : pages matrix <Array<Number>>
	PPTArray *matrix = new PPTArray(&_parser);
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
	dict->setTokenAndKey((PPToken *)new PPTName(&_parser,new string("Form")), "Subtype");

	// Type : XObject <Name>
	dict->setTokenAndKey((PPToken *)new PPTName(&_parser,new string("XObject")), "Type");

	++_objNumber;
	PPTIndirectObj *indirObj = new PPTIndirectObj(&_parser, token_list, _objNumber, 0);
	PushObj(indirObj, _objNumber);
	_xobjects[_objNumber] = indirObj;
//	_tokens.push_back(indirObj);
	ret_name = new PPTName(&_parser, new string(name_str));
	return ret_name;
}


PPTIndirectObj *PPDocument::SetRefTokenForKey(PPTDictionary *dict, PPToken *token, string key)
{
	PPTIndirectObj *obj = dict->SetRefTokenAndKey(token, key, ++_objNumber);  // return new PPTIndirectObj
	PushObj(obj, _objNumber);
//	_tokens.push_back(obj);
	return obj;
}


/*
PPTIndirectObj *PPDocument::AddResource(PPToken *rsc, string type, string key)
{
	PPToken *already_have = ResourceForKey(type, key);
	if(already_have)
		return (PPTIndirectObj *)rsc;
	if(rsc->classType() == PPTN_INDIRECTREF) {
		PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc;
		rsc = rsc_ref->targetObject();
	}
	if(rsc->classType() != PPTN_INDIRECTOBJ) {
		PPTIndirectObj *container_obj = new PPTIndirectObj(&_parser, 0, 0);
		container_obj->AddObj(rsc);
	//	PushObj(container_obj, _objNumber);
		rsc = (PPToken *)container_obj;
	}
	string newkey = type;
	newkey += "_";
	newkey += key;
	_resources[newkey] = rsc;
	
//	if(rsc->classType() == PPTN_INDIRECTOBJ) {
//		PPTIndirectObj *rsc_obj = (PPTIndirectObj *)rsc;
//		++_objNumber;
//		rsc_obj->_objNum = _objNumber;
//		PushObj(rsc_obj,_objNumber);
//	}
	
	return (PPTIndirectObj *)rsc;
}
*/
PPTIndirectObj *PPDocument::AddResource(PPToken *rsc, int num)
{
	PPToken *already_have = ResourceForExtObjNum(num);
	if(already_have)
		return (PPTIndirectObj *)rsc;
	if(rsc->classType() == PPTN_INDIRECTREF) {
		PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc;
		rsc = rsc_ref->targetObject();
	}
	if(rsc->classType() != PPTN_INDIRECTOBJ) {
		PPTIndirectObj *container_obj = new PPTIndirectObj(&_parser, 0, 0);
		container_obj->AddObj(rsc);
	//	PushObj(container_obj, _objNumber);
		rsc = (PPToken *)container_obj;
	}

	_resources[num] = rsc;
	return (PPTIndirectObj *)rsc;
}


PPTIndirectObj *PPDocument::WriteResource(PPToken *rsc, int obj_num)
{
	PPToken *new_rsc = (PPToken *)rsc->Copy();

//	if(new_rsc->classType() == PPTN_INDIRECTOBJ) {
//		PPTIndirectObj *indir = (PPTIndirectObj *)new_rsc;
//		int num = NewObjNum();
//		indir->_objNum = num;
//	}
	PPTIndirectObj *rsc_obj = AddResource(new_rsc, obj_num);
	
//	if(rcs->classType() == PPTN_INDIRECTOBJ) {
//		PPTIndirectObj *rcs_obj = (PPTIndirectObj *)rcs;
//		++_objNumber;
//		rcs_obj->_objNum = _objNumber;
//		PushObj(rcs_obj,_objNumber);
//	}
	
	rsc_obj->MoveInto(this);
	return rsc_obj;
}

PPToken *PPDocument::ResourceForExtObjNum(int num)
{
	PPToken *ret_token = _resources[num];

	if(ret_token == NULL) {
		_resources.erase(num);
	}
	return ret_token;
}