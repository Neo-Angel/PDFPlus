

#ifndef __PDFPlusLib__PPLayer__
#define __PDFPlusLib__PPLayer__

#include <PPBase.h>

class PPLayer : public PPBase {

public:
	PPTDictionary *_layer_dict;
    vector <PPElement *> _elements;
	
	string _properties;
	PPFormBase *_parent;

public:
	PPLayer();
	PPLayer(PPTDictionary *layer_dict);

	bool IsEqualName(string other);
	void AddElement(PPElement *element);
	PPElement *ElementAtIndex(int idx);
	void RemoveElementAtIndex(int idx);
	void ChangeProperties(string str);

	string Name();

	void Merge(PPLayer *layer);

	PPBase *Copy(PPFormBase *tar_form);
	void CopyMembersTo(PPBase *obj) ;
	PPClassType ClassType() {return PPTN_LAYER;}
	PPBase *Create() {return new PPLayer();}

};


#endif /* defined(__PDFPlusLib__PPLayer__) */