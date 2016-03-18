


#include <vector>
#include <iostream>
#include <sstream>

#include "PPFormBase.h"
#include "PPToken.h"
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
#include "PPParser.h"

#include "PPTString.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"
#include "PPLayer.h"

// =========================================================== PPContext - E
//
//  FormBase
//
///////////////////////////////////////////////////////

PPFormBase::PPFormBase():_graphicParser((vector <PPToken *> *)&_commands)
{
	_cur_element_idx = 0;
	_indirObj = NULL;
	_resourceDict = NULL;
	_curLayer = NULL;

	PPLayer *layer = new PPLayer();
	_layers.push_back(layer);
}

PPFormBase::PPFormBase(PPFormBase *form_base):_graphicParser((vector <PPToken *> *)&_commands)
{
	_cur_element_idx = 0; // size_t 

	_indirObj = (PPTIndirectObj *)form_base->_indirObj->Copy(); //PPTIndirectObj *
    // _document; // PPDocument * : this set when this added to document
    _resourceDict = NULL; //PPTDictionary *
	_curLayer = NULL;
//    _commands; // vector <PPTCommand *>
//    _graphicParser; // PPCommandParser
	PPLayer *layer = new PPLayer();
	_layers.push_back(layer);

}

PPFormBase::PPFormBase(PPDocument *doc, PPTIndirectObj *indir):_graphicParser((vector <PPToken *> *)&_commands)
{
	PPLayer *layer = new PPLayer();
	_layers.push_back(layer);

	_document = doc;
	_indirObj = indir; //(PPTIndirectObj *)indir->Copy();
	_formDict = _indirObj->firstDictionary();
	PPTName *subtype_name = (PPTName *)_formDict->ValueObjectForKey("Subtype"); //objectForKey("Subtype");
	_cur_element_idx = 0; // size_t 

	_resourceDict = (PPTDictionary *)_formDict->ValueObjectForKey("Resources"); // objectForKey("Resources");

	if (*subtype_name->_name == "Form") {
		PPTStream *stream = indir->stream();
		if(stream) {
			_graphicParser.parseStream(*stream);
			buildElements();
		}
		else {
			cout << "Created empty FormBase." << PP_ENDL;
		}
	}
	_curLayer = NULL;
}

PPFormBase::~PPFormBase()
{
	PPBase::~PPBase();

	int i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		delete layer;
	}
}

// form_obj를 기반으로 새로운 Form 객체를 만든다.
//사용하는 데가 없어 봉인중 
PPFormBase *PPFormBase::NewFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *indir = (PPTIndirectObj *)form_obj->_indirObj->Copy();
	if(indir->_array.size() == 2) {
		indir->_array.erase(indir->_array.begin()+1);
	}

	PPFormBase *ret_form = NULL;
	if(indir->_array.size() == 1) {
		indir->MoveInto(_document); // 검색되지 않토록 변형
		int new_obj_num = _document->NewObjNum();
		indir->_objNum = new_obj_num;
		_document->PushObj(indir, new_obj_num);
		ret_form = new PPFormBase(_document, indir);
		ret_form->_form_key = (PPTName *)form_obj->_form_key->Copy();
	}
	return ret_form;
}


PPParser *PPFormBase::documentParser()
{
	return &_document->_parser;
}
PPToken *PPFormBase::ResourceForKey( int obj_num){
	return _document->ResourceForExtObjNum( obj_num);
}

PPToken *PPFormBase::WriteResource(PPToken *rcs, int obj_num) {
	return _document->WriteResource(rcs,  obj_num);
}

int PPFormBase::GetXObjNumOf(string name)
{
	// Page->Resource->XObject->Dict(name, xobj)
	if(_resourceDict == NULL) {
		return 0;
	}
    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("XObject");
    if (xobject_dict) {
        map <string, PPToken *> &dict = xobject_dict->_dict;
        PPTIndirectRef *indir_ref = (PPTIndirectRef *)dict[name];
        int obj_num = indir_ref->_objNum;
        return obj_num;
    }
    return 0;
}

