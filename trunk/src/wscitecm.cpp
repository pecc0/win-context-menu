//---------------------------------------------------------------------------
// Copyright 2002-2003 by Andre Burgaud <andre@burgaud.com>
// See license.txt
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// wscitecm.cpp
// Defines the entry point for the DLL application.
//---------------------------------------------------------------------------

#ifndef STRICT
#define STRICT
#endif

#define INC_OLE2

//#define UNICODE
#define _WIN32_WINNT 0x0500


#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>

#include <userenv.h>
#include <vector>
#include <string>

#define GUID_SIZE 128
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#define MAX_FILES 10
#define MAX_CMDSTR (MAX_PATH * MAX_FILES)
#define ResultFromShort(i) ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>


#include "resource.h"

#include "cmstring.h"
#include "cmsettings.h"

#include "wscitecm.h"

#include "ItemIDList.h"

#include "regexp/regexp/Pattern.h"

#pragma data_seg()

//#define COMPARE_WITH_SVN

#define COMMAND_PROMPT

//---------------------------------------------------------------------------
//  Global variables
//---------------------------------------------------------------------------
UINT _cRef = 0; // COM Reference count.
HINSTANCE _hModule = NULL; // DLL Module.

typedef struct{
	HKEY  hRootKey;
	LPTSTR szSubKey;
	LPTSTR lpszValueName;
	LPTSTR szData;
} DOREGSTRUCT, *LPDOREGSTRUCT;

//char szSciTEName[] = "compareWithSvn.bat\0ahahahahahhahahhahahahhahahahahha";
char szShellExtensionTitle[] = "contextMenuEditor\0ahahahahahhahahhahahahhahahahahha";
//char szShellExtensionExt[] = ".bat\0ahahahahahhahahhahahahhahahahahha";

BOOL RegisterServer(CLSID, LPTSTR);
BOOL UnregisterServer(CLSID, LPTSTR);
void MsgBox(LPTSTR);
void MsgBoxDebug(LPTSTR);
void MsgBoxError(LPTSTR);

//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

	if (dwReason == DLL_PROCESS_ATTACH) {
		TCHAR moduleName[MAX_PATH];
		GetModuleFileName(hInstance, moduleName, MAX_PATH);
		ContextMenuString str = moduleName;
		//MessageBox(0, str.data(), "", 0);
		if (!ContextMenuSettings::init(str)) {
			return FALSE;
		}
		/*
		for (ContextMenuItemsIterator i = ContextMenuSettings::begin(); !i.end(); i++) {
			ContextMenuString name = "d:\\opa\\bla.txt";
			if (name.matches(i.item().getFilter())){
				MessageBox(0, i.item().getName().data(), "", 0);
			}
		}
		*/
		//ContextMenuString s1("notepad %F");
		//ContextMenuString s2("C:\\a\\w\\b");
		//ContextMenuString s3("C:\\a\\w\\b");
		
		//std::string result = Pattern::replace("%f", std::string(s1.data()), Pattern::replace("\\\\", std::string(s2.data()), "\\\\\\\\"));


		//result = Pattern::replace("%F", result, Pattern::replace("\\\\", std::string(s3.data()), "\\\\\\\\"));
		//ContextMenuString cmd(result.data());
		//MessageBox(0,cmd.data(),"",0);
		
		_hModule = hInstance;
	}
	return 1;
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------
STDAPI DllCanUnloadNow(void) {
	return (_cRef == 0 ? S_OK : S_FALSE);
}

