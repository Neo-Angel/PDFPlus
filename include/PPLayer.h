

#ifndef __PDFPlusLib__PPLayer__
#define __PDFPlusLib__PPLayer__

#include <PPBase.h>

class PPLayer : PPBase {

public:
    vector <PPElement *> _elements;
	string _key;
	PPFormBase *_parent;

	void AddElement(PPElement *element);

};


#endif /* defined(__PDFPlusLib__PPLayer__) */