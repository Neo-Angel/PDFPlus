


#include <vector>
#include <iostream>
#include <sstream>

#include "PPFormBase.h"
#include "PPToken.h"
#include "PPImage.h"
#include "PPElement.h"
#include "PPTStream.h"
#include "PPContext.h"
#include "PPEText.h"
#include "PPETextState.h"
#include "PPEInlineImage.h"
#include "PPEGSave.h"
#include "PPEGRestore.h"
#include "PPPath.h"
#include "PPEPath.h"
#include "PPEImage.h"
#include "PPEForm.h"
#include "PPEBeginMarkedContent.h"
#include "PPEMarkedContent.h"
#include "PPEEndMarkedContent.h"
#include "PPEShading.h"
#include "PPEBeginCompatibility.h"
#include "PPEEndCompatibility.h"
#include "PPDocument.h"

#include "PPTString.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"
#include "PPLayer.h"
#include "PPCommandParser.h"
// =========================================================== PPContext - E

//
//  FormBase
//
////////////////////////////////////////////////////////////////////////////
PPFormBase::PPFormBase():_graphicParser((vector <PPToken *> *)&_commands) // _graphicParser 초기화
{
	_parentForm = NULL;
	_document = NULL;
	_cur_element_idx = 0; // iterating 용 
	_indirObj = NULL;
	_resourceDict = NULL;
	_curLayer = NULL;

	// 기본적으로 최소한 1개의 레이어가 있어야 한다.
	PPLayer *layer = new PPLayer(); // 사실 레이어라기보다 OCG 임 
	_layers.push_back(layer);

	_context = new PPContext(this);

}

PPFormBase::PPFormBase(PPFormBase *form_base):_graphicParser((vector <PPToken *> *)&_commands)
{
	_parentForm = NULL;
    _document = NULL; //  : this will be set when this added to document

	_cur_element_idx = 0; // size_t 

	_indirObj = (PPTIndirectObj *)form_base->_indirObj->Copy(); //PPTIndirectObj *
    _resourceDict = NULL; //PPTDictionary *
	_curLayer = NULL;

	// 기본적으로 최소한 1개의 레이어가 있어야 한다.
	PPLayer *layer = new PPLayer();
	_layers.push_back(layer);

	_context = new PPContext(this);
}

PPFormBase::PPFormBase(PPDocument *doc, PPTIndirectObj *indir):_graphicParser((vector <PPToken *> *)&_commands)
{
	// 기본적으로 최소한 1개의 레이어가 있어야 한다.
	PPLayer *layer = new PPLayer();
	_layers.push_back(layer);

	_parentForm = NULL;
	_document = doc;
	_indirObj = indir; 
	_formDict = _indirObj->FirstDictionary();
	PPTName *subtype_name = (PPTName *)_formDict->ValueObjectForKey("Subtype"); 
	_cur_element_idx = 0; // size_t 

	_resourceDict = (PPTDictionary *)_formDict->ValueObjectForKey("Resources"); 

	if (*subtype_name->_name == "Form") {
		PPTStream *stream = _indirObj->Stream();
		if(stream) {
			_graphicParser.ParseStream(*stream);
			BuildElements();
		}
		else {
			cout << "Created empty FormBase." << PP_ENDL;
		}
	}
	_curLayer = NULL;

	_context = new PPContext(this);
}

PPFormBase::~PPFormBase()
{
//	PPBase::~PPBase();

	// layer들은 이 클래스에서 new 로 생성 된 것들이라 지워줘야 함.
	size_t i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		delete layer;
	}
	delete _context;
}

// form_obj를 기반으로 새로운 Form 객체를 만든다. 
PPFormBase *PPFormBase::NewFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *indir = (PPTIndirectObj *)form_obj->_indirObj->Copy();
	if(indir->NumberOfTokens() == 2) {
		indir->Erase(indir->Begin()+1);
	}

	PPFormBase *ret_form = NULL;
	if(indir->NumberOfTokens() == 1) {
		indir->MoveInto(_document); // 검색되지 않토록 변형
		int new_obj_num = _document->NewObjNum();
		indir->_objNum = new_obj_num;
		_document->PushObj(indir, new_obj_num);
		ret_form = new PPFormBase(_document, indir);
		ret_form->_form_key = (PPTName *)form_obj->_form_key->Copy();
	}
	return ret_form;
}

PPParser *PPFormBase::DocumentParser()
{
	return &_document->_parser;
}

///////////////////////////////////////////////////////////////////////
// Resource 관련 함수들
///////////////////////////////////////////////////////////////////////
PPToken *PPFormBase::ResourceForKey( int obj_num){
	return _document->ResourceForExtObjNum( obj_num);
}

