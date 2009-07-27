#pragma once
#include "tinyxml/tinyxml.h"
class ContextMenuString : public TiXmlString {
public:
	ContextMenuString() {}
	ContextMenuString(TiXmlString & a): TiXmlString(a) {}
	ContextMenuString(const char * pc) : TiXmlString(pc){ 
	}
	ContextMenuString& operator = (const char * copy) ;
	size_type rfind(char tofind);
	/**
	Returns the substring from start included to end not included
	@param start substring start index
	@param end substring end index
	*/
	ContextMenuString substring(int start, int end);
};
