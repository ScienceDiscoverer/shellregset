#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include "data.h"

#define CC(x) if((x) != S_OK) { goto exit; } // Check-Call
#define RELEASE(x) if(x != NULL) { x->Release(); }

HKEY app_root = DEF_ROOT;
//LPCWSTR app_key = L"SOFTWARE";
LPCWSTR app_ar_key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
LPCWSTR app_ar_val = L"MicroAudioSwitcher";




/*___________________________________________________________________
|  regKeyCrOp:
|    Attempts to create new registry key, if already exists, opens it
|
|  sub_k: Path to the new key from application root key
|
|  Return value:
|    Handle to created or opened key
|____________________________________________________________________*/
HKEY regKeyCrOp(LPCWSTR sub_k);


/*___________________________________________________________________
|  regKeyOp:
|    Opens already exising key
|
|  sub_k: Path to the key from application root key
|
|  Return value:
|            Key exists -> Handle to the key
|    Key does not exist -> NULL
|____________________________________________________________________*/
HKEY regKeyOp(LPCWSTR sub_k);


/*___________________________________________________________________
|  regKeyOp:
|    Destroys registry key and all it's sub-keys/values
|
|  sub_k: Path to the key from application root key
|____________________________________________________________________*/
void regKeyDel(LPCWSTR sub_k);


/*___________________________________________________________________
|  incFname:
|    Adds _x to the file name, incrementing x accordingly
|
|  fn: Full path name to increment. Must be in a large enough buffer
|____________________________________________________________________*/
void incFname(LPWSTR fn);




bool regChk(LPCWSTR app_key, LPCWSTR name)
{
	HKEY key = regKeyOp(app_key);

	// Get Registry Value ============================================================================
	LSTATUS res = RegGetValue(
		key,							//    [I]  Handle to opened key (KEY_QUERY_VALUE  acc. req.)
		NULL,							//  [I|O]  Name of the registry subkey to be retrieved
		name,							//  [I|O]  Name of the value (if NULL or "" get def. value)
		RRF_RT_ANY,						//  [I|O]  Flags comb. of data types to set filter (+ opt.)
		NULL,							//  [O|O]  Ptr to variable that recieves value type
		NULL,							//  [O|O]  Ptr to var that receive data of the value || NULL
		NULL);							// [IO|O]  Ptr sz. B -> N. of B cop. (NULL only pvData = NULL)
	// ===============================================================================================

	RegCloseKey(key);
	return !res;
}

DWORD regGet(LPCWSTR app_key, LPCWSTR name)
{
	HKEY key = regKeyOp(app_key);
	DWORD dat = 0, sz = sizeof(dat);

	// Get Registry Value ============================================================================
	RegGetValue(
		key,							//    [I]  Handle to opened key (KEY_QUERY_VALUE  acc. req.)
		NULL,							//  [I|O]  Name of the registry subkey to be retrieved
		name,							//  [I|O]  Name of the value (if NULL or "" get def. value)
		RRF_RT_REG_DWORD,				//  [I|O]  Flags comb. of data types to set filter (+ opt.)
		NULL,							//  [O|O]  Ptr to variable that recieves value type
		(PVOID)&dat,					//  [O|O]  Ptr to var that receive data of the value || NULL
		&sz);							// [IO|O]  Ptr sz. B -> N. of B cop. (NULL only pvData = NULL)
	// ===============================================================================================

	RegCloseKey(key);
	return dat;
}

void regSet(LPCWSTR app_key, LPCWSTR name)
{
	HKEY key = regKeyCrOp(app_key);

	// Set Registry Value ============================================================================
	RegSetValueEx(
		key,							//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
		name,							// [I|O]  Value name (if not exist, will be created)
		0,								//        Reserved, must be 0
		REG_SZ,							//   [I]  Type of value beeing set
		NULL,							//   [I]  Ptr to the data that will be stored
		NULL);							//   [I]  Sizeof data in bytes (for strings must include \0)
	// ===============================================================================================

	RegCloseKey(key);
}

