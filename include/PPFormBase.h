

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


// PDF ���� �߿� Form�� ����� ����Ѵ�.
// PPPage�� ����Ŭ�����̱⵵ �ϴ�.
class PPFormBase : public PPBase {
protected:
	PPContext *_context;
public:
	PPTName *_form_key;

    PPDocument *_document;
    PPCommandParser _graphicParser;  // form ���� �ִ� �׷��� ��Ʈ���� �Ľ��ϱ� ����.

	size_t _cur_element_idx; // next() �Լ���. (���� Layer)->_elements ���� ���� �ε��� ��ġ.
    PPTIndirectObj *_indirObj;  // XObject

	// ExtGState, ColorSpace, Pattern, Shading, XObject, Font, ProcSet, Properties 
    PPTDictionary *_resourceDict;
	PPTDictionary *_formDict;

    vector <PPTCommand *> _commands;

	// _layers���� �ּ��� �ϳ��� ���̾ �����ؾ� �Ѵ�.
	vector <PPLayer *> _layers;

	// _layers�� �ΰ� �̻��� ���̾ ���� ��� �����(������)�� ���� ���̾ ������ �� �ִ�.
	// AddElement���� �Լ��� �� _curLayer�� ����ȴ�.
	// _curLayer �� null�� ��� ù��° ���̾ 
	PPLayer *_curLayer;
    
public:
    PPFormBase();
	PPFormBase(PPFormBase *form_base);
	PPFormBase(PPDocument *doc, PPTIndirectObj *indir);

	~PPFormBase();

	PPParser *DocumentParser();

	// virtual methods
    virtual int BuildElements();
	virtual PPToken *ResourceForKey(int obj_num);
	virtual PPToken *WriteResource(PPToken *rcs, int obj_num);

	// Layer Related Methods
	PPLayer *AddLayerWithProperties(string property_name);
	PPLayer *AddLayer(string layer_name);
	PPLayer *LayerForName(string name, int *idx = NULL);
	PPLayer *BeginLayer(char *layerName);
	void EndLayer();
	void BeginReadLayer(char *layer_name);
	void ReorderLayer(int from_idx, int to_idx);
	void MergeLayer(string tar, string src);

	// Element Related Methods
    void AddElement(PPElement *element);
	void WriteElement(PPElement *element);
	size_t NumberOfElements();
	PPElement *ElementAtIndex(int idx);

	void AddXObjRef(PPTIndirectObj *xobj, string key);
	void AddFormObj(PPFormBase *form_obj);

	void ResetCurrentIndex() {_cur_element_idx = 0;}
	PPElement *next(); PPElement *Next() {return next();}
	PPElement *first() {ResetCurrentIndex(); return next();} 

	int ObjectNumber(){return _indirObj != NULL ? _indirObj->ObjNum() : 0;}
    int XObjectNumberFor(string name);
	PPTIndirectObj *XObject();

	// Resources Related Methods
	PPTIndirectRef *ResourceForKey(string key);
	PPTDictionary *ResourceDictForKey(string key);
	PPTIndirectObj *ResourceObjForName(string name, string resource_type);
	string NameFromResourceObj(PPTIndirectObj *obj, string resource_type);

	bool HasElements() {return (NumberOfElements() > 0 ? true : false);}

	PPContext *ptContext() {return _context;}
	void SetValueToGState(PPTCommand *cmd, PPContext &gcontext);
	void AddCommandToPath(PPTCommand *cmd, PPPath *path);
	PPTStream *BuildStream();
	PPFormBase *NewFormObj(PPFormBase *form_obj); //CutOpt: Additional.cpp ���� ����� 

	string SubtypeFor(string name);

	// Layer Related Methods

};

#endif /* defined(__PDFPlusLib__PPFormBase__) */