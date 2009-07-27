#include "cmstring.h"
class ContextMenuItem {
public:
	ContextMenuItem(TiXmlElement* e): elem(e){
	}
	ContextMenuString getName();
	ContextMenuString getFilter();
	friend class ContextMenuItemsIterator;
private:
	TiXmlElement* elem;
};

class ContextMenuItemsIterator {
public:
	ContextMenuItemsIterator():current(NULL) {}
	ContextMenuItemsIterator& operator+ (int i) ;
	ContextMenuItemsIterator& operator++ (int) ;
	bool end() { return !current.elem; }
	ContextMenuItem& item() { return current; }
	friend class ContextMenuSettings;
private:
	ContextMenuItem current;
};

class ContextMenuSettings {
public:
	static void init(ContextMenuString& dllFile); 
	
	static TiXmlDocument configDoc;
	
	static ContextMenuItemsIterator begin();
};
