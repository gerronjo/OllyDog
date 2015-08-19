

#include "CmdExe.hpp"



CCmdExe::CCmdExe()
{
	// Initialize the internal commands we supported .
	CCmdEx cmd;
	// OllyDog Extented Commands
	cmd.m_exeFunc = &CCmdExe::OnExtn;
	cmd.m_strCmds = TEXT("."); 
	cmd.m_strFull = TEXT(".");
	cmd.m_strHelp = TEXT("This command is to execute extended commands.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("."), cmd));
	// OllyDog External Commands
	cmd.m_exeFunc = &CCmdExe::OnTool;
	cmd.m_strCmds = TEXT("!"); 
	cmd.m_strFull = TEXT("!ping www.baidu.com");
	cmd.m_strHelp = TEXT("This command is to execute external commands.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("!"), cmd));
	// OllyDog Internal Commands
	cmd.m_exeFunc = &CCmdExe::OnEcho;
	cmd.m_strCmds = TEXT(".echo"); 
	cmd.m_strFull = TEXT(".echo");
	cmd.m_strHelp = TEXT("This command is to display format string.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".echo"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnClog;
	cmd.m_strCmds = TEXT(".clog"); 
	cmd.m_strFull = TEXT(".clog");
	cmd.m_strHelp = TEXT("This command is to clear the log window.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".clog"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnCmds;
	cmd.m_strCmds = TEXT(".cmds");
	cmd.m_strFull = TEXT(".cmds \"bp 0x004010cc\"");
	cmd.m_strHelp = TEXT("This command is to execute internal command line.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".cmds"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnExec;
	cmd.m_strCmds = TEXT(".exec");
	cmd.m_strFull = TEXT(".exe \"import os;\"");
	cmd.m_strHelp = TEXT("This command is to execute the script command line.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".exec"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnLoad;
	cmd.m_strCmds = TEXT(".load");
	cmd.m_strFull = TEXT(".load \"c:/pycmd/bp.py\" \"/h /p 0x004010cc\"");
	cmd.m_strHelp = TEXT("This command is to load and execute the python script.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".load"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnOpen;
	cmd.m_strCmds = TEXT(".open");
	cmd.m_strFull = TEXT(".open \"c:/Windows/SysWOW64/notepad.exe\" \"/h /p \"");
	cmd.m_strHelp = TEXT("This command is to open executable file to debug.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".open"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnShut;
	cmd.m_strCmds = TEXT(".shut");
	cmd.m_strFull = TEXT(".shut /r");
	cmd.m_strHelp = TEXT("This command is to close/restart the debugged app.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".shut"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnQuit;
	cmd.m_strCmds = TEXT(".quit");
	cmd.m_strFull = TEXT(".quit");
	cmd.m_strHelp = TEXT("This command is to close OllyDBG.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".quit"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnQuit;
	cmd.m_strCmds = TEXT(".exit");
	cmd.m_strFull = TEXT(".exit");
	cmd.m_strHelp = TEXT("This command is to close OllyDBG.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".exit"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnStop;
	cmd.m_strCmds = TEXT(".stop");
	cmd.m_strFull = TEXT(".stop");
	cmd.m_strHelp = TEXT("This command is to stop the running app.");
	m_mapCmds.insert(MapCmds::value_type(TEXT(".stop"), cmd));
	// Memory Dumping Commands .
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("db");
	cmd.m_strFull = TEXT("db MessageBoxW+1");
	cmd.m_strHelp = TEXT("This command is to display byte values.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("db"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("dw");
	cmd.m_strFull = TEXT("dw esi+2");
	cmd.m_strHelp = TEXT("This command is to display word values.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("dw"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("dd");
	cmd.m_strFull = TEXT("dd edi+4");
	cmd.m_strHelp = TEXT("This command is to display dword values.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("dd"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("df");
	cmd.m_strFull = TEXT("df 0x004010cc");
	cmd.m_strHelp = TEXT("This command is to display floating-point values.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("df"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("dp");
	cmd.m_strFull = TEXT("dp esp+4");
	cmd.m_strHelp = TEXT("This command is to display address values.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("dp"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("da");
	cmd.m_strFull = TEXT("da esp+8");
	cmd.m_strHelp = TEXT("This command is to display ASCII characters.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("da"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnDump;
	cmd.m_strCmds = TEXT("du");
	cmd.m_strFull = TEXT("du esp+8");
	cmd.m_strHelp = TEXT("This command is to display UNICODE characters.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("du"), cmd));
	// Software Breakpoint Commands .
	cmd.m_exeFunc = &CCmdExe::OnSwbp;
	cmd.m_strCmds = TEXT("bp");
	cmd.m_strFull = TEXT("bp /ps /s 1 /t 3eb /c 'eax > 0' '[dword eax+8]'");
	cmd.m_strHelp = TEXT("This command sets a new breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bp"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnSwbp;
	cmd.m_strCmds = TEXT("bs");
	cmd.m_strFull = TEXT("bs ollydbg.mess.+?");
	cmd.m_strHelp = TEXT("This command sets a new breakpoint on pattern symbols.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bs"), cmd));
	// Memory Breakpoint Commands .
	cmd.m_exeFunc = &CCmdExe::OnMmbp;
	cmd.m_strCmds = TEXT("bm");
	cmd.m_strFull = TEXT("bm /ps /w /s 3 /t 3eb /c 'eax > 0' '[dword eax+8]' 10");
	cmd.m_strHelp = TEXT("This command sets a new memory breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bm"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnMmbp;
	cmd.m_strCmds = TEXT("bmr");
	cmd.m_strFull = TEXT("bmr /s 3 '[dword eax+8]' 10 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new memory read access breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bmr"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnMmbp;
	cmd.m_strCmds = TEXT("bmw");
	cmd.m_strFull = TEXT("bmw /s 3 '[dword eax+8]' 10 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new memory write access breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bmw"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnMmbp;
	cmd.m_strCmds = TEXT("bme");
	cmd.m_strFull = TEXT("bme /s 3 '[dword eax+8]' 10 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new memory execution breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bme"), cmd));
	// Hardware Breakpoint Commands .
	cmd.m_exeFunc = &CCmdExe::OnHwbp;
	cmd.m_strCmds = TEXT("bh");
	cmd.m_strFull = TEXT("bh /ps /s 1 /t 3eb /c 'eax > 0' '[dword eax+8]'");
	cmd.m_strHelp = TEXT("This command sets a new hardware breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bh"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnHwbp;
	cmd.m_strCmds = TEXT("bha");
	cmd.m_strFull = TEXT("bha /s 3 '[dword eax+8]' 2 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new hardware read access breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bha"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnHwbp;
	cmd.m_strCmds = TEXT("bhw");
	cmd.m_strFull = TEXT("bhw /s 1 '[dword eax+8]' 1 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new hardware write access breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bhw"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnHwbp;
	cmd.m_strCmds = TEXT("bhe");
	cmd.m_strFull = TEXT("bhe /s 1 '[dword eax+8]' 'bp MessageBoxW; go;'");
	cmd.m_strHelp = TEXT("This command sets a new hardware execution breakpoint.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("bhe"), cmd));
	// Application Control Commands .
	cmd.m_exeFunc = &CCmdExe::OnCtrl;
	cmd.m_strCmds = TEXT("g");
	cmd.m_strFull = TEXT("g eip+5");
	cmd.m_strHelp = TEXT("This command executes the app.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("g"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnCtrl;
	cmd.m_strCmds = TEXT("p");
	cmd.m_strFull = TEXT("p");
	cmd.m_strHelp = TEXT("This command Steps over a single instruction.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("p"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnCtrl;
	cmd.m_strCmds = TEXT("pi");
	cmd.m_strFull = TEXT("pi");
	cmd.m_strHelp = TEXT("This command Steps into a single instruction.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("pi"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnCtrl;
	cmd.m_strCmds = TEXT("po");
	cmd.m_strFull = TEXT("po");
	cmd.m_strHelp = TEXT("This command steps over a single instruction.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("po"), cmd));
	// App Tracing Control Commands .
	cmd.m_exeFunc = &CCmdExe::OnTrak;
	cmd.m_strCmds = TEXT("t");
	cmd.m_strFull = TEXT("t");
	cmd.m_strHelp = TEXT("This command traces over instructions.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("t"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnTrak;
	cmd.m_strCmds = TEXT("ti");
	cmd.m_strFull = TEXT("ti");
	cmd.m_strHelp = TEXT("This command traces into instructions.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("ti"), cmd));
	cmd.m_exeFunc = &CCmdExe::OnTrak;
	cmd.m_strCmds = TEXT("to");
	cmd.m_strFull = TEXT("to");
	cmd.m_strHelp = TEXT("This command traces over instructions.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("to"), cmd));
	// Information Finding Commands .
	cmd.m_exeFunc = &CCmdExe::OnFind;
	cmd.m_strCmds = TEXT("s");
	cmd.m_strFull = TEXT("s");
	cmd.m_strHelp = TEXT("This command traces over instructions.");
	m_mapCmds.insert(MapCmds::value_type(TEXT("s"), cmd));

	// .....
	this->m_htCmdCb = nullptr; this->m_dwCmdCb = 0;
}
CCmdExe::~CCmdExe()
{
	// Clear command list .
	m_mapCmds.clear();
}


int CCmdExe::Stop(unsigned long timeout)
{
	return 0;
}


bool CCmdExe::ExeInit()
{
	ulong id = 0; bool brslt = false;

	Addtolist(			// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter initialization function CCmdExe::ExeInit")
	);

	// Register an exception handler ..
	brslt = AddExceptionCallback(CCmdExe::OnBpCb, this);

	// Create a thread to handle asynchronous execution
	if ( (this->m_htCmdCb = CreateThread(
		nullptr, 0, CCmdExe::CmdsCb, this, 0, &this->m_dwCmdCb
	)) == INVALID_HANDLE_VALUE ) {
		Addtolist(		// .................
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Fail to create thread 'CCmdExe::CmdsCb'")
		);	brslt = false;
	// Initialize the critical section 
	} else InitializeCriticalSection(&this->m_csShare);

	Addtolist(			// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave initialization function CCmdExe::ExeInit")
	);

	return true; // Now, we finish all stuff, then return true.
}

void CCmdExe::ExeFree()
{
	PostThreadMessage(this->m_dwCmdCb, WM_QUIT, 0, 0);
	WaitForSingleObject(this->m_htCmdCb, INFINITE);
	CloseHandle(this->m_htCmdCb); this->m_htCmdCb = nullptr;
	DeleteCriticalSection(&this->m_csShare);
}


int CCmdExe::AddEnCb(const CCmdEn &cmd)
{
	CCmdEn *p = new CCmdEn; *p = cmd;
	PostThreadMessage(
		this->m_dwCmdCb, WM_USER_ENCB, reinterpret_cast<WPARAM>(p), 0
	); return 0;
}
int CCmdExe::AddEtCb(const CCmdEt &cmd)
{
	CCmdEt *p = new CCmdEt; *p = cmd;
	PostThreadMessage(
		this->m_dwCmdCb, WM_USER_ETCB, reinterpret_cast<WPARAM>(p), 0
	); return 0;
}


int CCmdExe::AddBpCb(ulong addr, BPTYPE type, const CCmdBp &cmd)
{
	int irslt = 0;
	EnterCriticalSection(&this->m_csShare); switch(type) {
		case BPTYPE_INT3: this->m_mapSwBp[addr] = cmd; break;
		case BPTYPE_MEMORY: this->m_mapMmBp[addr] = cmd; break;
		case BPTYPE_HARDWARE: this->m_mapHwBp[addr] = cmd; break;
		default: irslt = -1; break;
	} LeaveCriticalSection(&this->m_csShare); return irslt;
}
int CCmdExe::DelBpCb(ulong addr, BPTYPE type)
{
	int irslt = 0;
	EnterCriticalSection(&this->m_csShare); switch(type) {
		case BPTYPE_INT3: this->m_mapSwBp.erase(addr); break;
		case BPTYPE_MEMORY: this->m_mapMmBp.erase(addr); break;
		case BPTYPE_HARDWARE: this->m_mapHwBp.erase(addr); break;
		default: irslt = -1; break;
	} LeaveCriticalSection(&this->m_csShare); return irslt;
}
int CCmdExe::GetBpCb(ulong addr, BPTYPE type, CCmdBp &cmd) 
{
	std::map<ulong, CCmdBp>::const_iterator itr; 
	switch(type) { 
		case BPTYPE_INT3: 
			itr = this->m_mapSwBp.find(addr); 
			if (itr != this->m_mapSwBp.cend()) cmd = itr->second; 
			break;
		case BPTYPE_MEMORY:
			itr = this->m_mapMmBp.find(addr); 
			if (itr != this->m_mapMmBp.cend()) cmd = itr->second; 
			break;
		case BPTYPE_HARDWARE:
			itr = this->m_mapHwBp.find(addr); 
			if (itr != this->m_mapHwBp.cend()) cmd = itr->second; 
			break;
		default: return -1;
	} return 0;
}


bool CCmdExe::OnBpCb(t_run* run, const t_disasm* disasm, t_thread* thread, int& rslt, void* parm)
{
	if (run->de.dwDebugEventCode != EXCEPTION_DEBUG_EVENT) return true;
	if (run->de.u.Exception.dwFirstChance <= 0) return true;
	CCmdExe *pthis = reinterpret_cast<CCmdExe*>(parm);
	switch (run->status) {
		case STAT_RUNNING: case STAT_RUNTHR:
		case STAT_TILLRET: case STAT_OVERRET: case STAT_TILLUSER:
		if (pthis) return pthis->OnBpCb(run, disasm, thread, rslt);
		default: break;
	} return true;
}
bool CCmdExe::OnBpCb(t_run* run, const t_disasm* disasm, t_thread* thread, int& rslt)
{
	EXCEPTION_RECORD *pRecord = &run->de.u.Exception.ExceptionRecord;
	BPTYPE type = BPTYPE_NONE; int ntype = NM_NONAME; ulong otype = 0;
	CCmdBp cmd; ulong addr, solt; wchar buffer[TEXTLEN] = {0}; 
	t_bpoint *bpsw; t_bpmem *bpmm; t_bphard *bphw;
	ulong *limit = nullptr, *count = nullptr; 

	// Get the breakpoint information.
	switch(pRecord->ExceptionCode) {
		// For software breakpoint.
		case STATUS_BREAKPOINT:
		case STATUS_PRIVILEGED_INSTRUCTION:
			addr = reinterpret_cast<ulong>(pRecord->ExceptionAddress);
			bpsw = GetSwBp(addr); if (bpsw == nullptr) return true;
			solt = addr; if (bpsw->count == 0) bpsw->count = 1;
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("limit:%d, count:%d"), bpsw->limit, bpsw->count
			);
			type = BPTYPE_INT3; ntype = NM_INT3COND; otype = bpsw->type;
			limit = &bpsw->limit; count = &bpsw->count; break;
		// For page breakpoint . 
		case STATUS_GUARD_PAGE_VIOLATION:
			return true;	// Not support yet ..
		// For memory breakpoint ..
		case STATUS_ACCESS_VIOLATION: 
			addr = static_cast<ulong>(pRecord->ExceptionInformation[1]);
			bpmm = GetMmBp(addr); if (bpmm == nullptr) return true;
			solt = bpmm->addr; if (bpmm->count == 0) bpmm->count = 1;
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("limit:%d, count:%d"), bpmm->limit, bpmm->count
			);
			type = BPTYPE_MEMORY; ntype = NM_MEMCOND; otype = bpmm->type;
			limit = &bpmm->limit; count = &bpmm->count; break;
		// For hardware breakpoint ..
		case STATUS_SINGLE_STEP: 
			switch(thread->context.Dr6 & 0x0f) {
				case 1: solt = 0; bphw = GetHwBp(0);
				addr = thread->context.Dr0; break;
				case 2: solt = 1; bphw = GetHwBp(1); 
				addr = thread->context.Dr1; break;
				case 4: solt = 2; bphw = GetHwBp(2); 
				addr = thread->context.Dr2; break;
				case 8: solt = 3; bphw = GetHwBp(3); 
				addr = thread->context.Dr3; break;
				default: return true;				}
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("limit:%d, count:%d"), bphw->limit, bphw->count
			);
			if (bphw == nullptr) return true;
			if (bphw->count == 0) bphw->count = 1;
			type = BPTYPE_HARDWARE; ntype = NM_HARDCOND; otype = bphw->type;
			limit = &bphw->limit; count = &bphw->count; break;
		// For other case ..
		default: return true;
	}
	Addtolist(				// .................
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("breakpoint type: %x, breakpoint address: %x"), type, addr
	);
	GetBpCb(solt, type, cmd); if (cmd.empty()) return true;

	// Handle Common Options ...
	std::vector<std::wstring>::const_iterator itr, eitr;
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for ( ; itr != eitr; ++itr) {
		if (itr->compare(text("/t")) == 0) {
			ulong id = wcstoul((++itr)->c_str(), nullptr, 10);
			Addtolist(				// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("/t:%x, thread id:%x, thread tib:%x"), 
				id, thread->threadid, thread->tib
			);
			if (id != thread->threadid && id != thread->tib) {
				--*count; return true;
			}
		}
	}

	// Check if the od conditional expression is true ..
	if (FindnameW(solt, ntype, buffer, _countof(buffer)) > 1) {
		t_result trslt = {0};
		Expression(	//	value > 0 means true ..
			&trslt, buffer, nullptr, 0, 0, thread->threadid, 
			0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
		);
		if ((trslt.datatype & EXPR_TYPEMASK) && trslt.u <= 0) {
			Addtolist(				// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("%s is false, skip it"),  buffer
			); return true;
		}
	}

	// Check if the counter reach the limit ..
	if (*count+1 < *limit) return true;

	// Parse commands and execute them ..
	std::wstring err; std::vector<CCmd> cmds;
	if (cmd.m_strFull.length() <= 0) return true;
	if (GetCmds(cmd.m_strFull, cmds) == 0) {
		std::vector<CCmd>::iterator citr, ecitr;
		ecitr = cmds.end(); citr = cmds.begin();
		for (int irslt = 0; citr != ecitr; ++citr) {
			// Handle special commands ..
			if (citr->m_strCmds.compare(text("go")) == 0) {
				if (otype & BP_PERIODICAL) *count = 0;
				else --*count; continue;
			} 
			// Execute inside command ...
			citr->m_ulCmdId = cmd.m_ulCmdId; err.clear();
			if ( (irslt = Exec(*citr, err)) != 0 ) {
				err = citr->m_strFull + text("  -->  ") + err;
				set(citr->m_ulCmdId, irslt, CMD_INLINE, 0);
				break;
			}	set(citr->m_ulCmdId, irslt, CMD_INLINE, 0);
		} if (citr == ecitr) {
			err.insert(0, text("Execute '"));
			err += cmd.m_strFull + text("' successfully");
		}
	} else err = text("Fail to parse callback expression");

	// Generate tip message ..
	wchar *action = nullptr;
	switch(type) {
		case BPTYPE_INT3: 
			swprintf_s(
				buffer, _countof(buffer), 
				text("Hit int3 breakpoint at 0x%x. "), addr
			); break;
		case BPTYPE_PAGE:
			return true;		// Not support yet ..
		case BPTYPE_MEMORY:
			switch(pRecord->ExceptionInformation[0]) {
				case  0: action = text("reading from"); break;
				case  1: action = text("writing to"); break;
				case  8: action = text("executing at"); break;
				default: action = text("operating at"); break;
			}
			swprintf_s(
				buffer, _countof(buffer),
				text("Hit memory breakpoint at 0x%x while %s 0x%x. "),
				addr, action, pRecord->ExceptionInformation[1]
			); break;
		case BPTYPE_HARDWARE:
			switch(((thread->context.Dr7>>16)>>(solt*4)) & 0x3) {
				case  0: action = text("executing at");	break;
				case  1: action = text("writing to");		break;
				case  3: action = text("accessing at");	break;
				default: action = text("operating at");	break;
			}
			swprintf_s(
				buffer, _countof(buffer),
				text("Hit hardware breakpoint at 0x%x while %s 0x%x. "),
				disasm->ip, action, addr
			); break;
		default: return true; 
	}

	// Output the error message ..
	err.insert(0, text("    Callback execution: ")); 
	err.insert(0, buffer); Tempinfo(const_cast<wchar*>(err.c_str())); 

	return true;
}

DWORD WINAPI CCmdExe::CmdsCb(LPVOID lpParam)
{
	std::wstring err; MSG msg = {0};
	CCmdEn *en = nullptr; CCmdEt *et = nullptr;
	CCmdExe *pthis = reinterpret_cast<CCmdExe*>(lpParam);

	Addtolist ( 
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter Thread 'CCmdExe::CmdsCb'")
	);

	// force the system to create the message queue.
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	// Message loop ...
	for ( ; GetMessage(&msg, nullptr, 0, WM_APP); ) {
		switch (msg.message) {
		case WM_USER_ENCB: 
			en = reinterpret_cast<CCmdEn*>(msg.wParam);
			if (en == nullptr) break; pthis->OnEnCb(*en, err); 
			err = en->m_strFull + text("  -->  ") + err;
			pthis->setidle(en->m_ulCmdId); delete en;
			break;
		case WM_USER_ETCB: 
			et = reinterpret_cast<CCmdEt*>(msg.wParam);
			if (et == nullptr) break; pthis->OnEtCb(*et, err);
			err = et->m_strFull + text("  -->  ") + err;
			pthis->setidle(et->m_ulCmdId); delete et; 
			break;
		default: 
			err = text("This is an unrecognized command type");
			err = et->m_strFull + text("  -->  ") + err;
			break;
		}
		//Tempinfo(const_cast<wchar*>(err.c_str())); err.clear();
	} 

	Addtolist(
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave Thread 'CCmdExe::CmdsCb'")
	);

	return 0;
}
int CCmdExe::OnEnCb(const CCmdEn &cmd, std::wstring &err)
{
	int irslt = 0;
	switch (cmd.m_intType) {
	case ENTYPE_LUA: 
		err = text("Current version doesn't support lua yet");
		return -1;
	case ENTYPE_JAVA: 
		err = text("Current version doesn't support java yet");
		return -1; 
	case ENTYPE_PERL: 
		err = text("Current version doesn't support perl yet"); 
		return -1; 
	case ENTYPE_RUBY: 
		err = text("Current version doesn't support ruby yet"); 
		return -1; 
	case ENTYPE_NONE: 
	case ENTYPE_PYTHON: 
		connect(cmd.m_ulCmdId);	// Connect to logging system . 
		switch (cmd.m_intExec) { 
		case ENEXEC_FILE: 
			irslt = CPython::ExecFile(cmd.m_vetArgv, err, getpipe()); 
			Addtolist(1,1,text("ENEXEC_FILE")); 
			break; 
		case ENEXEC_MODULE: 
			irslt = CPython::ExecModl(cmd.m_strCmds, err, getpipe()); 
			Addtolist(1,1,text("ENEXEC_MODULE")); 
			break; 
		case ENEXEC_FUNCTION: 
			irslt = CPython::ExecFunc(cmd.m_vetArgv, err, getpipe()); 
			Addtolist(1,1,text("ENEXEC_FUNCTION")); 
			break; 
		case ENEXEC_VARIABLE: 
			irslt = CPython::ExecEcho(cmd.m_strCmds, err, getpipe()); 
			Addtolist(1,1,text("ENEXEC_VARIABLE")); 
			break; 
		default: Addtolist(1,1,text("Unknow")); break; 
		} 
		disconn(cmd.m_ulCmdId);	// Disonnect from logging system . 
		break; 
	default: 
		err = text("This is an unrecognized extended command"); 
		return 1;
	}
	return irslt;
}
int CCmdExe::OnEtCb(const CCmdEt &cmd, std::wstring &err)
{
	std::wstring name; std::size_t npos = 0;
	typedef void* (__cdecl *TOOLFUNCTION)(...);
	TOOLFUNCTION fn = nullptr; std::vector<ulong> args;
	std::vector<std::wstring>::const_iterator itr, eitr;
	PROCESS_INFORMATION pi; STARTUPINFO si; HMODULE hmod;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;

	switch (cmd.m_intType) {
	case ETTYPE_DLL:
		eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin(); 
		npos = itr->find(text('.'), 0); name = itr->substr(0,npos);
		hmod = LoadLibrary(name.append(text(".dll")).c_str());
		if (hmod == nullptr) {	// Can't load the dll ..
			err = text("Internal error that LoadLibrary fail.");
			return -1;
		}
		if (npos != std::wstring::npos) {
			name = itr->substr(++npos, std::wstring::npos);
			fn = reinterpret_cast<TOOLFUNCTION>(
				GetProcAddress(hmod, cvt.to_bytes(name.c_str()).c_str())
			);
			if (fn == nullptr) {
				err = text("Internal error that GetProcAddress fail.");
				return -1;
			}
			for (++ itr; itr != eitr; ++ itr) {
				t_result trslt = {0}; int irslt = Expression (
					&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
					Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|NEXPR
				);
				if (trslt.datatype & EXPR_DWORD) args.push_back(trslt.u);
				else args.push_back(reinterpret_cast<ulong>(itr->c_str())); 
			}
			//
			// This feature is not secure, so don't use it ..
			// Pointer type like "void* (__stdcall *)(...)" is similar to 
			// __cdecl, which will balance stack outside the function. So
			// it's really not suitable for this usage. if you really want
			// to use this feature, make sure the calling conventions is 
			// __cdecl
			//
			//for i in range(20) :
			//	line = "case{: 3d}: fn(".format(i)
			//	for j in range(i):
			//		line = line + "args[{:02d}]".format(j)
			//		if i > 0 and j < i-1: line += ","
			//	line += "); break;"
			//	print line
			switch (args.size()) {
			case  0: fn(); break;
			case  1: fn(args[0]); break;
			case  2: fn(args[0],args[1]); break;
			case  3: fn(args[0],args[1],args[2]); break;
			case  4: fn(args[0],args[1],args[2],args[3]); break;
			case  5: fn(args[0],args[1],args[2],args[3],args[4]); break;
			case  6: fn(args[0],args[1],args[2],args[3],args[4],args[5]); break;
			case  7: fn(args[0],args[1],args[2],args[3],args[4],args[5],args[6]); break;
			case  8: fn(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]); break;
			case  9: fn(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8]); break;
			default: err = text("Too many arguments, tool function fail"); break;
			}
		}

	case ETTYPE_IDA: break;			// Not support yet ..

	default:
		connect(cmd.m_ulCmdId);		// Connect to logging system .
		// Create the child process
		memset( &si, 0, sizeof(STARTUPINFO) );
		memset( &pi, 0, sizeof(PROCESS_INFORMATION) );
		si.cb			= sizeof(STARTUPINFO);
		si.hStdOutput	= getpipe();
		si.hStdError	= getpipe();
		si.dwFlags |= STARTF_USESTDHANDLES;
		if ( CreateProcess(
			nullptr, 
			const_cast<wchar*>(cmd.m_strFull.c_str()),// command line 
			nullptr,					// process security attributes 
			nullptr, 					// primary thread security attributes 
			true, 						// handles are inherited 
			DETACHED_PROCESS, 		// creation flags 
			nullptr, 					// use parent's environment 
			nullptr, 					// use parent's current directory 
			&si, 						// STARTUPINFO pointer 
			&pi							// receives PROCESS_INFORMATION
		) == FALSE ) {
			CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
			err = text("Internal error that CreateProcess fail.");
			return -1;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		disconn(cmd.m_ulCmdId);		// Disonnect from logging system .
   }
   return 0;
}


//
// Extented Command Handler.
//
int CCmdExe::OnExtn(const CCmd & cmd, std::wstring & err)
{
	CCmdEn ccmd; ccmd.m_intType = ENTYPE_NONE; 
	std::size_t len = cmd.m_strCmds.length(), pos = 0;
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse extended command and check if it exists.. 
	ccmd.m_strCmds.assign(cmd.m_strCmds, 1, len-1); 
	HANDLE hFile = CreateFile (	// Check if it's file path .. 
		ccmd.m_strCmds.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
	);
	if (hFile != INVALID_HANDLE_VALUE) ccmd.m_intExec = ENEXEC_FILE;
	else ccmd.m_intExec = ENEXEC_VARIABLE; CloseHandle(hFile);

	// Parse first option, check the command type .. 
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin(); 
	for (++ itr; itr != eitr; ++ itr) { 
		if (itr->compare(text("/jar")) == 0) { 
			ccmd.m_intType = ENTYPE_JAVA; 
		} else if (itr->compare(text("/lua")) == 0) {
			ccmd.m_intType = ENTYPE_LUA;
		} else if (itr->compare(text("/py")) == 0) {
			ccmd.m_intType = ENTYPE_PYTHON;
		} else if (itr->compare(text("/pl")) == 0) {
			ccmd.m_intType = ENTYPE_PERL;
		} else if (itr->compare(text("/rb")) == 0) {
			ccmd.m_intType = ENTYPE_RUBY;
		} else break;					// Finish parsing ....
	} 

	// Convert command to file ..
	switch (ccmd.m_intType) { 
	case ENTYPE_LUA: 
		err = text("Current version doesn't support lua yet"); 
		set(cmd.m_ulCmdId, 0, CMD_LUA, 0); 
		return -1; 
	case ENTYPE_JAVA: 
		err = text("Current version doesn't support java yet"); 
		set(cmd.m_ulCmdId, 0, CMD_JAVA, 0); 
		return -1; 
	case ENTYPE_PERL: 
		err = text("Current version doesn't support perl yet"); 
		set(cmd.m_ulCmdId, 0, CMD_PERL, 0); 
		return -1; 
	case ENTYPE_RUBY: 
		err = text("Current version doesn't support ruby yet"); 
		set(cmd.m_ulCmdId, 0, CMD_RUBY, 0); 
		return -1; 
	case ENTYPE_NONE: 
	case ENTYPE_PYTHON: 
		if (ccmd.m_strCmds.find(text('.')) == std::wstring::npos) { 
			ccmd.m_strCmds.append(text(".py")); 
			ccmd.m_intExec = ENEXEC_FILE; 
			// Check if the file exist ..
			if (GetFilePath(ccmd.m_strCmds, err) != 0) {
				err = text("This is an unrecognized python command"); 
				set(cmd.m_ulCmdId, 0, CMD_PYTHON, 0); 
				return 1;
			} else ccmd.m_strCmds = err;
		} 
		set(cmd.m_ulCmdId, 0, CMD_PYTHON, 0); 
		ccmd.m_strFull.assign(ccmd.m_strCmds); 
		ccmd.m_vetArgv.push_back(ccmd.m_strCmds); 
		break; 
	default: 
		err = text("This is an unrecognized extended command"); 
		return 1;
	}

	// Parse parameters and rebuild the command ...
	for (t_result trslt; itr != eitr; ++itr) {
		if (itr->front() == '/' && itr->front() == '-') {
			ccmd.m_vetArgv.push_back(*itr); continue;
		} 
		wchar buffer[32*2]; int irslt = Expression (
			&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
			Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|NEXPR
		);
		switch (trslt.datatype & EXPR_TYPEMASK) { 
		case EXPR_BYTE: 
		case EXPR_WORD: 
		case EXPR_DWORD: 
			swprintf_s ( buffer, _countof(buffer), 
				trslt.datatype & EXPR_SIGNED ? text("%d") : text("%u"), 
				trslt.datatype & EXPR_SIGNED ? trslt.u : trslt.l
			);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
		case EXPR_FLOAT4:
			swprintf_s ( 
				buffer, _countof(buffer), text("%f"), trslt.f
			);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
		case EXPR_FLOAT8:
			swprintf_s ( 
				buffer, _countof(buffer), text("%f"), trslt.f
			);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
		case EXPR_FLOAT10:
			swprintf_s ( 
				buffer, _countof(buffer), text("%Lf"), trslt.f
			);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break; 
		case EXPR_TEXT:
		case EXPR_ASCII:
		case EXPR_UNICODE:
			err = text('"'); err += trslt.value; err += text('"');
			ccmd.m_vetArgv.push_back(err); break;
		default: ccmd.m_vetArgv.push_back(*itr); break;
		}
		ccmd.m_strFull.push_back(' '); 
		ccmd.m_strFull.append(*ccmd.m_vetArgv.rbegin());
	}

	if (ccmd.m_vetArgv.size() >= 2 && ccmd.m_intExec == ENEXEC_VARIABLE) 
		ccmd.m_intExec = ENEXEC_FUNCTION; 
	if (ccmd.m_vetArgv.size() == 2 && ccmd.m_vetArgv[1] == text("void")) {
		itr = ccmd.m_vetArgv.cbegin(); ccmd.m_vetArgv.erase(itr+1);
	}

	Addtolist(1,1,text("ccmd.m_strCmds: %s"),ccmd.m_strCmds.c_str());
	Addtolist(1,1,text("ccmd.m_strFull: %s"),ccmd.m_strFull.c_str());

	// Queue this command to asynchronous execution queue
	setbusy(cmd.m_ulCmdId); ccmd.m_ulCmdId = cmd.m_ulCmdId; 
	if (AddEnCb(ccmd) != 0) {
		err = text("Internel error that AddEnCb fail."); return -1;
	} else err = text("Command executed successfully."); return 0;
}


//
// Enternal Command Handler.
//
int CCmdExe::OnTool(const CCmd & cmd, std::wstring & err)
{
	std::size_t len = cmd.m_strCmds.length();
	CCmdEt ccmd; ccmd.m_intType = ETTYPE_NONE;
	t_result trslt; int irslt; wchar buffer[TEXTLEN];
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse external tool name and check if it exists..
	ccmd.m_strCmds.assign(cmd.m_strCmds, 1, len);
	ccmd.m_vetArgv.push_back(ccmd.m_strCmds);
	ccmd.m_strFull = ccmd.m_strCmds; 
	set(cmd.m_ulCmdId, 0, CMD_TOOL, 0);

	// Parse first option, check the tool type ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->compare(text("/dll")) == 0) {
			ccmd.m_intType = ETTYPE_DLL;
		} else if (itr->compare(text("/exe")) == 0) {
			ccmd.m_intType = ETTYPE_EXE;
		} else if (itr->compare(text("/ida")) == 0) {
			ccmd.m_intType = ETTYPE_IDA;
		} else break;			// Finish parsing ....
	}

	// Commands like "!user32.MessageBoxW" will be treat as ETTYPE_DLL
	// if no option type specified ..
	if (ccmd.m_intType == ETTYPE_NONE) {
		if (ccmd.m_strCmds.find(text('.'), 0) == std::wstring::npos)
			ccmd.m_intType = ETTYPE_EXE;
		else ccmd.m_intType = ETTYPE_DLL;
	}

	// Parse parameters and rebuild the command ...
	switch (ccmd.m_intType) {
	case ETTYPE_DLL:
		for (itr; itr != eitr; ++itr) {
			if (itr->front() == '/') {
				err  = text("Found unrecognized option '");
				err += *itr + text("'"); return 1;
			} else ccmd.m_vetArgv.push_back(*itr);
			ccmd.m_strFull.push_back(' '); ccmd.m_strFull += *itr;
		}
		break;
	case ETTYPE_IDA:		// Not support yet ..
		err = text("Current version doesn't support ruby yet");
		return -1;
	case ETTYPE_EXE:
		for (itr; itr != eitr; ++ itr) {
			if (itr->front() == '/' && itr->front() == '-') {
				ccmd.m_vetArgv.push_back(*itr); continue;
			}
			irslt = Expression (
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|NEXPR
			);
			switch (trslt.datatype & EXPR_TYPEMASK) {
			case EXPR_BYTE:
			case EXPR_WORD:
			case EXPR_DWORD:
				swprintf_s( buffer, _countof(buffer), 
					trslt.datatype & EXPR_SIGNED ? text("%d") : text("%u"), 
					trslt.datatype & EXPR_SIGNED ? trslt.u : trslt.l
				);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
			case EXPR_FLOAT4:
				swprintf_s( 
					buffer, _countof(buffer), text("%f"), trslt.f
				);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
			case EXPR_FLOAT8:
				swprintf_s( 
					buffer, _countof(buffer), text("%f"), trslt.f
				);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
			case EXPR_FLOAT10:
				swprintf_s( 
					buffer, _countof(buffer), text("%Lf"), trslt.f
				);	ccmd.m_vetArgv.push_back(std::wstring(buffer)); break;
			case EXPR_TEXT:
			case EXPR_ASCII:
			case EXPR_UNICODE:
				ccmd.m_vetArgv.push_back(std::wstring(trslt.value)); break;
			default: ccmd.m_vetArgv.push_back(*itr); break;
			}
			ccmd.m_strFull.push_back(' '); 
			ccmd.m_strFull.append(*ccmd.m_vetArgv.rbegin());
		}
		break;
	default:
		err = text("This is an unrecognized extended command");
		return 1;
	}

	// Queue this command to asynchronous execution queue
	setbusy(cmd.m_ulCmdId); ccmd.m_ulCmdId = cmd.m_ulCmdId; 
	if (AddEtCb(ccmd) != 0) {
		err = text("Internel error that AddEtCb fail."); return -1;
	} else err = text("Command executed successfully."); return 0;
}


//
// Internal Command Handler.
//
int CCmdExe::OnEcho(const CCmd& cmd, std::wstring& err)
{
	int irslt = 0; t_result trslt = {0}; wchar buffer[TEXTLEN];
	std::vector<std::wstring>::const_iterator itr, eitr;
	// Parse options ..
	eitr = cmd.m_vetArgv.end(); itr = cmd.m_vetArgv.begin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->front() == '/') {
			err  = text("Found unrecognized option '");
			err += *itr + TEXT("'"); return 1;
		} else break;
	}
	if (itr == eitr) { err = text("No content."); return 0; }
	
	// The format string ...
	for (err.clear(); itr != eitr; ++itr) {
		irslt = Expression (
			&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
			Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|NEXPR
		);
		switch (trslt.datatype & EXPR_TYPEMASK) { 
			case EXPR_BYTE: 
			case EXPR_WORD: 
			case EXPR_DWORD: 
				swprintf_s( buffer, _countof(buffer), 
					trslt.datatype & EXPR_SIGNED ? text("%d") : text("%x"), 
					trslt.datatype & EXPR_SIGNED ? trslt.u : trslt.l 
				);	err += buffer; break; 
			case EXPR_FLOAT4: 
				swprintf_s( 
					buffer, _countof(buffer), text("%f"), trslt.f 
				);	err += buffer; break; 
			case EXPR_FLOAT8: 
				swprintf_s( 
					buffer, _countof(buffer), text("%f"), trslt.f 
				);	err += buffer; break; 
			case EXPR_FLOAT10: 
				swprintf_s( 
					buffer, _countof(buffer), text("%Lf"), trslt.f 
				);	err += buffer; break; 
			case EXPR_TEXT: err += trslt.value; break; 
			case EXPR_ASCII: err += trslt.value; break; 
			case EXPR_UNICODE: err += trslt.value; break; 
			default: err += *itr; break; 
		} 
	}
	msg(cmd.m_ulCmdId, err, true); err.clear(); return 0;
}
int CCmdExe::OnClog(const CCmd& cmd, std::wstring& err)
{
	wchar buffer[TEXTLEN] = {0}; swprintf_s( 
		buffer, _countof(buffer), 
		text("Clear %d logger data successfully"), clear()
	);	err = buffer; return 0;
}
int CCmdExe::OnCmds(const CCmd& cmd, std::wstring& err)
{
	int irslt = 0; std::vector<CCmd> cmds;
	std::vector<std::wstring>::const_iterator itr, eitr;
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();

	// Check options and arguments ...
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->front() == '/') {
			err  = TEXT("Found unrecognized option '");
			err += *itr + TEXT("'"); return 1;
		} else break;					// Finish option parsing ..
	}
	if (itr == eitr) { err = text("No command found."); return 2; } 

	// Execute each commands ....
	for (itr; itr != eitr; ++ itr) {
		// Now parse the command .
		if ((irslt = GetCmds(*itr, cmds)) != 0) {
			set(cmd.m_ulCmdId, irslt, CMD_INLINE, 0); 
			err = text("Fail to parse commands"); return irslt;
		}
		// Execute commands ..
		std::vector<CCmd>::iterator citr, ecitr = cmds.end();
		for (citr = cmds.begin(); citr != ecitr; ++citr) {
			citr->m_ulCmdId = cmd.m_ulCmdId;
			if ((irslt = Exec(*citr, err)) != 0) {
				msg(cmd.m_ulCmdId, err, true);
				err = citr->m_strFull + text("  -->  ") + err; 
				set(cmd.m_ulCmdId, irslt, CMD_INLINE, 0); return irslt;
			}
		} return irslt;
	}

	// Finish ........
	err = text("Execute successfully"); return 0;
}
int CCmdExe::OnExec(const CCmd& cmd, std::wstring& err)
{
	err = text("Not support this command yet."); return -1;
}
int CCmdExe::OnLoad(const CCmd& cmd, std::wstring& err)
{
	ulong type = 0; int irslt = 0; t_result trslt = {0};
	std::vector<std::wstring>::const_iterator itr, eitr;
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->compare(text("/dr")) == 0) {
			type = DONT_RESOLVE_DLL_REFERENCES;
		} else if (itr->compare(text("/ical")) == 0) {
			type = LOAD_IGNORE_CODE_AUTHZ_LEVEL;
		} else if (itr->compare(text("/ad")) == 0) {
			type = LOAD_LIBRARY_AS_DATAFILE;
		} else if (itr->compare(text("/ade")) == 0) {
			type = LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE;
		} else if (itr->compare(text("/air")) == 0) {
			type = LOAD_LIBRARY_AS_IMAGE_RESOURCE;
		} else if (itr->compare(text("/sad")) == 0) {
			type = /*LOAD_LIBRARY_SEARCH_APPLICATION_DIR*/0x200;
		} else if (itr->compare(text("/sdd")) == 0) {
			type = /*LOAD_LIBRARY_SEARCH_DEFAULT_DIRS*/0x1000;
		} else if (itr->compare(text("/sdld")) == 0) {
			type = /*LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR*/0x100;
		} else if (itr->compare(text("/ss")) == 0) {
			type = /*LOAD_LIBRARY_SEARCH_SYSTEM32*/ 0x800;
		} else if (itr->compare(text("/sud")) == 0) {
			type = /*LOAD_LIBRARY_SEARCH_USER_DIRS*/0x400;
		} else if (itr->compare(text("/sp")) == 0) {
			type = LOAD_WITH_ALTERED_SEARCH_PATH;
		} else if (itr->front() == '/') {
			err  = TEXT("Found unrecognized option '");
			err += *itr + TEXT("'"); return 1;
		} else break;				// Finish option parsing ..
	}
	if (itr == eitr) {
		err = text("The dll name can't be empty."); return 2;
	} else if (itr->front() == '/') {
		err = text("Only one option can be specified."); return 3;
	} else {
		// Here parse the name expression ..
		wchar *dll ; irslt = Expression(
			&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
			Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|NEXPR
		);
		switch (trslt.datatype & EXPR_TYPEMASK) {
		case EXPR_UNICODE: case EXPR_ASCII: 
		case EXPR_TEXT: dll = trslt.value; break;
		default: dll = const_cast<wchar*>(itr->c_str());
		}
		if (type == 0) { if (LoadLibrary(dll) == nullptr) {
			err = text("Internal error that LoadLibrary fail");
			return -1;
		}} else { if (!LoadLibraryEx(dll, nullptr, type)) {
			err = text("Internal error that LoadLibraryEx fail");
			return -1;
		}}
		err  = text("Load '"); err += dll;
		err += text("' successfully.");
	}
	return 0;
}
int CCmdExe::OnOpen(const CCmd& cmd, std::wstring& err)
{
	if (cmd.m_vetArgv.size() > 1) {
		err = TEXT("Not support parameters yet !");
	} else {
		Open(); err = TEXT("Select a file to debug !");
	} return 0;
}
int CCmdExe::OnShut(const CCmd& cmd, std::wstring& err)
{
	std::vector<std::wstring>::const_iterator itr, eitr;
	eitr = cmd.m_vetArgv.end();
	itr = std::find(cmd.m_vetArgv.begin(), eitr, TEXT("/r"));
	if (itr == eitr) { Close();
		err = TEXT("Close the debugged file successfully !");
	} else { Restart();
		err = TEXT("Restart debugged file successfully !");
	} return 0;
}
int CCmdExe::OnStop(const CCmd& cmd, std::wstring& err)
{
	Pause(); err = text("Stop it successfully !"); return 0;
}
int CCmdExe::OnQuit(const CCmd& cmd, std::wstring& err)
{
	Exit(); err = TEXT("Exiting ......"); return 0;
}


// Display Memory Command Handler .
int CCmdExe::OnDump(const CCmd& cmd, std::wstring& err)
{
	t_result trslt = {0};
	t_dump* dump = GetCpuHexDump(); 
	int irslt = 0; ulong type = 0; ulong addr = 0;
	std::vector<std::wstring>::const_iterator itr, eitr;
	if (dump == nullptr) {
		err = text("Internal error that Getcpudisasmdump fail");
		return -1;
	}

	// Dump commands ... 'switch-case' is faster .
	if (cmd.m_strCmds.compare(text("db")) == 0) type = DUMP_HEXA16;
	else if (cmd.m_strCmds.compare(text("dw")) == 0) type = DUMP_IHEX16;
	else if (cmd.m_strCmds.compare(text("dd")) == 0) type = DUMP_IHEX32;
	else if (cmd.m_strCmds.compare(text("df")) == 0) type = DUMP_FLOAT32;
	else if (cmd.m_strCmds.compare(text("dp")) == 0) type = DUMP_ADDR;
	else if (cmd.m_strCmds.compare(text("da")) == 0) type = DUMP_ASC64;
	else if (cmd.m_strCmds.compare(text("du")) == 0) type = DUMP_UNI32;
	else { err = text("This is an unrecognized command"); return 1; }

	// Parse options ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->compare(text("/s")) == 0) {
			if ((type & DU_TYPEMASK) == DU_IHEX) {
				type &= ~DU_TYPEMASK; type |= DU_INT;
			} else {
				err = text("Found unrecognized option '");
				err += *itr + text("'"); return 2;
			}
		} else if (itr->compare(text("/us")) == 0) {
			if ((type & DU_TYPEMASK) == DU_IHEX) {
				type &= ~DU_TYPEMASK; type |= DU_UINT;
			} else {
				err = text("Found unrecognized option '");
				err += *itr + text("'"); return 3;
			}
		} else if (itr->front() == '/') {
			err  = text("Found unrecognized option '");
			err += *itr + text("'"); return 4;
		} else break;					// Finish option parsing ..
	}

	// Parse address ..
	if (itr == eitr) {
		err = TEXT("Please type in the address to dump"); return 5;
	}
	irslt = Expression(
		&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
		Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
	);
	if (trslt.datatype & EXPR_DWORD) { 
		addr = trslt.u; ++itr;	// ............................
	} else {
		err  = text("'"); err += *itr + text("' ");
		err += text("is an invalid address expression"); return 6;
	}

	// Parse the count that dump window display per line ..
	if (itr != eitr) {
		irslt = Expression(
			&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
			Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
		);
		if (trslt.datatype & EXPR_DWORD) { ++itr;	// ...
			if (trslt.u > 255) {
				err  = text("The value of '"); err += *itr + text("' ");
				err += text("is larger than 255"); return 7;
			}
		} else {
			err  = text("'"); err += *itr + text("' ");
			err += text("is an invalid count expression"); return 8;
		}
		type &= ~DU_COUNTMASK; type |= (trslt.u << 8) & DU_COUNTMASK;
	}

	// Now dumping ..
	Setcpu(0, 0, addr, 0, 0, CPU_DUMPHIST|CPU_DUMPFOCUS);
	Setdumptype(dump, type); 

	// Check if it still has arguments, if so, warn the user ..
	if (itr != eitr) {
		err  = text("Too many arguments, '");
		err += *itr + text("' and arguments behind it will be ignore");
	} else err = text("Dump memory successfully"); return 0;
}
// Software Breakpoint Command Handler .
int CCmdExe::OnSwbp(const CCmd& cmd, std::wstring& err)
{
	ulong type = BP_MANUAL | BP_BREAK;
	wchar *condition = nullptr; CCmdBp ccmd;
	t_result trslt = {0}; int irslt, limit = 2;
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse options ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->compare(TEXT("/ps")) == 0) {
			type |= BP_PERIODICAL;	// Pauses each passcount
		} else if (itr->compare(text("/t")) == 0) {
			++ itr;					// The thread id value ..
			if (itr == eitr || itr->front() == '/') {
				err = text("'/t' must specify a thread ID"); return -1;
			}
			// Parse the thread id value expression ..
			irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				wchar buffer[2 * 32] = {0};
				swprintf_s(buffer, _countof(buffer), text("%lu"), trslt.u);
				ccmd.m_vetArgv.push_back(*(itr-1));
				ccmd.m_vetArgv.push_back(std::wstring(buffer));
			} else {
				err  = TEXT("Invalid thread ID value expression '");
				err += *itr + TEXT("'"); return 1;
			}
		} else if (itr->compare(TEXT("/s")) == 0) {
			++ itr;					// The skipping count ..
			if (itr == eitr || itr->front() == '/') {
				err = TEXT("'/s' must specify a skipping count");
				return 2;
			}
			// Parse the count expression ..
			irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) limit += trslt.u;
			else {
				err  = TEXT("Invalid skipping count value expression '");
				err += *itr + TEXT("'"); return 3;
			}
		} else if (itr->compare(TEXT("/c")) == 0) {
			++ itr; type &= ~BP_BREAK; type |= BP_CONDBREAK;
			if (itr == eitr || itr->front() == '/') {
				err = TEXT("'/c' must specify a conditional expression");
				return 4;
			}
			// Parse the conditional expression ..
			uchar cexpr[TEXTLEN] = {0}; wchar msg[TEXTLEN] = {0};
			irslt = Cexpression (
				const_cast<wchar*>(itr->c_str()), cexpr, sizeof(cexpr),
				nullptr, msg, EMOD_CHKEXTRA|EMOD_MULTI
			);
			if (itr->length() == irslt) {
				condition = const_cast<wchar_t*>(itr->c_str());
			} else {
				err  = TEXT("Invalid conditional expression '");
				err += *itr + TEXT("'"); return 5;
			}
		} else if (itr->front() == '/') {
			err  = TEXT("Found unrecognized option '");
			err += *itr + TEXT("'"); return 6;
		} else break;					// Finish option parsing ..
	}

	// Parse the address expression ...
	if (itr == eitr) {
		err = TEXT("Please type in the int3 breakpoint address"); 
		return 1;
	}
	irslt = Expression(
		&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
		Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
	);
	if (trslt.datatype & EXPR_DWORD) { ++itr;	// ...
	} else {
		err  = text("'"); err += *itr + text("' ");
		err += text("is an invalid address expression"); return 1;
	}
	// Now set the int3 breakpoint ...
	Addtolist(1, 1, text("Set int 3 breakpoint at:%x"), trslt.u);
	if (Setint3breakpoint(
		trslt.u, type, 0, limit, 0, BA_PLUGIN|BA_PERMANENT, condition, 
		nullptr, nullptr	// Disable the logging expression ..
	) != 0) {
		err = TEXT("Internal error that Setint3breakpoint fail!");
		return -1;
	}

	// Insert to callback execution queue if the caller specify 
	// a callback expression.
	if (itr != eitr) { ccmd.m_strFull = *itr; ++itr; }
	if (!ccmd.empty()) {
		ccmd.m_ulCmdId = cmd.m_ulCmdId; setbusy(cmd.m_ulCmdId);
		irslt = AddBpCb(trslt.u, BPTYPE_INT3, ccmd);
		if (irslt != 0) {
			err  = TEXT("Fail to set callback expression, '"); 
			err += *itr + text("'") + text(" won't be executed");
			return 0;
		}
	} else DelBpCb(trslt.u, BPTYPE_INT3);

	// Check if it still has arguments, if so, warn the user ..
	if (itr != eitr) {
		err  = text("Too many arguments, '");
		err += *itr + text("' and arguments behind it will be ignore");
	} else if (condition == nullptr) {
		err = TEXT("Set the int3 breakpoint successfully"); 
	} else err = text("Set the conditional int3 breakpoint successfully");

	return 0;
}
// Memory Breakpoint Command Handler .
int CCmdExe::OnMmbp(const CCmd& cmd, std::wstring& err)
{
	ulong addr = 0; ulong size = 0;
	ulong type = BP_MANUAL | BP_BREAK;
	wchar *condition = nullptr; CCmdBp ccmd;
	t_result trslt = {0}; int irslt, limit = 2;
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse options ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		// Using 'switch-case' is faster ....
		if (itr->compare(TEXT("/ps")) == 0) {
			type |= BP_PERIODICAL;	// Pauses each passcount
		} else if (itr->compare(text("/r")) == 0) {
			type |= BP_READ;
		} else if (itr->compare(text("/w")) == 0) {
			type |= BP_WRITE;
		} else if (itr->compare(text("/e")) == 0) {
			type |= BP_EXEC;
		} else if (itr->compare(text("/t")) == 0) {
			++ itr;					// The thread id value ..
			if (itr == eitr || itr->front() == '/') {
				err = text("'/t' must specify a thread ID"); return -1;
			}
			// Parse the thread id value expression ..
			irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				wchar buffer[2 * 32] = {0};
				swprintf_s(buffer, _countof(buffer), text("%lu"), trslt.u);
				ccmd.m_vetArgv.push_back(*(itr-1));
				ccmd.m_vetArgv.push_back(std::wstring(buffer));
			} else {
				err  = TEXT("Invalid thread ID value '");
				err += *itr + TEXT("'"); return 1;
			}
		} else if (itr->compare(TEXT("/s")) == 0) {
			++ itr;					// The skipping count ..
			if (itr == eitr || itr->front() == '/') {
				err = text("'/s' must specify a skipping count");
				return 2;
			}
			// Parse the count expression ..
			irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) limit += trslt.u;
			else {
				err  = text("Invalid skipping count value '");
				err += *itr + text("'"); return 3;
			}
		} else if (itr->compare(text("/c")) == 0) {
			++ itr; type &= ~BP_BREAK; type |= BP_CONDBREAK;
			if (itr == eitr || itr->front() == '/') {
				err = text("'/c' must specify a conditional expression");
				return 4;
			}
			// Parse the conditional expression ..
			uchar cexpr[TEXTLEN] = {0}; wchar msg[TEXTLEN] = {0};
			irslt = Cexpression (
				const_cast<wchar*>(itr->c_str()), cexpr, sizeof(cexpr),
				nullptr, msg, EMOD_CHKEXTRA|EMOD_MULTI
			);
			if (itr->length() == irslt) {
				condition = const_cast<wchar_t*>(itr->c_str());
			} else {
				err  = TEXT("Invalid conditional expression '");
				err += *itr + TEXT("'"); return 5;
			}
		} else if (itr->front() == '/') {
			err  = text("Found unrecognized option '");
			err += *itr + text("'"); return 6;
		} else break;					// Finish option parsing ..
	}

	// The bmx commands ..
	if (cmd.m_strCmds.compare(text("bmr")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_READ; 
	} else if (cmd.m_strCmds.compare(text("bmw")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_WRITE;
	} else if (cmd.m_strCmds.compare(text("bme")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_EXEC;
	} else type |= 0;		// Obviously kidding on you .

	// Parse the address expression ...
	if (itr == eitr) {
		err = TEXT("Please type in the memory breakpoint address");
		return 7;
	}
	irslt = Expression(
		&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
		Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
	);
	if (trslt.datatype & EXPR_DWORD) { addr = trslt.u; ++itr;
	} else {
		err  = text("'"); err += *itr + text("' ");
		err += text("is an invalid address expression"); return 8;
	}
	// If no access flag specifed, set it to memory's access attr
	if ((type & BP_ACCESSMASK) == 0) {
		t_memory* mm = Findmemory(addr);
		switch(mm->access & 0x0FFF) {
			case PAGE_GUARD: case PAGE_NOACCESS: break;
			case PAGE_EXECUTE: type |= BP_EXEC; break;
			case PAGE_READONLY: type |= BP_READ; break;
			case PAGE_WRITECOPY: type |= BP_WRITE|BP_READ; break;
			case PAGE_READWRITE: type |= BP_READ|BP_WRITE; break;
			case PAGE_EXECUTE_READ: type |= BP_EXEC|BP_READ; break;
			case PAGE_EXECUTE_WRITECOPY: type |= BP_EXEC|BP_READ|BP_WRITE; break;
			case PAGE_EXECUTE_READWRITE: type |= BP_EXEC|BP_READ|BP_WRITE; break;
			default: break;
		}
	}
	// Parse the size expression ...
	if (itr == eitr) {
		err = TEXT("Please type in the size of the memory block");
		return 9;
	}
	irslt = Expression(
		&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
		Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
	);
	if (trslt.datatype & EXPR_DWORD) { 
		if (trslt.u <= 0) {
			err  = text("The value of size expression '");
			err += *itr + text("' is zero. The memory block");
			err += text("size can not be zero ."); return 10;
		}	size = trslt.u; ++itr; 
	} else {
		err  = text("'"); err += *itr + text("' ");
		err += text("is an invalid size expression"); return 11;
	}

	// Now set the memory breakpoint ..
	Addtolist(1, 1, text("Set memory breakpoint at:%x"), addr);
	if (Setmembreakpoint(
		addr, size, type, limit, 0, condition, nullptr, nullptr
	) != 0) {
		err = TEXT("Internal error that Setmembreakpoint fail!");
		return -1;
	}

	// Insert to user ipc queue if the caller specify a 
	// callback expression.
	if (itr != eitr) { ccmd.m_strFull = *itr; ++itr; }
	if (!ccmd.empty()) {
		ccmd.m_ulCmdId = cmd.m_ulCmdId; setbusy(cmd.m_ulCmdId);
		if ((irslt=AddBpCb(addr,BPTYPE_MEMORY,ccmd)) != 0) {
			err  = TEXT("Fail to set callback expression, '"); 
			err += *itr + text("'") + text(" won't be executed");
			return 0;
		}
	} else DelBpCb(addr, BPTYPE_MEMORY);

	// Check if it still has arguments, if so, warn the user ..
	if (itr != eitr) {
		err  = text("Too many arguments, '");
		err += *itr + text("' and arguments behind it will be ignore");
	} else if (condition == nullptr) {
		err = TEXT("Set the memory breakpoint successfully"); 
	} else err = text("Set the conditional memory breakpoint successfully");

	return 0;			// All done .....
}
// Hardware Breakpoint Command handler .
int CCmdExe::OnHwbp(const CCmd& cmd, std::wstring& err)
{
	ulong addr = 0; ulong size = 0;
	ulong type = BP_MANUAL | BP_BREAK;
	ulong solt = -1; wchar buffer[2*32];
	wchar *condition = nullptr; CCmdBp ccmd;
	t_result trslt = {0}; int irslt, limit = 2;
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse options ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		// Using 'switch-case' is faster ....
		if (itr->compare(TEXT("/ps")) == 0) {
			type |= BP_PERIODICAL;	// Pauses each passcount
		} else if (itr->compare(text("/a")) == 0) {
			type |= BP_READ|BP_WRITE;
		} else if (itr->compare(text("/w")) == 0) {
			type |= BP_WRITE;
		} else if (itr->compare(text("/e")) == 0) {
			type |= BP_EXEC; size = 1;
		} else if (itr->compare(text("/i")) == 0) {
			++ itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/i' must specify a solt value");
				return 1;
			}
			// Parse the solt value expression of hardware breakpoin
			irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				if (trslt.u >= 4) {
					err  = text("'"); err += *itr + text("' ");
					err += text("is a invalid solt value"); return 1;
				} else solt = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is a invalid solt expression"); return 1;
			}
		} else if (itr->compare(text("/t")) == 0) {
			++ itr; if (itr == eitr || itr->front() == '/') {
				err = text("'/t' must specify a thread ID"); return -1;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				swprintf_s(buffer, _countof(buffer), text("%lu"), trslt.u);
				ccmd.m_vetArgv.push_back(*(itr-1));
				ccmd.m_vetArgv.push_back(std::wstring(buffer));
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is a invalid thread ID expression"); return 1;
			}
		} else if (itr->compare(TEXT("/s")) == 0) {
			++ itr; if (itr == eitr || itr->front() == '/') {
				err = text("'/s' must specify a skipping count"); return 2;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) limit += trslt.u;
			else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is a invalid count expression"); return 1;
			}
		} else if (itr->compare(text("/c")) == 0) {
			++ itr; type &= ~BP_BREAK; type |= BP_CONDBREAK;
			if (itr == eitr || itr->front() == '/') {
				err = text("'/c' must specify a conditional expression");
				return 4;
			} 
			// Parse the conditional expression ..
			uchar cexpr[TEXTLEN] = {0}; wchar msg[TEXTLEN] = {0};
			irslt = Cexpression (
				const_cast<wchar*>(itr->c_str()), cexpr, sizeof(cexpr),
				nullptr, msg, EMOD_CHKEXTRA|EMOD_MULTI
			);
			if (itr->length() == irslt) {
				condition = const_cast<wchar_t*>(itr->c_str());
			} else {
				err  = TEXT("Invalid conditional expression '");
				err += *itr + TEXT("'"); return 5;
			}
		} else if (itr->front() == '/') {
			err  = text("Found unrecognized option '");
			err += *itr + text("'"); return 6;
		} else break;					// Finish option parsing ..
	}

	// The bhx commands ..
	if (cmd.m_strCmds.compare(text("bha")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_READ|BP_WRITE; 
	} else if (cmd.m_strCmds.compare(text("bhw")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_WRITE;
	} else if (cmd.m_strCmds.compare(text("bhe")) == 0) {
		type &= ~BP_ACCESSMASK; type |= BP_EXEC; size = 1;
	} else type |= 0;		// Obviously kidding on you .

	// Parse the address expression ...
	if (itr == eitr) {
		err = TEXT("Please type in the hardware breakpoint address");
		return 7;
	} irslt = Expression(
		&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
		Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
	);
	if (trslt.datatype & EXPR_DWORD) { addr = trslt.u; ++itr;
	} else {
		err  = text("'"); err += *itr + text("' ");
		err += text("is an invalid address expression"); return 8;
	}
	// Parse the size expression ...
	if ((type & BP_EXEC) != BP_EXEC) {// size must be 1 for exec-bp
		if (itr == eitr) {
			err = TEXT("Please type in the size of the memory block");
			return 9;
		}
		irslt = Expression(
			&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
			Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
		);
		if (trslt.datatype & EXPR_DWORD) { 
			size = trslt.u;
			for(irslt=trslt.u&1; trslt.u >>= 1; irslt+=trslt.u&1);
			if (size <= 0) {
				err  = text("The value of size expression '");
				err += *itr + text("' is zero. The block");
				err += text("size can not be zero ."); return 10;
			} else if (size > sizeof(size_t) || irslt != 1) {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid block size"); return 11;
			} else if ((addr&sizeof(size_t))+size > sizeof(size_t)) {
				err  = text("The address '"); err += *itr + text("' ");
				err += text("can't use '"); err += text("' ");
				err += text("as the breakpoint block size"); return 12;
			}	++ itr;
		} else {
			err  = text("'"); err += *itr + text("' ");
			err += text("is an invalid size expression"); return 13;
		}
	}
	// If no access flag specified, set it to memory's access attr
	if ((type & BP_ACCESSMASK) == 0) {
		t_memory* mm = Findmemory(addr);
		switch(mm->access & 0x0FFF) {
			case PAGE_GUARD: case PAGE_NOACCESS: break;
			case PAGE_EXECUTE: type |= BP_EXEC; break;
			case PAGE_READONLY: type |= BP_EXEC; break;
			case PAGE_WRITECOPY: type |= BP_WRITE|BP_READ; break;
			case PAGE_READWRITE: type |= BP_READ|BP_WRITE; break;
			case PAGE_EXECUTE_READ: type |= BP_EXEC; break;
			case PAGE_EXECUTE_WRITECOPY: type |= BP_READ|BP_WRITE; break;
			case PAGE_EXECUTE_READWRITE: type |= BP_READ|BP_WRITE; break;
			default: break;
		}
	}
	// If no solt value specified, find a free solt ..
	if (solt == -1) solt = GetHwBbSolt(addr);
	// Now set the hardware breakpoint ..
	if (Sethardbreakpoint(
		solt, size, type, 0, addr, limit, 0, BA_PLUGIN, condition, 
		nullptr, nullptr	// Disable the logging expression ..
	) != 0) {
		err = TEXT("Internal error that Sethardbreakpoint fail!");
		return -1;
	}

	// Insert to user ipc queue if the caller specify a callback expression.
	if (itr != eitr) { ccmd.m_strFull = *itr; ++itr; }
	if (!ccmd.empty()) {
		ccmd.m_ulCmdId = cmd.m_ulCmdId; setbusy(cmd.m_ulCmdId);
		irslt = AddBpCb(solt, BPTYPE_HARDWARE, ccmd);
		if (irslt != 0) {
			err  = TEXT("Fail to set callback expression, '"); 
			err += *itr + text("'") + text(" won't be executed");
			return 0;
		}
	} else DelBpCb(solt, BPTYPE_HARDWARE);

	// Check if it still has arguments, if so, warn the user ..
	if (itr != eitr) {
		err  = text("Too many arguments, '");
		err += *itr + text("' and arguments behind it will be ignore");
	} else if (condition == nullptr) {
		err = TEXT("Set the hardware breakpoint successfully"); 
	} else err = text("Set the conditional hardware breakpoint successfully");

	return 0;			// All done .....
}

// Application Control Command Handler .
int CCmdExe::OnCtrl(const CCmd& cmd, std::wstring& err)
{
	t_result trslt = {0};
	ulong addr = 0; ulong size = 0; int irslt;
	std::vector<std::wstring>::const_iterator itr, eitr;

	// Parse options ..
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		// Using 'switch-case' is faster ....
		if (itr->front() == '/') {
			err  = text("Found unrecognized option '");
			err += *itr + text("'"); return 1;
		} else break;					// Finish option parsing ..
	}

	// All control command we suported ..
	if (cmd.m_strCmds.compare(text("g")) == 0) {
		if (itr != eitr) {
			irslt = Expression(
				&trslt, const_cast<wchar_t*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) { addr = trslt.u; ++itr;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 2;
			}
		}
		// If user specify an address. we use F4 function ..
		if (addr != 0) ExecHere(addr); else Start();
	} else if (cmd.m_strCmds.compare(text("p")) == 0) {
		StepOver();
	} else if (cmd.m_strCmds.compare(text("pi")) == 0) {
		StepInto();
	} else if (cmd.m_strCmds.compare(text("po")) == 0) {
		StepOver();
	} else {
		err = text("This is an unrecognized command."); return 4;
	} return 0;
}

// App Tracing Control Command Handler .
int CCmdExe::OnTrak(const CCmd& cmd, std::wstring& err)
{
	t_result trslt = {0}; int options = 0;
	ulong addr = 0; ulong size = 0; int irslt;
	t_rtcond *prtc = const_cast<t_rtcond*>(&rtcond);
	std::vector<std::wstring>::const_iterator itr, eitr;
	
	// Parse options ..
	prtc->currcount = 0; prtc->options = 0;
	eitr = cmd.m_vetArgv.cend(); itr = cmd.m_vetArgv.cbegin();
	for (++ itr; itr != eitr; ++ itr) {
		if (itr->compare(text("/n")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/n' must specify a counter"); return 1;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_COUNT; prtc->count = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid counter expression"); return 1;
			}
		} else if (itr->compare(text("/ir")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/ir' must specify a range"); return 2;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_INRANGE; prtc->inrange0 = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 2;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/ir' must specify a range"); return 2;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_INRANGE; prtc->inrange1 = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 3;
			}
		} else if (itr->compare(text("/or")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/or' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_OUTRANGE; prtc->outrange0 = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 4;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/or' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_OUTRANGE; prtc->outrange1 = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 5;
			}
		} else if (itr->compare(text("/m1")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m1' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_MEM1; prtc->memrange0[0] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 6;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m1' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_MEM1; prtc->memrange1[0] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 7;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m1' must specify R/W attr"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD && trslt.u < 3) {
				prtc->options |= RTC_MEM1; prtc->memaccess[0] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid access expression"); return 8;
			}
		} else if (itr->compare(text("/m2")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m2' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_MEM2; prtc->memrange0[1] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 9;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m2' must specify a range"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_MEM2; prtc->memrange1[1] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid address expression"); return 10;
			}
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/m1' must specify R/W attr"); return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD && trslt.u < 3) {
				prtc->options |= RTC_MEM2; prtc->memaccess[1] = trslt.u;
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid access expression"); return 11;
			}
		} else if (itr->compare(text("/c1")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/c1' must specify a expression"); 
				return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_COND1;
				wcscpy_s(prtc->cond[0], TEXTLEN, itr->data());
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid cond expression"); return 12;
			}
		} else if (itr->compare(text("/c2")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/c2' must specify a expression"); 
				return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_COND2;
				wcscpy_s(prtc->cond[1], TEXTLEN, itr->data());
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid cond expression"); return 13;
			}
		} else if (itr->compare(text("/c3")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/c3' must specify a expression"); 
				return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_COND3;
				wcscpy_s(prtc->cond[2], TEXTLEN, itr->data());
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid cond expression"); return 14;
			}
		} else if (itr->compare(text("/c4")) == 0) {
			++itr; if (itr == eitr || itr->front() == '/') {
				err = text("Option '/c4' must specify a expression"); 
				return 4;
			} irslt = Expression(
				&trslt, const_cast<wchar*>(itr->c_str()), nullptr, 0, 0, 
				Getcputhreadid(), 0, 0, EMOD_CHKEXTRA|EMOD_NOVALUE|NEXPR
			);
			if (trslt.datatype & EXPR_DWORD) {
				prtc->options |= RTC_COND4;
				wcscpy_s(prtc->cond[3], TEXTLEN, itr->data());
			} else {
				err  = text("'"); err += *itr + text("' ");
				err += text("is an invalid cond expression"); return 15;
			}
		} else if (itr->front() == '/') {
			err  = text("Found unrecognized command '");
			err += *itr + text("'"); return 16;
 		} else break;					// Finish option parsing ..
	} 

	// Start tracing ..
	if (cmd.m_strCmds.compare(text("t")) == 0) {
		QuitTrace(); OpenTrace(); TraceOver();
	} else if (cmd.m_strCmds.compare(text("ti")) == 0) {
		QuitTrace(); OpenTrace(); TraceInto();
	} else if (cmd.m_strCmds.compare(text("to")) == 0) {
		QuitTrace(); OpenTrace(); TraceOver();
	} else {
		err = text("Found unrecognized command '");
		err += cmd.m_strCmds + text("'"); return 17;
	} 
	err = text("OllyDog is tracing .....") ; return 18;
}

// Information Finding Command Handler .
int CCmdExe::OnFind(const CCmd& cmd, std::wstring& err)
{
	err = text("Not support this command yet"); return -1;
}


int CCmdExe::Exec(const CCmd &cmd, std::wstring &err)
{
	std::wstring tkn; MapCmds::const_iterator itr;
	// Nothing to do if it's an empty command .
	if (cmd.m_strCmds.length() <= 0 || cmd.m_vetArgv.size() <= 0) {
		err = text("Can not execute an empty command!"); 
		return 1;	// return 1 means no command inputed .
	}
	// Nothing to do if it's an unrecognized command.
	itr = m_mapCmds.find(cmd.m_strCmds.c_str());
	if (itr == m_mapCmds.end()) {
		if (GetTkn(cmd.m_strCmds,tkn) != 0) {
			err = text("Fail to parse token ."); return -1;
		} else itr = m_mapCmds.find(tkn.c_str());
		if (itr == m_mapCmds.end()) {
			err = text("This is an unrecognized command ."); 
			return 2;	// return 2 means unrecognized command.
		}
	}
	// Now, Let the command handler handle it.
	return (this->*(itr->second.m_exeFunc))(cmd, err);
}
int CCmdExe::Exec(const std::wstring &cmd, std::wstring &err)
{
	std::vector<CCmd> cmds; int irslt = 0; ulong id = 0;
	// Log the command ...
	if ((id = log(0, 0, CMD_INLINE, cmd)) == 0) {
		err = text("Fail to log message."); return -1;
	}
	// Nothing to do if it's an empty command .
	if (cmd.length() <= 0) {	// return 1 means no command inputed .
		err = text("Please input a command to execute !");
		msg(id, err); set(id, 1, CMD_INLINE, 0); return 1;
	}
	// Now parse the command .
	if ((irslt = GetCmds(cmd, cmds)) != 0) {
		err = text("Fail to parse commands"); msg(id,err);
		set(id, irslt, CMD_INLINE, 0); return irslt;
	}
	// Execute commands ..
	std::vector<CCmd>::iterator itr, eitr;
	eitr = cmds.end(); itr = cmds.begin();
	for (; itr != eitr; ++itr) { 
		itr->m_ulCmdId = id; err.clear(); 
		if ((irslt = Exec(*itr, err)) != 0) {
			err = itr->m_strFull + text("  -->  ") + err; 
			if (cmds.size() > 1) set(id, irslt, CMD_MULTICMD, 0);
			return irslt; 
		} 
	} 
	if (cmds.size() > 1) set(id, irslt, CMD_MULTICMD, 0);
	// ................................
	return 0;
}


int CCmdExe::GetTkn(const std::wstring &cmd, std::wstring &tkn) const
{
	tkn.clear(); tkn.push_back(cmd.front()); return 0;
}
int CCmdExe::GetCmds(const std::wstring &cmd, std::vector<CCmd> &cmds) const
{
	CCmd ccmd;
	bool bQuote = false, bsQuote = false;
	std::wstring::const_iterator itr, eitr, bitr;
	size_t nPos = cmd.find_first_not_of(text(" ")); 
	if (nPos == std::wstring::npos) return 1;
	
	eitr = cmd.cend(); bitr = cmd.cbegin(); bitr += nPos;
	for (itr = bitr, itr += nPos; itr != eitr; ++ itr) {
		// Use "switch-case" is faster ..
		if (*itr == text('\\')) {
			if (itr+1 == eitr) break;
			switch( *++itr ) {
				case 't': ccmd.m_strCmds.push_back('\t'); break;
				case 'n': ccmd.m_strCmds.push_back('\n'); break;
				case 'r': ccmd.m_strCmds.push_back('\r'); break;
				case 'f': ccmd.m_strCmds.push_back('\f'); break;
				case 'v': ccmd.m_strCmds.push_back('\v'); break;
				default : ccmd.m_strCmds.push_back(*itr); break;
			};
		} else if (*itr == text('\'')) {
			if (bQuote) ccmd.m_strCmds.push_back('\''); 
			else bsQuote = bsQuote ? false : true;
		} else if (*itr == text('\"')) {
			if (bsQuote) ccmd.m_strCmds.push_back('\"');
			else bQuote = bQuote ? false : true;
		} else if (*itr == text(' ')) {
			if (!bQuote && !bsQuote) {
				if (ccmd.m_strCmds.length() > 0) {
					// Add new item ..
					ccmd.m_vetArgv.push_back(ccmd.m_strCmds); 
					ccmd.m_strCmds.clear();
				}
				// Find next pos to parse .
				for ( ; *(itr+1) == text(' '); ++itr);
			} else ccmd.m_strCmds.push_back(text(' '));
		} else if (*itr == text(';')) {
			if (!bQuote && !bsQuote) {
				// Add new item ..
				if (ccmd.m_strCmds.length() > 0)
					ccmd.m_vetArgv.push_back(ccmd.m_strCmds); 
				if (ccmd.m_vetArgv.size() > 0) {
					ccmd.m_strFull.assign(bitr, itr-0);
					ccmd.m_strCmds = ccmd.m_vetArgv[0];
					cmds.push_back(ccmd); ccmd.clear();
				}
				// Find next pos to parse .
				for (bitr = itr+1; bitr != eitr; ++bitr, ++itr) {
					if (*bitr != text(' ') && *bitr != text(';')) 
						break;
				}
			} else ccmd.m_strCmds.push_back(text(';'));
		} else ccmd.m_strCmds.push_back(*itr);
	}
	// The last argument ...
	if (ccmd.m_strCmds.length() > 0) {
		ccmd.m_vetArgv.push_back(ccmd.m_strCmds); 
	}
	// Push back the last part if ccmd.m_vetArgv is not empty .
	if (ccmd.m_vetArgv.size() > 0) {
		ccmd.m_strFull.assign(bitr, itr-0);
		ccmd.m_strCmds = ccmd.m_vetArgv[0];
		cmds.push_back(ccmd);
	} return 0;	// Everything is fine, return 0 ...
}
int CCmdExe::GetOpts(const std::wstring &cmd, std::vector<std::wstring> &opt) const
{
	bool bQuote = false, bsQuote = false;
	size_t nPos = 0; size_t nLen = cmd.length();
	const wchar *lpsCmd = cmd.c_str(); std::wstring arg;
	// Nothing to do if it's an empty command .
	if (nLen <= 0) return 1;	// 1 --> Nothing to parse .
	// Here, we parse the command line..
	for(nPos=cmd.find_first_not_of(TEXT(" ")); nPos < nLen; ++nPos)
	{
		// Use "switch-case" is faster ..
		if (lpsCmd[nPos] == '\\') {
			if (nPos+1 >= nLen) break;	// it's already the last char .
			switch( lpsCmd[++nPos] ) {	// We parse these special chars .
				case 't': arg.push_back('\t'); break;
				case 'n': arg.push_back('\n'); break;
				case 'r': arg.push_back('\r'); break;
				case 'f': arg.push_back('\f'); break;
				case 'v': arg.push_back('\v'); break;
				default : arg.push_back(lpsCmd[nPos]); break;
			};
		} else if (lpsCmd[nPos] == '\'') {
			if (bQuote) arg.push_back('\''); 
			else bsQuote = bsQuote ? false : true;
		} else if (lpsCmd[nPos] == '\"') {
			if (bsQuote) arg.push_back('\"');
			else bQuote = bQuote ? false : true;
		} else if (lpsCmd[nPos] == ' ') {
			if (bQuote || bsQuote) arg.push_back(' ');
			else {
				if (arg.length() > 0) {
					// Add new item ..
					opt.push_back(arg); arg.clear();
				}
				// Find next pos to parse .
				nPos = cmd.find_first_not_of(TEXT(" "), nPos)-1;
			}
		} else {
			arg.push_back(lpsCmd[nPos]);
		}
	}
	// Push back the last part if strArg is not empty .
	if (arg.length() > 0) opt.push_back(arg);
	
	return 0;	// Everything is fine, return 0 ...
}
int CCmdExe::GetCmd(const std::wstring& cmd, std::wstring& arg) const
{
	const wchar *lpsCmd = cmd.c_str();
	bool bQuote = false, bsQuote = false;
	size_t nPos = 0; size_t nLen = cmd.length();
	
	// Nothing to do if it's an empty command .
	if (nLen <= 0) return 1;	// 1 --> Nothing to parse .
	// Here, we parse the command line..
	for(nPos=cmd.find_first_not_of(text(" ")); nPos < nLen; ++nPos)
	{
		// Use "switch-case" is faster ..
		if (lpsCmd[nPos] == '\\') {
			if (nPos+1 >= nLen) break;	// it's already the last char .
			switch( lpsCmd[++nPos] ) {	// We parse these special chars .
				case 't': arg.push_back('\t'); break;
				case 'n': arg.push_back('\n'); break;
				case 'r': arg.push_back('\r'); break;
				case 'f': arg.push_back('\f'); break;
				case 'v': arg.push_back('\v'); break;
				default : arg.push_back(lpsCmd[nPos]); break;
			};
		} else if (lpsCmd[nPos] == '\'') {
			if (bQuote) arg.push_back('\''); 
			else bsQuote = bsQuote ? false : true;
		} else if (lpsCmd[nPos] == '\"') {
			if (bsQuote) arg.push_back('\"');
			else bQuote = bQuote ? false : true;
		} else if (lpsCmd[nPos] == ' ') {
			if (bQuote || bsQuote) arg.push_back(' ');
			else break;	// Just parse the first field ..
		} else arg.push_back(lpsCmd[nPos]);
	}	
	return 0;	// Everything is fine, return 0 ...
}

int CCmdExe::GetFilePath(const std::wstring &file, std::wstring &path) const
{
	wchar buffer[MAX_PATH*2] = {0}; DWORD dwRslt = 0; std::wstring tmp;
	dwRslt = GetEnvironmentVariable(text("PATH"), buffer, _countof(buffer));
	if (dwRslt == 0) return -1;
	if (dwRslt <  _countof(buffer)) tmp = buffer; 
	if (dwRslt >= _countof(buffer)) {
		tmp.resize(dwRslt+1);
		dwRslt = GetEnvironmentVariable(
			text("PATH"), const_cast<wchar*>(tmp.data()), tmp.size()
		);
		if (dwRslt == 0 || dwRslt >= tmp.size()) return -1;
		tmp.resize(dwRslt+0); tmp.push_back(text(';'));
	}
	std::size_t pos = tmp.find(text(";"), 0); std::size_t pre = 0;
	for (pos; pos != std::wstring::npos; pos = tmp.find(text(";"), pos)) {
		path.assign(tmp, pre, pos - pre); path = path + text('\\') + file;
		HANDLE hFile = CreateFile (
			path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
		);
		Addtolist(1,1,text("Check Path - %s"),path.c_str());
		if (hFile != INVALID_HANDLE_VALUE) { CloseHandle(hFile); return 0; }
		pos = pos + 1; pre = pos;		// .................................
	}
	return -1;
}
int CCmdExe::GetCmdHelps(const std::wstring& cmd, std::wstring &info) const
{
	MapCmds::const_iterator itr = m_mapCmds.find(cmd.c_str());
	if (itr == m_mapCmds.end()) {
		if (GetTkn(cmd, info) == 0) {
			itr = m_mapCmds.find(info.c_str());
			if (itr != m_mapCmds.end()) {
				info = itr->second.m_strHelp; return 0;
			}
		}
		info = text("Can't not find any help information for ");
		info = info + text("'") + cmd + text("'");
	} else {
		info = itr->second.m_strHelp;
	} return 0;
}
int CCmdExe::GetCmdUsage(const std::wstring& cmd, std::wstring &info) const
{
	MapCmds::const_iterator itr = m_mapCmds.find(cmd.c_str());
	if (itr == m_mapCmds.end()) {
		if (GetTkn(cmd, info) == 0) {
			itr = m_mapCmds.find(info.c_str());
			if (itr != m_mapCmds.end()) {
				info = itr->second.m_strFull; return 0;
			}
		}
		info = text("Can't not find any usage information for ");
		info = info + text("'") + cmd + text("'");
	} else {
		info = itr->second.m_strFull;
	} return 0;
}

int CCmdExe::RelatedCmds(const std::wstring& cmd, std::vector<const wchar*> &cmds) const
{
	std::size_t nLen = cmd.length();
	MapCmds::const_iterator itr, eitr = m_mapCmds.end();
	if (nLen <= 0) return 1;	// 1 means there is no cmd need to handle .
	for(itr = m_mapCmds.begin(); itr != eitr; ++itr) {
		// nLen may longer than the length of itr->first
		// Cross-border access is ok here, it just compare data ...
		if (cmd.compare(0, nLen, itr->first, nLen) == 0)
			cmds.push_back(itr->first);
	}
	return 0;		// 0 means success ..
}

int CCmdExe::Update(void)
{
	std::map<ulong, CCmdBp>::const_iterator itr, eitr;
	
	EnterCriticalSection(&this->m_csShare);
	
	itr = this->m_mapSwBp.cbegin(); eitr = this->m_mapSwBp.cend();
	for (; itr != eitr; ++ itr) {
		if (GetSwBp(itr->first) == nullptr) {
			this->m_mapSwBp.erase(itr);
		}
	}
	itr = this->m_mapMmBp.cbegin(); eitr = this->m_mapMmBp.cend();
	for (; itr != eitr; ++ itr) {
		if (GetMmBp(itr->first) == nullptr) {
			this->m_mapMmBp.erase(itr);
		}
	}
	itr = this->m_mapHwBp.cbegin(); eitr = this->m_mapHwBp.cend();
	for (; itr != eitr; ++ itr) {
		if (GetHwBp(itr->first) == nullptr) {
			this->m_mapHwBp.erase(itr);
		}
	}
	
	LeaveCriticalSection(&this->m_csShare);
	
	return 0;
}