PPToken *PPFormBase::WriteResource(PPToken *rcs, int obj_num) {
	return _document->WriteResource(rcs,  obj_num);
}

int PPFormBase::XObjectNumberFor(string name)
{
	if(_resourceDict == NULL) {
		return 0;
	}
    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("XObject");
    if (xobject_dict) {
        //map <string, PPToken *> &dict = xobject_dict->_dict;
		PPTIndirectRef *indir_ref = (PPTIndirectRef *)xobject_dict->ObjectForKey(name);
        int obj_num = indir_ref->_objNum;
        return obj_num;
    }
    return 0;
}

PPTIndirectObj *PPFormBase::XObject()
{
	return _indirObj; 
}

PPTDictionary *PPFormBase::ResourceDictForKey(string key)
{
	if (_resourceDict == NULL)
		return NULL;

	PPTDictionary *ret = (PPTDictionary *)_resourceDict->ValueObjectForKey(key);
	return ret;
}

PPTIndirectRef *PPFormBase::ResourceForKey(string key)
{
	if (_resourceDict == NULL)
		return NULL;

	PPTIndirectRef *ret = (PPTIndirectRef *)_resourceDict->ObjectForKey(key);
	return ret;
}

PPTIndirectObj *PPFormBase::ResourceObjForName(string name, string resource_type)
{
	PPTDictionary *dict = ResourceDictForKey(resource_type);
	if(dict == NULL) {
		return NULL;
	}
	PPTIndirectObj *obj = dict->IndirectObjectForKey(name);
	return obj;
}

string PPFormBase::NameFromResourceObj(PPTIndirectObj *obj, string resource_type)
{
	int obj_num = obj->_objNum;
	PPTDictionary *rsc_dict = ResourceDictForKey(resource_type);
	if(rsc_dict == NULL) {
		rsc_dict = new PPTDictionary(_document);
		_resourceDict->SetTokenAndKey(rsc_dict, resource_type);
	}
    map <string, PPToken *>::iterator it_dict;
	vector <string> namelist;

	for(it_dict = rsc_dict->Begin(); it_dict != rsc_dict->End(); it_dict++) {
        PPTIndirectRef *rsc_ref = (PPTIndirectRef *)it_dict->second;
		string name = it_dict->first;
		if(rsc_ref->_objNum == obj->_objNum) {
			return name;
		}
		namelist.push_back(name);
    }


	char pname[10] = "RC0";
	char *format = "RC%d";
	if(resource_type == "XObject") {
		strcpy(pname, "IM0");
		format = "IM%d";
	}

	int name_idx = 0;
	size_t i, icnt = namelist.size();
	for(i=0;i<icnt;i++) {
		string name = namelist[i];
		if(name == pname) {
			name_idx ++;
			sprintf(pname, format,name_idx);
			i = 0;
			continue;
		}
	}
	
//	PPTIndirectRef *new_rsc_ref = new PPTIndirectRef(_document, obj->_objNum, obj->_genNum);
	rsc_dict->SetRefTokenAndKey(obj, pname/*"IM%d"*/, obj->_objNum);

	return pname;
}