PPTIndirectObj *PPFormBase::GetXObject()
{
	return _indirObj; 
}

PPTDictionary *PPFormBase::ResourceDictForKey(string key)
{
	if (_resourceDict == NULL)
		return NULL;

	PPTDictionary *ret = (PPTDictionary *)_resourceDict->valueObjectForKey(key);
	return ret;
}

PPTIndirectObj *PPFormBase::ResourceObjForName(string name, string resource_type)
{
	PPTDictionary *dict = ResourceDictForKey(resource_type);
	if(dict == NULL) {
		return NULL;
	}
	PPTIndirectObj *obj = dict->indirectObjectForKey(name);
	return obj;
}

string PPFormBase::NameFromResourceObj(PPTIndirectObj *obj, string resource_type)
{
	int obj_num = obj->_objNum;
	PPTDictionary *rsc_dict = ResourceDictForKey(resource_type);
	if(rsc_dict == NULL) {
		rsc_dict = new PPTDictionary(&_document->_parser);
		_resourceDict->SetTokenAndKey(rsc_dict, resource_type);
	}
	map <string, PPToken *> *map_dict = &rsc_dict->_dict;
    map <string, PPToken *>::iterator it_dict;
	vector <string> namelist;

	for(it_dict = map_dict->begin(); it_dict != map_dict->end(); it_dict++) {
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
	int i, icnt = namelist.size();
	for(i=0;i<icnt;i++) {
		string name = namelist[i];
		if(name == pname) {
			name_idx ++;
			sprintf(pname, format,name_idx);
			i = 0;
			continue;
		}
	}
	
	PPTIndirectRef *new_rsc_ref = new PPTIndirectRef(&_document->_parser, obj->_objNum, obj->_genNum);
	rsc_dict->SetRefTokenAndKey(obj, pname, obj->_objNum);

	return pname;
}


PPTIndirectRef *PPFormBase::ResourceForKey(string key)
{
	if (_resourceDict == NULL)
		return NULL;

	PPTIndirectRef *ret = (PPTIndirectRef *)_resourceDict->objectForKey(key);
	return ret;
}


void PPFormBase::addElement(PPElement *element)
{
    element->willAddToParent(this);
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

PPLayer *PPFormBase::AddLayerWithProperties(string property_name)
{
	PPTDictionary *properties_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("Properties");
	PPTDictionary *layer_dict = (PPTDictionary *)properties_dict->ValueObjectForKey(property_name);
	if(layer_dict) {
		PPTString *str_obj = (PPTString *)layer_dict->ObjectForKey("Name");
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
	return NULL;
}

PPLayer *PPFormBase::AddLayer(string layer_name)
{
	// layer_name의 레이어가 없다고 간주하고 시작함.
	PPTIndirectObj *layer_obj = _document->LayerObjForName(layer_name);
	PPTIndirectRef *layer_ref = new PPTIndirectRef(&_document->_parser, layer_obj->_objNum, layer_obj->_genNum);

	PPLayer *ret_layer = NULL;
	PPTDictionary *properties_dict = (PPTDictionary *)_resourceDict->ValueObjectForKey("Properties");
	if(properties_dict == NULL) {
		properties_dict = new PPTDictionary(&_document->_parser);
		_resourceDict->SetTokenAndKey(properties_dict, "Properties");
	}

	/* layer_name의 레이어 찾기. 없다고 간주하므로 찾을 필요가 없어졌음.
	map <string, PPToken *> dict = properties_dict->_dict;
    map <string, PPToken *> ::iterator it_token;
	for(it_token = dict.begin(); it_token != dict.end(); it_token++) {
		string property_name = it_token->first;
		PPTDictionary *layer_dict = (PPTDictionary *)properties_dict->ValueObjectForKey(property_name);

    }*/

	// 새 properties key 만들기 
	char pname[10] = "LP0";
	int name_idx = 0;
	int i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		if(layer->_properties == pname) {
			name_idx ++;
			sprintf(pname, "LP%d",name_idx);
			i = 0;
			continue;
		}
	}

	properties_dict->SetTokenAndKey(layer_ref, pname);
	layer_obj->addRefObj(layer_ref);
	PPTDictionary *layer_dict = (PPTDictionary *)layer_ref->valueObject();

	ret_layer = new PPLayer();
	ret_layer->_layer_dict = layer_dict;
	ret_layer->_properties = pname;
	ret_layer->_parent = this;
	_layers.push_back(ret_layer);

	return ret_layer;
	/*
	PPTDictionary *layer_dict = (PPTDictionary *)properties_dict->ValueObjectForKey(pname);
	if(layer_dict == NULL) {
		properties_dict->SetTokenAndKey(layer_ref, pname);
		layer_dict = (PPTDictionary *)layer_ref->valueObject();

		ret_layer = new PPLayer();
		ret_layer->_layer_dict = layer_dict;
		ret_layer->_properties = property_name;
		ret_layer->_parent = this;
		_layers.push_back(ret_layer);
	}
	else {
		int i, icnt = _layers.size();
		for(i=0;i<icnt;i++) {
			PPLayer *layer = _layers[i];
			if(layer->_properties == pname) {
				ret_layer = layer;
				break;
			}
		}
	}
	return ret_layer;
	*/
}

PPLayer *PPFormBase::BeginLayer(char *lname) 
{
	_curLayer = this->LayerForName(lname);
	if(_curLayer == NULL) {
		_curLayer = this->AddLayer(lname);
	}
	PPEBeginMarkedContent *begin_mark = new PPEBeginMarkedContent(&_document->_parser, _curLayer->_properties, ptContext());

	writeElement(begin_mark);
	return _curLayer;
}

void PPFormBase::EndLayer()
{
	PPEEndMarkedContent *end_mark = new PPEEndMarkedContent(ptContext());
	writeElement(end_mark);
	_curLayer = NULL;
}

size_t PPFormBase::numberOfElements()
{
	size_t tot_cnt = 0;
	int i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		int el_cnt = _layers[i]->_elements.size();
		tot_cnt += el_cnt;
	}
	return tot_cnt;
}

PPElement *PPFormBase::elementAtIndex(int idx)
{
	int tot_cnt = 0;
	int i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		int el_cnt = _layers[i]->_elements.size();
		if(idx < (tot_cnt+el_cnt)) {
			int r_idx = idx - tot_cnt;
			return _layers[i]->_elements[r_idx];
		}
		tot_cnt += el_cnt;
	}
	return NULL;
}

