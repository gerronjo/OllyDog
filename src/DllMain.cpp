
#include "DllMain.hpp"

static CDllMain * CApp = nullptr;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// PLUGIN INITIALIZATION /////////////////////////////

// ODBG2_Pluginquery() is a "must" for valid OllyDbg plugin. It must check
// whether given OllyDbg version is correctly supported, and return 0 if not.
// Then it should fill plugin name and plugin version (as UNICODE strings) and
// return version of expected plugin interface. If OllyDbg decides that this
// plugin is not compatible, it will be unloaded. Plugin name identifies it
// in the Plugins menu. This name is max. 31 alphanumerical UNICODE characters
// or spaces + terminating L'\0' long. To keep life easy for users, name must
// be descriptive and correlate with the name of DLL. Parameter features is
// reserved for the future. I plan that features[0] will contain the number
// of additional entries in features[]. Attention, this function should not
// call any API functions: they may be incompatible with the version of plugin!
pentry (int) ODBG2_Pluginquery(
	int odver, ulong *features, wchar_t name[SHORTNAME], wchar_t version[SHORTNAME]) {
	// Check whether OllyDbg has compatible version. This plugin uses only the
	// most basic functions, so this check is done pro forma, just to remind of
	// this option.
	if (odver<201) return 0;
	// Create instance ..
	CApp = new CDllMain();
	// Report name and version to OllyDbg.
	::wcscpy_s(name, SHORTNAME, BIGNAME);		// Name of plugin
	::wcscpy_s(version, SHORTNAME, VERSION);	// Version of plugin
	return PLUGIN_VERSION;          			// Expected API version
};

// Optional entry, called immediately after ODBG2_Pluginquery(). Plugin should
// make one-time initializations and allocate resources. On error, it must
// clean up and return -1. On success, it must return 0.
pentry (int) ODBG2_Plugininit(void) {
	return CApp->PluginInit();	// 0 means that report success.
};

// Function is called when user opens new or restarts current application.
// Plugin should reset internal variables and data structures to the initial
// state.
pentry (void) ODBG2_Pluginreset(void) {
};

// Adds items to OllyDbgs menu system.
pentry (t_menu *) ODBG2_Pluginmenu(wchar_t *type) {
	return CApp->PluginMenu(type);
}


// Optional entry, implements plugin's options page in the Plugin options
// dialog. First time this function is called with msg=WM_INITDIALOG, wp=0 and
// lp=0. Note that this is the artificial message. If plugin wants to insert
// its own page (or even several pages), it must return pointer to page
// description (array of structures t_control). Each new page must begin with
// CA_COMMENT. When user makes any actions and dialog receives WM_COMMAND, it
// passes this message here. Returned value is ignored. Finally, after user
// pressed OK or Cancel, this entry is called for the last time with
// msg=WM_CLOSE and wp=1 on OK and 0 on cancel. This is the best time for
// plugin to adopt new settings. Do not forget to undo custom controls on
// Cancel!
pentry (t_control *) ODBG2_Pluginoptions(UINT msg, WPARAM wp, LPARAM lp) {
	return CApp->PluginOptions(msg, wp, lp);
};

// OllyDbg calls this optional function when user wants to terminate OllyDbg.
// All MDI windows created by plugins still exist. Function must return 0 if
// it is safe to terminate. Any non-zero return will stop closing sequence. Do
// not misuse this possibility! Always inform user about the reasons why
// termination is not good and ask for his decision! Attention, don't make any
// unrecoverable actions for the case that some other plugin will decide that
// OllyDbg should continue running.
pentry (int) ODBG2_Pluginclose(void) {
	// For automatical restoring of open windows, mark in .ini file whether
	return CApp->PluginClose();
};

// OllyDbg calls this optional function once on exit. At this moment, all MDI
// windows created by plugin are already destroyed (and received WM_DESTROY
// messages). Function must free all internally allocated resources, like
// window classes, files, memory etc.
pentry (void) ODBG2_Plugindestroy(void) {
	if (CApp != nullptr) delete CApp;
};



////////////////////////////////////////////////////////////////////////////////
/////////////////////////// EVENTS AND NOTIFICATIONS ///////////////////////////

