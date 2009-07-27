#pragma once
#include "tinyxml/tinyxml.h"
class ContextMenuString : public TiXmlString {
public:
	ContextMenuString() {}
	ContextMenuString(char * pc) : TiXmlString(pc){ 
	}
	
	size_type rfind(char tofind) {
		const char* start = c_str();
		const char* p;
		for (p = start + size(); *p != *start; --p)
		{
			if (*p == tofind) return static_cast< size_type >( p - start );
		}
		if (*p == tofind) return 0;
		return npos;
	}
	/**
	Returns the substring from start included to end not included
	@param start substring start index
	@param end substring end index
	*/
	ContextMenuString substring(int start, int end) {

		if (start < 0 || start > end) {
			return ContextMenuString();
		}
		if (end > size()) {
			end = size();
		}
		char* buf = new char [end - start + 1];
		strncpy(buf, c_str() + start, end - start);
		ContextMenuString result(buf);
		delete[] buf;
		return result;
	}
};
