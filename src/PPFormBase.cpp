


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

	_indirObj = NULL; // = (PPTIndirectObj *)form_base->_indirObj->Copy(); //PPTIndirectObj *
    // _document; // PPDocument * : this set when this added to document
    _resourceDict = NULL; //PPTDictionary *
//    _commands; // vector <PPTCommand *>
//    _graphicParser; // PPCommandParser
//    _elements; // vector <PPElement *> 

}

PPFormBase::PPFormBase(PPTIndirectObj *indir):_graphicParser((vector <PPToken *> *)&_commands)
{
	PPTDictionary *dict = indir->firstDictionary();
	PPTName *subtype_name = (PPTName *)dict->objectForKey("Subtype");
	_cur_element_idx = 0; // size_t 
	_indirObj = (PPTIndirectObj *)indir->Copy();


	if (*subtype_name->_name == "Form") {
		PPTStream *stream = indir->stream();
		_graphicParser.parseStream(*stream);
		buildElements();
	}
}

PPParser *PPFormBase::documentParser()
{
	return &_document->_parser;
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

void PPFormBase::writeElement(PPElement *element)
{
	PPElement *copied = (PPElement *)element->Copy();

	addElement(copied);
}


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
            gcontext.setStrokeColorSpace(cmd->getStringValue(0));
            break;
        
        case PPC_NonStrokeColorSpace:
            gcontext.setFillColorSpace(cmd->getStringValue(0));
            break;
        
        case PPC_SetColor:
        case PPC_SetColorN:
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
        case PPC_SetNonStrokeColor:
        case PPC_SetNonStrokeColorN:
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
        
        case PPC_DeviceGray:
            gcontext.setStrokeColorSpace(PPCSN_DeviceGray);
            gcontext.setStrokeColor(cmd->getFloatValue(0));
            break;
        
        case PPC_DeviceRGB:
            gcontext.setStrokeColorSpace(PPCSN_DeviceRGB);
            gcontext.setStrokeColor(cmd->getFloatValue(0),cmd->getFloatValue(1),cmd->getFloatValue(2));
            break;
        
        case PPC_DeviceCMYK:
            gcontext.setStrokeColorSpace(PPCSN_DeviceRGB);
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
                    shading_element->_name = (PPTName *)cmd->getTokenValue(0);
                    addElement(shading_element);
					gcontext.clearGFlags();
                }
                break;
            
            case PPCG_BeginCompatibility:
                cout << " PPCG_BeginCompatibility ..." << PP_ENDL;
                break;
            case PPCG_EndCompatibility:
                cout << " PPCG_EndCompatibility ..." << PP_ENDL;
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
	return stream;
}

PPElement *PPFormBase::next()
{
	if (_elements.size() > 0 && _cur_element_idx < _elements.size()-1) {
		return _elements[ _cur_element_idx++];
	}
	return NULL;
}

void PPFormBase::AddXObj(PPTIndirectObj *xobj) 
{
	 int obj_num = xobj->getObjNum();
	_document->_xobjects[obj_num] = xobj;

}

void PPFormBase::AddFormObj(PPFormBase *form_obj)
{
	PPTIndirectObj *xobj = form_obj->GetXObject();
	form_obj->_document = _document;
	int new_obj_num = _document->NewObjNum();
	_document->_xobjects[new_obj_num] = xobj;
}