// If you define ODBG2_Pluginmainloop(), this function will be called each time
// from the main Windows loop in OllyDbg. If there is some (real) debug event
// from the debugged application, debugevent points to it, otherwise it's NULL.
// If fast command emulation is active, it does not receive all (emulated)
// exceptions, use ODBG2_Pluginexception() instead. Do not declare these two
// functions unnecessarily, as this may negatively influence the overall speed!
//
// pentry (void) ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent) {
// };
// pentry (void) ODBG2_Plugintempbreakpoint (
// 		ulong addr, const t_disasm *da, t_thread *pthr,t_reg *preg
// ) {
// }
pentry (int) ODBG2_Pluginexception (
	t_run *prun, const t_disasm *da, t_thread *pthr,t_reg *preg, wchar_t *message
	) {
	return CApp->PluginException(prun, da, pthr, preg, message);
}

//
// Optional entry, called each time OllyDbg analyses some module and analysis
// is finished. Plugin can make additional analysis steps. Debugged application
// is paused for the time of processing. Bookmark plugin, of course, does not
// analyse code. If you don't need this feature, remove ODBG2_Pluginanalyse()
// from the plugin code.
//
pentry (void) ODBG2_Pluginanalyse(t_module *pmod)
{
}

// Optional entry, notifies plugin on relatively infrequent events.
pentry (void) ODBG2_Pluginnotify(int code, void *data, ulong parm1, ulong parm2) {
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// .UDD FILES //////////////////////////////////

// Time to save data to .udd file! This is done by calling Pluginsaverecord()
// for each data item that must be saved. Global, process-oriented data must
// be saved in main .udd file (named by .exe); module-relevant data must be
// saved in module-related files. Don't forget to save all addresses relative
// to module's base, so that data will be restored correctly even when module
// is relocated.
pentry (void) ODBG2_Pluginsaveudd(t_uddsave *psave, t_module *pmod, int ismainmod) {
};

// OllyDbg restores data from .udd file and has encountered record belonging
// to our plugin. Note that module descriptor pointed to by pmod can be
// incomplete, i.e. does not necessarily contain all informations, especially
// that from .udd file.
pentry (void) ODBG2_Pluginuddrecord(
	t_module *pmod,int ismainmod, ulong tag,ulong size,void *data) {
	if (pmod==NULL || data==NULL) return;	// Must not happen!
	if (size!=sizeof(ulong)) return;		// Invalid record size
	if (tag!=0x0001) return;					// Invalid tag
};




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Main Class //////////////////////////////////


CDllMain::CDllMain()						// Here is the constructor ..
{
	static t_menu g_tMainMenu[] = {		// It's the plugin menu .
		{// KK_DIRECT | KK_ALT | VK_F7
			text("Options"), 
			text("Open The Options Panel."),  
			K_PLUGOPTIONS, CDllMain::MenuProc, nullptr, MENU_ITEM_OPTIONS},
		{// KK_DIRECT | KK_ALT | VK_F8
			text("|Show/Hide CmdBar"), 
			text("Show or Hide the Command Bar."),  
			K_SHOWBAR, CDllMain::MenuProc, nullptr, MENU_ITEM_CMDLINE},
		{// KK_DIRECT | KK_ALT | VK_F9
			text("|Open Command Logger"), 
			text("Show or Hide the Command Bar."),  
			K_NONE, CDllMain::MenuProc, nullptr, MENU_ITEM_CMDLOGGER},
		{// KK_DIRECT | KK_ALT | VK_F1			
			text("|About"), 
			text("OllyDBG Extension, Copyright By Gerron Jo."),  
			K_ABOUT, CDllMain::MenuProc, nullptr, MENU_ITEM_ABOUTME},
		{ nullptr, nullptr, K_NONE, nullptr, nullptr, 0 }
	};
	this->m_pMenu = g_tMainMenu + 0;
	
	static t_control g_tMainOpts[] = {
		{ CA_COMMENT, -1, 0, 0, 0, 0, nullptr, BIGNAME, nullptr },
		{ CA_TITLE, OPT_TITLE, 80, 4, 160, 15, nullptr, BIGNAME, nullptr },

		{ CA_CHECK, OPT_1, 80, 26, 155, 10, nullptr, 
			text("Show The Command Bar"), 
			text("Show The Command Bar After OllyDBG Ran"), },
			
		/*{ CA_TEXT, OPT_2, 80, 40, 155, 10, nullptr, 
			text("The python directory"), 
			text("Set the python directory"), }, 
		{ CA_FILE, OPT_3, 80, 50, 150, 10, reinterpret_cast<int*>(this->m_PyPath), 
			text("./python"), 
			text("Set the python directory"), }, 
		{ CA_BRDIR, OPT_4, 235, 50, 10, 10, reinterpret_cast<int*>(this->m_PyPath), 
			text("The python directory"), 
			text("Select the python directory"), }, 
		{ CA_CHECK, OPT_2, 105, 38, 140, 10, &skipgdi32, 
			text("Don't protocol GDI32"), 
			text("Ignore all APIs that reside in GDI32") },*/

		{ CA_END, -1, 0, 0, 0, 0, nullptr, nullptr, nullptr }
	};
	this->m_pCtrl = g_tMainOpts + 0;
}
CDllMain::~CDllMain()
{
}


int CDllMain::PluginInit(void)
{
	int intValue = 0; std::wstring strValue;
	
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter initialization function 'CDllMain::PluginInit'")
	);

	
	// Init control library ..
	// It's an unnecesuary operation for WinXP .
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("'CDllMain::PluginInit' --> Initialize control library")
	);
	INITCOMMONCONTROLSEX stInitCtl = {0};
	stInitCtl.dwSize = sizeof(stInitCtl); 
	stInitCtl.dwICC  = ICC_USEREX_CLASSES;
	if (InitCommonControlsEx(&stInitCtl) == FALSE) {
		MessageBox (
			hwollymain, 
			TEXT("Fail to load the common controls library !\n"),
			TEXT("OllyDog"),
			MB_OK | MB_ICONWARNING
		);
	}
	
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("'CDllMain::PluginInit' --> Replace window procedure")
	);
	// Replace the procedure of OD window .
	m_hOllyWndProc = (WNDPROC)SetWindowLongPtr(
		hwollymain, GWL_WNDPROC, (LONG_PTR)(WNDPROC)CDllMain::OnOllyWndProc
	);
	// Replace the procedure of mdi window .
	m_hMdiWndProc = (WNDPROC)SetWindowLongPtr(
		hwclient, GWL_WNDPROC, (LONG_PTR)(WNDPROC)CDllMain::OnMdiWndProc
	);

	// Now create the command bar.
	Addtolist(					// ................. 
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("'CDllMain::PluginInit' --> Create the command bar")
	); 
	if (m_cCmdBar.Create(hollyinst, hwollymain, hwclient) == false) {
		Addtolist (			// ................. 
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Fail to create the command bar!") 
		); 
		MessageBox ( 
			hwollymain, 
			TEXT("Fail to create the command bar !\n"), 
			TEXT("OllyDog"), 
			MB_OK | MB_ICONWARNING 
		);
	}
	
	// Check if we need to display the command bar .
	if (GetOption(TEXT("Show CmdBar"), intValue)) {
		m_cCmdBar.Hide(); if (intValue > 0) m_cCmdBar.Show();
	}
	
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave initialization function 'CDllMain::PluginInit'")
	);
	
	return 0;		// Report success.
}


