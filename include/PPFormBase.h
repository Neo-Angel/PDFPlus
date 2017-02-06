

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
class PPRect;
class PPImage;

class PPToken;
class PPDocument;
class PPElement;

class PPTIndirectObj;
class PPTDictionary;
class PPTCommand;
class PPLayer;


// PDF 스펙 중에 Form의 기능을 담당한다.
// PPPage의 슈퍼클레스이기도 하다.
class PPFormBase : public PPBase {
protected:
	PPContext *_context;
public:
	PPTName *_form_key;

    PPDocument *_document;
    PPCommandParser _graphicParser;  // form 마다 있는 그래픽 스트림을 파싱하기 위함.

	size_t _cur_element_idx; // next() 함수용. (현재 Layer)->_elements 내의 현재 인덱스 위치.
    PPTIndirectObj *_indirObj;  // XObject

	// ExtGState, ColorSpace, Pattern, Shading, XObject, Font, ProcSet, Properties 
    PPTDictionary *_resourceDict;
	PPTDictionary *_formDict;

    vector <PPTCommand *> _commands;

	// _layers에는 최소한 하나의 레이어가 존재해야 한다.
	vector <PPLayer *> _layers;

	// _layers에 두개 이상의 레이어가 있을 경우 사용자(개발자)는 현재 레이어를 지정할 수 있다.
	// AddElement등의 함수는 이 _curLayer에 적용된다.
	// _curLayer 가 null일 경우 첫번째 레이어가... 
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
	////////////////////////////////////////////////////////////////////////////////////////////////
	PPLayer *AddLayerWithProperties(string property_name);
	PPLayer *AddLayer(string layer_name, PPLayer *layer = NULL);
	PPLayer *LayerForName(string name, int *idx = NULL);
	PPLayer *BeginLayer(char *layerName);
	void EndLayer();
	void ReorderLayer(int from_idx, int to_idx);
	void MergeLayer(string tar, string src);

	// Element Related Methods
	////////////////////////////////////////////////////////////////////////////////////////////////
    void AddElement(PPElement *element);
	void WriteElement(PPElement *element);
	void WritePlacedElement(PPElement *element);
	size_t NumberOfElements();
	PPElement *ElementAtIndex(size_t idx);

	void AddXObjRef(PPTIndirectObj *xobj, string key);
	void AddFormObj(PPFormBase *form_obj);

	void ResetCurrentIndex() {_cur_element_idx = 0;}
	PPElement *Next();
	PPElement *First() {ResetCurrentIndex(); return Next();} 

	int ObjectNumber(){return _indirObj != NULL ? _indirObj->ObjNum() : 0;}
    int XObjectNumberFor(string name);
	PPTIndirectObj *XObject();

	// Resources Related Methods
	PPTIndirectRef *ResourceForKey(string key);
	PPTDictionary *ResourceDictForKey(string key);
	PPTIndirectObj *ResourceObjForName(string name, string resource_type);
	string NameFromResourceObj(PPTIndirectObj *obj, string resource_type);

	bool HasElements() {return (NumberOfElements() > 0 ? true : false);}

	PPContext *ContextRef() {return _context;}
	void SetValueToGState(PPTCommand *cmd, PPContext &gcontext);
	void AddCommandToPath(PPTCommand *cmd, PPPath *path);
	PPTStream *BuildStream();
	PPFormBase *NewFormObj(PPFormBase *form_obj); //CutOpt: Additional.cpp 에서 사용중 

	string SubtypeFor(string name);

	// Editing

	void AddImage(PPRect rect, PPImage *image);
	void AddImage(PPRect rect, string path);
	void AddText(PPRect rect, string text);
	void ReplaceString(string org_str, string new_str);
	void ReplaceImage(PPRect area, string image_path);
};

#endif /* defined(__PDFPlusLib__PPFormBase__) */