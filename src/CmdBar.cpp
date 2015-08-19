
#include "CmdBar.hpp"


CCmdBar::CCmdBar(HINSTANCE hInst, HWND hParent, HWND hClient)
{
	this->m_pThis = this;
	this->m_bShowBar = TRUE;		// Show the command bar.
	this->m_bCmdNotify = TRUE;
	this->m_iLastVtSel = -1;
	this->m_iScrollCount = 0;
	this->m_hEditWndProc = NULL;	// Set to NULL.
	
	this->m_hwCmd = NULL;
	this->m_hFont = NULL; 
	this->m_hInst = NULL;
	this->m_hwMan = this->m_hwMdi = NULL;
	this->m_hwLst = this->m_hwBox = NULL;

	if (hInst)		this->m_hInst = hInst;
	if (hParent)	this->m_hwMan = hParent;
	if (hClient)	this->m_hwMdi = hClient;
}
// Operators .....................
CCmdBar::operator HWND() { return this->m_hwCmd; }
CCmdBar::operator HANDLE() { return this->m_hwCmd; }


bool CCmdBar::Create(HINSTANCE hInst, HWND hParent, HWND hClient)
{
	RECT rect = {0}; WNDCLASS wClass = {0}; LOGFONT lFont = {0}; 
	/*TCHAR buffer[MAX_PATH];*/ LONG_PTR UserData;

	// Save to class members.
	if (hInst)		this->m_hInst = hInst;
	if (hParent)	this->m_hwMan = hParent;
	if (hClient)	this->m_hwMdi = hClient;

	// Initialize the command environment .
	//if (CmdInit() == false) return false;
	ExeInit();

	// Save this pointer to the user data of parent window.
	UserData = SetWindowLongPtr(hParent, GWLP_USERDATA, (LONG_PTR)this);

	// Here initialize the window class.
	wClass.hIcon = LoadIconW(hInst, TEXT("ICO_PLUGIN"));
	wClass.hCursor = NULL;
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = CCmdBar::OnMainWndProc;
	wClass.lpszClassName = TEXT("CmdBar");
	wClass.hbrBackground = (HBRUSH)(16);

	if(RegisterClass(&wClass) == 0) return false;

	// Use our specified font.
	lFont.lfHeight = MYFONT_HIGHT; 
	lFont.lfWidth = 0;
	lFont.lfEscapement = lFont.lfOrientation = 0;
	lFont.lfWeight = FW_NORMAL;
	lFont.lfItalic = false;
	lFont.lfUnderline = false;
	lFont.lfStrikeOut = false;
	lFont.lfCharSet = DEFAULT_CHARSET;
	lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.lfQuality = CLEARTYPE_QUALITY; 
	lFont.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	wcscpy_s(lFont.lfFaceName, _countof(lFont.lfFaceName), TEXT("FangSong"));
	this->m_hFont = CreateFontIndirect(&lFont);

	// Now Create the cmd bar window .
	GetWindowRect(hParent, &rect);
	this->m_hwCmd = CreateWindowEx(
		WS_EX_LTRREADING|WS_EX_CONTROLPARENT,
		TEXT("CmdBar"),
		TEXT("Command Bar"),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		0, rect.bottom-STABAR_HIGHT-CMDBAR_HIGHT, rect.right, CMDBAR_HIGHT,
		hParent, NULL, hInst, NULL
	);
	// Unregister the class and delete the font 
	// if we fail to create the window .
	if (this->m_hwCmd == NULL) {
		UnregisterClass(TEXT("CmdBar"),hInst); DeleteObject(this->m_hFont);
	} else SetWindowFont(this->m_hwCmd, this->m_hFont, FALSE);

	// Now, recover the user data.
	UserData = SetWindowLongPtr(hParent, GWLP_USERDATA, (LONG_PTR)UserData);

	// Finally ...
	return this->m_hwCmd != NULL;
}
void CCmdBar::Destroy()
{
	// Recover all window procedure we replaced .
	if (this->m_hwEdt) {
		SetWindowLongPtr(
			this->m_hwEdt, GWL_WNDPROC, (LONG_PTR)this->m_hEditWndProc);
		SetWindowLongPtr(
			this->m_hwEdt, GWLP_USERDATA, this->m_lpEditUserData);
		this->m_hwEdt = NULL;
	}

	// Destory all windows we created .
	if (this->m_hwStc) DestroyWindow(this->m_hwStc); this->m_hwStc = NULL;
	if (this->m_hwBox) DestroyWindow(this->m_hwBox); this->m_hwBox = NULL;
	if (this->m_hwCmd) DestroyWindow(this->m_hwCmd); this->m_hwCmd = NULL;
	
	// Unregister the class and delete the font 
	UnregisterClass(TEXT("CmdBar"), this->m_hInst); 
	if (this->m_hFont) DeleteObject(this->m_hFont);  this->m_hFont = NULL;
	
	// Properly ends the command environment
	ExeFree();
}


