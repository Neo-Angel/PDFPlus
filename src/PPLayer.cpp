

#include "PPElement.h"
#include "PPLayer.h"
#include "PPTDictionary.h"
#include "PPTString.h"

PPLayer::PPLayer()
{
	_layer_dict = NULL;
}

PPLayer::PPLayer(PPTDictionary *layer_dict)
{
	_layer_dict = layer_dict;
}


void PPLayer::AddElement(PPElement *element)
{
    element->willAddToParent(_parent);
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

void PPLayer::Merge(PPLayer *layer)
{
	int i, icnt = layer->_elements.size();
	for(i=0;i<icnt;i++) {
		PPElement *element = layer->_elements.at(i);
		PPElement *copied_element = (PPElement *)element->Copy();
		_elements.push_back(copied_element);
		//layer->_elements.erase(layer->_elements.begin()+i);
	}
}
