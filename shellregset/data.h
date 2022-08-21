#pragma once

#define DEF_ROOT HKEY_CURRENT_USER

#define HARD_ZELDA   0x0
#define SYMBOL_ZELDA 0x1
#define SHORT_ZELDA  0x2

/*___________________________________________________________________
|  regChk:
|    Checks if specified value exists in the app's registry key
|
|  app_key: Registry key to set in relation to root key
|     name: Name of the value to test
|
|  Return value:
|       Value exist -> true
|    No value exist -> false
|____________________________________________________________________*/
bool regChk(LPCWSTR app_key, LPCWSTR name);


/*___________________________________________________________________
|  regGet:
|    Reads DWORD value from the Registry, first check with regChk!
|
|  app_key: Registry key to set in relation to root key
|     name: Name of the value to retrieve
|  
|  Return value:
|       Value exist -> Data stored by the value
|    No value exist -> 0
|____________________________________________________________________*/
DWORD regGet(LPCWSTR app_key, LPCWSTR name);


/*___________________________________________________________________
|  regSet:
|    Sets empty value in the Registry
|
|  app_key: Registry key to set in relation to root key
|     name: Name of the value to set
|____________________________________________________________________*/
void regSet(LPCWSTR app_key, LPCWSTR name);


/*___________________________________________________________________
|  regSet(dw):
|    Sets DWORD value in the Registry
|
|  app_key: Registry key to set in relation to root key
|     name: Name of the value to set
|     data: Data to associate with the value
|____________________________________________________________________*/
void regSet(LPCWSTR app_key, LPCWSTR name, DWORD data);

/*___________________________________________________________________
|  regSet(sz):
|    Sets SZ (null-terminated string) value in the Registry
|
|  app_key: Registry key to set in relation to root key
|     name: Name of the value to set
|     data: Null terminated string to associate with the value
|____________________________________________________________________*/
void regSet(LPCWSTR app_key, LPCWSTR name, LPCWSTR data);


/*___________________________________________________________________
|  regDel:
|    Removes value from the Registry
|
|  name: Name of the value to be removed
|____________________________________________________________________*/
void regDel(LPCWSTR app_key, LPCWSTR name);


/*___________________________________________________________________
|  regAutoStChk:
|    Tests if application is scheduled to autorun on startup
|
|  Return value:
|    AR key exist -> true
|       No AR key -> false
|____________________________________________________________________*/
bool regAutoStChk();


/*___________________________________________________________________
|  regAutoStSet:
|    Switches the state of application autorun capabilities
|
|  state: New state for autorun (true -> on, false -> off)
|____________________________________________________________________*/
void regAutoStSet(bool state);


/*___________________________________________________________________
|  regSetRoot:
|    Changes root registry key for application.
|
|  app_root: New root registry key
|____________________________________________________________________*/
void regSetRoot(HKEY root);


/*___________________________________________________________________
|  makeZelda:
|    Creates spesific Shell Link object at spesified object's path
|
|    obj_path: Full path to linked object
|  zelda_type: Link type to create (shortcut/symbolic/hard)
|____________________________________________________________________*/
void makeZelda(LPCWSTR obj_path, int zelda_type);