void CCmdBar::Show()
{
	ShowWindow(this->m_hwCmd, SW_SHOW);
	this->m_bShowBar = true;
	AdjustCmdbar();
}
void CCmdBar::Hide()
{
	ShowWindow(this->m_hwCmd, SW_HIDE);
	this->m_bShowBar = false;
	AdjustCmdbar();
}
void CCmdBar::toggle()
{
	if (this->m_bShowBar) {
		ShowWindow(this->m_hwCmd, SW_HIDE); this->m_bShowBar = false;
	} else {
		ShowWindow(this->m_hwCmd, SW_SHOW); this->m_bShowBar = true;
	}
	AdjustCmdbar();
}

void CCmdBar::ShowList(bool bShow, int iCount)
{
	bool bState = ComboBox_GetDroppedState(this->m_hwBox) == TRUE;
	if (bState != bShow) {
		// Change the dropdown list state .
		ComboBox_ShowDropdown(this->m_hwBox, bShow);
	}
	if (bShow == true && iCount != 0) {
		// Fix the location of the dropdown list .
		RECT rect = {0}; int x=0, y=0, w=0, h=0, c=0;
		GetWindowRect(this->m_hwBox, &rect);
		c = ComboBox_GetCount(this->m_hwBox);
		c = iCount <= -1 ? c : iCount;
		h = ComboBox_GetItemHeight(this->m_hwBox) * (c+1);
		w = rect.right - rect.left - 1;
		y = rect.top - h;
		x = rect.left;
		MoveWindow(this->m_hwLst, x, y, w, h, true);
	}
}
void CCmdBar::SetCurCmd(const std::wstring& cmd, bool bUpdate)
{
	this->m_bCmdNotify = bUpdate == true;
	SetWindowText(this->m_hwBox, cmd.c_str());
	this->m_bCmdNotify = TRUE;
}
void CCmdBar::ResetCmdBar(bool bList, bool bEdit, bool bUpdate)
{
	// Close command notification ..
	this->m_bCmdNotify = FALSE;
	
	if (bList && bEdit) {	// Reset both listbox and edit control..
		ComboBox_ResetContent(this->m_hwBox);
	} else if (bList) {	// Reset listbox control .
		DWORD dwRange = ComboBox_GetEditSel(this->m_hwBox);
		ComboBox_ResetContent(this->m_hwBox);
		SetWindowText(this->m_hwBox, this->m_strCommand.c_str());
		ComboBox_SetEditSel(	// Recover the selection ..
			this->m_hwBox, LOWORD(dwRange), HIWORD(dwRange)
		);
	} else if (bEdit) {	// Reset edit control .
		SetWindowText(this->m_hwBox, TEXT(""));
	} else bUpdate = false;
	
	if (bUpdate) {		// Now, Handle updating ..
		if (bList) {		// Recover histroy list ..
			std::vector<std::wstring>::const_iterator itr, eitr;
			eitr = this->m_vetHistories.end();
			for(itr = this->m_vetHistories.begin(); itr != eitr; ++itr)
				ComboBox_AddString(this->m_hwBox, itr->c_str());
		}
		if (bEdit) this->m_strCommand.clear();
	}
	
	// Open command notification ..
	this->m_bCmdNotify = TRUE;
}
void CCmdBar::ResetContent(bool bCmd, bool bHistroies)
{
	if (bCmd) this->m_strCommand.clear();
	if (bHistroies) this->m_strCommand.clear();
}



