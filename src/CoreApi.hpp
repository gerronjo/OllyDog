
#pragma once

#include <map>
#include <stdio.h>
#include <windows.h>

#include <plugin.h>

class CCoreApi
{
public:	
	static void* GetRetAddr();

	// The common OD functions ..
	virtual int Exit(bool sync = false);
	virtual int Open(bool sync = false);
	virtual int Close(bool sync = false);
	virtual int Start(bool sync = false);
	virtual int Pause(bool sync = false);
	virtual int Restart(bool sync = false);
	virtual int Startup(wchar*, wchar*, wchar*);

	// Logger ..
	virtual int ClearLog();

	// Debugged app controling ..
	virtual int StepOver(bool sync = false);
	virtual int StepInto(bool sync = false);
	virtual int TillUser(bool sync = false);
	virtual int TillRetn(bool sync = false);
	virtual int ExecHere(ulong addr, bool sync = false);
	// Debugged app tracing ..
	virtual int QuitTrace(bool sync = false);
	virtual int OpenTrace(bool sync = false);
	virtual int TraceInto(bool sync = false);
	virtual int TraceOver(bool sync = false);

	// Breakpoint ...
	virtual t_bpoint* GetSwBp(ulong addr);
	virtual t_bpmem* GetMmBp(ulong addr);
	virtual t_bphard* GetHwBp(ulong addr);
	virtual int GetHwBbSolt(ulong addr);

	// Hex dump ...
	virtual t_dump* GetCpuHexDump(); 

	// We don't use C++ 11/14, so std::bind is not valid ..
	typedef bool (*_Exception)(t_run*, const t_disasm*, t_thread*, int&, void*);
	typedef bool (*_Notify)(int, void*, ulong, ulong, void*);
	// Callback dispatcher ..
	virtual int DispatchException(t_run*, const t_disasm*, t_thread*);
	virtual int DispatchNotify(int, void*, ulong, ulong);
	// Callback manager ..
	virtual bool AddExceptionCallback(_Exception exp, void*);
	virtual bool DelExceptionCallback(_Exception exp);
	virtual bool AddNotifyCallback(_Notify ntf, void*);
	virtual bool DelNotifyCallback(_Notify ntf);

	// Menu produre hooking ..
	virtual MENUFUNC* SetTableMenuHook(t_table*, int, ulong, MENUFUNC*);

private:
	// Use to search undocument APIs ..
	void * FindApiAddrByCode(uchar*, size_t);
	// Sunday search function ..
	size_t Sunday(uchar*, size_t, uchar*, size_t);
	bool   DosPathNameToNtPathName(wchar*, ushort, const wchar*);

protected:
	CCoreApi(); ~CCoreApi();
	virtual bool InitCore(); virtual void FreeCore();

public:	
private:
	#define COREAPI_INVALID		0x00000000
	#define COREAPI_WORKING		0x00000001
	static size_t		CCoreApi::m_nReffer; 
	static ulong		CCoreApi::m_uStatus; 
	static HANDLE		CCoreApi::m_hExeMap; 
	static void *		CCoreApi::m_pBuffer; 
protected:
	typedef std::map<_Exception,pvoid>	MapExps;
	typedef std::map<_Notify,pvoid>		MapNtfs;
	static MapExps	CCoreApi::m_expList;
	static MapNtfs	CCoreApi::m_ntfList;
};

