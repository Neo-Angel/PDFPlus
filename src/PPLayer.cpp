

#include "PPElement.h"
#include "PPLayer.h"

void PPLayer::AddElement(PPElement *element)
{
    element->willAddToParent(_parent);
	element->SetParser(&_parent->_document->_parser);

	// update _commands list from element 
}