LRESULT CALLBACK CCmdBar::OnMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCmdBar * pthis = (CCmdBar *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (pthis == nullptr && uMsg == WM_CREATE) {
		pthis = (CCmdBar *)GetWindowLongPtr(GetParent(hWnd), GWLP_USERDATA);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pthis);
	}
	if (IsBadReadPtr(pthis,sizeof(*pthis)) || pthis->m_pThis != pthis)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	return pthis->OnMainWndProc(hWnd, uMsg, lParam, wParam);
}
LRESULT CALLBACK CCmdBar::OnMainWndProc(HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam)
{
	LRESULT lResult = 0; BOOL bResult = TRUE;

	switch(uMsg) {		// Here is the message dispatcher ..
	case WM_CREATE: 
		lResult = OnCreate(hWnd, (LPCREATESTRUCT)lParam, bResult);
		if (bResult) { break; } else return lResult;

	case WM_SETFOCUS:
		lResult = OnSetFocus(hWnd, (HWND)wParam, bResult);
		if (bResult) { break; } else return lResult;

	case WM_DESTROY:
		lResult = OnDestroy(hWnd, bResult);
		if (bResult) { break; } else return lResult;

	case WM_PAINT:
		lResult = OnPaint(hWnd, bResult);
		if (bResult) { break; } else return lResult;

	case WM_SIZE:
		lResult = OnSize(hWnd, wParam, LOWORD(lParam), HIWORD(lParam), bResult);
		if (bResult) { break; } else return lResult;

	case WM_COMMAND:
		// WM_COMMAND Handelr .
		lResult = OnCommand(hWnd, wParam, lParam, bResult);
		if (bResult == FALSE) return lResult;
		// CBN_DROPDOWN Handler ..
		if (LOWORD(wParam)==CTRLITEM_HWBOX && HIWORD(wParam)==CBN_DROPDOWN) {
			// This is a very import operation, if we don't do this, 
			// The mouse cursor will disappear while typing text in combobox. 
			// https://social.msdn.microsoft.com/Forums/windows/en-US/
			// 8ac44f99-a04f-4703-830c-27a7f298ded7/combobox-mouse-disappears
			SendMessage(this->m_hwBox, WM_SETCURSOR, 0, 0);
			break;
		}
		// CBN_SELCHANGE Handler ..
		if (LOWORD(wParam)==CTRLITEM_HWBOX && HIWORD(wParam)==CBN_SELCHANGE) {
			//MessageBox(hwollymain,NULL,NULL,MB_OK);
			//lResult = OnCbnEditChange((HWND)lParam, LOWORD(wParam), bResult);
			//if (bResult == FALSE) return lResult;
			this->m_iLastVtSel = ComboBox_GetCurSel(this->m_hwBox);
		}
		// CBN_EDITCHANGE Handler ..
		if (LOWORD(wParam)==CTRLITEM_HWBOX && HIWORD(wParam)==CBN_EDITCHANGE) {
			// This is a very import operation, if we don't do this, 
			// The mouse cursor will disappear while typing text in combobox. 
			// https://social.msdn.microsoft.com/Forums/windows/en-US/
			// 8ac44f99-a04f-4703-830c-27a7f298ded7/combobox-mouse-disappears
			//SendMessage(this->m_hwBox, WM_SETCURSOR, 0, 0);
			lResult = OnCbnEditChange((HWND)lParam, LOWORD(wParam), bResult);
			if (bResult == FALSE) return lResult;
		}
		// Let default handler handle it .
		break;

	case WM_CTLCOLORLISTBOX:
		lResult = OnCtlColorListbox((HWND)lParam, (HANDLE)wParam, bResult);
		if (bResult) { break; } else return lResult;

	case WM_VSCROLL: return OnCmdScrolling(LOWORD(wParam));

	case WM_USER_CHGALL: return Update();

	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// The default handler ..
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CCmdBar::OnCboxWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCmdBar * pthis = (CCmdBar *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (IsBadReadPtr(pthis,sizeof(*pthis)) || pthis->m_pThis != pthis) 
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	return pthis->OnCboxWndProc(hWnd, uMsg, lParam, wParam);
}
LRESULT CALLBACK CCmdBar::OnCboxWndProc(HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam)
{
	switch(uMsg) {
	case WM_COMMAND: break;
	//case WM_SETTEXT: return OnCmdBeChanged((LPCTSTR)lParam);
	case WM_VSCROLL: return OnCmdScrolling(LOWORD(wParam));
	default: break;
	}
	return CallWindowProc(this->m_hCboxWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CCmdBar::OnEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCmdBar * pthis = (CCmdBar *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (IsBadReadPtr(pthis,sizeof(*pthis)) || pthis->m_pThis != pthis) 
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	return pthis->OnEditWndProc(hWnd, uMsg, lParam, wParam);
}
LRESULT CALLBACK CCmdBar::OnEditWndProc(HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam)
{
	static bool ctrlkey = false; 
	
	switch(uMsg) {

	case WM_KEYDOWN:
		switch(wParam) {
			case VK_CONTROL: ctrlkey = true; break;
			case VK_RETURN:  return OnCmdExecution();
			case VK_ESCAPE:  ResetCmdBar(true,true,true); return 0;
			default:         break;
		}
		break;

	case WM_CHAR:
		switch(wParam) {
			case VK_CONTROL: break;
			case VK_ESCAPE:  return 0;
			case VK_RETURN:  return 0;
			default:         break;
		} 
		break;

	case WM_KEYUP:
		switch(wParam) {
			case VK_CONTROL: ctrlkey = false; break;
			case VK_RETURN:  return 0;
			case VK_ESCAPE:  return 0;
			default:         break;
		}
		break;
		
	case WM_SETTEXT: OnCmdBeChanged((LPCTSTR)lParam); break;
	case WM_VSCROLL: return OnCmdScrolling(LOWORD(wParam));

	default: break;
	}
	
	return CallWindowProc(this->m_hEditWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CCmdBar::OnListWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCmdBar * pthis = (CCmdBar *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (IsBadReadPtr(pthis,sizeof(*pthis)) || pthis->m_pThis != pthis) 
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	return pthis->OnListWndProc(hWnd, uMsg, lParam, wParam);
}
LRESULT CALLBACK CCmdBar::OnListWndProc(HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam)
{
	switch(uMsg) {		// Here is the message dispatcher ..
	case WM_SHOWWINDOW:
		if (wParam == TRUE) {	// Display the control .
			LRESULT lResult = CallWindowProc(
				this->m_hListWndProc, hWnd, uMsg, wParam, lParam
			); 
			AdjustListbox(); return lResult;
		} else break;
	default: break;
	}
	return CallWindowProc(this->m_hListWndProc, hWnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK CCmdBar::OnCreate(HWND hWnd, LPCREATESTRUCT lParam, BOOL &bResult)
{
	RECT rect; HDC hDc = NULL; DWORD dwRc = 0;
	
	// Save command bar handle .
	// We just want to make sure that 
	// the value of this->m_hwCmd is always valid .
	this->m_hwCmd = hWnd;
	
	// Create the static control, 
	GetClientRect(hWnd, &rect);
	this->m_hwStc = CreateWindowEx(
		0, 
		WC_STATIC, TEXT("Command>"),
		WS_CHILD|WS_VISIBLE|SS_SIMPLE|SS_GRAYFRAME|SS_CENTER,
		5, 5, 60, CMDBAR_HIGHT - 10,
		hWnd, (HMENU)CTRLITEM_HWSTC, this->m_hInst, NULL
	);
	if(this->m_hFont) {	// Here set font, not necessary.
		SetWindowFont(this->m_hwStc, this->m_hFont, TRUE);
	}

	// Create edit control with history list, assign default OllyDbg font and
	// limit length of entered commands to OllyDbg-wide default.
	// For WinXP support, I don't use ComboBoxEx .
	this->m_hwBox = CreateWindowEx(
		0,
		WC_COMBOBOX, TEXT(""),
		WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL|
		CBS_HASSTRINGS|CBS_NOINTEGRALHEIGHT|CBS_DISABLENOSCROLL|CBS_SORT,
		65, 2, rect.right-70, CMDBAR_HIGHT - 4,
		hWnd, (HMENU)CTRLITEM_HWBOX, this->m_hInst, NULL
	);
	ComboBox_SetItemHeight(this->m_hwBox, -1, 50);
	ComboBox_SetExtendedUI(this->m_hwBox, TRUE);
	if(this->m_hFont) {
		SetWindowFont(this->m_hwBox, this->m_hFont, TRUE);
	}
	this->m_lpCboxUserData = (LONG_PTR)SetWindowLongPtr(
		this->m_hwBox, GWLP_USERDATA, (LONG_PTR)this
	);	// Save this pointer ..
	this->m_hCboxWndProc = (WNDPROC)SetWindowLongPtr(
		this->m_hwBox, GWL_WNDPROC, (LONG_PTR)(WNDPROC)CCmdBar::OnCboxWndProc
	);	// Replace the procedure of the windows ..

	COMBOBOXINFO cbbInfo = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(this->m_hwBox, &cbbInfo);
	this->m_hwEdt = cbbInfo.hwndItem; this->m_hwLst = cbbInfo.hwndList;
	
	// To intercept return key, subclass the edit control inside the combobox.
	this->m_lpEditUserData = (LONG_PTR)SetWindowLongPtr(
		this->m_hwEdt, GWLP_USERDATA, (LONG_PTR)this
	);	// Save this pointer ..
	this->m_hEditWndProc = (WNDPROC)SetWindowLongPtr(
		this->m_hwEdt, GWL_WNDPROC, (LONG_PTR)(WNDPROC)CCmdBar::OnEditWndProc
	);	// Replace the procedure of the windows ..
	// To custom the dropdownlist, subclass the list control inside the cbox.
	this->m_lpListUserData = (ULONG_PTR)SetWindowLongPtr(
		this->m_hwLst, GWLP_USERDATA, (LONG_PTR)this
	);	// Save this pointer ..
	this->m_hListWndProc = (WNDPROC)SetWindowLongPtr(
		this->m_hwLst, GWL_WNDPROC, (LONG_PTR)(WNDPROC)CCmdBar::OnListWndProc
	);	// Replace the procedure of the windows ..
	
	return 0;
}

LRESULT CALLBACK CCmdBar::OnSetFocus(HWND hWnd, HWND hLost, BOOL &bResult)
{
	SetFocus(this->m_hwBox); return 0;
}

LRESULT CALLBACK CCmdBar::OnDestroy(HWND hWnd, BOOL &bResult)
{
	this->m_hwBox = NULL; this->m_hwEdt = NULL; 
	this->m_hwStc = NULL; this->m_hwCmd = NULL; return 0;
}

LRESULT CALLBACK CCmdBar::OnPaint(HWND hWnd, BOOL &bResult)
{
	RECT rc; PAINTSTRUCT ps; HBRUSH br; HDC dc;
	AdjustCmdbar();
	// Fill background of command line window with default button colour.
	// Necessary because Registerpluginclass() sets hbrBackground to NULL.
	dc=BeginPaint(hWnd, &ps); GetClientRect(hWnd, &rc);
	br=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	FillRect(dc,&rc,br); DeleteObject(br); EndPaint(hWnd,&ps);
	InvalidateRect(this->m_hwMan, NULL, TRUE);
	return 0;
}

LRESULT CALLBACK CCmdBar::OnSize(HWND hWnd, WPARAM wParam, WORD wWidth, WORD wHeight, BOOL &bResult)
{
	AdjustCmdbar();
	InvalidateRect(this->m_hwMan,NULL,TRUE);
	return 0;
}

LRESULT CALLBACK CCmdBar::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL &bResult)
{
	return 0 ;
}

LRESULT CALLBACK CCmdBar::OnCbnEditChange(HWND hWnd, UINT uId, BOOL& bResult)
{
	std::vector<wchar> buffer; size_t cmdlen = 0;
	if (cmdlen = GetWindowTextLength(hWnd)) {
		cmdlen += 10; buffer.reserve(cmdlen);
		GetWindowText(hWnd, buffer.data(), cmdlen);
		// Save current command line .
		this->m_strCommand = buffer.data();
		ShowCmdUsage(this->m_strCommand); 
		ShowSimilarCmds(this->m_strCommand); 
	} else { 
		ShowList(false); ResetCmdBar(true,true,true); 
	} return 0 ;
}

LRESULT CALLBACK CCmdBar::OnCtlColorListbox(HWND hListbox, HANDLE hDevice, BOOL& bResult)
{
	POINT point = {0}; GetCursorPos(&point);
	int index = LBItemFromPt(this->m_hwLst, point, FALSE);
	if (index >= 0) return OnCmdVtChanged(index);
	return 0;
}

LRESULT CALLBACK CCmdBar::OnCmdExecution()
{
	size_t cmdlen = 0; std::wstring err; int irslt;
	if ((cmdlen = this->m_strCommand.length()) > 0) {
		ResetCmdBar(true, false, false); ShowList(false);
		// Add line only if command valid
		if((irslt=Exec(this->m_strCommand,err)) == 0) 
			AddCmdRecord(this->m_strCommand);
		// Display result or error message
		if(err.length()) {
			std::wstringstream buffer;
			buffer << text("Error Code:  ") << irslt ;
			buffer << text("     ");
			buffer << text("Error Msge:  ") << err;
			Tempinfo(const_cast<wchar*>(buffer.str().c_str())); 
		}
		ComboBox_SetEditSel(this->m_hwBox, 0, -1); 
		SetFocus(this->m_hwBox);
	}
	return 0;
}

LRESULT CALLBACK CCmdBar::OnCmdVtChanged(int index)
{
	if (index == this->m_iLastVtSel) return 0;
	int len = ComboBox_GetLBTextLen(this->m_hwBox, index);
	if (len > 0) {
		std::vector<wchar> buffer; buffer.reserve(++len);
		ComboBox_GetLBText(this->m_hwBox, index, buffer.data());
		ShowCmdUsage(std::wstring(buffer.data()));
	} this->m_iLastVtSel = index; return 0;
}

LRESULT CALLBACK CCmdBar::OnCmdScrolling(UINT code)
{
	// OD will translate WM_MOUSEWHEEL to WM_VSCROLL, 
	// and one WM_MOUSEWHEEL message become 2 WM_VSCROLL
	// message, but this may be changed in the future version.
	this->m_iScrollCount = ++this->m_iScrollCount % 2;
	if (this->m_iScrollCount == 0) return 0;
	switch (code) {
	case SB_LINEUP:
		keybd_event(
			VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 
			KEYEVENTF_EXTENDEDKEY | 
			(this->m_iScrollCount>0 ? 0:KEYEVENTF_KEYUP), 0
		);
		break;
	case SB_LINEDOWN:
		keybd_event(
			VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 
			KEYEVENTF_EXTENDEDKEY | 
			(this->m_iScrollCount>0 ? 0:KEYEVENTF_KEYUP), 0
		);
		break;
			
	case SB_TOP: 
		ComboBox_SetCurSel(this->m_hwBox, 0); 
		break;
	case SB_BOTTOM:
		ComboBox_SetCurSel(this->m_hwBox, ComboBox_GetCount(this->m_hwBox)-1);
		break;
			
	case SB_PAGEUP:
		keybd_event(
			VK_PRIOR, MapVirtualKey(VK_PRIOR, MAPVK_VK_TO_VSC), 
			KEYEVENTF_EXTENDEDKEY | 
			(this->m_iScrollCount>0 ? 0:KEYEVENTF_KEYUP), 0
		);
		break;
	case SB_PAGEDOWN:
		keybd_event(
			VK_NEXT, MapVirtualKey(VK_NEXT, MAPVK_VK_TO_VSC), 
			KEYEVENTF_EXTENDEDKEY | 
			(this->m_iScrollCount>0 ? 0:KEYEVENTF_KEYUP), 0
		);
		break;
			
	default: break;
	}
	return 0;
}

LRESULT CALLBACK CCmdBar::OnCmdBeChanged(LPCTSTR cmd)
{
	if (this->m_bCmdNotify == FALSE) return 0;
	if (cmd==NULL || cmd[0]=='\0') cmd = TEXT("");
	this->m_strCommand = cmd;	// Save the command .
	ShowCmdUsage(this->m_strCommand); return 0;
}
LRESULT CALLBACK CCmdBar::OnCmdBeChanged(std::wstring cmd)
{
	if (this->m_bCmdNotify == FALSE) return 0;
	this->m_strCommand = cmd;
	ShowCmdUsage(this->m_strCommand); return 0;
}

void CCmdBar::AdjustCmdbar(void) const
{
	RECT rc; int x, y, w, h; bool bToolbar = IsToolbarShown();

	GetClientRect(this->m_hwMan,&rc);
	x = 0;
	y = rc.bottom - STABAR_HIGHT - CMDBAR_HIGHT;
	w = rc.right - rc.left;
	h = CMDBAR_HIGHT;
	MoveWindow(this->m_hwCmd, x, y, w, h, TRUE);

	GetClientRect(this->m_hwCmd, &rc);
	x = 65;
	y = 2;
	w = rc.right - 70;
	h = CMDBAR_HIGHT - 4;
	MoveWindow(this->m_hwBox, x, y, w, h, TRUE);
	
	GetClientRect(this->m_hwMan, &rc);
	x = 0;
	y = bToolbar == true ? ULTBAR_HIGHT : 0;
	w = rc.right - rc.left;
	h = rc.bottom - rc.top - STABAR_HIGHT - \
		(this->m_bShowBar ? CMDBAR_HIGHT : 0) - \
		(bToolbar== true ? ULTBAR_HIGHT : 0);
	MoveWindow(this->m_hwMdi, x, y, w, h, TRUE);
}

void CCmdBar::AdjustListbox(void) const
{
	RECT rect = {0}; int x=0, y=0, w=0, h=0, c=0;
	GetWindowRect(this->m_hwBox, &rect);
	c = ComboBox_GetCount(this->m_hwBox);
	h = ComboBox_GetItemHeight(this->m_hwBox) * (c+1);
	w = rect.right - rect.left - 1;
	y = rect.top - h;
	x = rect.left;
	MoveWindow(this->m_hwLst, x, y, w, h, true);
}

bool CCmdBar::IsToolbarShown(void) const
{
	RECT rcMan, rcMDI; POINT pt;
	GetClientRect(this->m_hwMan, &rcMan);
	GetWindowRect(this->m_hwMdi, &rcMDI);
	pt.x = rcMDI.left; pt.y = rcMDI.top;
	ScreenToClient(this->m_hwMan, &pt);
	if(rcMan.top == pt.y) return false;
	return true;
}

void CCmdBar::AddCmdRecord(const std::wstring &cmd)
{
	std::vector<std::wstring>::iterator itr, eitr;
	if (cmd.length() <= 0) return ;	// Empty command .
	
	// Find the existed record
	eitr = this->m_vetHistories.end();
	itr  = std::find(this->m_vetHistories.begin(), eitr, cmd);
	// Insert new record if it doesn't exist .
	if (itr == eitr || this->m_vetHistories.size() == 0) {
		this->m_vetHistories.push_back(cmd);
	}
	
	// Remove all menu items ..
	ListBox_ResetContent(this->m_hwLst);
	
	// Recover all record ..
	eitr = this->m_vetHistories.end();
	for(itr = this->m_vetHistories.begin(); itr != eitr; ++itr) {
		ComboBox_AddString(this->m_hwBox,itr->c_str());
	}
}

void CCmdBar::ShowCmdUsage(const std::wstring &cmd)
{
	std::wstring out; std::wstring msg; int iRslt = 0;
	// Parse the command(without options) first ..
	if ((iRslt = GetCmd(cmd, out)) == 0) 
		iRslt = GetCmdUsage(out, msg);
	// Now, show the detail information of the command .
	if (iRslt == 1) { Tempinfo(TEXT("")); return;
	} else if (iRslt > 0) {
		msg = TEXT("Can't not find any information about ");
		msg = msg + TEXT("'") + cmd + TEXT("'");
	} else if (iRslt < 0) {
		msg = TEXT("Internal error, Please try again.");
	} else assert(msg.c_str());
	// Everything's fine, display the message .
	Tempinfo(const_cast<wchar_t*>(msg.c_str()));
}

void CCmdBar::ShowSimilarCmds(const std::wstring& cmd)
{
	// Get related commands, here we use "< 0", which means
	// we accept external errors ("> 0" means external errors),
	// including empty input ..
	std::vector<LPCTSTR> cmds; if (RelatedCmds(cmd, cmds) < 0) return ;

	// Remove all menu items.. 
	ListBox_ResetContent(this->m_hwLst);

	// Add all new item, related commands or command histroy records .
	if (cmds.size() > 0) {		// We got related commands .
		// Show the dropdown menu.
		ShowList(true, cmds.size()+1);
		// Add all related cmds .
		std::vector<LPCTSTR>::const_iterator itr, eitr = cmds.end();
		for(itr = cmds.begin(); itr != eitr; ++itr) {
			ComboBox_AddString(this->m_hwBox, *itr);
		}
	} else {						// We got no related command .
		// Hide the dropdown menu.
		ComboBox_ShowDropdown(this->m_hwBox, FALSE);
		// Recover all histroy record ..
		std::vector<std::wstring>::const_iterator itr, eitr;
		eitr = this->m_vetHistories.end();
		for(itr = this->m_vetHistories.begin(); itr != eitr; ++itr) {
			ComboBox_AddString(this->m_hwBox, itr->c_str());
		}
	}
}