PPTIndirectRef *PPFormBase::AddResourceRef(int ref_num, string name, string resource_type)
{
	PPTDictionary *rsc_dict = ResourceDictForKey(resource_type);
	if(rsc_dict == NULL) {
		rsc_dict = new PPTDictionary(_document);
		_resourceDict->SetTokenAndKey(rsc_dict, resource_type);
	}
	PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc_dict->ObjectForKey(name);
	if(rsc_ref == NULL) {
		rsc_ref = new PPTIndirectRef(_document, ref_num, 0);
		rsc_dict->SetTokenAndKey(rsc_ref, name);
		return rsc_ref;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Layer 관련 함수들
///////////////////////////////////////////////////////////////////////
PPLayer *PPFormBase::AddLayerWithProperties(string property_name)
{
	PPTDictionary *properties_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("Properties");
	PPTDictionary *layer_dict = (PPTDictionary *)properties_dict->ValueObjectForKey(property_name);
	if(layer_dict) {
		PPTString *str_obj = (PPTString *)layer_dict->ObjectForKey("Name");
        if(str_obj) {
            string *layer_name = str_obj->_string;
            PPLayer *layer = this->LayerForName(*layer_name);
            if(layer == NULL) {
                layer = new PPLayer();
                layer->_layer_dict = layer_dict;
                layer->_properties = property_name;
                layer->_parent = this;
                _layers.push_back(layer);
            }
            return layer;
        }
        PPTDictionary *meta_dict = (PPTDictionary *)layer_dict->ValueObjectForKey("Metadata");
        if(meta_dict) {
            PPTIndirectObj *meta_obj = (PPTIndirectObj *)meta_dict->ValueObjectForKey("Metadata");
            if(meta_obj) {
                cout << "'" << property_name << "' has Metadata!" << PP_ENDL;
            }
        }
        
	}
	return NULL;
}

// 현재의 폼 안에 레이어를 추가하는 함수
// 그러나 도큐먼트 레이어 정보 리스트에 없는 레이어를 추가할 경우
// 오류가 날 것으로 예상됨. 수정 바람.
PPLayer *PPFormBase::AddLayer(string layer_name, PPLayer *layer)
{
	// this 객체 안에 layer_name의 레이어가 없다고 간주하고 시작함.
	PPTIndirectObj *layer_obj = _document->LayerObjForName(layer_name); // 도큐먼트의 레이어 정보 리스트에서
	/* layer_obj가 NULL일 경우에 대한 처리가 없음(오류). */				// layer_name 의 레이어 정보를 찾음
																		

	PPTIndirectRef *layer_ref = new PPTIndirectRef(_document, layer_obj->_objNum, layer_obj->_genNum);

	PPLayer *ret_layer = NULL;
	// Properties 리소스가 필요하다.
	PPTDictionary *properties_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("Properties");
	if(properties_dict == NULL) {
		// 없으면 만들어 주고...
		properties_dict = new PPTDictionary(_document);
		_resourceDict->SetTokenAndKey(properties_dict, "Properties");
	}

	// 새 properties key 만들기 
	char pname[10] = "LP0";
	int name_idx = 0;
	size_t i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		if(layer->_properties == pname) {
			name_idx ++;
			sprintf(pname, "LP%d",name_idx);
			i = 0;
			continue;
		}
	}

	// properties_dict 에 properties key 레이어 정보의 참조를 pname을 키로해서 저장.
	properties_dict->SetTokenAndKey(layer_ref, pname);
	layer_obj->AddRefObj(layer_ref);  // IndirectObj는 항상 자신을 참조한 ref를 저장해서 관리한다.
	PPTDictionary *layer_dict = (PPTDictionary *)layer_ref->ValueObject();
	if(layer == NULL) { 
		ret_layer = new PPLayer();
	}
	else {
		ret_layer = layer;
	}
	ret_layer->_layer_dict = layer_dict; // 
	ret_layer->_parent = this;
	ret_layer->ChangeProperties(pname);
	_layers.push_back(ret_layer);

	return ret_layer;
}

PPLayer *PPFormBase::BeginLayer(char *lname) 
{
	_curLayer = this->LayerForName(lname);
	if(_curLayer == NULL) {
		_curLayer = this->AddLayer(lname);
	}
	PPEBeginMarkedContent *begin_mark = new PPEBeginMarkedContent(_document, _curLayer->_properties, ContextRef());

	WriteElement(begin_mark);
	return _curLayer;
}

void PPFormBase::EndLayer()
{
	PPEEndMarkedContent *end_mark = new PPEEndMarkedContent(ContextRef());
	WriteElement(end_mark);
	_curLayer = NULL;
}

PPLayer *PPFormBase::LayerForName(string name, int *idx)
{
	size_t icnt = _layers.size();
	uint i;
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		if(layer->IsEqualName(name)) {
			if (idx)
				*idx = i;
			return layer;
		}
	}
	return NULL;
}

void PPFormBase::ReorderLayer(int from_idx, int to_idx)
{
	from_idx++;
	to_idx++;
	PPLayer *layer = _layers[from_idx];
	_layers.erase(_layers.begin() + from_idx);
	_layers.insert(_layers.begin()+to_idx, layer);
}

// 레이어 내의 요소들을 이동시키는 예제로도 활용 가능
void PPFormBase::MergeLayer(string tar_name, string src_name)
{
	int src_idx;
	int tar_idx;
	PPLayer *tar = LayerForName(tar_name, &tar_idx);
	PPLayer *src = LayerForName(src_name, &src_idx);

	size_t i, icnt = src->_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = src->_elements[i];
		if(element->Type() != PPET_MARKED_CONTENT
			&& element->Type() != PPET_END_MARKED_CONTENT
			&& element->Type() != PPET_BEGIN_MARKED_CONTENT) {
				size_t tar_cnt = tar->_elements.size();
				// src 레이어의 Marked Contents 관련 엘리먼트들을 제외하고
				// tar 레이어의 마지막 바로 앞에 인서트를 한다.
				// tar 레이어의 마지막에 있는 Marked content end  엘리먼트를 
				// 유지해 줘야 하기 때문이다.
				tar->_elements.insert(tar->_elements.begin() + tar_cnt - 1, element);
		}
	}

	_layers.erase(_layers.begin() + src_idx);

}