int CDllMain::PluginClose(void)
{
	SetWindowLongPtr(	// Replace the procedure of OD window .
		hwollymain, GWL_WNDPROC, (LONG_PTR)this->m_hOllyWndProc
	);
	SetWindowLongPtr(	// Replace the procedure of mdi window .
		hwclient, GWL_WNDPROC, (LONG_PTR)this->m_hMdiWndProc
	); return 0;
}


void CDllMain::PluginReset(void)
{
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter Reset function 'CDllMain::PluginReset'")
	);
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave Reset function 'CDllMain::PluginReset'")
	);
}


void CDllMain::PluginDestroy(void)
{
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter Destroy Function 'CDllMain::PluginDestroy'")
	);
	// Destory command bar .
	m_cCmdBar.Destroy();
	Addtolist(					// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave Destroy Function 'CDllMain::PluginDestroy'")
	);
}


int CDllMain::PluginException(
	t_run* trun, const t_disasm* tda, t_thread* thrd, t_reg* treg, wchar_t* msg
){
	/*DEBUG_EVENT *debugevent = &trun->de;
	Addtolist(
		1,1,L"Debug Code:%x, First Change:%x, ExceptionCode:%x, ExceptionFlags:%x, ExceptionAddress:%x, Debug Register 6:%x, ContextFlags:%x", 
		debugevent->dwDebugEventCode, debugevent->u.Exception.dwFirstChance,
		debugevent->u.Exception.ExceptionRecord.ExceptionCode,
		debugevent->u.Exception.ExceptionRecord.ExceptionFlags,
		debugevent->u.Exception.ExceptionRecord.ExceptionAddress,
		thrd->context.Dr6, thrd->context.ContextFlags
	);*/
	return DispatchException(trun, tda, thrd);
}
void CDllMain::PluginNotify(int code, void* data, ulong parm1, ulong parm2)
{
	DispatchNotify(code, data, parm1, parm2);
}