void PPFormBase::writeElement(PPElement *src_element)
{
	if(src_element->_parentForm == NULL) {
		addElement(src_element);
	}
	else {
		PPElement *copied = (PPElement *)src_element->Copy();
		addElement(copied);
	}
	// src_element 에 리소스 정보가 있으면
	if(src_element->HasResource()) {
		// src_element의 리소스 '타입'과 '키'로 this에 리소스가 있는지 체크
		vector <const char *> type_list = src_element->ResourceTypeList();
		int i, icnt = type_list.size();
		// 한 엘리먼트에 여려 종류(icnt)의 리소스가 있을 수 있음.
		for(i=0;i<icnt;i++) {
			const char *rsc_type = type_list[i]; //src_element->ResourceType();
			if(rsc_type == PPRT_PROPERTIES) {
				cout << " PPRT_PROPERTIES.." << PP_ENDL;
			}
			if(rsc_type == PPRT_PROSET) {
			}
			else {
				do {
					PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->objectForKey(rsc_type);
					if(!rsc_dict) {
						rsc_dict = new PPTDictionary(&_document->_parser);
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
						PPToken *src_rsc = src_element->GetResource(rsc_type);
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

// 여러 GState 의 값들 중에 cmd 내용에 부합하는 값을 변경한다.
void PPFormBase::SetValueToGState(PPTCommand *cmd, PPContext &gcontext)
{
    
    switch (cmd->_cmdInfo->type) {
        case PPC_LineWidth:
            gcontext.setLineWidth(cmd->getFloatValue(0));
            break;
        
        case PPC_LineCap:
            gcontext.setLineCap(cmd->getIntValue(0));
            break;
        
        case PPC_LineJoin:
            gcontext.setLineJoin(cmd->getIntValue(0));
            break;
        
        case PPC_MiterLimit:
            gcontext.setMiterLimit(cmd->getFloatValue(0));
            break;
        
        case PPC_Dash:
        {
            PPDash dash;
            cmd->getDash(&dash);
            gcontext.setDash(dash);
            break;
        }
        
        case PPC_Intent:
            gcontext.setIntent(cmd->getStringValue(0));
            break;
        
        case PPC_Flatness:
            gcontext.setFlatness(cmd->getFloatValue(0));
            break;
        
        case PPC_DictName:
            gcontext.setDictName(cmd->getStringValue(0));
            break;
        
        case PPC_Matrix:
        {
            PPMatrix mtx;
            mtx._a = cmd->getFloatValue(0);
            mtx._b = cmd->getFloatValue(1);
            mtx._c = cmd->getFloatValue(2);
            mtx._d = cmd->getFloatValue(3);
            mtx._x = cmd->getFloatValue(4);
            mtx._y = cmd->getFloatValue(5);
            gcontext.setMatrix(mtx);
            break;
        }
        
        case PPC_StrokeColorSpace:
			{
				string color_name = cmd->getStringValue(0);

				PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("ColorSpace");
				PPTArray *rsc_arr = (PPTArray *)rsc_dict->valueObjectForKey(color_name);
				gcontext.SetUserStrokeColorSpace(color_name, rsc_arr);
			}
            break;
        
        case PPC_NonStrokeColorSpace:
			{
				string color_name = cmd->getStringValue(0);
				PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("ColorSpace");
				PPTArray *rsc_arr = (PPTArray *)rsc_dict->valueObjectForKey(color_name);
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
		/*
		{
		 
            int n = gcontext.numberOfStrokeColorCoponents();
            if (n == 1) {
                gcontext.setStrokeColor(cmd->getFloatValue(0));
            }
            else if(n == 3) {
                gcontext.setStrokeColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2));
            }
            else if(n == 4) {
                gcontext.setStrokeColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2), cmd->getFloatValue(3));
            }
			
            break;
        }
		*/
        case PPC_SetNonStrokeColor:
			gcontext.SetFillColor(cmd->_operands);
			break;

        case PPC_SetNonStrokeColorN:
			gcontext.SetFillColorN(cmd->_operands);
			break;
			/*
        {
            int n = gcontext.numberOfNonStrokeColorCoponents();
            if (n == 1) {
                gcontext.setFillColor(cmd->getFloatValue(0));
            }
            else if(n == 3) {
                gcontext.setFillColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2));
            }
            else if(n == 4) {
                gcontext.setFillColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2), cmd->getFloatValue(3));
            }
            break;
        }
        */
        case PPC_DeviceGray:
            gcontext.setStrokeColorSpace(PPCSN_DeviceGray);
            gcontext.setStrokeColor(cmd->getFloatValue(0));
            break;
        
        case PPC_DeviceRGB:
            gcontext.setStrokeColorSpace(PPCSN_DeviceRGB);
            gcontext.setStrokeColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2));
            break;
        
        case PPC_DeviceCMYK:
            gcontext.setStrokeColorSpace(PPCSN_DeviceCMYK);
            gcontext.setStrokeColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2), cmd->getFloatValue(3));
            break;
            
        case PPC_NonStrokeDeviceGray:
            gcontext.setFillColorSpace(PPCSN_DeviceGray);
            gcontext.setFillColor(cmd->getFloatValue(0));
            break;
        
        case PPC_NonStrokeDeviceRGB:
            gcontext.setFillColorSpace(PPCSN_DeviceRGB);
            gcontext.setFillColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2));
            break;
        
        case PPC_NonStrokeDeviceCMYK:
            gcontext.setFillColorSpace(PPCSN_DeviceCMYK);
            gcontext.setFillColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2), cmd->getFloatValue(3));
            break;
        
        default:
            break;
    }
}