void regSet(LPCWSTR app_key, LPCWSTR name, DWORD data)
{
	HKEY key = regKeyCrOp(app_key);

	// Set Registry Value ============================================================================
	RegSetValueEx(
		key,							//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
		name,							// [I|O]  Value name (if not exist, will be created)
		0,								//        Reserved, must be 0
		REG_DWORD,						//   [I]  Type of value beeing set
		(const BYTE*)&data,				//   [I]  Ptr to the data that will be stored
		sizeof(data));					//   [I]  Sizeof data in bytes (for strings must include \0)
	// ===============================================================================================

	RegCloseKey(key);
}

void regSet(LPCWSTR app_key, LPCWSTR name, LPCWSTR data)
{
	HKEY key = regKeyCrOp(app_key);
	DWORD s = DWORD(wcslen(data)+1) * sizeof(wchar_t);

	// Set Registry Value ============================================================================
	RegSetValueEx(
		key,							//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
		name,							// [I|O]  Value name (if not exist, will be created)
		0,								//        Reserved, must be 0
		REG_SZ,							//   [I]  Type of value beeing set
		(const BYTE*)data,				//   [I]  Ptr to the data that will be stored
		s);								//   [I]  Sizeof data in bytes (for strings must include \0)
	// ===============================================================================================

	RegCloseKey(key);
}

void regDel(LPCWSTR app_key, LPCWSTR name)
{
	HKEY key = regKeyOp(app_key);

	// Delete Registry Value =========================================================================
	RegDeleteValue(
		key,							//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
		name);							// [I|O]  Value name to be removed
	// ===============================================================================================

	RegCloseKey(key);
}

bool regAutoStChk()
{
	HKEY key = regKeyOp(app_ar_key);

	// Get Registry Value ============================================================================
	LSTATUS res = RegGetValue(
		key,							//    [I]  Handle to opened key (KEY_QUERY_VALUE  acc. req.)
		NULL,							//  [I|O]  Name of the registry subkey to be retrieved
		app_ar_val,						//  [I|O]  Name of the value (if NULL or "" get def. value)
		RRF_RT_ANY,						//  [I|O]  Flags comb. of data types to set filter (+ opt.)
		NULL,							//  [O|O]  Ptr to variable that recieves value type
		NULL,							//  [O|O]  Ptr to var that receive data of the value || NULL
		NULL);							// [IO|O]  Ptr sz. B -> N. of B cop. (NULL only pvData = NULL)
	// ===============================================================================================

	RegCloseKey(key);
	return !res;
}

void regAutoStSet(bool state)
{
	HKEY key = regKeyCrOp(app_ar_key);

	if(state)
	{
		wchar_t data[309];
		data[0] = L'\"';
		data[308] = L'\0';
		GetModuleFileName(NULL, data+1, 300-1);
		DWORD s = (DWORD)wcslen(data)+1;

		data[s-1] = L'\"';
		wcscpy(data+s, L" -silent");
		// What? I don't want to include <string> and <sstream> here, OK?

		// Set Registry Value ========================================================================
		RegSetValueEx(
			key,						//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
			app_ar_val,					// [I|O]  Value name (if not exist, will be created)
			0,							//        Reserved, must be 0
			REG_SZ,						//   [I]  Type of value beeing set
			(const BYTE*)data,			//   [I]  Ptr to the data that will be stored
			(s+10) * sizeof(wchar_t));	//   [I]  Sizeof data in bytes (for strings must include \0)
		// ===========================================================================================

		RegCloseKey(key);
	}
	else
	{
		// Delete Registry Value =====================================================================
		RegDeleteValue(
			key,						//   [I]  Handle to opened key (KEY_SET_VALUE acc. req.)
			app_ar_val);					// [I|O]  Value name to be removed
		// ===========================================================================================
	}

	RegCloseKey(key);
}

void regSetRoot(HKEY root)
{
	app_root = root;
}

