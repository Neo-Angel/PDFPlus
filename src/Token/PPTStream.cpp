//
//  PPTStream.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 11. 9..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <iostream>
#include <fstream>

#include <sstream>
#include <assert.h>
#include "PPDocument.h"
#include "PPTStream.h"
#include "zlib.h"
#include "PPTNumber.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"

#include "PPDefines.h"

string PPTabStr(int cnt);


///////////////////////////////////////// PPStreamBuf Class Declation
struct PPData; // PPDefines.h

// 스트림을 처리하기위한 보조 클래스...
class PPStreamBuf {
protected:
    vector<PPData *> _streamBuf;
    size_t _length;

public:
    PPStreamBuf();
    ~PPStreamBuf();
    void AppendBuf(char *buf, size_t len);
    size_t TotalLength();
    void Collect(char *buf); // _streamBuf 의 내용을 하나로 묶어 buf에 담는다.
};

///////////////////////////////////////// PPStreamBuf Methods Implement
PPStreamBuf::PPStreamBuf()
{
    _length = 0;
}

PPStreamBuf::~PPStreamBuf()
{
    int i, icnt = (int)_streamBuf.size();
    for (i=0; i<icnt; i++) {
        PPData *data = _streamBuf.at(i);
        delete[] data->_data;
        delete data;
    }
}

void PPStreamBuf::AppendBuf(char *buf, size_t len)
{
    PPData *data = new PPData();
    data->_data = new char[len]; //buf;
    data->_length = len;
    memcpy(data->_data, buf, len);
    _streamBuf.push_back(data);
    _length += len;
}

size_t PPStreamBuf::TotalLength()
{
    return _length;
}

void PPStreamBuf::Collect(char *buf)
{
    int i, icnt = (int)_streamBuf.size();
    size_t pos = 0;
    for (i=0; i<icnt; i++) {
        PPData *data = _streamBuf.at(i);
        memcpy(buf + pos, data->_data, data->_length);
        pos += data->_length;
    }
}



// PPTStream   ////////////////////////////////
//             ////////////////////////////////
///////////////////////////////////////////////
PPTStream::PPTStream()
{
    _streamData = NULL;
    _cur_pos = 0;
    _streamSize = 0;
    _decoded = false;
    _decodeFailed = false;
	_filterName = "None";
}

PPTStream::PPTStream(PPDocument *doc) : PPToken(doc)
{
    _streamData = NULL;
    _cur_pos = 0;
    _streamSize = 0;
    _decoded = false;
    _decodeFailed = false;
	_filterName = "None";
}

PPTStream::PPTStream(PPDocument *doc, unsigned long length) : PPToken(doc)
{
    _streamData = new char[length];
    //    bzero(_stream_data, length);
    _cur_pos = 0;
    _streamSize = length;
    _decoded = false;
    _decodeFailed = false;
	_filterName = "None";
}

PPTStream::~PPTStream()
{
    delete[] _streamData;
}

void PPTStream::SetDictionary(PPTDictionary *dict)
{
	_infoDict = dict;
}

void PPTStream::AppendData(char *data, unsigned long length)
{
    memcpy(_streamData + _cur_pos, data, length);
    _cur_pos += length;
}

