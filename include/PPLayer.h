

#ifndef __PDFPlusLib__PPLayer__
#define __PDFPlusLib__PPLayer__

#include <PPBase.h>

class PPLayer : PPBase {

public:
	PPTDictionary *_layer_dict;
    vector <PPElement *> _elements;
	
	string _properties;
	PPFormBase *_parent;

public:
	PPLayer();
	PPLayer(PPTDictionary *layer_dict);
	void AddElement(PPElement *element);
	PPElement *ElementAtIndex(int idx);
	void RemoveElementAtIndex(int idx);

	string Name();

	void Merge(PPLayer *layer);
};


#endif /* defined(__PDFPlusLib__PPLayer__) */