void PPFormBase::AddCommandToPath(PPTCommand *cmd, PPPath *path)
{
    switch (cmd->_cmdInfo->type) {
        case PPC_MoveTo:
            path->moveTo(cmd->getFloatValue(0), cmd->getFloatValue(1));
            break;
        case PPC_LineTo:
            path->lineTo(cmd->getFloatValue(0), cmd->getFloatValue(1));
            break;
        case PPC_CurveTo:
            path->curveTo(cmd->getFloatValue(0), cmd->getFloatValue(1), cmd->getFloatValue(2), 
				cmd->getFloatValue(3), cmd->getFloatValue(4), cmd->getFloatValue(5));
            break;
        case PPC_CurveTo1:
            path->curveTo1(cmd->getFloatValue(0), cmd->getFloatValue(1), cmd->getFloatValue(2), cmd->getFloatValue(3));
            break;
        case PPC_CurveTo2:
            path->curveTo2(cmd->getFloatValue(0), cmd->getFloatValue(1), cmd->getFloatValue(2), cmd->getFloatValue(3));
            break;
        case PPC_ClosePath:
            path->close();
            break;
        case PPC_Rectangle:
            path->rectangle(cmd->getFloatValue(0), cmd->getFloatValue(1), cmd->getFloatValue(2), cmd->getFloatValue(3));
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
			xobj_dict = (PPTDictionary *)xobj_ref->valueObject();
			if (!xobj_dict) {
				cout << "Shading Dictionary not found..." << PP_ENDL;
				break;
			}
		}
		else if(xobj_ref->ClassType() == PPTN_DICTIONARY) {
			xobj_dict = (PPTDictionary *)xobj_ref;
		}
		xobj_ref = (PPTIndirectRef *)xobj_dict->objectForKey(name);
		if (!xobj_ref) {
			cout << "Shading IndirectRef not found..." << PP_ENDL;
			break;
		}
		PPTIndirectObj *xobj = (PPTIndirectObj *)xobj_ref->targetObject();
		if (!xobj) {
			cout << "Shading Resource Object not found..." << PP_ENDL;
			break;
		}
		xobj_dict = xobj->firstDictionary();
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

int PPFormBase::buildElements()
{
    PPContext gcontext;
    PPPath *opened_path = NULL;
    PPEPath *path_element = NULL;
    PPEText *text_element = NULL;
	PPETextState *textstate_element = NULL;
    PPEInlineImage *inline_img_element = NULL;
    //    PPEImage *imageElement = NULL;
    
    
    size_t i, icnt = _commands.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _commands[i];
        if(textstate_element != NULL) {
			switch (cmd->_cmdInfo->group) {
			case PPCG_TextState:
				break;
			default:
				textstate_element->SetGContext(&gcontext);
				addElement(textstate_element);
				gcontext.clearGFlags();
				textstate_element = NULL;
			}
		}

        switch (cmd->_cmdInfo->group) {
            case PPCG_GState:
				if(text_element) {
					text_element->addCommand(cmd);
				}
				else {
					SetValueToGState(cmd, gcontext);
				}
                break;
            case PPCG_SaveGState:
                {
                    // make SaveGStateElement and add to element list
                    gcontext.saveGState();
                    PPEGSave *gsave = new PPEGSave(&gcontext);
                    addElement(gsave);
					gcontext.clearGFlags();
                }
                break;
            case PPCG_RestoreGState:
                {
                    // make RestoreGStateElement and add to element list
                    gcontext.restoreGState();
                    PPEGRestore *grestore = new PPEGRestore(&gcontext);
                    addElement(grestore);
					gcontext.clearGFlags();
                }
                break;
            case PPCG_DrawPath:
                if (opened_path == NULL) {
                    opened_path = new PPPath;
                }
                if (path_element != NULL) {
                    path_element = NULL;
                }
                //cmd->addCommandToPath(opened_path);
				AddCommandToPath(cmd, opened_path);
                break;
            case PPCG_FinishPath:
            case PPCG_Clipping:
            
                if (opened_path != NULL) {
                    // make path element and add to element list
                    path_element = new PPEPath(opened_path, &gcontext);
					gcontext.clearGFlags();
                    addElement(path_element);
                    opened_path = NULL; 
                }
                if (path_element != NULL) {
                    path_element->setPaintingType(cmd->_cmdInfo->type);
                }
                break;
                
            case PPCG_BeginText:
                text_element = new PPEText();
                break;
            case PPCG_Text:
				if(text_element == NULL) {
					text_element = new PPEText();
				}
                text_element->addCommand(cmd);
                break;
            case PPCG_EndText:
                //                delete text_element;  //  You don't need to delete
				if(text_element != NULL) {
					text_element->SetGContext(&gcontext);

					addElement(text_element);
					gcontext.clearGFlags();
					text_element = NULL;
				}
                break;
            case PPCG_TextState:
				if(text_element) {
					text_element->addCommand(cmd);
				}
				else {
					if(textstate_element == NULL) {
						textstate_element = new PPETextState();
					}
					textstate_element->addCommand(cmd);
				}
                break;
            
            case PPCG_BeginInlineImage:
                inline_img_element = new PPEInlineImage(&gcontext);
                inline_img_element->setBeginImageObject(cmd);
                break;
            case PPCG_InlineImage:
                inline_img_element->setBeginImageData(cmd);
                break;
            case PPCG_EndInlineImage:
                addElement(inline_img_element);
				gcontext.clearGFlags();
                inline_img_element = NULL;
                break;
            
            case PPCG_XObject:
                {
					PPTName *name = (PPTName *)cmd->getTokenValue(0);
					string name_str = *name->_name;

					string subtype = SubtypeFor(name_str);
					if(subtype == "Form") {
						PPEForm *form_element = new PPEForm(name, &gcontext);
						addElement(form_element);
					}
					else if(subtype == "Image") {
						PPEImage *form_element = new PPEImage(name, &gcontext);
						addElement(form_element);
					}
					gcontext.clearGFlags();
					/*
                    int obj_num = GetXObjNumOf(name_str);  // Object Number for XObject named 'name'
                    PPTIndirectObj *indir_obj = (PPTIndirectObj *)_document->_xobjects[obj_num];
                    if(indir_obj) {
                        PPTDictionary *obj_dict = indir_obj->firstDictionary();
                        PPTName *subtype = (PPTName *)obj_dict->objectForKey("Subtype");
                        if (*subtype->_name == "Image") {
                            PPEImage *image_element = new PPEImage(obj_dict, &gcontext);
                            addElement(image_element);
							gcontext.clearGFlags();
                        }
                        else if(*subtype->_name == "Form") {
                            PPEForm *form_element = new PPEForm(name, &gcontext);
//                            form_element->_document = _document;
//                            form_element->setStream(indir_obj->stream());
//                            form_element->buildElements();
                            addElement(form_element);
							gcontext.clearGFlags();
                        }
                    }
					*/
                }
                break;
                
            case PPCG_BeginMarkedContent:
                {
					PPTName *tag = (PPTName *)cmd->getTokenValue(0);
					PPToken *properties = NULL;
					if(cmd->_cmdInfo->type == PPC_BeginMarkedContentP) {
						properties = (PPToken *)cmd->getTokenValue(1);
						if(properties->ClassType() == PPTN_NAME) {
							PPTName *property_name = (PPTName *)properties;
							AddLayerWithProperties(*property_name->_name);
						}
					}
					PPEBeginMarkedContent *marked_content_element = new PPEBeginMarkedContent(tag, properties, &gcontext);;
					addElement(marked_content_element);
					gcontext.clearGFlags();
                }
                break;
            case PPCG_MarkedContent:
                {
                    PPTName *tag = (PPTName *)cmd->getTokenValue(0);
                    PPTDictionary *porpert = (PPTDictionary *)cmd->getTokenValue(1);
                    PPEMarkedContent *marked_content_element = new PPEMarkedContent(tag, porpert, &gcontext);
                    addElement(marked_content_element);
                }
                break;
            case PPCG_EndMarkedContent:
                {
                    PPEEndMarkedContent *marked_content_element = new PPEEndMarkedContent(&gcontext);
                    addElement(marked_content_element);
					gcontext.clearGFlags();
                }
                break;
            case PPCG_Shading:
                {
                    PPEShading *shading_element = new PPEShading(&gcontext);
					shading_element->_name = (PPTName *)cmd->getTokenValue(0)->Copy();
                    addElement(shading_element);
					gcontext.clearGFlags();
                }
                break;
            
            case PPCG_BeginCompatibility:
				{
                    PPEBeginCompatibility *compatibility_element = new PPEBeginCompatibility(&gcontext);
                    addElement(compatibility_element);
					gcontext.clearGFlags();
				}
                break;
            case PPCG_EndCompatibility:
				{
                    PPEEndCompatibility *compatibility_element = new PPEEndCompatibility(&gcontext);
                    addElement(compatibility_element);
					gcontext.clearGFlags();
				}
                break;
                
            default:
				cout << " Unprocessed Command " << cmd->pdfString() << PP_ENDL;
                break;
        }
    }
    
    return 0;
}