///////////////////////////////////////////////////////////////////////
// Element 관련 함수들
///////////////////////////////////////////////////////////////////////

void PPFormBase::InsertElement(PPElement *element, int dir)
{
    element->WillAddToParent(this);
	element->SetParser(&_document->_parser);

	PPLayer *layer;
	if(_curLayer != NULL) {
		layer = _curLayer;
	}
	else {
		layer = _layers[0];
	}
	//layer->_elements.push_back(element);
	layer->_elements.insert(layer->_elements.end() + dir, element);
}

void PPFormBase::AddElement(PPElement *element)
{
    element->WillAddToParent(this);
	element->SetParser(&_document->_parser);

	PPLayer *layer;
	if(_curLayer != NULL) {
		layer = _curLayer;
	}
	else {
		layer = _layers[0];
	}
	layer->_elements.push_back(element);

	// update _commands list from element 
}

size_t PPFormBase::NumberOfElements()
{
	size_t tot_cnt = 0;
	size_t i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		size_t el_cnt = _layers[i]->_elements.size();
		tot_cnt += el_cnt;
	}
	return tot_cnt;
}

PPElement *PPFormBase::ElementAtIndex(size_t idx)
{
	uint i, tot_cnt = 0;
	size_t icnt = _layers.size();

	for(i=0;i<icnt;i++) {
		size_t el_cnt = _layers[i]->_elements.size();
		if(idx < (tot_cnt+el_cnt)) {
			int r_idx = (uint)idx - tot_cnt;
			return _layers[i]->_elements[r_idx];
		}
		tot_cnt += (uint)el_cnt;
	}
	return NULL;
}

void PPFormBase::WriteElement(PPElement *src_element)
{
	if(src_element->_parentForm == NULL) { // 신규 엘리먼트로 간주
		AddElement(src_element);
	}
	else { // 다른곳에 속해있던 엘리먼트로 간주하고 객체를 복사함.
		PPElement *copied = (PPElement *)src_element->Copy();
		AddElement(copied);
	}
	// src_element 에 리소스 정보가 있으면
	if(src_element->HasResource()) {
		// src_element의 리소스 '타입'과 '키'로 this에 리소스가 있는지 체크
		vector <const char *> type_list = src_element->ResourceTypeList();
		size_t i, icnt = type_list.size();
		// 한개의 엘리먼트에 여려 종류(icnt)의 리소스가 있을 수 있음.
		for(i=0;i<icnt;i++) {
			const char *rsc_type = type_list[i]; //src_element->ResourceType();
			if(rsc_type == PPRT_PROPERTIES) {
				cout << " PPRT_PROPERTIES.." << PP_ENDL;
			}
			if(rsc_type == PPRT_PROSET) {
			}
			else {
				do {
					PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->ObjectForKey(rsc_type);
					if(!rsc_dict) {
						rsc_dict = new PPTDictionary(_document);
						_resourceDict->SetTokenAndKey(rsc_dict, rsc_type);
					}
					string rsc_key = src_element->ResourceKeyFor(rsc_type);
					int src_obj_num = src_element->ResourceObjNum(rsc_type);
					if(src_obj_num == 0) {
						cout << " External Resource Object number cannot be Zero." << PP_ENDL;
						break;
					}
					PPToken *rsc = _document->ResourceForExtObjNum(src_obj_num); //ExtObjNum == src_obj_num

					if(rsc) {
						PPTIndirectObj *obj = (PPTIndirectObj *)rsc;
						rsc_dict->SetRefTokenAndKey(obj, rsc_key, obj->_objNum);
					}
					else  {
						// 없으면 리소스 복사
						PPToken *src_rsc = src_element->ResourceObjectFor(rsc_type);
						if(src_rsc) {
							rsc = WriteResource(src_rsc, src_obj_num);  // _resources
							if(rsc) {
								_document->SetRefTokenForKey(rsc_dict,rsc,rsc_key); //rsc_dict, _tokens, parser->_objDict
								// PPElement::willAddToParent 에서 해결할 수 없었을까? 추후 검토.
								if(rsc_type == PPRT_PROPERTIES && _document->_OCProperties == NULL) { // OCG(Layer)
									PPDocument *src_doc = src_element->_parentForm->_document;
									PPTDictionary *src_ocproperties = src_doc->_OCProperties;
									if(src_ocproperties) {
										_document->WriteOCProperties(src_ocproperties);  //###
									}
								}
							}
						}
					}
				}while(0);
			}
		}
	}
}

void PPFormBase::WritePlacedElement(PPElement *src_element)
{
	PPElement *gsave_element = new PPEGSave();
	this->WriteElement(gsave_element);
	this->WriteElement(src_element);

	PPElement *grestore_element = new PPEGRestore();
	this->WriteElement(grestore_element);

}


