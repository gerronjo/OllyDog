
#pragma once

#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x600
#endif 
#define SIDEBYSIDE_COMMONCONTROLS 1

#define VERSION	L"v1.0.01\0"
#define BIGNAME	L"OllyDog\0"

#define MENU_ITEM_ABOUTME	0x00001
#define MENU_ITEM_OPTIONS	0x00002
#define MENU_ITEM_CMDLINE	0x00003
#define MENU_ITEM_CMDLOGGER	0x00004

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <CommCtrl.h>

#include <plugin.h>

#include "CmdBar.hpp"
#include "CmdLog.hpp"
#include "CoreApi.hpp"
#include "Options.hpp"

class CDllMain : public CCoreApi, public COptions
{
public:
	CDllMain(); ~CDllMain();

	int  PluginInit(void);
	int  PluginClose(void);
	void PluginReset(void);
	void PluginDestroy(void);
	

	void PluginNotify(int, void *data, ulong, ulong);
	int  PluginException(t_run*, const t_disasm*, t_thread*, t_reg*, wchar_t*);
	void PluginTempBreakpoint(ulong, const t_disasm*, t_thread*, t_reg*);

	t_menu * PluginMenu(wchar_t *type);
	t_control * PluginOptions(UINT msg, WPARAM wp, LPARAM lp);

private:
	// The nemu item message produce ..
	static  int MenuProc(t_table*, wchar_t*, ulong, int);
	// The OD main window message produce ..
	static  LRESULT CALLBACK OnOllyWndProc(HWND, UINT, WPARAM, LPARAM);
	// The MDI Client window message produce ..
	static  LRESULT CALLBACK OnMdiWndProc (HWND, UINT, WPARAM, LPARAM);
	// Message filter produce ..
	static  LRESULT CALLBACK OnMouseLLProc(int   code, WPARAM, LPARAM);

public:
	HINSTANCE				m_hInst;
	t_menu*				m_pMenu;
	t_control*				m_pCtrl;
	CCmdBar 				m_cCmdBar;
	
	wchar					m_PyPath[MAX_PATH+1];
	wchar					m_RbPath[MAX_PATH+1];
	wchar					m_JarPath[MAX_PATH+1];
	wchar					m_LuaPath[MAX_PATH+1];

protected:
	WNDPROC				m_hMdiWndProc;
	WNDPROC				m_hOllyWndProc;
};

