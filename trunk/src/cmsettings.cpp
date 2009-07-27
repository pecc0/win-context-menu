#include "cmsettings.h"
#include <windows.h>
TiXmlDocument ContextMenuSettings::configDoc;

const char* emptyMenu = 
"<?xml version=\"1.0\" >\n\
<!-- \n\
Use this file to configure your custom Windows explorer context menu items.\n\
This file is loaded only when you register (install) win_context_menu.dll. \n\
In order your changes to take effect you need to call uninstall.bat and then install.bat.\n\
-->\n\
<CustomMenuItems>\n\
<!-- Add menu itmes here. This is an example MenuItem node:\n\
<MenuItem >\n\
</MenuItem>\n\
-->\n\
</CustomMenuItems>";

void ContextMenuSettings::init(ContextMenuString& dllFile)
{
	ContextMenuString configFile =  dllFile.substring(0, dllFile.rfind('\\'));
	configFile += "\\explorermenu.xml";
	
	
	configDoc = TiXmlDocument( configFile.c_str() );
	bool loadOkay = configDoc.LoadFile();
	/*
	MessageBox(NULL,
	configFile.c_str(),
	"DEBUG",
	MB_OK);
	*/
	if ( !loadOkay )
	{
		configDoc.Parse(emptyMenu);
		configDoc.SaveFile();
		
		TiXmlOutStream message;
		message<<"File '"<<configFile.c_str() << "' not found. New one created. Check it for instructions about how to use this tool";
		MessageBox(NULL,
		message.c_str(),
		"DEBUG",
		MB_OK);
	}
	
	
}

ContextMenuItemsIterator ContextMenuSettings::begin() {
	ContextMenuItemsIterator result;
	TiXmlNode* node = 0;
	TiXmlElement* customMenuItems = 0;
	TiXmlElement* item = 0;
	node = configDoc.FirstChild( "CustomMenuItems" );
	assert( node );
	customMenuItems = node->ToElement();
	assert( customMenuItems  ); 
	item = customMenuItems->FirstChildElement("MenuItem");
	result.current = ContextMenuItem(item);
	
	return result;
	
}


/***********ContextMenuItemsIterator*************/
ContextMenuItemsIterator& ContextMenuItemsIterator::operator+ (int i)
{
	while (current.elem && i > 0) {
		current.elem = current.elem->NextSiblingElement("MenuItem");
		i--;
	}
	return *this;
}

ContextMenuItemsIterator& ContextMenuItemsIterator::operator++ (int)
{
	return (*this) + 1;
}
/***********ContextMenuItemsIterator*************/


/***********ContextMenuItem********************/
ContextMenuString ContextMenuItem::getName()
{
	ContextMenuString result("error");
	if (elem) {
		const char * text = elem->GetText();
		if (text) {
			result = elem->GetText();
		} else {
			result = "";			
		}
	}
	return result;
}
/***********ContextMenuItem********************/
