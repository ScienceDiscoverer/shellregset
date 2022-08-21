#include <windows.h>
#include "data.h"

//#include "C:\ScienceDiscoverer\PROGRAMZ\MicroTests\sddb.h"

// TODO Add context menu for custom folder add/rem to My PC using MY_PC_CUST_DIR_ADD.REG

#define NINJA
#ifndef NINJA
#include <iostream>
#endif

#define MAX_S 500

#define HIDDEN_TOGGLE !wcscmp(cmd, htog_arg)
const wchar_t *htog_arg = L"-hidtogg";
const wchar_t *htog_exp_setts = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
const wchar_t *htog_shell = L"SOFTWARE\\Classes\\Directory\\Background\\shell\\HiddenToggle";
const wchar_t *htog_shell_cmd =
	L"SOFTWARE\\Classes\\Directory\\Background\\shell\\HiddenToggle\\command";

#define CLIP_PATH !wcsncmp(cmd, path_arg, 6)
const wchar_t *path_arg = L"-cpath %V"; // %V gives 1st argument if present, working dir. otherwise
const wchar_t *path_shell_all = L"AllFilesystemObjects\\shell\\copyaspath";
const wchar_t *path_shell_all_cmd = L"AllFilesystemObjects\\shell\\copyaspath\\command";
const wchar_t *path_shell_bdir = L"Directory\\Background\\shell\\copyaspath";
const wchar_t *path_shell_bdir_cmd = L"Directory\\Background\\shell\\copyaspath\\command";

const wchar_t *sico_arg = L"1";
const wchar_t *sico_exp_setts =
	L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons";

#define MAKE_LINK !wcsncmp(cmd, link_arg, 6)
const wchar_t *link_arg = L"-mlink %1";
const wchar_t *link_shell_all = L"AllFilesystemObjects\\shell\\makelink";
const wchar_t *link_shell_all_cmd = L"AllFilesystemObjects\\shell\\makelink\\command";

#define MAKE_SLNK !wcsncmp(cmd, slnk_arg, 6)
const wchar_t *slnk_arg = L"-mslnk %1";
const wchar_t *slnk_shell_all = L"AllFilesystemObjects\\shell\\makeslink";
const wchar_t *slnk_shell_all_cmd = L"AllFilesystemObjects\\shell\\makeslink\\command";

#define MAKE_HLNK !wcsncmp(cmd, hlnk_arg, 6)
const wchar_t *hlnk_arg = L"-mhlnk %1";
const wchar_t *hlnk_shell_files = L"*\\shell\\makehlink";
const wchar_t *hlnk_shell_files_cmd = L"*\\shell\\makehlink\\command";

void quoteExePath(LPCWSTR args, LPWSTR out, bool ico = false);
void prskb(WORD key);
bool hasSpace(LPCWSTR txt);
void clip(LPCWSTR txt);

