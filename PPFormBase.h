

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

class PPFormBase : public PPBase {
protected:
		PPContext *_context;
public:
    PPDocument *_document;
    PPCommandParser _graphicParser;

	size_t _cur_element_idx;

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
    PPTIndirectObj *_indirObj;  // XObject

    PPTDictionary *_resourceDict;
    vector <PPTCommand *> _commands;
    vector <PPElement *> _elements;
    
public:
    PPFormBase();
	PPFormBase(PPFormBase *form_base);
	PPFormBase(PPTIndirectObj *indir);

	PPParser *documentParser();
    virtual int buildElements();
    void addElement(PPElement *element);
	void AddElement(PPElement *element) {addElement(element);}
	void AddXObj(PPTIndirectObj *xobj);
	void AddFormObj(PPFormBase *form_obj);
	PPElement *next(); PPElement *Next() {return next();}
	void writeElement(PPElement *element);
	void WriteElement(PPElement *element) {addElement(element);}
	int getObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
	int GetObjNum(){return _indirObj != NULL ? _indirObj->getObjNum() : 0;}
    int GetXObjNumOf(string name);
	PPTIndirectObj *GetXObject();
	bool HasElements() {return (_elements.size() > 0 ? true : false);}

	PPContext *ptContext() {return _context;}

	PPTStream *BuildStream();
};

#endif /* defined(__PDFPlusLib__PPFormBase__) */