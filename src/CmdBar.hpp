
#pragma once


#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <Windows.h>
#include <CommCtrl.h>
#include <Windowsx.h>

#include "CmdExe.hpp"

#define ULTBAR_HIGHT	24
#define STABAR_HIGHT	20
#define CMDBAR_HIGHT	24
#define MYFONT_HIGHT	13

#define CTRLITEM_HWBOX	1001	// Identifier of hwbox
#define CTRLITEM_HWMSG	1002	// Identifier of hwerr
#define CTRLITEM_HWSTC	1003	// Identifier of hwstc

class CCmdBar : public CCmdExe
{
public:
	CCmdBar(HINSTANCE hInst=NULL, HWND hParent=NULL, HWND hClient=NULL);
	// Operators .....................
	operator HWND(); operator HANDLE();

	virtual bool Create(HINSTANCE hInst, HWND hParent, HWND hClient);
	virtual void Destroy();

	void Show(); void Hide(); void toggle();

	void ShowList(bool bShow=true, int iItem=-1);
	void SetCurCmd(const std::wstring &cmd, bool bUpdate);
	void ResetCmdBar(bool bList, bool bEdit, bool bUpdate);
	void ResetContent(bool bCmd, bool bHistroies);

	virtual void AddCmdRecord(const std::wstring &cmd);
	virtual void ShowCmdUsage(const std::wstring &cmd);
	virtual void ShowSimilarCmds(const std::wstring &cmd);

protected:
	// The main window message dispatcher ..
	static  LRESULT CALLBACK OnMainWndProc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT CALLBACK OnMainWndProc(HWND, UINT, LPARAM, WPARAM);
	// The cbox windows message dispatcher ..
	static  LRESULT CALLBACK OnCboxWndProc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT CALLBACK OnCboxWndProc(HWND, UINT, LPARAM, WPARAM);
	// The edit window message dispatcher ..
	static  LRESULT CALLBACK OnEditWndProc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT CALLBACK OnEditWndProc(HWND, UINT, LPARAM, WPARAM);
	// The list window message dispatcher ..
	static  LRESULT CALLBACK OnListWndProc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT CALLBACK OnListWndProc(HWND, UINT, LPARAM, WPARAM);

	// Common window message handlers ..
	virtual LRESULT CALLBACK OnCreate(HWND, LPCREATESTRUCT, BOOL&);
	virtual LRESULT CALLBACK OnSetFocus(HWND, HWND, BOOL&);
	virtual LRESULT CALLBACK OnDestroy(HWND, BOOL&);
	virtual LRESULT CALLBACK OnPaint(HWND, BOOL&);
	virtual LRESULT CALLBACK OnSize(HWND, WPARAM, WORD, WORD, BOOL&);
	virtual LRESULT CALLBACK OnCommand(HWND, WPARAM, LPARAM, BOOL&);
	virtual LRESULT CALLBACK OnCbnEditChange(HWND, UINT, BOOL&);
	virtual LRESULT CALLBACK OnCtlColorListbox(HWND, HANDLE, BOOL&);
	// Custom window message handlers ..
	virtual LRESULT CALLBACK OnCmdExecution();
	virtual LRESULT CALLBACK OnCmdVtChanged(int);
	virtual LRESULT CALLBACK OnCmdScrolling(UINT);
	virtual LRESULT CALLBACK OnCmdBeChanged(LPCTSTR);
	virtual LRESULT CALLBACK OnCmdBeChanged(std::wstring);

	virtual void AdjustCmdbar(void) const;
	virtual void AdjustListbox(void) const;
	virtual bool IsToolbarShown(void) const;
	
private:

public:
	void*							m_pThis;	// Verify itself ..
private:
protected:
	HWND							m_hwCmd;	// Command line window
	HWND							m_hwBox;	// Combobox with edit and history list
	HWND							m_hwLst;	// List control inside the combobox
	HWND							m_hwEdt;	// Edit control inside the combobox
	HWND							m_hwStc;	// Static control
	HWND							m_hwMan;	// Handle of main OllyDbg window
	HWND							m_hwMdi;
	HFONT							m_hFont;
	HINSTANCE						m_hInst;
	
	BOOL							m_bShowBar;
	BOOL							m_bCmdNotify;
	UINT							m_iLastVtSel;
	UINT							m_iScrollCount;
	WNDPROC						m_hEditWndProc;
	LONG_PTR						m_lpEditUserData;
	WNDPROC						m_hCboxWndProc;
	LONG_PTR						m_lpCboxUserData;
	WNDPROC						m_hListWndProc;
	LONG_PTR						m_lpListUserData;
	
	std::wstring					m_strCommand;
	std::vector<std::wstring>	m_vetHistories;
};

