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
#include "PPTStream.h"
#include "zlib.h"
#include "PPTNumber.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"

#include "PPDefines.h"

string tapStr(int cnt);


///////////////////////////////////////// PPStreamBuf Class Declation
struct PPData;

class PPStreamBuf {
protected:
    vector<PPData *> _streamBuf;
    size_t _length;
public:
    PPStreamBuf();
    ~PPStreamBuf();
    void appendBuf(char *buf, size_t len);
    size_t totalLength();
    void collect(char *buf);
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

void PPStreamBuf::appendBuf(char *buf, size_t len)
{
    PPData *data = new PPData();
    data->_data = new char[len]; //buf;
    data->_length = len;
    memcpy(data->_data, buf, len);
    _streamBuf.push_back(data);
    _length += len;
}

size_t PPStreamBuf::totalLength()
{
    return _length;
}

void PPStreamBuf::collect(char *buf)
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
PPTStream::PPTStream(PPParser *parser) : PPToken(parser)
{
    _streamData = NULL;
    _index = 0;
    _streamSize = 0;
    _decoded = false;
    _decodeFailed = false;
}

PPTStream::PPTStream(PPParser *parser, unsigned long length) : PPToken(parser)
{
    _streamData = new char[length];
    //    bzero(_stream_data, length);
    _index = 0;
    _streamSize = length;
    _decoded = false;
    _decodeFailed = false;
}

PPTStream::~PPTStream()
{
    delete[] _streamData;
}

void PPTStream::appendData(char *data, unsigned long length)
{
    memcpy(_streamData + _index, data, length);
    _index += length;
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

string PPTStream::xmlString(int level)
{
    if (_decoded == false) {
        PPToken *val_obj = (PPToken *)_dict->valueObjectForKey("Length");
        if (val_obj) {
            PPTNumber *len_obj = (PPTNumber *)val_obj;
            unsigned long length = len_obj->longValue();
            _streamSize = length;
            PPTName *filter = (PPTName *)_dict->objectForKey("Filter");
            if (filter != NULL && *filter->_name == "FlateDecode") {
                flateDecodeStream();
            }
        }
    }
    string retstr;
    retstr += tapStr(level) + "<Stream><![CDATA[";
    //    retstr += tapStr(level) + "<String><![CDATA[" +*_string + "]]></String>\xa";
    
    PPTName *filter = (PPTName *)_dict->objectForKey("Filter");
    if (_decoded && filter && *filter->_name == "FlateDecode") {
        retstr.append(_streamData, _streamSize);
        //        retstr += _streamData;
    }
    else {
        int chidx = 0;
        unsigned long i;
        retstr += tapStr(level + 1);
        for (i=0; i<_streamSize; i++) {
            unsigned char ch = _streamData[i];
            retstr += hexStr(ch);
            
            //        if (ch > 26 && ch < 0x80) {
            //            retstr += ch;
            //        }
            //        else {
            //            retstr += "[" + hexStr(ch) + "]";
            //        }
            chidx ++;
            if (chidx == 40) {
                if (i < +_streamSize - 1) {
                    retstr += "\xa";
                    retstr += tapStr(level + 1);
                }
                chidx = 0;
            }
        }
    }
    retstr += "\xa";
    retstr += tapStr(level) + "]]></Stream>\xa";
    
    return retstr;
}

string PPTStream::makePDFString(unsigned long &length)
{
    string retstr = "stream\xa";
    PPTName *filter = (PPTName *)_dict->objectForKey("Filter");
    if (_decoded && filter && *filter->_name == "FlateDecode") {
        char *strm_buf;
        length = flateEncodeStream(&strm_buf);
        retstr.append(strm_buf, length);
        delete[] strm_buf;
    }
    else {
        retstr.append(_streamData, _streamSize);
    }
    retstr += "endstream\xa";
    return retstr;
}

string PPTStream::pdfString()
{
    unsigned long length;
    string retstr = makePDFString(length);
    
    PPTNumber *len_num = new PPTNumber(_parser, (int)length);
    _dict->setTokenAndKey(len_num, "Length");
    return retstr;
}


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
        strm.avail_in = (uInt)source->read(&in, CHUNK);
        flush = source->z_eof() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        
        /* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            dest->appendBuf((char *)out, have);
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
        strm.avail_in = (uInt)source->read(&in, CHUNK);
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
            dest->appendBuf((char *)out, have);
        } while (strm.avail_out == 0);
        
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    
    /* clean up and return */
    (void)inflateEnd(&strm);
    return (ret == Z_STREAM_END) ? Z_OK : Z_DATA_ERROR;
}

size_t PPTStream::read(unsigned char **buf, size_t length)
{
    size_t len = _streamSize > (_index + length) ? length : (_streamSize - _index);
    if (len <= 0) {
        return 0;
    }
    *buf = (unsigned char *)(_streamData+_index);
    _index += len;
    return len;
}

bool PPTStream::z_eof() // for zlip
{
    if (_index >= _streamSize) {
        return true;
    }
    return false;
}

void PPTStream::flateDecodeStream()
{
    if (_streamSize == 0) {
        return;
    }
    PPStreamBuf *stream_buf = new PPStreamBuf();
    _index = 0;
    _decodeFailed = false;
    
    int ret = inf(this, stream_buf);
    if (ret != Z_OK) {
        cout << "Error : 'FlateDecode' Decoding Failed." << PP_ENDL;
        delete stream_buf;
        _decodeFailed = true;
        return;
    }
    delete[] _streamData;
    _streamSize = stream_buf->totalLength();
    _index = 0;
    _streamData = new char[_streamSize];
    
    // decoding 하면서 나눠졌던 데이터들을 하나로 모음
    stream_buf->collect(_streamData);
    _decoded = true;
    
    delete stream_buf;
}

bool PPTStream::isDecoded()
{
    return _decoded;
}

unsigned long PPTStream::flateEncodeStream(char **strm_dat)
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
    unsigned long strm_size = stream_buf->totalLength();
    *strm_dat = new char[strm_size];
    
