
#pragma once

#include <vector>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <time.h>
#include <stdio.h>
#include <windows.h>

#include <plugin.h>

#include "CoreApi.hpp"

#define LOGNAME TEXT("Command Bar Logger")
#define MENU_ITEM_GROUP	0x00000
#define MENU_ITEM_EMPTY	0x00000
#define MENU_ITEM_CLEAR	0x00001
#define MENU_ITEM_STORE	0x00002
#define MENU_ITEM_CPYLN	0x00003
#define MENU_ITEM_CPYLG	0x00004
#define MENU_ITEM_CPYTB	0x00005

#define LOGGER_EXPAND		0x00000000
#define LOGGER_SINGLE		0x00000001

#define LOGGER_FLAG_NONE	0x00000000
#define LOGGER_FLAG_BUSY	0x00000001
#define LOGGER_FLAG_IDLE	0x00000002


class CCmdLog : public CCoreApi
{
public:
	// Pipe message protocol, just for internal usage ..
	typedef struct _PIPEMSG{ 
		long long start; ulong id; HANDLE event; long long end;
	} PIPEMSG, *PPIPEMSG;
	// The logger sorted data, just for internal usage ..
	typedef time_t  ttime;
	typedef struct _LOGGER {		// Header of sorted data item
		ulong			addr;		// Base address of the entry
		ulong			size;		// Size of the entry
		ulong			type;		// Command type .
		ulong			rows;		// The row count.
		ulong			flag;		// The flag ....
		ttime			time;		// Logging time .
		long			code;		// The error code .
		pvoid			msge;		// The reply message .
	} LOGGER, *PLOGGER, *LPLOGGER ;
private:
	#define CMDLOG_INVALID		0x00000000
	#define CMDLOG_WORKING		0x00000001
	static ulong					m_nRows;	// Unused yet ..
	static t_menu*				m_pMenu;	// The logger Menu .
	static t_table				m_Table;	// The logger window .
	static size_t					m_nReffer;	// The refference counter.
	static ulong					m_uStatus;	// ..
	static ulong					m_nNextId;	// 
protected:
	static HANDLE					m_hPipeCb;	// The handle of Pipe thread .
	static DWORD					m_dPipeCb;	// The identify of Pipe thread .
	static HANDLE					m_hPipeRd; // The handle to read from pipe .
	static HANDLE					m_hPipeWr;	// The handle to write to pipe ....
	static HANDLE					m_hNotify;	// The notification event ..
	static CRITICAL_SECTION		m_csShare;	// The critical section object .


public:
	// Logger status .
	virtual ulong setidle(ulong);
	virtual ulong setbusy(ulong);
	// Wnd logging ...
	virtual ulong set(ulong, int, int, ulong);
	virtual ulong log(ulong, int, int, const wchar*);
	virtual ulong log(ulong, int, int, const std::wstring&);
	virtual ulong msg(ulong, const wchar*, bool append = true);
	virtual ulong msg(ulong, const std::wstring&, bool append = true);
	// Pipe logging ..
	virtual ulong connect(ulong);
	virtual pvoid getpipe();
	virtual ulong disconn(ulong);
	virtual ulong sendmsg(pvoid, ulong);

	// Erase logger
	virtual ulong erase(ulong);
	// Store logger 
	virtual ulong store(void);
	// Clear logger ..
	virtual ulong clear(void);
	
	static bool create();
	static void destroy();

private:
	// Output capturing thread function ..
	static  DWORD WINAPI PipeProc(LPVOID lpParam);
	// The nemu item message produce ..
	static  int  MenuProc(t_table*, wchar*, ulong, int);
	// The destructor function that frees resources allocated in the
	// descriptor of the logger window
	static  void DestProc(t_sorthdr*);
	// Function that compares two logger according to the specified criterium.
	static  int  SortProc(const t_sorthdr*, const t_sorthdr*, const int);
	// Prepares contents of table cells displayed on the screen
	static  int  DrawProc(
		wchar*, uchar*, int*, struct t_table*, t_sorthdr*, int, void*
	);
	// Pointer to the callback of type TABFUNC that processes messages and 
	// notifications sent to the table, or NULL if default processing is sufficient
	static  long TableProc(struct t_table*, HWND, UINT, WPARAM, LPARAM);
	// Pointer to the optional callback of type UPDATEFUNC that actualizes 
	// contents of the whole table. this function may be called in the following cases
	//   - when window is created;
	//   - in WM_PAINT when version of the associated sorted data is changed;
	//   - when debugged application is paused by the user or due to the debugging 
	//     request, but not when debugging event is continued immediately
	static  int  UpdateProc(struct t_table *);
	// Pointer to the optional callback of type TABSELFUNC that is called each time 
	// the selection in the table window is changed
	static  void TabSelProc(struct t_table *, int, int);
	
protected:
	CCmdLog(); ~CCmdLog(); 
	virtual bool Create(); virtual void Destroy();
	virtual bool InitLog(); virtual void FreeLog();
	// Copy logger to clipboard ..
	virtual HGLOBAL CopyRow(int row, int count);
	virtual HGLOBAL CopyLog(int num);
	virtual HGLOBAL CopyTab(void);
	// Pipe management ..
	virtual bool initcon(); virtual void freecon();
};