t_menu* CDllMain::PluginMenu(wchar_t* type)
{
	if(::wcscmp(type, PWM_MAIN) != 0) return nullptr;
	else return this->m_pMenu;
}
t_control* CDllMain::PluginOptions(UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_CLOSE && wp != 0) {
	};
	// It makes no harm to return page descriptor on all messages.
	return this->m_pCtrl;
}

// Here is the menu handler .
int CDllMain::MenuProc(t_table* pTable, wchar_t* pName, ulong index, int nMode)
{
	if(nMode == MENU_VERIFY) return MENU_NORMAL;
	else if(nMode == MENU_EXECUTE) {
		switch(index)
		{
			case MENU_ITEM_CMDLINE: CApp->m_cCmdBar.toggle(); break;
			case MENU_ITEM_OPTIONS: Pluginshowoptions(CApp->m_pCtrl); break;
			case MENU_ITEM_ABOUTME:
				MessageBox (
					hwollymain,
					TEXT("Production: \n")
					TEXT("OllyDOG Debugger, Compiled with MSVC 2010\n\n")
					TEXT("Bug report or Suggestion: \n")
					TEXT("Follow me: https://twitter.com/zzydog\n")
					TEXT("Email me: GerronJo@gmail.com\n\n")
					TEXT("OllyDOG Debugger, Copyright (C) Gerron Jo."),
					TEXT("About OllyDog"),
					MB_OK | MB_ICONINFORMATION
				);
				break;
			case MENU_ITEM_CMDLOGGER:
				CCmdLog::create();
				break; 
			default: break;
		}
		return MENU_NOREDRAW;
    } else return MENU_ABSENT;
}


// Mdi Client Window Procedure
LRESULT CALLBACK CDllMain::OnMdiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_MOUSEWHEEL: break;
		default: break;
	}
	return CallWindowProc(CApp->m_hMdiWndProc, hWnd, uMsg, wParam, lParam);
}


// OllyDBG Window Procedure
LRESULT CALLBACK CDllMain::OnOllyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_MOUSEWHEEL: break;
		case WM_SIZE: 
			//HWND hTar = HWND(m_cCmdBar);
			//if (hTar) SendMessage(hTar, WM_SIZE, wParam, lParam);
			break;
		default: break;
	}
	return CallWindowProc(CApp->m_hOllyWndProc, hWnd, uMsg, wParam, lParam);
}


// GetMessage Hook Procedure .
LRESULT CALLBACK CDllMain::OnMouseLLProc(int  code, WPARAM wParam, LPARAM lParam)
{
	LPMSLLHOOKSTRUCT lpMouse = (LPMSLLHOOKSTRUCT)lParam;
	HWND hWnd = WindowFromPoint(lpMouse->pt);
	if (code < 0) return CallNextHookEx(NULL, code, wParam, lParam);
	// For now, we just do this for the command bar .
	if (hWnd == (HWND)CApp->m_cCmdBar || IsChild((HWND)CApp->m_cCmdBar, hWnd)) {
		if (wParam == WM_MOUSEWHEEL) {
			MessageBox(hwollymain,NULL,NULL,MB_OK);
			// Let other filters handle it first..
			//CallNextHookEx(NULL, code, wParam, lParam);
			// Build the parameters ..
			wParam = lpMouse->mouseData; 
			lParam = MAKELPARAM(lpMouse->pt.x, lpMouse->pt.y);
			// Notify our command bar . Must use SendMessage .
			SendMessage((HWND)CApp->m_cCmdBar, /*WM_MOUSEHWHEEL*/0x020e, wParam, lParam);
			// Don't dispatch this message, because we don't
			// want OD to translate this message to WM_VSCROLL
			return 0;
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}


// Entry point of the plugin DLL. Many system calls require DLL instance
// which is passed to DllEntryPoint() as one of parameters. Remember it. Do
// not make any initializations here, preferrable way is to place them into
// ODBG_Plugininit() and cleanup in ODBG_Plugindestroy().
BOOL WINAPI DllEntryPoint(HINSTANCE hi, DWORD reason, LPVOID reserved) {
	if (reason==DLL_PROCESS_ATTACH) 
	{ DisableThreadLibraryCalls(hi); CApp->m_hInst = hi; }
	return TRUE;	// Report success
};

