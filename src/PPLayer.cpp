

#include "PPElement.h"
#include "PPLayer.h"
#include "PPTDictionary.h"
#include "PPTString.h"
#include "PPEBeginMarkedContent.h"

PPLayer::PPLayer()
{
	_layer_dict = NULL;
	_parent = NULL;
}

PPLayer::PPLayer(PPTDictionary *layer_dict)
{
	_layer_dict = layer_dict;
	_parent = NULL;
}


void PPLayer::AddElement(PPElement *element)
{
    element->WillAddToParent(_parent);
	element->SetParser(&_parent->_document->_parser);
	_elements.push_back(element);
	// update _commands list from element 
}


PPElement *PPLayer::ElementAtIndex(int idx)
{
	return _elements[idx];
}

void PPLayer::RemoveElementAtIndex(int idx)
{
	_elements.erase(_elements.begin() + idx);
}

string PPLayer::Name()
{
	if(_layer_dict) {
		PPTString *str_obj = (PPTString *)_layer_dict->ObjectForKey("Name");
		string *ret_str = str_obj->_string;
		return *ret_str;
	}
	return "";
}

bool PPLayer::IsEqualName(string other)
{
	if(_layer_dict) {
		PPTString *str_obj = (PPTString *)_layer_dict->ObjectForKey("Name");
		bool ret = str_obj->IsEqualToString(other);
		return ret;
	}
	return false;
}

void PPLayer::Merge(PPLayer *layer)
{
	size_t i, icnt = layer->_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = layer->_elements.at(i);
		PPElement *copied_element = (PPElement *)element->Copy();
		_elements.push_back(copied_element);
		//layer->_elements.erase(layer->_elements.begin()+i);
	}
}

void PPLayer::ChangeProperties(string str)
{
	_properties = str;
	uint icnt = (uint)_elements.size();
	for(uint i=0;i<icnt;i++) {
		PPElement *el = _elements[i];
		if(el->Type() == PPET_BEGIN_MARKED_CONTENT) {
			PPEBeginMarkedContent *begin_mark_el = (PPEBeginMarkedContent *)el;
			string *new_name = new string(_properties);
			begin_mark_el->SetPropertiesName(new_name);
		}
	}

}

void PPLayer::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPLayer *layer = (PPLayer *)obj;

	// 외부에 의해서 _document가 다시 지정되기 전 까지는 
	//이전 _document(출처)를 가지고 있는다.
	layer->_properties = _properties; 
	layer->_layer_dict = _layer_dict;
//	layer->_parent = _parent;

	uint icnt = (uint)_elements.size();
	for(uint i=0;i<icnt;i++) {
		PPElement *el = _elements[i];
		PPElement *new_el;
		if(layer->_parent != NULL) {
			new_el = (PPElement *)el->Copy(layer->_parent);
		}
		else {
			new_el = (PPElement *)el->Copy();
		}
		 /*
		if(new_el->Type() == PPET_BEGIN_MARKED_CONTENT) {
			PPEBeginMarkedContent *begin_mark_el = (PPEBeginMarkedContent *)new_el;
			string *new_name = new string(_properties);
			begin_mark_el->SetPropertiesName(new_name);
		}
		*/
		layer->_elements.push_back(new_el);
	}

}

// 객체를 복사하기위한 기반함수. 상속할 팔요는 없음
PPBase *PPLayer::Copy(PPFormBase *tar_form)
{
	PPLayer *new_obj = (PPLayer *)this->Create();
	new_obj->_parent = tar_form;
	new_obj->_clone = this;
	CopyMembersTo(new_obj);

	return new_obj;
}