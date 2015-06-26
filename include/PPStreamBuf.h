
#ifndef __PDFPlusLib__PPTStreamBuf__
#define __PDFPlusLib__PPTStreamBuf__

#include <PPToken.h>



///////////////////////////////////////// PPStreamBuf

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

/////////////////////////////////////////

#endif