// HINSTANCE -> "handle" to "instance" aka "module".
// It's NOT a handle. And not to "instance" or "module".
// I's all 16 bit Windows legacy backwards compatability nonsense.
// Since 16-bit Windows had a common address space, the GetInstanceData function was really
// nothing more than a hmemcpy, and many programs relied on this and just used raw hmemcpy
// instead of using the documented API.
// In Win32/Win64 it's actually executable (DLL or EXE) image.
// HINSTANCE points to actual virtual adress where first byte of
// executable's code is located: cout << (const char*)hinst ---> MZ? (? = 0x90/0x00)
int WINAPI wWinMain(
	_In_		HINSTANCE hinst,	// "Handle" to "instance"
	_In_opt_	HINSTANCE phinst,	// "Handle" to "previous instance", always NULL
	_In_		PWSTR cmd,			// Command line arguments
	_In_		int show)			// Default user preference for ShowWindow()
{
#ifndef NINJA
	AllocConsole();
	FILE *s = freopen("CONIN$", "r", stdin);
	s = freopen("CONOUT$", "w", stdout);
	s = freopen("CONOUT$", "w", stderr);
#endif

	if(HIDDEN_TOGGLE)
	{
		if(regGet(htog_exp_setts, L"Hidden") == 2) // Was hidden
		{
			regSet(htog_exp_setts, L"Hidden", (DWORD)0x1);
			regSet(htog_exp_setts, L"ShowSuperHidden", (DWORD)0x1);
			regSet(htog_shell, L"", L"Приховати замасковані");
			prskb(VK_F5);
		}
		else // Was shown
		{
			regSet(htog_exp_setts, L"Hidden", (DWORD)0x2);
			regSet(htog_exp_setts, L"ShowSuperHidden", (DWORD)0x0);
			regSet(htog_shell, L"", L"Показати замасковані");
			prskb(VK_F5);
		}

		return 0;
	}
	else if(CLIP_PATH)
	{
		if(hasSpace(cmd+7))
		{
			SIZE_T l = wcslen(cmd+7);
			cmd[6] = '"';
			cmd[7+l] = '"';
			cmd[8+l] = '\0';
			clip(cmd+6);
		}
		else
		{
			clip(cmd+7);
		}

		return 0;
	}
	else if(MAKE_LINK)
	{
		makeZelda(cmd+7, SHORT_ZELDA);
	}
	else if(MAKE_SLNK)
	{
		makeZelda(cmd+7, SYMBOL_ZELDA);
	}
	else if(MAKE_HLNK)
	{
		makeZelda(cmd+7, HARD_ZELDA);
	}

	wchar_t path[MAX_S];

	// Set up Show Hidden Files/Folders toggle
	DWORD hidden = regGet(htog_exp_setts, L"Hidden");
	const wchar_t *label = hidden == 2 ? L"Показати замасковані" : L"Приховати замасковані";
	quoteExePath(htog_arg, path);
	regSetRoot(HKEY_CURRENT_USER);
	regSet(htog_shell, L"", label);
	regSet(htog_shell_cmd, L"", path);

	// Set up Copy As Path toggle
	quoteExePath(path_arg, path);
	regSetRoot(HKEY_CLASSES_ROOT);
	regSet(path_shell_all, L"", L"Копіювати як шлях");
	regSet(path_shell_all, L"Icon", L"imageres.dll,-5302");
	regSet(path_shell_all_cmd, L"", path);
	regSet(path_shell_bdir, L"", L"Копіювати як шлях");
	regSet(path_shell_bdir, L"Icon", L"imageres.dll,-5302");
	regSet(path_shell_bdir_cmd, L"", path);

	// Set up Shortcut Overlay Removal
	quoteExePath(sico_arg, path, true);
	regSetRoot(HKEY_LOCAL_MACHINE);
	regSet(sico_exp_setts, L"29", path);

	// Set up Shortcut creation
	quoteExePath(link_arg, path);
	regSetRoot(HKEY_CLASSES_ROOT);
	regSet(link_shell_all, L"", L"Створити ярлик");
	regSet(link_shell_all, L"Icon", L"shell32.dll,263");
	regSet(link_shell_all_cmd, L"", path);

	// Set up Symbolic Link creation
	quoteExePath(slnk_arg, path);
	regSetRoot(HKEY_CLASSES_ROOT);
	regSet(slnk_shell_all, L"", L"Створити symlink");
	regSet(slnk_shell_all, L"Icon", L"shell32.dll,122");
	regSet(slnk_shell_all, L"Extended");
	regSet(slnk_shell_all_cmd, L"", path);

	// Set up Hard Link creation
	quoteExePath(hlnk_arg, path);
	regSetRoot(HKEY_CLASSES_ROOT);
	regSet(hlnk_shell_files, L"", L"Створити hardlink");
	regSet(hlnk_shell_files, L"Icon", L"shell32.dll,134");
	regSet(hlnk_shell_files, L"Extended");
	regSet(hlnk_shell_files_cmd, L"", path);

	return 0;
}

void quoteExePath(LPCWSTR args, LPWSTR out, bool ico)
{
	out[0] = L'\"';
	out[MAX_S-1] = L'\0';
	GetModuleFileName(NULL, out+1, MAX_PATH);
	DWORD s = (DWORD)wcslen(out)+1;

	out[s-1] = L'\"';
	out[s] = ico ? ',' : ' ';
	wcscpy(out+s+1, args);
}

void prskb(WORD key)
{
	INPUT inp[2];

	inp[0].type = INPUT_KEYBOARD;

	inp[0].ki.wVk = key;
	inp[0].ki.wScan = 0x0;
	inp[0].ki.dwFlags = 0x0;
	inp[0].ki.time = 0x0;
	inp[0].ki.dwExtraInfo = 0x0;

	inp[1] = inp[0];
	inp[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, inp, sizeof(INPUT));
}

bool hasSpace(LPCWSTR txt)
{
	while(*txt++ != 0)
	{
		if(*txt == ' ')
		{
			return true;
		}
	}

	return false;
}

void clip(LPCWSTR txt)
{
	SIZE_T l = wcslen(txt) + 1;
	HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, l * sizeof(wchar_t));
	memcpy(GlobalLock(hmem), txt, l * sizeof(wchar_t));
	GlobalUnlock(hmem);
	OpenClipboard(NULL);
	EmptyClipboard(); // Using this with OpenClipboard(NULL) should not work, but it does! Magick?
	SetClipboardData(CF_UNICODETEXT, hmem);
	CloseClipboard();
}