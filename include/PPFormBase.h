

#ifndef __PDFPlusLib__PPFormBase__
#define __PDFPlusLib__PPFormBase__

#include <PPBase.h>
#include <PPCommandParser.h>
#include <PPTIndirectObj.h>
#include <PPContext.h>
//
//      PPEFormBase
//
///////////////////////////////////////////////////////////////
class PPToken;
class PPDocument;
class PPElement;

class PPTIndirectObj;
class PPTDictionary;
class PPTCommand;
class PPLayer;

class PPFormBase : public PPBase {
protected:
	PPContext *_context;
public:
	PPTName *_form_key;

    PPDocument *_document;
    PPCommandParser _graphicParser;

	size_t _cur_element_idx;
    PPTIndirectObj *_indirObj;  // XObject

	// ExtGState, ColorSpace, Pattern, Shading, XObject, Font, ProcSet, Properties 
    PPTDictionary *_resourceDict;
	PPTDictionary *_formDict;

    vector <PPTCommand *> _commands;

	vector <PPLayer *> _layers;
	PPLayer *_curLayer;
    
public:
    PPFormBase();
	PPFormBase(PPFormBase *form_base);
	PPFormBase(PPDocument *doc, PPTIndirectObj *indir);

	~PPFormBase();

	PPParser *documentParser();
    virtual int buildElements();
	virtual PPToken *ResourceForKey(int obj_num);
	virtual PPToken *WriteResource(PPToken *rcs, int obj_num);

	PPLayer *AddLayerWithProperties(string property_name);
	PPLayer *AddLayer(string layer_name);
	PPLayer *LayerForName(string name);

    void addElement(PPElement *element);
	void AddElement(PPElement *element) {addElement(element);}
//	void AddXObj(PPTIndirectObj *xobj);
	void AddXObjRef(PPTIndirectObj *xobj, string key);
	void AddFormObj(PPFormBase *form_obj);
	void RemoveElementAtIndex(int idx);

	size_t numberOfElements();
	PPElement *elementAtIndex(int idx);

	void initCurrentIndex() {_cur_element_idx = 0;}
	PPElement *next(); PPElement *Next() {return next();}
	PPElement *first() {initCurrentIndex(); return next();} 

	void writeElement(PPElement *element);
	void WriteElement(PPElement *element) {writeElement(element);}
	int getObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
	int GetObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
    int GetXObjNumOf(string name);
	PPTIndirectObj *GetXObject();
	PPTIndirectRef *ResourceForKey(string key);
	PPTDictionary *ResourceDictForKey(string key);
	PPTIndirectObj *ResourceObjForName(string name, string resource_type);
	string NameFromResourceObj(PPTIndirectObj *obj, string resource_type);

	bool HasElements() {return (numberOfElements() > 0 ? true : false);}

	PPContext *ptContext() {return _context;}
	void SetValueToGState(PPTCommand *cmd, PPContext &gcontext);
	void AddCommandToPath(PPTCommand *cmd, PPPath *path);
	PPTStream *BuildStream();
	PPFormBase *NewFormObj(PPFormBase *form_obj); //CutOpt: Additional.cpp 에서 사용중 

	string SubtypeFor(string name);
	PPLayer *BeginLayer(char *layerName);
	void EndLayer();
	void BeginReadLayer(char *layer_name);
};

#endif /* defined(__PDFPlusLib__PPFormBase__) */