#include "cmsettings.h"
#include <windows.h>
TiXmlDocument ContextMenuSettings::configDoc;

const char* emptyMenu = 
"<?xml version=\"1.0\" >\n\
<!-- \n\
Use this file to configure your custom Windows explorer context menu items.\n\
This file is loaded only when you register (install) win_context_menu.dll. \n\
In order your changes to take effect you need to call uninstall.bat and then install.bat (or reinstall.bat).\n\
\n\
I'm looking for a variant this xml to be loaded in a shared memory, and win_context_menu.dll to provide a callback method for reloading the xml from the disk. \n\
This way you will not need to restart your explorer every time you make a change in this xml.\n\
The most naive approach for implementing this feachure is every time the context menu is shown, the xml to be read from the disk (but I don't like it :) )\n\
-->\n\
<CustomMenuItems>\n\
<!-- \n\
	Add menu items here. This is the MenuItem general schema (TODO: create an xsd)\n\
	<MenuItem \n\
		filter=\"{regexp}\"  Filter if the menu item is \"onfile\". Default value \".*\".  If this filter matches any of the selected files, then the menu item is added into the context menu. \n\
		separator=\"true|false\" default fasle\n\
		ondir=\"true|false\" default false\n\
		onfile=\"true|false\" default true if ondir=false else false\n\
		foreach=\"true|false\" Whether the command is executed for each of the selected files, or once for all files. default=false. The command is executed for each of the files that matches the filter regex\n\
		command=\"{string}\" The executed command. Use %f to specify the first file, %F to specify all files. If foreach is true %f returns the current file\n\
		user=\"{username}\" Use this if you want the command to be executed with a different privileges. This is only the user name (no domain). Domain might be implemented later\n\
		pass=\"{pass}\" A password for the user\n\
		position=\"{number}\" An explicit position for the menu item\n\
	>\n\
	{name}\n\
	</MenuItem>\n\
	\n\
	Here are some examples:\n\
	<MenuItem separator=\"true\" ondir=\"true\" onfile=\"true\" />\n\
	<MenuItem position='0' ondir=\"true\" command=\"cmd\" user=\"lu\" pass=\"pass\">Petko's CMD</MenuItem>\n\
	<MenuItem filter=\".*\\.TXT\" command=\"notepad %f\" foreach=\"true\">notepad all</MenuItem>\n\
	<MenuItem filter=\".*\\.txt\" command=\"notepad %f\">notepad first</MenuItem>\n\
	<MenuItem command=\"C:\\Program Files\\Notepad++\\notepad++.exe %F\">Notepad++</MenuItem>\n\
	<MenuItem separator=\"true\" ondir=\"true\" onfile=\"true\"/>\n\
-->\n\
</CustomMenuItems>";

bool ContextMenuSettings::init(ContextMenuString& dllFile)
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
	TiXmlOutStream message;
	if ( !loadOkay )
	{
		configDoc.Clear();
		configDoc.Parse(emptyMenu);
		configDoc.SaveFile();
		
		
		message<<"Can't find '"<<configFile.c_str() << "'. New one created. Check it for instructions about how to use this tool";
		MessageBox(NULL,
		message.c_str(),
		"Custom Context Menu",
		MB_OK);
		
		
	}
	TiXmlNode* node = 0;
	node = configDoc.FirstChild( "CustomMenuItems" );
	bool error = false;
	if (node) {
		if (!node->ToElement()) {
			error = true;
		}
	} else {
		error = true;
	}
	if (error) {
		
		message<<"Can't parse '"<<configFile.c_str() << "'. Should I recreate the file?";
		
		if (MessageBox(NULL,
					message.c_str(),
					"Custom Context Menu",
					MB_YESNO) == IDYES)
		{
			configDoc.Clear();
			configDoc.Parse(emptyMenu);
			configDoc.SaveFile();
		} else {
			return false;
		}
	}
	return true;
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
		current = ContextMenuItem(current.elem->NextSiblingElement("MenuItem"));
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

ContextMenuItem::ContextMenuItem(TiXmlElement* e): elem(e){
	name = parseName();
	filter = parseFilter();
	flags = parseFlags();
	user = parseUser();
	pass = parsePass();
	command = parseCommand();
	ContextMenuString str = parseAttribute(ContextMenuString("position"));
	if (str.size() > 0){
		position = atoi(str.data());
	} else {
		position = -1;
	}
}

ContextMenuString ContextMenuItem::parseName()
{
	ContextMenuString result("error");
	if (elem) {
		const char * text = elem->GetText();
		if (text) {
			result = text;
		} else {
			result = "";			
		}
	}
	return result;
}
ContextMenuString ContextMenuItem::parseFilter(){
	ContextMenuString result("error");
	if (elem) {
		const char * str = elem->Attribute("filter");
		if (str) {
			result = str;
		} else {
			result = ".*";
		}
	}
	return result;
}

ContextMenuString ContextMenuItem::parseUser(){
	ContextMenuString result("error");
	if (elem) {
		const char * str = elem->Attribute("user");
		if (str) {
			result = str;
		} else {
			result = "";
		}
	}
	return result;
}
ContextMenuString ContextMenuItem::parsePass(){
	ContextMenuString result("error");
	if (elem) {
		const char * str = elem->Attribute("pass");
		if (str) {
			result = str;
		} else {
			result = "";
		}
	}
	return result;
}
ContextMenuString ContextMenuItem::parseCommand(){
	ContextMenuString result("error");
	if (elem) {
		const char * str = elem->Attribute("command");
		if (str) {
			result = str;
		} else {
			result = "";
		}
	}
	return result;
}

ContextMenuString ContextMenuItem::parseAttribute(const ContextMenuString& attrname){
	ContextMenuString result("error");
	if (elem) {
		const char * str = elem->Attribute(attrname.data());
		if (str) {
			result = str;
		} else {
			result = "";
		}
	}
	return result;
}

unsigned ContextMenuItem::parseFlags(){
	unsigned result = MENU_ITEM_ONFILE;
	if (elem) {
		const char * str = elem->Attribute("ondir");
		if (str) {
			ContextMenuString strondir(str);
			if (strondir.matches("true")) {
				result = MENU_ITEM_ONDIR;
			}
		}
		str = elem->Attribute("onfile");
		if (str) {
			ContextMenuString stronfile(str);
			if (stronfile.matches("true")) {
				result |= MENU_ITEM_ONFILE;
			}
		}
		str = elem->Attribute("separator");
		if (str) {
			ContextMenuString obj(str);
			if (obj.matches("true")) {
				result |= MENU_ITEM_SEPARATOR;
			}
		}
		str = elem->Attribute("foreach");
		if (str) {
			ContextMenuString obj(str);
			if (obj.matches("true")) {
				result |= MENU_ITEM_FOREACH;
			}
		}
	}
	return result;
}
/***********ContextMenuItem********************/
