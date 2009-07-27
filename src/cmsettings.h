#include "cmstring.h"
class ContextMenuItem {
public:
	ContextMenuItem(TiXmlElement* e): elem(e){
	}
	friend class ContextMenuItemsIterator;
private:
	TiXmlElement* elem;
};

class ContextMenuItemsIterator {
public:
	ContextMenuItemsIterator& operator+ (int i) ;
	ContextMenuItemsIterator& operator++ (int) ;
	friend class ContextMenuSettings;
private:
	ContextMenuItem* current;
};

class ContextMenuSettings {
public:
	static void init(ContextMenuString& dllFile); 
	
	static TiXmlDocument configDoc;
	
	static ContextMenuItemsIterator begin();
};