/* 여러 GState 의 값들 중에 cmd 내용에 부합하는 값을 변경한다. */
/* PPContext는 PPGState의 서브클래스다. */
void PPFormBase::SetValueToGState(PPTCommand *cmd, PPContext &gcontext)
{
	PPCommandInfo *cmdinfo = cmd->_cmdInfo;
	unsigned int g_code = cmdinfo->code;
    switch (g_code) {
        case PPC_LineWidth:
            gcontext.SetLineWidth(cmd->FloatValueAt(0));
            break;
        
        case PPC_LineCap:
            gcontext.SetLineCap(cmd->IntValueAt(0));
            break;
        
        case PPC_LineJoin:
            gcontext.SetLineJoin(cmd->IntValueAt(0));
            break;
        
        case PPC_MiterLimit:
            gcontext.SetMiterLimit(cmd->FloatValueAt(0));
            break;
        
        case PPC_Dash:
        {
            PPDash dash;
            cmd->GetDash(&dash);
            gcontext.SetDash(dash);
            break;
        }
        
        case PPC_Intent:
            gcontext.SetIntent(cmd->StringValueAt(0));
            break;
        
        case PPC_Flatness:
            gcontext.SetFlatness(cmd->FloatValueAt(0));
            break;
        
        case PPC_DictName:
            gcontext.SetDictName(cmd->StringValueAt(0));
            break;
        
        case PPC_Matrix:
        {
            PPMatrix mtx;
            mtx._a = cmd->FloatValueAt(0);
            mtx._b = cmd->FloatValueAt(1);
            mtx._c = cmd->FloatValueAt(2);
            mtx._d = cmd->FloatValueAt(3);
            mtx._x = cmd->FloatValueAt(4);
            mtx._y = cmd->FloatValueAt(5);
            gcontext.SetMatrix(mtx);
            break;
        }
        
        case PPC_StrokeColorSpace:
			{
				string color_name = cmd->StringValueAt(0);

				PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("ColorSpace");
				PPTArray *rsc_arr = (PPTArray *)rsc_dict->ValueObjectForKey(color_name);
				gcontext.SetUserStrokeColorSpace(color_name, rsc_arr);
			}
            break;
        
        case PPC_NonStrokeColorSpace:
			{
				string color_name = cmd->StringValueAt(0);
				PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("ColorSpace");
				PPTArray *rsc_arr = (PPTArray *)rsc_dict->ValueObjectForKey(color_name);
				gcontext.SetUserFillColorSpace(color_name, rsc_arr);
			}
            break;
        
			// Set Color
        case PPC_SetColor: 
			gcontext.SetStrokeColor(cmd->_operands);
			break;
        case PPC_SetColorN:
			gcontext.SetStrokeColorN(cmd->_operands);
			break;
        case PPC_SetNonStrokeColor:
			gcontext.SetFillColor(cmd->_operands);
			break;

        case PPC_SetNonStrokeColorN:
			gcontext.SetFillColorN(cmd->_operands);
			break;

        case PPC_DeviceGray:
            gcontext.SetStrokeColorSpace(PPCSN_DeviceGray);
            gcontext.SetStrokeColor(cmd->FloatValueAt(0));
            break;
        
        case PPC_DeviceRGB:
            gcontext.SetStrokeColorSpace(PPCSN_DeviceRGB);
            gcontext.SetStrokeColor(cmd->FloatValueAt(0),cmd->FloatValueAt(1),cmd->FloatValueAt(2));
            break;
        
        case PPC_DeviceCMYK:
            gcontext.SetStrokeColorSpace(PPCSN_DeviceCMYK);
            gcontext.SetStrokeColor(cmd->FloatValueAt(0),cmd->FloatValueAt(1),cmd->FloatValueAt(2), cmd->FloatValueAt(3));
            break;
            
        case PPC_NonStrokeDeviceGray:
            gcontext.SetFillColorSpace(PPCSN_DeviceGray);
            gcontext.SetFillColor(cmd->FloatValueAt(0));
            break;
        
        case PPC_NonStrokeDeviceRGB:
            gcontext.SetFillColorSpace(PPCSN_DeviceRGB);
            gcontext.SetFillColor(cmd->FloatValueAt(0),cmd->FloatValueAt(1),cmd->FloatValueAt(2));
            break;
        
        case PPC_NonStrokeDeviceCMYK:
            gcontext.SetFillColorSpace(PPCSN_DeviceCMYK);
            gcontext.SetFillColor(cmd->FloatValueAt(0),cmd->FloatValueAt(1),cmd->FloatValueAt(2), cmd->FloatValueAt(3));
            break;
        
        default:
            break;
    }
}

