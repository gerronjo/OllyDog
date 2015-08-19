
#pragma once

#include <codecvt>

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <Shlwapi.h>
#include <windows.h>

#include <plugin.h>

#include "CmdLog.hpp"
#include "Python.hpp"

typedef enum _CMDTYPE {	// Command types we supported .
	CMD_NONE,
	CMD_BAT, CMD_LUA, CMD_JAVA, CMD_PERL, CMD_RUBY,
	CMD_TOOL, CMD_INLINE, CMD_PYTHON, CMD_SYSTEM,
	CMD_MULTICMD
} CMDTYPE;

class CCmd
{
public:
	ulong							m_ulCmdId;
	std::wstring					m_strFull;
	std::wstring					m_strCmds;
	std::vector<std::wstring>	m_vetArgv;
	void clear() {	// Clear the command ..
		m_strCmds.clear(); m_strFull.clear(); 
		m_vetArgv.clear(); // ...............
	};
	bool empty() {	// Check if the command is empty
		if (m_strFull.length()) return false;
		if (m_strCmds.length()) return false;
		if ( m_vetArgv.size() ) return false;
		return true;
	};
};

class CCmdExe : public CCmdLog, public CPython
{
public:
private:
protected:
	#define WM_USER_ENCB		(WM_USER+100)
	#define WM_USER_ETCB		(WM_USER+101)
	typedef enum _ENEXEC {
		ENEXEC_NONE, ENEXEC_FILE, ENEXEC_FUNCTION, ENEXEC_MODULE, 
		ENEXEC_VARIABLE
	} ENEXEC;
	typedef enum _ENTYPE {
		ENTYPE_NONE, ENTYPE_LUA, ENTYPE_JAVA, ENTYPE_PERL, 
		ENTYPE_RUBY, ENTYPE_PYTHON
	} ENTYPE;
	typedef enum _ETTYPE {
		ETTYPE_NONE, ETTYPE_EXE, ETTYPE_DLL, ETTYPE_IDA
	} ETTYPE;
	typedef enum _BPTYPE {		// For internal usage ..
		BPTYPE_NONE, BPTYPE_INT3, BPTYPE_PAGE, BPTYPE_MEMORY, 
		BPTYPE_HARDWARE
	} BPTYPE;
	class CCmdEn : public CCmd {	// For internal or subclass
		public: ENTYPE m_intType; ENEXEC m_intExec;
	};
	class CCmdEt : public CCmd {	// For internal or subclass
		public: ETTYPE m_intType;
	};
	class CCmdBp : public CCmd {	// For internal or subclass
		//ulong          m_nLimit;	// Original pass count (0 if not set)
		//ulong          m_nCount;	// Actual pass count
	};
	class CCmdEx : public CCmd {	// For internal or subclass
		typedef int (CCmdExe::*CmdHandler)(const CCmd&, std::wstring&);
		public: CmdHandler	m_exeFunc;	// The command handler .
		public: std::wstring	m_strHelp;
		std::map<wchar*, std::wstring> m_mapArgs;
		std::map<wchar*, std::wstring> m_mapOpts;
	};
	class CCmdKeyComp {			// For key comparison.
		public: bool operator() (
			const wchar* lpsDst, const wchar* lpsSrc) const {
			return wcscmp(lpsDst, lpsSrc) < 0;
		};
	};
	typedef std::map<const wchar*,CCmdEx,CCmdKeyComp> MapCmds;
	MapCmds									m_mapCmds;
	std::map<ulong, CCmdBp>					m_mapSwBp;
	std::map<ulong, CCmdBp>					m_mapMmBp;
	std::map<ulong, CCmdBp>					m_mapHwBp;

	HANDLE										m_htCmdCb;
	DWORD										m_dwCmdCb;

