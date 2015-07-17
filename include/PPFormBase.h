

#ifndef __PDFPlusLib__PPFormBase__
#define __PDFPlusLib__PPFormBase__

#include <PPBase.h>
#include <PPCommandParser.h>
#include <PPTIndirectObj.h>
//
//      PPEFormBase
//
///////////////////////////////////////////////////////////////
class PPToken;
class PPDocument;
class PPElement;
class PPContext;
class PPTIndirectObj;
class PPTDictionary;
class PPTCommand;



/*
	<Object ID='2' Gen='0' Pos='886486'>
		<Dict>
			<Key>BBox</Key>
			<Array>
				<Number>5</Number>
				<Number>5</Number>
				<Number>2029.6</Number>
				<Number>364.11</Number>
			</Array>
			<Key>Filter</Key>
			<Name>FlateDecode</Name>
			<Key>Length</Key>
			<Number>14733</Number>
			<Key>Matrix</Key>
			<Array>
				<Number>1</Number>
				<Number>0</Number>
				<Number>0</Number>
				<Number>1</Number>
				<Number>-5</Number>
				<Number>-5</Number>
			</Array>
			<Key>Resources</Key>
			<Ref ObjID='143' Gen='0'/>
			<Key>Subtype</Key>
			<Name>Form</Name>
			<Key>Type</Key>
			<Name>XObject</Name>
		</Dict>
		<Stream>
			¡¦
		</Stream>
	</Object>
*/


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
    vector <PPElement *> _elements;
    
public:
    PPFormBase();
	PPFormBase(PPFormBase *form_base);
	PPFormBase(PPDocument *doc, PPTIndirectObj *indir);

	PPParser *documentParser();
    virtual int buildElements();
	virtual PPToken *ResourceForKey(int obj_num);
	virtual PPToken *WriteResource(PPToken *rcs, int obj_num);


    void addElement(PPElement *element);
	void AddElement(PPElement *element) {addElement(element);}
//	void AddXObj(PPTIndirectObj *xobj);
	void AddXObjRef(PPTIndirectObj *xobj, string key);
	void AddFormObj(PPFormBase *form_obj);
	PPElement *next(); PPElement *Next() {return next();}
	void writeElement(PPElement *element);
	void WriteElement(PPElement *element) {writeElement(element);}
	int getObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
	int GetObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
    int GetXObjNumOf(string name);
	PPTIndirectObj *GetXObject();
	PPTIndirectRef *ResourceForKey(string key);
	bool HasElements() {return (_elements.size() > 0 ? true : false);}

	PPContext *ptContext() {return _context;}
	void SetValueToGState(PPTCommand *cmd, PPContext &gcontext);
	void AddCommandToPath(PPTCommand *cmd, PPPath *path);
	PPTStream *BuildStream();
	PPFormBase *NewFormObj(PPFormBase *form_obj);

	string SubtypeFor(string name);
};

#endif /* defined(__PDFPlusLib__PPFormBase__) */