// path 객체에 cmd 에 있는 패스 정보를 더한다.
void PPFormBase::AddCommandToPath(PPTCommand *cmd, PPPath *path)
{
    switch (cmd->_cmdInfo->code) {
        case PPC_MoveTo:
            path->MoveTo(cmd->FloatValueAt(0), cmd->FloatValueAt(1));
            break;
        case PPC_LineTo:
            path->LineTo(cmd->FloatValueAt(0), cmd->FloatValueAt(1));
            break;
        case PPC_CurveTo:
            path->CurveTo(cmd->FloatValueAt(0), cmd->FloatValueAt(1), cmd->FloatValueAt(2), 
				cmd->FloatValueAt(3), cmd->FloatValueAt(4), cmd->FloatValueAt(5));
            break;
        case PPC_CurveTo1:
            path->CurveTo1(cmd->FloatValueAt(0), cmd->FloatValueAt(1), cmd->FloatValueAt(2), cmd->FloatValueAt(3));
            break;
        case PPC_CurveTo2:
            path->CurveTo2(cmd->FloatValueAt(0), cmd->FloatValueAt(1), cmd->FloatValueAt(2), cmd->FloatValueAt(3));
            break;
        case PPC_ClosePath:
            path->Close();
            break;
        case PPC_Rectangle:
            path->Rectangle(cmd->FloatValueAt(0), cmd->FloatValueAt(1), cmd->FloatValueAt(2), cmd->FloatValueAt(3));
            break;
        default:
            break;
    }
}

string PPFormBase::SubtypeFor(string name)
{
	string subtype;
	do {
		PPTIndirectRef *xobj_ref = (PPTIndirectRef *)ResourceForKey("XObject");
		if (!xobj_ref) {
			cout << "XObje IndirectRef not found..." << PP_ENDL;
			break;
		}
		PPTDictionary *xobj_dict = NULL;
		if(xobj_ref->ClassType() == PPTN_INDIRECTREF) {
			xobj_dict = (PPTDictionary *)xobj_ref->ValueObject();
			if (!xobj_dict) {
				cout << "Shading Dictionary not found..." << PP_ENDL;
				break;
			}
		}
		else if(xobj_ref->ClassType() == PPTN_DICTIONARY) {
			xobj_dict = (PPTDictionary *)xobj_ref;
		}
		xobj_ref = (PPTIndirectRef *)xobj_dict->ObjectForKey(name);
		if (!xobj_ref) {
			cout << "Shading IndirectRef not found..." << PP_ENDL;
			break;
		}
		PPTIndirectObj *xobj = (PPTIndirectObj *)xobj_ref->TargetObject();
		if (!xobj) {
			cout << "Shading Resource Object not found..." << PP_ENDL;
			break;
		}
		xobj_dict = xobj->FirstDictionary();
		if(xobj_dict == NULL) {
			break;
		}
		PPTName *subtype_name =  (PPTName *)xobj_dict->ObjectForKey("Subtype");
		if(subtype_name == NULL) {
			break;
		}
		subtype = *subtype_name->_name;
	} while(0);
	return subtype;
}