void makeZelda(LPCWSTR obj_path, int zelda_type)
{
	SIZE_T l = wcslen(obj_path);
	LPCWSTR ldot = obj_path+l, lslash = ldot; // Null wchar aka. Path End
	while(*(--ldot) != '.' && ldot >= obj_path)
	{
		;
	}
	while(*(--lslash) != L'\\' && lslash >= obj_path)
	{
		;
	}

	wchar_t buff[MAX_PATH+5]; // + .lnk\0
	if(ldot < lslash) // No file extention present
	{
		wcscpy(buff, obj_path);

		if(zelda_type == SYMBOL_ZELDA)
		{
			buff[l] = '.';
			buff[l+1] = 's';
			buff[l+2] = 'l';
			buff[l+3] = 'n';
			buff[l+4] = 'k';
			buff[l+5] = 0;
		}
	}
	else // File extention detected
	{
		if(zelda_type == HARD_ZELDA)
		{
			wcscpy(buff, obj_path);
		}
		else
		{
			l = ldot - obj_path; // Dam babysitting compiler. Stop converting from bytes to elements!
			memcpy(buff, obj_path, l * sizeof(wchar_t));

			if(zelda_type == SYMBOL_ZELDA)
			{
				buff[l] = '.';
				buff[l+1] = 's';
				buff[l+2] = 'l';
				buff[l+3] = 'n';
				buff[l+4] = 'k';

				wcscpy(buff + l + 5, ldot);
			}
			else
			{
				buff[l] = '.';
				buff[l+1] = 'l';
				buff[l+2] = 'n';
				buff[l+3] = 'k';
				buff[l+4] = 0;
			}
		}
	}

	// Check if link with the same name already exist
	// Note: its useless to create duplicate links for same file/folder
	// But with proper links that have no file extentions, there could be files with same name
	// but different extnetion! Duplicate links for same file will be just side-effect, not feature!
	// Like a standart OS "Create Link" function

	HANDLE file;
	_WIN32_FIND_DATAW fdata;

	while((file = FindFirstFile(buff, &fdata)) != INVALID_HANDLE_VALUE)
	{
		LPCWSTR fn = fdata.cFileName;
		incFname(buff);

		FindClose(file);
	}

	// TODO: Possible optimisation is to FindNextFile with mask fname_*.lnk then inc last
	// Advantage: much faster on large amount of duplicate files, unlikely in real world scenario
	// Limitation 1: f, f_1, f_2, f_8 -> creates f_9, not f_3
	// Limitation 2: f, f_1, f_2, f_foo -> creates f_foo_1, not f_3


	switch(zelda_type)
	{
	case HARD_ZELDA:
		CreateHardLink(buff, obj_path, NULL);
		break;
	case SYMBOL_ZELDA:
		
		if(GetFileAttributes(obj_path) & FILE_ATTRIBUTE_DIRECTORY)
		{
			int res = CreateSymbolicLink(buff, obj_path, SYMBOLIC_LINK_FLAG_DIRECTORY);
		}
		else
		{
			int res = CreateSymbolicLink(buff, obj_path, 0);
		}
		break;
	case SHORT_ZELDA:
	{
		IShellLink *slnk = NULL; // Get Link interface...
		IPersistFile *pf = NULL; // Get Persistent File interface to save the link...

		CC(CoInitialize(NULL)); // Initialise COM concurrency...

		CC(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&slnk));
		slnk->SetPath(obj_path);
		slnk->SetDescription(L"Created with shellregset");

		CC(slnk->QueryInterface(IID_IPersistFile, (LPVOID *)&pf));
		CC(pf->Save(buff, TRUE));

	exit:
		RELEASE(pf);
		RELEASE(slnk);
		CoUninitialize();
	}
		break;
	default:
		break;
	}
}