unsigned char hexnum[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

string hexStr(unsigned char ch)
{
    unsigned char front = ch & 0xf0;
    front = front >> 4;
    unsigned char back = ch & 0x0f;
    ostringstream ostr;
    ostr << hexnum[front] << hexnum[back];
    string retstr =ostr.str();
    
    return retstr;
}

string PPTStream::XMLString(int level)
{
    if (_decoded == false) {
        PPToken *val_obj = (PPToken *)_infoDict->ValueObjectForKey("Length");
        if (val_obj) {
            PPTNumber *len_obj = (PPTNumber *)val_obj;
            unsigned long length = len_obj->longValue();
            _streamSize = length;
            PPTName *filter = (PPTName *)_infoDict->ObjectForKey("Filter");
            if (filter != NULL && *filter->_name == "FlateDecode") {
                FlateDecodeStream();
            }
        }
    }
    string retstr;
    retstr += PPTabStr(level) + "<Stream><![CDATA[";
    //    retstr += PPTabStr(level) + "<String><![CDATA[" +*_string + "]]></String>\xa";
    
    PPTName *filter = (PPTName *)_infoDict->ObjectForKey("Filter");

	// 디코딩이 되어있고 필터명이 'FlateDecode'이면 스트림 데이터를 그대로 보여줌
    if (_decoded && filter && *filter->_name == "FlateDecode") {
        retstr.append(_streamData, _streamSize);
        //        retstr += _streamData;
    }
    else { // 그렇지 않으면 Hex 16진수로 표시해 줌
        int chidx = 0;
        unsigned long i;
        retstr += PPTabStr(level + 1);
        for (i=0; i<_streamSize; i++) {
            unsigned char ch = _streamData[i];
            retstr += hexStr(ch);            
            chidx ++;
            if (chidx == 40) {
                if (i < +_streamSize - 1) {
                    retstr += "\xa";
                    retstr += PPTabStr(level + 1);
                }
                chidx = 0;
            }
        }
    }
    retstr += "\xa";
    retstr += PPTabStr(level) + "]]></Stream>\xa";
    
    return retstr;
}

string PPTStream::MakePDFString(unsigned long &length)
{
    string retstr = "stream\xa";
    PPTName *filter = (PPTName *)_infoDict->ObjectForKey("Filter");
    if (_decoded && filter && *filter->_name == "FlateDecode") {
        char *strm_buf;
        length = FlateEncodeStream(&strm_buf);
        retstr.append(strm_buf, length);
        delete[] strm_buf;
    }
    else {
        retstr.append(_streamData, _streamSize);
		length = _streamSize;
    }
    retstr += "endstream\xa";
    return retstr;
}

string PPTStream::PDFString()
{
    unsigned long length;
    string retstr = MakePDFString(length);
    
    PPTNumber *len_num = new PPTNumber(_document, (int)length);
    _infoDict->SetTokenAndKey(len_num, "Length");
    return retstr;
}

// 스트림 인코딩/디코딩을 위한 함수

#define CHUNK 16384

/* Compress from file source to file dest until EOF on source.
 def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 allocated for processing, Z_STREAM_ERROR if an invalid compression
 level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
 version of the library linked do not match, or Z_ERRNO if there is
 an error reading or writing the files. */
int def(PPTStream *source, PPStreamBuf *dest)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char *in;
    unsigned char out[CHUNK];
    
    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return ret;
    
    /* compress until end of file */
    do {
        strm.avail_in = (uInt)source->GetReadPointer(&in, CHUNK);
        flush = source->IsZipEOF() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        
        /* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            dest->AppendBuf((char *)out, have);
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)deflateEnd(&strm);
//                return Z_ERRNO;
//            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */
    
    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}

/* Decompress from file source to file dest until stream ends or EOF.
 inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 allocated for processing, Z_DATA_ERROR if the deflate data is
 invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
 the version of the library linked do not match, or Z_ERRNO if there
 is an error reading or writing the files. */
int inf(PPTStream *source, PPStreamBuf *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char *in;
    unsigned char out[CHUNK];
    
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;
    
    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = (uInt)source->GetReadPointer(&in, CHUNK);
        if (strm.avail_in == 0) {
            ret = Z_STREAM_END;
            break;
        }
        strm.next_in = in;
        
        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
            }
            have = CHUNK - strm.avail_out;
            dest->AppendBuf((char *)out, have);
        } while (strm.avail_out == 0);
        
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    
    /* clean up and return */
    (void)inflateEnd(&strm);
    return (ret == Z_STREAM_END) ? Z_OK : Z_DATA_ERROR;
}



size_t PPTStream::GetReadPointer(unsigned char **buf, size_t length)
{
    size_t len = _streamSize > (_cur_pos + length) ? length : (_streamSize - _cur_pos);
    if (len <= 0) {
        return 0;
    }
    *buf = (unsigned char *)(_streamData+_cur_pos);
    _cur_pos += len;
    return len;
}

// zlib 디코딩시 현재 위치가 스트림이 끝인지 알려준다.
bool PPTStream::IsZipEOF() // for zlip
{
    if (_cur_pos >= _streamSize) {
        return true;
    }
    return false;
}

// PDF를 해석하기 위해선 스트림을 디코딩해야 한다.
// FlateDecode 방식은 주로 오브젝 스트림(ObjStm)일 경우다.
/////////////////////////////////////////////////////////////
void PPTStream::FlateDecodeStream()
{
    if (_streamSize == 0) {
        return;
    }
    PPStreamBuf *stream_buf = new PPStreamBuf();
    _cur_pos = 0;
    _decodeFailed = false;
    
    int ret = inf(this, stream_buf);
    if (ret != Z_OK) {
        cout << "Error : 'FlateDecode' Decoding Failed." << PP_ENDL;
        delete stream_buf;
        _decodeFailed = true;
        return;
    }
    delete[] _streamData;
    _streamSize = stream_buf->TotalLength();
    _cur_pos = 0;
    _streamData = new char[_streamSize+1];
    
    // decoding 하면서 나눠졌던 데이터들을 하나로 모음
    stream_buf->Collect(_streamData);
	_streamData[_streamSize] = NULL;
	cout << "SteamData: " << _streamData << PP_ENDL;
    _decoded = true;
    
    delete stream_buf;
}

bool PPTStream::IsDecoded()
{
    return _decoded;
}

// PDF를 만들기 전에 먼저 인코딩을 해야한다.
unsigned long PPTStream::FlateEncodeStream(char **strm_dat)
{
    if (_streamSize == 0) {
        return 0;
    }
    PPStreamBuf *stream_buf = new PPStreamBuf();
    
    int ret = def(this, stream_buf);
    if (ret != Z_OK) {
        cout << "Error : 'FlateDecode' Decoding Failed." << PP_ENDL;
        delete stream_buf;
        return 0;
    }
    unsigned long strm_size = stream_buf->TotalLength();
    *strm_dat = new char[strm_size];
    
    // decoding 하면서 나눠졌던 데이터들을 하나로 모음
    stream_buf->Collect(*strm_dat);
    
    delete stream_buf;
    return strm_size;
}