// 파싱된 _commands 로 elements 리스트(layer->_elements)를 구축한다.
int PPFormBase::BuildElements()
{
    PPContext gcontext(this);
    PPPath *opened_path = NULL;
    PPEPath *path_element = NULL;
    PPEText *text_element = NULL;
	PPETextState *textstate_element = NULL;
    PPEInlineImage *inline_img_element = NULL;

    size_t i = 0, icnt = _commands.size();
    while (i<icnt) {
        PPTCommand *cmd = _commands[i];
        if(textstate_element != NULL) {
			switch (cmd->_cmdInfo->group) {
			case PPCG_TextState:
				break;
			default:
				textstate_element->SetGContext(&gcontext);
				AddElement(textstate_element);
				gcontext.ClearGFlags();
				textstate_element = NULL;
			}
		}

        switch (cmd->_cmdInfo->group) {
            case PPCG_GState:
				if(text_element) {
					text_element->AddCommand(cmd);
				}
				else {
					SetValueToGState(cmd, gcontext);
				}
                break;
            case PPCG_SaveGState:
                {
                    // make SaveGStateElement and add to element list
                    gcontext.SaveGState();
                    PPEGSave *gsave = new PPEGSave(&gcontext);
                    AddElement(gsave);
					gcontext.ClearGFlags();
                }
                break;
            case PPCG_RestoreGState:
                {
                    // make RestoreGStateElement and add to element list
                    gcontext.RestoreGState();
                    PPEGRestore *grestore = new PPEGRestore(&gcontext);
                    AddElement(grestore);
					gcontext.ClearGFlags();
                }
                break;
            case PPCG_DrawPath:
                if (opened_path == NULL) {
                    opened_path = new PPPath;
                }
                if (path_element != NULL) {
                    path_element = NULL;
                }

				AddCommandToPath(cmd, opened_path);
                break;
            case PPCG_FinishPath:
            case PPCG_Clipping:
                if (opened_path != NULL) {
                    // make path element and add to element list
                    path_element = new PPEPath(opened_path, &gcontext);
					gcontext.ClearGFlags();
                    AddElement(path_element);
                    opened_path = NULL; 
                }
                if (path_element != NULL) {
					path_element->SetPaintingType(cmd->_cmdInfo->code);
                }
                break;
                
            case PPCG_BeginText:
                text_element = new PPEText();
                break;
            case PPCG_Text:
				if(text_element == NULL) {
					text_element = new PPEText();
				}
                text_element->AddCommand(cmd);
                break;
            case PPCG_EndText:
                //                delete text_element;  //  You don't need to delete
				if(text_element != NULL) {
					text_element->SetGContext(&gcontext);

					AddElement(text_element);
					gcontext.ClearGFlags();
					text_element = NULL;
				}
                break;
            case PPCG_TextState:
				if(text_element) {
					text_element->AddCommand(cmd);
				}
				else {
					if(textstate_element == NULL) {
						textstate_element = new PPETextState();
					}
					textstate_element->AddCommand(cmd);
				}
                break;
            
            case PPCG_BeginInlineImage:
                inline_img_element = new PPEInlineImage(&gcontext);
                inline_img_element->SetBeginImageObject(cmd);
                break;
            case PPCG_InlineImage:
                inline_img_element->SetBeginImageData(cmd);
                break;
            case PPCG_EndInlineImage:
                AddElement(inline_img_element);
				gcontext.ClearGFlags();
                inline_img_element = NULL;
                break;
            
            case PPCG_XObject:
                {
					PPTName *name = (PPTName *)cmd->TokenValueAt(0);
					string name_str = *name->_name;

					string subtype = SubtypeFor(name_str);
					if(subtype == "Form") {
						PPEForm *form_element = new PPEForm(name, &gcontext);
						AddElement(form_element);
					}
					else if(subtype == "Image") {
						PPEImage *form_element = new PPEImage(name, &gcontext);
						AddElement(form_element);
					}
					gcontext.ClearGFlags();
                }
                break;
                
            case PPCG_BeginMarkedContent:
                {
					PPTName *tag = (PPTName *)cmd->TokenValueAt(0);
					PPToken *properties = NULL;
					if(cmd->_cmdInfo->code == PPC_BeginMarkedContentP) {
						properties = (PPToken *)cmd->TokenValueAt(1);
						if(properties->ClassType() == PPTN_NAME) {
							PPTName *property_name = (PPTName *)properties;
							PPLayer *layer = AddLayerWithProperties(*property_name->_name);
							if(i < icnt-1) {
								PPTCommand *next_cmd = _commands[i+1];
								PPCommandGroup next_group = next_cmd->_cmdInfo->group;
								// because illustrator has a bug.
								if(next_group == PPCG_RestoreGState && _curLayer) {
				                    gcontext.RestoreGState();
								    PPEGRestore *grestore = new PPEGRestore(&gcontext);
									this->InsertElement(grestore, -1);
//				                    AddElement(grestore);
									gcontext.ClearGFlags();
									i++;
								}
							}
							_curLayer = layer;
						}
					}
					PPEBeginMarkedContent *marked_content_element = new PPEBeginMarkedContent(tag, properties, &gcontext);;
					AddElement(marked_content_element);
					gcontext.ClearGFlags();
                }
                break;
            case PPCG_MarkedContent:
                {
                    PPTName *tag = (PPTName *)cmd->TokenValueAt(0);
                    PPTDictionary *porpert = (PPTDictionary *)cmd->TokenValueAt(1);
                    PPEMarkedContent *marked_content_element = new PPEMarkedContent(tag, porpert, &gcontext);
                    AddElement(marked_content_element);
                }
                break;
            case PPCG_EndMarkedContent:
                {
							if(i < icnt-1) {
								PPTCommand *next_cmd = _commands[i+1];
								PPCommandGroup next_group = next_cmd->_cmdInfo->group;
								// because illustrator has a bug.
								if(next_group == PPCG_RestoreGState && _curLayer) {
				                    gcontext.RestoreGState();
								    PPEGRestore *grestore = new PPEGRestore(&gcontext);
									this->AddElement(grestore);
//				                    AddElement(grestore);
									gcontext.ClearGFlags();
									i++;
								}
							}

                    PPEEndMarkedContent *marked_content_element = new PPEEndMarkedContent(&gcontext);
                    AddElement(marked_content_element);
					gcontext.ClearGFlags();
                }
                break;
            case PPCG_Shading:
                {
                    PPEShading *shading_element = new PPEShading(&gcontext);
					shading_element->_name = (PPTName *)cmd->TokenValueAt(0)->Copy();
                    AddElement(shading_element);
					gcontext.ClearGFlags();
                }
                break;
            
            case PPCG_BeginCompatibility:
				{
                    PPEBeginCompatibility *compatibility_element = new PPEBeginCompatibility(&gcontext);
                    AddElement(compatibility_element);
					gcontext.ClearGFlags();
				}
                break;
            case PPCG_EndCompatibility:
				{
                    PPEEndCompatibility *compatibility_element = new PPEEndCompatibility(&gcontext);
                    AddElement(compatibility_element);
					gcontext.ClearGFlags();
				}
                break;
                
            default:
				cout << " Unprocessed Command " << cmd->PDFString() << PP_ENDL;
                break;
        }
		i++;
    }
    return 0;
}