PPTStream *PPFormBase::BuildStream()
{
	string retstr; 
	ostringstream ostr;
	size_t i, icnt = numberOfElements(); //_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = elementAtIndex(i); //_elements.at(i);
		string cmdstr = element->commandString();
		retstr += cmdstr;
	}
	unsigned long size = retstr.size();
	PPTStream *stream = new PPTStream(&_document->_parser, size);
	const char *cstr = retstr.c_str();
	memcpy(stream->_streamData, cstr, size);
	stream->_decoded = true;
	return stream;
}

PPElement *PPFormBase::next()
{
	if (numberOfElements() > 0 && _cur_element_idx < numberOfElements()) {
		return elementAtIndex(_cur_element_idx++);
	}
	_cur_element_idx = 0;
	return NULL;
}


void PPFormBase::AddXObjRef(PPTIndirectObj *xobj, string key) 
{
	 int obj_num = xobj->getObjNum();
	_document->_xobjects[obj_num] = xobj;
    PPTDictionary *xobject_dict = (PPTDictionary *)_resourceDict->valueObjectForKey("XObject");
	if(xobject_dict == NULL) {
		xobject_dict = new PPTDictionary(&_document->_parser);
		_resourceDict->SetTokenAndKey(xobject_dict, "XObject");
	}

	xobject_dict->SetRefTokenAndKey(xobj, key, xobj->_objNum);
}

