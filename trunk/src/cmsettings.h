#include "cmstring.h"

#define MENU_ITEM_ONDIR (1 << 0)
#define MENU_ITEM_ONFILE (1 << 1)
#define MENU_ITEM_SEPARATOR (1 << 2)
#define MENU_ITEM_FOREACH (1 << 3)
class ContextMenuItem {
	
public:

	ContextMenuItem(TiXmlElement* e);
	const ContextMenuString& getName() { return name; }
	const ContextMenuString& getFilter() { return filter; }
	const ContextMenuString& getUser() { return user; }
	const ContextMenuString& getPass() { return pass; }
	const ContextMenuString& getCommand() { return command; }
	unsigned getFlags() { return flags; }
	//friend class ContextMenuItemsIterator;
private:
	ContextMenuString parseName();
	ContextMenuString parseFilter();
	ContextMenuString parseUser();
	ContextMenuString parsePass();
    ContextMenuString parseCommand();
	unsigned parseFlags();
	TiXmlElement* elem;
	ContextMenuString name;
	ContextMenuString filter;
	ContextMenuString user;
	ContextMenuString pass;
    ContextMenuString command;
	unsigned flags;
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