void PPTStream::WriteTo(const char *tar_path)
{
    ofstream file;
	file.open(tar_path, std::ios::binary);
    file.write(_streamData, _streamSize);
    file.close();
    
//    filebuf fb;
//    fb.open (tar_path, std::ios::out);
//    ostream os1(&fb);
   
}

// parser를 이용해 Object Stream 을 파싱한다.
// 계승된 PPParserSource 함수들을 통애 데이터를 공급한다.
////////////////////////////////////////////////////////
bool PPTStream::ParseObjStm(vector<PPToken *> &token_list, PPParser *parser)
{
    PPTNumber *first_num = (PPTNumber *)_infoDict->ValueObjectForKey("First");
    PPTNumber *cnt_num = (PPTNumber *)_infoDict->ValueObjectForKey("N");
    int cnt = cnt_num->intValue();
    int first = first_num->intValue();
    char *nums_cstr = new char[first+1];
    strncpy(nums_cstr, _streamData, first);
    nums_cstr[first] = '\0';
    
    string nums_str = nums_cstr;
    
    vector<string> str_list;
    PPComponentsSepratedByChar(nums_str, ' ', str_list);
    size_t num_idx = 0;
    _cur_pos = first_num->intValue();
    for (int i=0; i<cnt; i++) {
        unsigned int obj_num = stoi(str_list[num_idx++]);
        unsigned int obj_start = stoi(str_list[num_idx++]);
        _next = _streamSize;
        if (num_idx + 1 < str_list.size()) {
            _next = first + stoi(str_list[num_idx + 1]);
        }
        PPTIndirectObj *indir_obj = new PPTIndirectObj(parser->_document, obj_num, 0);
        _cur_pos = first + obj_start;

		if (parser->ParseSource(*this, indir_obj->_array, indir_obj) == false) {
			delete indir_obj;
            return false;
		}
        //token_list.push_back(indir_obj);
        
        indir_obj->_filepos = -1;
        parser->_document->_filePtDict[indir_obj->_filepos] = indir_obj;
//        parser->_document->_objDict[obj_num] = (PPTIndirectObj *)indir_obj;
        if (parser->_document->_objNumber < obj_num) {
			_document->_objNumber = obj_num;
		}
		parser->_document->PushObj(indir_obj);
    }
    delete [] nums_cstr;
    return true;
}

// Overriding Part of PPParserSource
////////////////////////////////////////////////
bool PPTStream::canParseString(string str)
{
    if (str.length() == 1 && str == "R") {
        return true;
    }
    return false;
}

bool isKindOfNumber(PPToken *token);

PPToken *PPTStream::parseString(string str, vector <PPToken *> &tokens, PPParser *parser )
{
    
    if (str.length() == 1 && str == "R") {
        int cnt = (int)tokens.size();
        PPToken *token1 = tokens[cnt-2];
        PPToken *token2 = tokens[cnt-1];
        //            if (token1->isKindOfClass<PPTNumber *>() && token2->isKindOfClass<PPTNumber *>()) {
        if (isKindOfNumber((PPTNumber *)token1) && isKindOfNumber((PPTNumber *)token2)) {
            PPTNumber *num1 = (PPTNumber *)token1;
            PPTNumber *num2 = (PPTNumber *)token2;
			PPToken *token_ref = (PPToken *)new PPTIndirectRef(parser->_document, num1->intValue(), num2->intValue());
            if(token_ref) {
                tokens.erase(tokens.begin() + (cnt-1));
                tokens.erase(tokens.begin() + (cnt-2));
                tokens.push_back(token_ref);
                return token_ref;
            }
        }
    }
    
    return NULL;
}


void PPTStream::get(char &ch)
{
    if (_cur_pos < _next) {
        ch = _streamData[_cur_pos++];
    }
    else {
        ch = 0;
        _cur_pos = _next +1;
    }
}

bool PPTStream::eof() // for parsing
{
    if (_cur_pos > _next) {
        return true;
    }
    return false;
}

size_t PPTStream::tellg()
{
    return _cur_pos;
}

void PPTStream::seekg(size_t pos)
{
    _cur_pos = pos;
}

void PPTStream::read(char *buf,size_t size)
{
    memcpy(buf, _streamData+_cur_pos, size);
    _cur_pos += size;
}

void PPTStream::getline(char *buf, size_t size)
{
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
    _cur_pos --;
}
//////////////////////////////////////////////////// End of PPParserSource
/////////////////////////////////////////////////////////////////////////

void PPTStream::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);
	PPTStream *stream = (PPTStream *)obj;

    stream->_cur_pos = _cur_pos;
    
    stream->_streamSize = _streamSize;
	stream->_streamData = new char[_streamSize]; 
	memcpy(stream->_streamData, _streamData, _streamSize);
	stream->_filterName = _filterName;
    stream->_next = _next;

    stream->_decoded = _decoded;
    stream->_decodeFailed = _decodeFailed;
    stream->_infoDict = NULL;

}