void PPFormBase::AddFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *xobj = form_obj->GetXObject();
//	form_obj->_document = _document;
	PPTStream *stream = form_obj->BuildStream();
	stream->_dict = (PPTDictionary *)xobj->_array[0];
	stream->_dict->SetTokenAndKey(stream->_streamSize, "Length");
	xobj->AddObj(stream);
	AddXObjRef(xobj, *form_obj->_form_key->_name);
	delete form_obj;
}

void PPFormBase::BeginReadLayer(char *layer_name)
{

}

PPLayer *PPFormBase::LayerForName(string name, int *idx)
{
	int i, icnt = _layers.size();
	for(i=0;i<icnt;i++) {
		PPLayer *layer = _layers[i];
		if(layer->Name() == name) {
			if (idx)
				*idx = i;
			return layer;
		}
	}
	return NULL;
}

void PPFormBase::ReorderLayer(int to_idx, int from_idx)
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

	int i, icnt = src->_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = src->_elements[i];
		if(element->getType() != PPET_MARKED_CONTENT
			&& element->getType() != PPET_END_MARKED_CONTENT
			&& element->getType() != PPET_BEGIN_MARKED_CONTENT) {
				int tar_cnt = tar->_elements.size();
				// src 레이어의 Marked Contents 관련 엘리먼트들을 제외하고
				// tar 레이어의 마지막 바로 앞에 인서트를 한다.
				// tar 레이어의 마지막에 있는 Marked content end  엘리먼트를 
				// 유지해 줘야 하기 때문이다.
				tar->_elements.insert(tar->_elements.begin() + tar_cnt - 1, element);
		}
	}

	_layers.erase(_layers.begin() + src_idx);

}