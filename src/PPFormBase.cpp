


#include <vector>
#include <iostream>
#include <sstream>

#include "PPFormBase.h"
#include "PPToken.h"
#include "PPElement.h"
#include "PPTStream.h"
#include "PPContext.h"
#include "PPEText.h"
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

#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"

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
}

PPFormBase::PPFormBase(PPFormBase *form_base):_graphicParser((vector <PPToken *> *)&_commands)
{
	_cur_element_idx = 0; // size_t 

	_indirObj = (PPTIndirectObj *)form_base->_indirObj->Copy(); //PPTIndirectObj *
    // _document; // PPDocument * : this set when this added to document
    _resourceDict = NULL; //PPTDictionary *
//    _commands; // vector <PPTCommand *>
//    _graphicParser; // PPCommandParser
//    _elements; // vector <PPElement *> 

}

PPFormBase::PPFormBase(PPDocument *doc, PPTIndirectObj *indir):_graphicParser((vector <PPToken *> *)&_commands)
{
	_document = doc;
	_indirObj = indir; //(PPTIndirectObj *)indir->Copy();
	_formDict = _indirObj->firstDictionary();
	PPTName *subtype_name = (PPTName *)_formDict->objectForKey("Subtype");
	_cur_element_idx = 0; // size_t 

	_resourceDict = (PPTDictionary *)_formDict->objectForKey("Resources");

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
}

// form_obj를 기반으로 새로운 Form 객체를 만든다.
PPFormBase *PPFormBase::NewFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *indir = (PPTIndirectObj *)form_obj->_indirObj->Copy();
	if(indir->_array.size() == 2) {
		indir->_array.erase(indir->_array.begin()+1);
	}

	PPFormBase *ret_form = NULL;
	if(indir->_array.size() == 1) {
		indir->MoveInto(_document);
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
PPToken *PPFormBase::ResourceForKey(string rcs_type, string rcs_key){
	return _document->ResourceForKey(rcs_type, rcs_key);
}

PPToken *PPFormBase::WriteResource(PPToken *rcs, string type, string key) {
	return _document->WriteResource(rcs, type, key);
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
    _elements.push_back(element);
	// update _commands list from element 
}

void PPFormBase::writeElement(PPElement *src_element)
{
	PPElement *copied = (PPElement *)src_element->Copy();
	// src_element 에 리소스 정보가 있으면
	if(src_element->HasResource()) {
		// src_element의 리소스 '타입'과 '키'로 this에 리소스가 있는지 체크
		string rsc_type = src_element->ResourceType();
		if(rsc_type == "ProcSet") {
		}
		else {
			PPTDictionary *rsc_dict = (PPTDictionary *)_resourceDict->objectForKey(rsc_type);
			if(!rsc_dict) {
				rsc_dict = new PPTDictionary(&_document->_parser);
				_resourceDict->SetTokenAndKey(rsc_dict, rsc_type);
			}
			string rsc_key = src_element->ResourceKey();
			PPToken *rsc = ResourceForKey(rsc_type, rsc_key);
			if(rsc) {
				PPTIndirectObj *obj = (PPTIndirectObj *)rsc;
				rsc_dict->SetRefTokenAndKey(obj, rsc_key, obj->_objNum);
			}
			else  {
				// 없으면 리소스 복사
				PPToken *src_rsc = src_element->GetResource();
				if(src_rsc) {
/*					if(src_rsc->classType() != PPTN_INDIRECTOBJ) {
						int obj_num = _document->NewObjNum();
						PPTIndirectObj *container_obj = new PPTIndirectObj(&&_document->_parser, obj_num, 0);
						container_obj->AddObj(rsc);
						src_rsc = container_obj;
					}
					*/
					rsc = WriteResource(src_rsc, rsc_type, rsc_key);  // _resources
					_document->SetRefTokenForKey(rsc_dict,rsc,rsc_key); //rsc_dict, _tokens, parser->_objDict
				}
			}
			//_document->SetRefTokenForKey(rsc_dict,rsc,rsc_key); //rsc_dict, _tokens, parser->_objDict

			//int new_obj_num = _document->NewObjNum();
			//rsc_dict->SetRefTokenAndKey(rsc, rsc_key, new_obj_num);
		}
	}
	addElement(copied);
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
            gcontext.SetUserStrokeColorSpace(cmd->getStringValue(0));
            break;
        
        case PPC_NonStrokeColorSpace:
            gcontext.SetUserFillColorSpace(cmd->getStringValue(0));
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

int PPFormBase::buildElements()
{
    PPContext gcontext;
    PPPath *opened_path = NULL;
    PPEPath *path_element = NULL;
    PPEText *text_element = NULL;
    PPEInlineImage *inline_img_element = NULL;
    //    PPEImage *imageElement = NULL;
    
    
    size_t i, icnt = _commands.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _commands[i];
        
        switch (cmd->_cmdInfo->group) {
            case PPCG_GState:
                cmd->setValueToGState(gcontext);  // => 
				SetValueToGState(cmd, gcontext);
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
                    //                cmd->addCommandToPathElement(path_element);
                }
                break;
                
            case PPCG_BeginText:
                text_element = new PPEText(&gcontext);
                break;
            case PPCG_Text:
                text_element->addCommand(cmd);
                break;
            case PPCG_EndText:
                //                delete text_element;  //  You don't need to delete
                addElement(text_element);
				gcontext.clearGFlags();
                text_element = NULL;
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
                }
                break;
                
            case PPCG_BeginMarkedContent:
                {
                    PPTName *tag = (PPTName *)cmd->getTokenValue(0);
                    PPTDictionary *porpert = (PPTDictionary *)cmd->getTokenValue(1);
                    PPEBeginMarkedContent *marked_content_element = new PPEBeginMarkedContent(tag, porpert, &gcontext);
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
					gcontext.clearGFlags();
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
	size_t i, icnt = _elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = _elements.at(i);
		retstr += element->commandString();
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
	if (_elements.size() > 0 && _cur_element_idx < _elements.size()) {
		return _elements[ _cur_element_idx++];
	}
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