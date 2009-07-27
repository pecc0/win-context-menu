#include "cmstring.h"
#include "regexp/regexp/Pattern.h"
ContextMenuString& ContextMenuString::operator = (const char * copy) {
	assign( copy, (size_type)strlen(copy));
	return *this;
}

ContextMenuString::size_type ContextMenuString::rfind(char tofind)
{
	const char* start = c_str();
	const char* p;
	for (p = start + size(); *p != *start; --p)
	{
		if (*p == tofind) return static_cast< size_type >( p - start );
	}
	if (*p == tofind) return 0;
	return npos;
}

ContextMenuString ContextMenuString::substring(int start, int end) 
{
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

	bool ContextMenuString::matches(const ContextMenuString& regexp){
         return Pattern::matches(regexp.data(), this->data());
 }