HKEY regKeyCrOp(LPCWSTR sub_k)
{
	REGSAM sam = KEY_SET_VALUE | KEY_QUERY_VALUE;
	HKEY key = NULL;

	// Create Registry Key ===========================================================================
	RegCreateKeyEx(
		app_root,						//   [I]  Handle to open reg key, or root key
		sub_k,							//   [I]  Subkey name relative to 1st param (32 lvl deep)
		0,								//        Reserved, must be 0
		NULL,							// [I|O]  Class type of this key (OOP shenenigans?)
		REG_OPTION_NON_VOLATILE,		//   [I]  Key options (0 -> std non-volatile key)
		sam,							//   [I]  Security and Access Mask
		NULL,							// [I|O]  Ptr to SECURITY_ATTRIBUTES (handle inheritability)
		&key,							//   [O]  Ptr to HKEY variable that will hold returned handle
		NULL);							// [O|O]  Dis-pos -> CREATED_NEW_KEY | OPENED_EXISTING_KEY
	// ===============================================================================================

	return key;
}

HKEY regKeyOp(LPCWSTR sub_k)
{
	REGSAM sam = KEY_SET_VALUE | KEY_QUERY_VALUE;
	HKEY key = NULL;

	// Open Registry Key =============================================================================
	RegOpenKeyEx(
		app_root,						//   [I]  Handle to open reg key, or root key
		sub_k, 							// [I|O]  Name of the registry subkey to be opened
		0, 								//   [I]  Options -> 0 || REG_OPTION_OPEN_LINK
		sam,							//   [I]  Security and Access Mask
		&key);							//   [O]  Ptr to variable that receives opened key handle
	// ===============================================================================================

	return key;
}

void regKeyDel(LPCWSTR sub_k)
{
	// Delete Registry Key ===========================================================================
	RegDeleteKeyEx(
		HKEY_CLASSES_ROOT,				// [I]  Handle to open reg key, or root key
		sub_k,							// [I]  Subkey name to be deleted, relative to 1st param
		KEY_WOW64_64KEY, 				// [I]  Key platform -> KEY_WOW64_32KEY | KEY_WOW64_64KEY
		0);								//      Reserved, must be 0
	// ===============================================================================================
}

static void incFname(LPWSTR fn)
{
	SIZE_T l = wcslen(fn);
	LPWSTR ldot = fn+l, lslash = ldot, luscor = ldot; // Null wchar aka. File Name End
	while(*(--lslash) != L'\\' && lslash >= fn)
	{
		;
	}
	while(*(--ldot) != '.' && ldot >= lslash)
	{
		;
	}

	LPWSTR ext = NULL;
	if(ldot > lslash) // Filename extention detected, save it
	{
		luscor = ldot;
		SIZE_T extl = wcslen(ldot);
		ext = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (extl+1) * sizeof(wchar_t));
		wcscpy(ext, ldot);
	}
	else // No file extention found
	{
		ldot = fn+l;
	}

	int digs = 0;
	char num[10]; // 4�294�967�296 -> 10
	memset(num, 0x30, 10); // Memset to '0's
	while(*(--luscor) != '_' && luscor >= lslash) // Searching only up to last slash
	{
		if(*luscor >= 0x30 /*0*/ && *luscor <= 0x39 /*9*/)
		{
			num[digs++] = (char)*luscor; // Writes number in Little Endian order!
		}
		else
		{
			break;
		}
	}

	LPWSTR ext_targ = NULL;
	if(!digs) // First copy of a file
	{
		ldot[0] = '_';
		ldot[1] = '1';
		ext_targ = ldot+2;
	}
	else // File was incremented before
	{
		// Basically, this is General Purpose Base10 Little Endian Adder
		char carry = 1;

		char *num_s = num-1, *num_e = num+digs;
		while(++num_s != num_e)
		{
			if((*num_s += carry) > 0x39) // 9 -> Carry Over
			{
				*num_s = 0x30; // 0
				carry = 1;

				if(num_s+1 == num_e && num_e+1 < num+10)
				{
					++num_e;
				}
			}
			else // -> No Carry Over
			{
				carry = 0;
			}
		}

		// Ignoring overflow here, as it is unlikely that somebody will create
		// 10 000 000 000 copies of a link to same file in single directory

		++luscor;
		while(--num_e >= num)
		{
			*luscor++ = *num_e; // Convert to Big Endian
		}
		ext_targ = luscor;
	}

	if(ext != NULL)
	{
		wcscpy(ext_targ, ext);
		int res = HeapFree(GetProcessHeap(), 0, ext);
	}
}