	CRITICAL_SECTION							m_csShare;

public:
	CCmdExe(); ~CCmdExe();
	// return value > 0  --> The command inputed has problems .
	// return value < 0  --> The parsing process has problems .
	// return value = 0  --> The command parsed/executed successfully .
	virtual int Stop(unsigned long);	// Unsupport yet .
	virtual int Exec(const CCmd &cmd, std::wstring &err);
	virtual int Exec(const std::wstring &cmd, std::wstring &err);
	// Parsing function ..
	virtual int GetTkn(const std::wstring&, std::wstring&) const;
	virtual int GetCmd(const std::wstring&, std::wstring&) const;
	virtual int GetCmds(const std::wstring&, std::vector<CCmd>&) const;
	virtual int GetOpts(const std::wstring&, std::vector<std::wstring>&) const;
	// Get help information for the command.
	virtual int GetCmdHelps(const std::wstring &cmd, std::wstring &info) const;
	virtual int GetCmdUsage(const std::wstring &cmd, std::wstring &info) const;
	virtual int GetFilePath(const std::wstring &file, std::wstring &path) const;
	virtual int RelatedCmds(const std::wstring &cmd, std::vector<const wchar*> &cmds) const;

	// Update inside members ..
	virtual int Update(void);

private:
protected:
	virtual bool ExeInit(); virtual void ExeFree();
	// Asynchronous execution callback manager ..
	virtual int AddEnCb(const CCmdEn &cmd);
	virtual int AddEtCb(const CCmdEt &cmd);
	// Breakpoint callback manager ..
	virtual int AddBpCb(ulong addr, BPTYPE type, const CCmdBp &cmd);
	virtual int DelBpCb(ulong addr, BPTYPE type);
	virtual int GetBpCb(ulong addr, BPTYPE type, CCmdBp &cmd);
	// Asynchronous execution callback function ..
	static  DWORD WINAPI CmdsCb(LPVOID lpParameter);
	// Output capturing callback function ..
	static  DWORD WINAPI PipeCb(LPVOID lpParameter);
	// Breakpoint callback function ..
	static  bool OnBpCb(t_run*, const t_disasm*, t_thread*, int&, void*);
	virtual bool OnBpCb(t_run*, const t_disasm*, t_thread*, int&);
	// ...
	virtual int OnEnCb(const CCmdEn &cmd, std::wstring &err);
	// ...
	virtual int OnEtCb(const CCmdEt &cmd, std::wstring &err);
	// Extented Command Handler.
	virtual int OnExtn(const CCmd & cmd, std::wstring & err);
	// External Command Handler.
	virtual int OnTool(const CCmd & cmd, std::wstring & err);
	// Internal Command Handler.
	virtual int OnEcho(const CCmd & cmd, std::wstring & err);
	virtual int OnClog(const CCmd & cmd, std::wstring & err);
	virtual int OnCmds(const CCmd & cmd, std::wstring & err);
	virtual int OnExec(const CCmd & cmd, std::wstring & err);
	virtual int OnLoad(const CCmd & cmd, std::wstring & err);
	virtual int OnOpen(const CCmd & cmd, std::wstring & err);
	virtual int OnShut(const CCmd & cmd, std::wstring & err);
	virtual int OnStop(const CCmd & cmd, std::wstring & err);
	virtual int OnQuit(const CCmd & cmd, std::wstring & err);
	// Display Memory Command Handler .
	virtual int OnDump(const CCmd & cmd, std::wstring & err);
	// Software Breakpoint Command Handler .
	virtual int OnSwbp(const CCmd & cmd, std::wstring & err);
	// Memory Breakpoint Command Handler .
	virtual int OnMmbp(const CCmd & cmd, std::wstring & err);
	// Hardware Breakpoint Command handler .
	virtual int OnHwbp(const CCmd & cmd, std::wstring & err);
	// Application Control Command Handler .
	virtual int OnCtrl(const CCmd & cmd, std::wstring & err);
	// App Tracing Control Command Handler .
	virtual int OnTrak(const CCmd & cmd, std::wstring & err);
	// Information Finding Command Handler .
	virtual int OnFind(const CCmd & cmd, std::wstring & err);
};



