
#include "PPStreamBuf.h"

// PPStreamBuf ////////////////////////////////// This is not a PPToken's subclass
#include "PPDefines.h"
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