//---------------------------------------------------------------------------
// DllGetClassObject
//---------------------------------------------------------------------------
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut) {
	*ppvOut = NULL;
	if (IsEqualIID(rclsid, CLSID_ShellExtension)) {
		CShellExtClassFactory *pcf = new CShellExtClassFactory;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

//---------------------------------------------------------------------------
// DllRegisterServer
//---------------------------------------------------------------------------
STDAPI DllRegisterServer() {
	return (RegisterServer(CLSID_ShellExtension, szShellExtensionTitle) ? S_OK : E_FAIL);
}

//---------------------------------------------------------------------------
// DllUnregisterServer
//---------------------------------------------------------------------------
STDAPI DllUnregisterServer(void) {
	return (UnregisterServer(CLSID_ShellExtension, szShellExtensionTitle) ? S_OK : E_FAIL);
}


char * RegPath1 = "Directory\\Background\\shellex\\ContextMenuHandlers\\Petko";
char * RegPath2 = "*\\shellex\\ContextMenuHandlers\\Petko";


//---------------------------------------------------------------------------
// RegisterServer
//---------------------------------------------------------------------------
BOOL RegisterServer(CLSID clsid, LPTSTR lpszTitle) {
	int      i;
	HKEY     hKey;
	LRESULT  lResult;
	DWORD    dwDisp;
	TCHAR    szSubKey[MAX_PATH];
	TCHAR    szCLSID[MAX_PATH];
	TCHAR    szModule[MAX_PATH];
	LPWSTR   pwsz;

	StringFromIID(clsid, &pwsz);
	if(pwsz) {
#ifdef UNICODE
		lstrcpy(szCLSID, pwsz);
#else
		WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
#endif
		//free the string
		LPMALLOC pMalloc;
		CoGetMalloc(1, &pMalloc);
		pMalloc->Free(pwsz);
		pMalloc->Release();
	}

	//get this app's path and file name
	GetModuleFileName(_hModule, szModule, MAX_PATH);

	DOREGSTRUCT ClsidEntries[] = {
		HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s"),                              NULL,                   lpszTitle,
		HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),              NULL,                   szModule,
		HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),              TEXT("ThreadingModel"), TEXT("Apartment"),
		HKEY_CLASSES_ROOT,   TEXT(RegPath1), NULL,                   szCLSID,
		HKEY_CLASSES_ROOT,   TEXT(RegPath2), NULL,                   szCLSID,
		NULL,                NULL,                                           NULL,                   NULL
	};

	// Register the CLSID entries
	for(i = 0; ClsidEntries[i].hRootKey; i++) {
		// Create the sub key string - for this case, insert the file extension
		wsprintf(szSubKey, ClsidEntries[i].szSubKey, szCLSID);
		lResult = RegCreateKeyEx(ClsidEntries[i].hRootKey, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
		if(NOERROR == lResult) {
			TCHAR szData[MAX_PATH];
			// If necessary, create the value string
			wsprintf(szData, ClsidEntries[i].szData, szModule);
			lResult = RegSetValueEx(hKey, ClsidEntries[i].lpszValueName, 0, REG_SZ, (LPBYTE)szData, (lstrlen(szData) + 1) * sizeof(TCHAR));
			RegCloseKey(hKey);
		}
		else
		return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------------------------
// UnregisterServer
//---------------------------------------------------------------------------
BOOL UnregisterServer(CLSID clsid, LPTSTR lpszTitle) {
	TCHAR szCLSID[GUID_SIZE + 1];
	TCHAR szCLSIDKey[GUID_SIZE + 32];
	TCHAR szKeyTemp[MAX_PATH + GUID_SIZE];
	LPWSTR pwsz;

	StringFromIID(clsid, &pwsz);
	if(pwsz) {
#ifdef UNICODE
		lstrcpy(szCLSID, pwsz);
#else
		WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
#endif
		//free the string
		LPMALLOC pMalloc;
		CoGetMalloc(1, &pMalloc);
		pMalloc->Free(pwsz);
		pMalloc->Release();
	}

	lstrcpy(szCLSIDKey, TEXT("CLSID\\"));
	lstrcat(szCLSIDKey, szCLSID);

	wsprintf(szKeyTemp, TEXT(RegPath1));
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	
	wsprintf(szKeyTemp, TEXT(RegPath2));
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	
	wsprintf(szKeyTemp, TEXT("%s\\%s"), szCLSIDKey, TEXT("InprocServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	RegDeleteKey(HKEY_CLASSES_ROOT, szCLSIDKey);

	return TRUE;
}

//---------------------------------------------------------------------------
// MsgBoxDebug
//---------------------------------------------------------------------------
void MsgBoxDebug(LPTSTR lpszMsg) {
	MessageBox(NULL,
	lpszMsg,
	"DEBUG",
	MB_OK);
}

//---------------------------------------------------------------------------
// MsgBox
//---------------------------------------------------------------------------
void MsgBox(LPTSTR lpszMsg) {
	MessageBox(NULL,
	lpszMsg,
	"Custom Context Menu",
	MB_OK);
}

//---------------------------------------------------------------------------
// MsgBoxError
//---------------------------------------------------------------------------
void MsgBoxError(LPTSTR lpszMsg) {
	MessageBox(NULL,
	lpszMsg,
	"Custom Context Menu",
	MB_OK | MB_ICONSTOP);
}

//---------------------------------------------------------------------------
// CShellExtClassFactory
//---------------------------------------------------------------------------
CShellExtClassFactory::CShellExtClassFactory() {
	m_cRef = 0L;
	_cRef++;
}

CShellExtClassFactory::~CShellExtClassFactory() {
	_cRef--;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid, LPVOID FAR *ppv) {
	*ppv = NULL;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
		*ppv = (LPCLASSFACTORY)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef() {
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	if (--m_cRef)
	return m_cRef;
	delete this;
	return 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj) {
	*ppvObj = NULL;
	if (pUnkOuter)
	return CLASS_E_NOAGGREGATION;
	LPCSHELLEXT pShellExt = new CShellExt();
	if (NULL == pShellExt)
	return E_OUTOFMEMORY;
	return pShellExt->QueryInterface(riid, ppvObj);
}

STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock) {
	return NOERROR;
}

//---------------------------------------------------------------------------
// CShellExt
//---------------------------------------------------------------------------
CShellExt::CShellExt() {
	m_cRef = 0L;
	m_pDataObj = NULL;
	_cRef++;
	m_hSciteBmp = LoadBitmap(_hModule, MAKEINTRESOURCE(IDB_SCITE));
	HRESULT hr;
	//MessageBox(0,"CShellExt::CShellExt()","",0);
	hr = SHGetMalloc(&m_pAlloc);
	if (FAILED(hr))
	m_pAlloc = NULL;
}

CShellExt::~CShellExt() {
	if (m_pDataObj)
	m_pDataObj->Release();
	_cRef--;
	m_pAlloc->Release();
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv) {
	*ppv = NULL;
	if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown)) {
		*ppv = (LPSHELLEXTINIT)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu)) {
		*ppv = (LPCONTEXTMENU)this;
	}
	if (*ppv) {
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef() {
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release() {
	if (--m_cRef)
	return m_cRef;
	delete this;
	return 0L;
}



STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey) {

	
	HRESULT hres = 0;
	if (m_pDataObj)
	m_pDataObj->Release();
	if (pDataObj) {
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
	}
	m_folder[0] = 0;
	if (pIDFolder)
	{
		ItemIDList list(pIDFolder);
		char buf[MAX_PATH];
		list.toString(buf);
		int len = strlen(buf);
		MultiByteToWideChar(
		CP_ACP, 0,
		buf,
		len,
		m_folder,
		MAX_PATH);
		m_folder[len] = 0;
		
	}
	return NOERROR;
}

STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
	UINT idCmd = idCmdFirst;
	BOOL bAppendItems=TRUE;
	TCHAR message[200];
	TCHAR szFileUserClickedOn[MAX_PATH];
	//wsprintf(message, "QueryContextMenu indexMenu %d | idCmdFirst %d | idCmdLast %d ", indexMenu, idCmdFirst, uFlags);
	
	//MessageBox(0,message,"",0);

	FORMATETC fmte = {
		CF_HDROP,
		(DVTARGETDEVICE FAR *)NULL,
		DVASPECT_CONTENT,
		-1,
		TYMED_HGLOBAL
	};

	UINT nIndex = indexMenu;
	m_strFiles.clear();
	if (m_pDataObj)
	{
		HRESULT hres = m_pDataObj->GetData(&fmte, &m_stgMedium);

		if (SUCCEEDED(hres)) {
			if (m_stgMedium.hGlobal) {
				m_cbFiles = DragQueryFile((HDROP)m_stgMedium.hGlobal, (UINT)-1, 0, 0);
			}
		}
		for (int i = 0; i < m_cbFiles; i++) {
			DragQueryFile((HDROP)m_stgMedium.hGlobal, i, szFileUserClickedOn, MAX_PATH);
			ContextMenuString fname(szFileUserClickedOn);
			m_strFiles.push_back(fname);
			
		}
	}
	m_cmiCurrentMenu.clear();
	for (ContextMenuItemsIterator i = ContextMenuSettings::begin(); !i.end(); i++) {
		ContextMenuItem mi = i.item();
		bool filtered = true;
		for (std::vector<ContextMenuString>::iterator it = m_strFiles.begin(); it != m_strFiles.end(); it++) {
			if ((*it).matches(mi.getFilter())) {
				filtered = false;
			}                          
		}
		if (((mi.getFlags() & MENU_ITEM_ONFILE) && m_pDataObj)
				|| ((mi.getFlags() & MENU_ITEM_ONDIR) && !m_pDataObj)) {
			if (mi.getFlags() & MENU_ITEM_SEPARATOR) 
			{
				if (mi.getPosition() >= 0) {
					nIndex = mi.getPosition();
				}
				InsertMenu(hMenu, nIndex++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);   
			} 
			else 
			{
				
				if ((mi.getFlags() & MENU_ITEM_ONDIR)
						|| !filtered
						) {
					if (mi.getPosition() >= 0) {
						nIndex = mi.getPosition();
					}
					InsertMenu(hMenu, nIndex, MF_STRING|MF_BYPOSITION, idCmd++, mi.getName().c_str());
					m_cmiCurrentMenu.push_back(mi);
					/*
				if (m_hSciteBmp) {
					SetMenuItemBitmaps (hMenu, nIndex, MF_BYPOSITION, m_hSciteBmp, m_hSciteBmp);
				}
				*/
					nIndex++;
				}
			}
		}
	}
	
	return ResultFromShort(idCmd-idCmdFirst);
}
STDMETHODIMP CShellExt::GetCommandString(UINT idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax) {
	if (uFlags == GCS_HELPTEXT && cchMax > 35) {
		lstrcpy(pszName, "Custom menu");
	}
	return NOERROR;
}
STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) {
	HRESULT hr = E_INVALIDARG;
	
	
	if (!HIWORD(lpcmi->lpVerb)) {
		UINT idCmd = LOWORD(lpcmi->lpVerb);
		ContextMenuItem mi = m_cmiCurrentMenu[idCmd];
		ContextMenuString allFiles("");
		
		for (std::vector<ContextMenuString>::iterator it = m_strFiles.begin(); it != m_strFiles.end(); it++) {
			allFiles += " \"";
			allFiles += (*it);
			allFiles += "\"";                  
		}
		if (mi.getFlags() & MENU_ITEM_FOREACH) {
			for (std::vector<ContextMenuString>::iterator it = m_strFiles.begin(); it != m_strFiles.end(); it++) {
				if ((mi.getFlags() & MENU_ITEM_ONDIR) || (*it).matches(mi.getFilter())) {
					hr = InvokeProgram(mi, lpcmi->hwnd, lpcmi->lpDirectory, lpcmi->lpVerb, lpcmi->lpParameters, 
					lpcmi->nShow, (*it), allFiles);
				}
			}
		} else {
			int i;
			for (i= 0; i < m_strFiles.size(); i++) {
				if(m_strFiles[i].matches(mi.getFilter())){
					break;
				}
			}
			hr = InvokeProgram(mi, lpcmi->hwnd, lpcmi->lpDirectory, lpcmi->lpVerb, lpcmi->lpParameters, lpcmi->nShow, 
			m_strFiles.size() > 0 ? m_strFiles[i] : allFiles, allFiles);
		}
	}
	return hr;
}


STDMETHODIMP CShellExt::InvokeProgram(
ContextMenuItem mi, 
HWND hParent, 
LPCSTR pszWorkingDir, 
LPCSTR pszCmd, 
LPCSTR pszParam, 
int iShowCmd,
ContextMenuString targetFile,
ContextMenuString allTargetFiles
) {
	
	
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	
	
	//
	//
	//MessageBox(0,message,"",0);
	//ContextMenuString cmd = ;
	
	std::string result = Pattern::replace("%f", std::string(mi.getCommand().data()), 
	Pattern::replace("\\\\", std::string(targetFile.data()), "\\\\\\\\"));
	
	result = Pattern::replace("%F", result, 
	Pattern::replace("\\\\", std::string(allTargetFiles.data()), "\\\\\\\\"));
	ContextMenuString cmd(result.data());
	WCHAR* wccommand = new WCHAR[cmd.size() + 1];
	cmd.toWideChar(wccommand);
	
	if (mi.getUser().size() > 0) 
	{
		WCHAR* wcuser = new WCHAR[mi.getUser().size() + 1];
		WCHAR* wcpass = new WCHAR[mi.getPass().size() + 1];
		
		mi.getUser().toWideChar(wcuser);
		mi.getPass().toWideChar(wcpass);
		
		//MessageBoxW(hParent, m_folder, L"aa", MB_OK);
		
		if (!CreateProcessWithLogonW(wcuser, L"", wcpass, 0,
					NULL, wccommand, 0, NULL, m_folder, &si, &pi)
				)
		{
			TCHAR message[200];
			wsprintf(message, "Failed to start \n\
							%s\n\
							user:%s", cmd.data(), mi.getUser().data());
			MessageBox(hParent,
			message,
			"Custom Context Menu",
			MB_OK);
		}
		delete[] wcuser;
		delete[] wcpass;
		delete[] wccommand;
		
	} else {
		if (!CreateProcessW (NULL, wccommand, NULL, NULL, FALSE, 0, NULL, m_folder, &si, &pi)) {
			TCHAR message[200];
			wsprintf(message, "Failed to start \n\
							%s", cmd.data());
			
			MessageBox(hParent,
			message,
			"Custom Context Menu",
			MB_OK);
			
		}
	}

	return NOERROR;
}