// element 리스트의 내용을 스트링으로 모아서 스트림으로 인코딩한다.
PPTStream *PPFormBase::BuildStream()
{
	string retstr; 
	ostringstream ostr;
	size_t i, icnt = NumberOfElements(); //_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = ElementAtIndex((uint)i); //_elements.at(i);
		string cmdstr = element->CommandString();
		retstr += cmdstr;
	}
	unsigned long size = (ulong)retstr.size();
	PPTStream *stream = new PPTStream(_document, size);
	const char *cstr = retstr.c_str();
	memcpy(stream->_streamData, cstr, size);
	stream->_decoded = true;
	return stream;
}

PPElement *PPFormBase::Next()  // iterating
{
	if (NumberOfElements() > 0 && _cur_element_idx < NumberOfElements()) {
		return ElementAtIndex(_cur_element_idx++);
	}
	_cur_element_idx = 0;
	return NULL;
}


void PPFormBase::AddXObjRef(PPTIndirectObj *xobj, string key) 
{
	 int obj_num = xobj->ObjNum();
	_document->_xobjects[obj_num] = xobj;
    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("XObject");
	if(xobject_dict == NULL) {
		xobject_dict = new PPTDictionary(_document);
		_resourceDict->SetTokenAndKey(xobject_dict, "XObject");
	}

	xobject_dict->SetRefTokenAndKey(xobj, key, xobj->_objNum);
}

void PPFormBase::AddFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *xobj = form_obj->XObject();
//	form_obj->_document = _document;
	PPTStream *stream = form_obj->BuildStream();
	stream->_infoDict = (PPTDictionary *)xobj->TokenAtIndex(0);
	stream->_infoDict->SetTokenAndKey(stream->_streamSize, "Length");
	stream->_parentObj = xobj;
	xobj->AddToken(stream);
	AddXObjRef(xobj, *form_obj->_form_key->_name);
	delete form_obj;
}

// Editing
void PPFormBase::AddImage(PPRect rect, PPImage *image)
{
//	_document->AddImage(image);
	PPEImage *ie = new PPEImage(image->ImagePath(), this->ContextRef());
	ie->SetTransform(rect.Width(), 0, 0, rect.Height(), rect.X1(), rect.Y1());
	this->WriteElement(ie);
}

void PPFormBase::AddImage(PPRect rect, string path)
{
	PPEImage *ie = new PPEImage(path, this->ContextRef());
	ie->SetTransform(rect.Width(), 0, 0, rect.Height(), rect.X1(), rect.Y1());
	this->WriteElement(ie);
}

void PPFormBase::AddText(PPRect rect, string text)
{

}

void PPFormBase::ReplaceString(string org_str, string new_str)
{
	size_t slen = org_str.length();
	size_t i, icnt = NumberOfElements(); //_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = ElementAtIndex(i); //_elements.at(i);
		if(element->Type() == PPET_TEXT) {
			PPEText *text_el = (PPEText *)element;
			string str = text_el->String();

			string::size_type offset = 0;
			offset = str.find(org_str, offset);

			if(offset != string::npos) {
				str.replace(offset, slen, new_str);
				text_el->SetString(str);
			}
		}
	}

}

void PPFormBase::ReplaceImage(PPRect area, string image_path)
{
	size_t i, icnt = NumberOfElements(); //_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = ElementAtIndex(i); //_elements.at(i);
		if(element->Type() == PPET_IMAGE) {
			PPEImage *img_el = (PPEImage *)element;
			PPRect rect = img_el->BBox();
			if(area.ContainsRect(rect) ) {
				img_el->SetImagePath(image_path);
			}
		}
	}
}