    // decoding 하면서 나눠졌던 데이터들을 하나로 모음
    stream_buf->collect(*strm_dat);
    
    delete stream_buf;
    return strm_size;
}


void PPTStream::writeTo(const char *tar_path)
{
    ofstream file;
	file.open(tar_path, std::ios::binary);
    file.write(_streamData, _streamSize);
    file.close();
    
//    filebuf fb;
//    fb.open (tar_path, std::ios::out);
//    ostream os1(&fb);
   
}

size_t createComponents(string &str, char ch, vector<string *> &str_list);

bool PPTStream::parseObjStm(vector<PPToken *> &token_list, PPParser *parser)
{
    PPTNumber *first_num = (PPTNumber *)_dict->valueObjectForKey("First");
    PPTNumber *cnt_num = (PPTNumber *)_dict->valueObjectForKey("N");
    int cnt = cnt_num->intValue();
    int first = first_num->intValue();
    char *nums_cstr = new char[first+1];
    strncpy(nums_cstr, _streamData, first);
    nums_cstr[first] = '\0';
    
    string nums_str = nums_cstr;
    
    vector<string *> str_list;
    createComponents(nums_str, ' ', str_list);
    size_t num_idx = 0;
    _index = first_num->intValue();
    for (int i=0; i<cnt; i++) {
        int obj_num = stoi(*str_list[num_idx++]);
        int obj_start = stoi(*str_list[num_idx++]);
        _next = _streamSize;
        if (num_idx + 1 < str_list.size()) {
            _next = first + stoi(*str_list[num_idx + 1]);
        }
        vector<PPToken *> sub_tokens;
        _index = first + obj_start;

        if (parser->parseSource(*this, sub_tokens) == false)
            return false;
        PPTIndirectObj *indir_obj = new PPTIndirectObj(parser, sub_tokens, obj_num, 0);
        token_list.push_back(indir_obj);
        
        indir_obj->_filepos = -1;
        parser->_filePtDict[indir_obj->_filepos] = indir_obj;
        parser->_objDict[obj_num] = (PPTIndirectObj *)indir_obj;
        
    }
    delete [] nums_cstr;
    return true;
}

// Overriding Part
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
            PPToken *token_obj = (PPToken *)new PPTIndirectRef(parser, num1->intValue(), num2->intValue());
            if(token_obj) {
                tokens.erase(tokens.begin() + (cnt-1));
                tokens.erase(tokens.begin() + (cnt-2));
                tokens.push_back(token_obj);
                return token_obj;
            }
        }
    }
    
    return NULL;
}


void PPTStream::get(char &ch)
{
    if (_index < _next) {
        ch = _streamData[_index++];
    }
    else {
        ch = 0;
        _index = _next +1;
    }
}

bool PPTStream::eof() // for parsing
{
    if (_index > _next) {
        return true;
    }
    return false;
}

size_t PPTStream::tellg()
{
    return _index;
}

void PPTStream::seekg(size_t pos)
{
    _index = pos;
}

void PPTStream::read(char *buf,size_t size)
{
    memcpy(buf, _streamData+_index, size);
    _index += size;
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
    _index --;
}

