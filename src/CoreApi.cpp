

#include "CoreApi.hpp"

////////////////////////////////////////////////////////////////////////////////////////

HANDLE							CCoreApi::m_hExeMap = nullptr; 
void *							CCoreApi::m_pBuffer = nullptr; 
size_t							CCoreApi::m_nReffer = 0;
ulong							CCoreApi::m_uStatus = COREAPI_INVALID;
CCoreApi::MapExps				CCoreApi::m_expList;
CCoreApi::MapNtfs				CCoreApi::m_ntfList;

////////////////////////////////////////////////////////////////////////////////////////

CCoreApi::CCoreApi() {
	if (m_uStatus == COREAPI_INVALID)
		if (InitCore() == true) m_uStatus = COREAPI_WORKING;
	++ m_nReffer;
}
CCoreApi::~CCoreApi() {
	if ( --m_nReffer <= 0 ) {
		FreeCore(); m_uStatus = COREAPI_WORKING;
	}
}

bool CCoreApi::InitCore()
{
	bool brslt = true;
	wchar path[MAX_PATH*2]; DWORD nRslt; HANDLE hFile; 
	if (m_hExeMap && m_hExeMap!=INVALID_HANDLE_VALUE && m_pBuffer)
		return true;
		
	if (wcsnlen_s(ollyfile, MAXPATH) <= 0) {
		nRslt = ::GetModuleFileName(NULL, path, _countof(path));
		if (nRslt==0 && GetLastError()!=ERROR_INSUFFICIENT_BUFFER)
			return false; // According to MSDN, it shouldn't happen.
		if (nRslt>=_countof(path) || GetLastError()==ERROR_INSUFFICIENT_BUFFER)
			return false;	// The buffer is not enough. Invalid path .
		if ( (hFile = CreateFile (
			path,
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		)) == INVALID_HANDLE_VALUE ) return false;
	} else {
		if ( (hFile = CreateFile (
			ollyfile,
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		)) == INVALID_HANDLE_VALUE ) return false;
	}
	
	do {
		
		if (m_hExeMap == nullptr || m_hExeMap == INVALID_HANDLE_VALUE) {
			if ( (m_hExeMap = CreateFileMapping (
				hFile, nullptr, PAGE_READONLY, 0, 0, nullptr
			)) == nullptr ) {
				Addtolist(		// .................
					reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
					text("Fail to create file mapping for ollydbg's main file.")
				);	brslt = false; break;
			}
		}
		
		if (m_pBuffer == nullptr) {
			if ( (m_pBuffer = MapViewOfFile(
				m_hExeMap, FILE_MAP_READ, 0, 0, 0
			)) == nullptr ) {
				Addtolist(		// .................
					reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
					text("Fail to map view of ollydbg's main file.")
				);	brslt = false; break;
			}
		}
		
	} while (false); CloseHandle(hFile); return brslt;
}
void CCoreApi::FreeCore()
{
	if (m_pBuffer != nullptr) {
		Addtolist(		// .................
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Unmap view of ollydbg's main file.")
		);
		UnmapViewOfFile(m_pBuffer); m_pBuffer = nullptr;
	}
	if (m_hExeMap && m_hExeMap != INVALID_HANDLE_VALUE) {
		Addtolist(		// .................
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Close the mapping of ollydbg's main file.")
		);
		CloseHandle(m_hExeMap); m_hExeMap = nullptr;
	}
}


void* CCoreApi::GetRetAddr()
{
	void * addr = nullptr; addr = &addr + 2;
	return *reinterpret_cast<void**>(addr);
}


int CCoreApi::Exit(bool sync)
{
	DestroyWindow(hwollymain); return 0;
}
int CCoreApi::Open(bool sync)
{
	PostMessage(hwollymain, WM_COMMAND, 2101, 0); return 0;
}
int CCoreApi::Close(bool sync)
{
	if (sync) return Closeprocess(0);
	else PostMessage(hwollymain, WM_COMMAND, 2310, 0); return 0;
}
int CCoreApi::Start(bool sync)
{
	if (sync) return Run(STAT_RUNNING, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2301, 0); return 0;
}
int CCoreApi::Pause(bool sync)
{
	if (sync) return Pauseprocess();
	else PostMessage(hwollymain, WM_COMMAND, 2302, 0); return 0;
}
int CCoreApi::Restart(bool sync)
{
	PostMessage(hwollymain, WM_COMMAND, 2309, 0); return 0;
}

int CCoreApi::Startup(wchar* file, wchar* arg, wchar* dir)
{
	typedef int (__cdecl *_StartupDebugFile)(wchar*, wchar*, wchar*);
	static _StartupDebugFile fnStartupDebugFile = nullptr;	
	if (fnStartupDebugFile == nullptr) {
		if (InitCore() == false) return false;
		unsigned char code[] = {
			0x55, 0x8B, 0xEC, 0x50, 0xB8, 0x02, 0x00, 0x00, 
			0x00, 0x81, 0xC4, 0x04, 0xF0, 0xFF, 0xFF, 0x50,
			0x48, 0x75, 0xF6, 0x8B, 0x45, 0xFC, 0x81, 0xC4, 
			0x14, 0xF8, 0xFF, 0xFF, 0x53, 0x56, 0x57, 0x8B,
			0x5D, 0x08, 0xBF, 0xD4, 0x20, 0x53, 0x00, 0x85, 
			0xDB, 0x75, 0x08, 0x83, 0xC8, 0xFF, 0xE9, 0x8A,
			0x0B, 0x00, 0x00, 0x33, 0xD2, 0x89, 0x55, 0xFC, 
			0x53, 0x68, 0x04, 0x01, 0x00, 0x00, 0x8D, 0x8D
		};
		fnStartupDebugFile = reinterpret_cast<_StartupDebugFile>(
			FindApiAddrByCode(code, _countof(code))
		);
		if (fnStartupDebugFile == nullptr) return false;
	}
	Addtolist(1,1,TEXT("Find the address:%x"),fnStartupDebugFile);
	return fnStartupDebugFile(file, arg, dir) == 0;
}

int CCoreApi::ClearLog()
{
	for (int i=0; i<10; ++i) {
		t_menu *tmenu = list.menu + i;
		if (tmenu->shortcutid == K_LOGCLEAR) {
			Callmenufunction(
				const_cast<t_table*>(&list), 
				tmenu, tmenu->menufunc, 0
			);
			return 0; 
		}
	}
	return -1;
}


int CCoreApi::StepInto(bool sync)
{
	if (sync) return Run(STAT_STEPIN, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2304, 0); return 0;
}
int CCoreApi::StepOver(bool sync)
{
	if (sync) return Run(STAT_STEPIN, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2305, 0); return 0;
}
int CCoreApi::TillRetn(bool sync)
{
	if (sync) return Run(STAT_OVERRET, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2306, 0); return 0;
}
int CCoreApi::TillUser(bool sync)
{
	if (sync) return Run(STAT_TILLUSER, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2307, 0); return 0;
}
int CCoreApi::ExecHere(ulong addr, bool sync)
{
	int irslt = Setint3breakpoint(
		addr, BP_EXEC|BP_ONESHOT|BP_BREAK, 0, 0, 0, 0, 
		nullptr, nullptr, nullptr
	); if (irslt != 0) return irslt;
	if (sync) return Run(STAT_RUNTHR, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2303, 0); return 0;
}


int CCoreApi::OpenTrace(bool sync)
{
	PostMessage(hwollymain, WM_COMMAND, 2401, 0); return 0;
}
int CCoreApi::QuitTrace(bool sync)
{
	PostMessage(hwollymain, WM_COMMAND, 2402, 0); return 0;
}
int CCoreApi::TraceInto(bool sync)
{
	if (sync) return Run(STAT_TRACEIN, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2405, 0); return 0;
}
int CCoreApi::TraceOver(bool sync)
{
	if (sync) return Run(STAT_TRACEOVER, 0);
	else PostMessage(hwollymain, WM_COMMAND, 2406, 0); return 0;
}


t_bpoint* CCoreApi::GetSwBp(ulong addr)
{
	return reinterpret_cast<t_bpoint*>(
		Findsorteddata(const_cast<t_sorted*>(&bpoint.sorted), addr, 0)
	); 
}
t_bpmem*  CCoreApi::GetMmBp(ulong addr)
{
	return reinterpret_cast<t_bpmem*>(Findsorteddatarange(
		const_cast<t_sorted*>(&bpmem.sorted), addr, addr+1
	));
}
t_bphard* CCoreApi::GetHwBp(ulong solt)
{
	return reinterpret_cast<t_bphard*>(
		Findsorteddata(const_cast<t_sorted*>(&bphard.sorted), solt, 0)
	); 
}
int CCoreApi::GetHwBbSolt(ulong addr)
{
	ulong npos, solt;
	for (npos = 0, solt = -1; npos < NHARD; ++npos) {
		t_bphard* bphw = GetHwBp(solt);
		if (bphw == nullptr) solt = npos;
		else if (bphw->addr == addr) return npos;
	} return solt;
}

t_dump* CCoreApi::GetCpuHexDump()
{
	return reinterpret_cast<t_dump*>(
		reinterpret_cast<uchar*>(hollyinst) + 0x1E6F90
	);
}

int CCoreApi::DispatchException(t_run* trun, const t_disasm* disasm, t_thread* thread)
{
	CCoreApi::MapExps::iterator itr, eitr;
	int iRslt = PE_IGNORED; bool bRslt = false; 
	eitr = m_expList.end(); itr = m_expList.begin();
	for (; itr != eitr; ++itr) {
		if (itr->first == nullptr) continue;
		bRslt = itr->first(trun, disasm, thread, iRslt, itr->second);
		if (bRslt == false) break;
	}
	return iRslt;
}
int CCoreApi::DispatchNotify(int code, void* data, ulong parm1, ulong parm2)
{
	int iRslt = 0; bool bRslt = false;
	CCoreApi::MapNtfs::iterator itr, eitr;
	eitr = m_ntfList.end(); itr = m_ntfList.begin();
	for (; itr != eitr; ++itr) {
		if (itr->first == nullptr) continue;
		bRslt = itr->first(code, data, parm1, parm2, itr->second);
		if (bRslt == false) return iRslt;
	}
	return iRslt;
}

bool CCoreApi::AddExceptionCallback(_Exception exp, void* parm)
{
	if (exp == nullptr) return false;
	m_expList.insert(CCoreApi::MapExps::value_type(exp, parm)); 
	return true;
}
bool CCoreApi::AddNotifyCallback(_Notify ntf, void* parm)
{
	if (ntf == nullptr) return false;
	m_ntfList.insert(CCoreApi::MapNtfs::value_type(ntf, parm));
	return true;
}
bool CCoreApi::DelExceptionCallback(_Exception exp)
{
	if (exp == nullptr) return false;
	m_expList.erase(exp); return true;
}
bool CCoreApi::DelNotifyCallback(_Notify ntf)
{
	if (ntf == nullptr) return false;
	m_ntfList.erase(ntf); return true;
}


MENUFUNC* CCoreApi::SetTableMenuHook(t_table* table, int id, ulong index, MENUFUNC* fn)
{
	t_menu *tmenu = list.menu + 0; MENUFUNC *pfn = nullptr;
	if (index != -1) {
		pfn = tmenu[index].menufunc; tmenu[index].menufunc = fn;
		return pfn;
	}
	for (; tmenu->name; ++tmenu) {
		if (tmenu->shortcutid == id) {
			pfn = tmenu->menufunc; tmenu->menufunc = fn; return pfn;
		}
	} return nullptr; 
}


////////////////////////////////////////////////////////////////////////////////


void* CCoreApi::FindApiAddrByCode(uchar* code, size_t size)
{
	PIMAGE_NT_HEADERS pHeader = nullptr;
	PIMAGE_SECTION_HEADER pSection = nullptr;
	if (InitCore() == false) return nullptr;
	if ((pHeader = (PIMAGE_NT_HEADERS)m_pBuffer) != nullptr) {
		for (uint i=0; i < pHeader->FileHeader.NumberOfSections; ++i)
		{
			pSection = IMAGE_FIRST_SECTION(pHeader);
			size_t pos = pSection->VirtualAddress;
			pos = Sunday(
				pos + reinterpret_cast<uchar*>(m_pBuffer),
				pSection->Misc.VirtualSize, code, size
			);
			if (pos != -1) {	//  We got a result ..
				uchar *p = reinterpret_cast<uchar*>(hollyinst);
				p += pSection->VirtualAddress + pos;
				return reinterpret_cast<void*>(p);
			}
			++ pSection;		// Next section ..
		}
	}
	return nullptr;
}
size_t CCoreApi::Sunday(uchar *szSource, size_t sLen, uchar *szPattern, size_t pLen)
{
	#define MAX_CHAR_COUNT 0x100    // It's not necessary .
	size_t sitNext[MAX_CHAR_COUNT] = { 0 }; size_t i = 0, j = 0;
	// Simply check the parameters .
	if(!szSource || !sLen || !szPattern || !pLen || pLen>sLen) return -1;
	// Initialize the 'next' array .
	for(i = 0; i < MAX_CHAR_COUNT; ++i)  sitNext[i] = pLen + 1;
	for(i = 0; i < pLen; ++i) sitNext[szPattern[i]] = pLen - i;
	// Now, Begin to search .
	for(i = 0; i+pLen <= sLen; ) {
		for(j = 0; j < pLen; ++j) {
			// Find difference, move the pattern string .
			if(szSource[i+j] != szPattern[j]) {
				// return -1 if it crossed the boundary .
				if(i+pLen >= sLen) return -1;
				// This means moving the pattern string .
				i += sitNext[szSource[i+pLen]];
				break;// exit this round after moved it .
			}
		}
		if(j == pLen) return i;  // All matched, return i.
	}
	return -1;   // Can't find a match, return -1 .
}
bool CCoreApi::DosPathNameToNtPathName(wchar* ntpath, unsigned short count, const wchar* dos)
{
	typedef struct _UNICODE_STRING {
		unsigned short Length; unsigned short MaximumLength; wchar *Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;
	
	typedef bool (__stdcall *_RtlDosPathNameToNtPathName)
	(const wchar*, PUNICODE_STRING, const wchar**, void*);
	typedef void (__stdcall *_RtlCopyUnicodeString)
	(PUNICODE_STRING DestinationString, PUNICODE_STRING SourceString);
	typedef void (__stdcall *_RtlFreeUnicodeString)
	(PUNICODE_STRING UnicodeString);

	static _RtlDosPathNameToNtPathName fnDosNameToNtName = nullptr;
	static _RtlCopyUnicodeString fnCopyUnicodeString = nullptr;
	static _RtlFreeUnicodeString fnFreeUnicodeString = nullptr;

	HMODULE hModule = GetModuleHandle(TEXT("Ntdll.dll"));
	if (hModule == nullptr) return false;
	if (fnDosNameToNtName == nullptr) {
		fnDosNameToNtName = reinterpret_cast<_RtlDosPathNameToNtPathName>(
			GetProcAddress(hModule, "RtlDosPathNameToNtPathName_U")
		); if (fnDosNameToNtName == nullptr) return false;
	} 
	if (fnCopyUnicodeString == nullptr) {
		fnCopyUnicodeString = reinterpret_cast<_RtlCopyUnicodeString>(
			GetProcAddress(hModule, "RtlCopyUnicodeString")
		); if (fnCopyUnicodeString == nullptr) return false;
	}
	if (fnFreeUnicodeString == nullptr) {
		fnFreeUnicodeString = reinterpret_cast<_RtlFreeUnicodeString>(
			GetProcAddress(hModule, "RtlFreeUnicodeString")
		); if (fnFreeUnicodeString == nullptr) return false;
	} 
	if (count==0 || ntpath==0) return false;
	
	UNICODE_STRING usNt = {0,count,ntpath}; UNICODE_STRING usDos= {0};
	bool bRslt = fnDosNameToNtName(dos, &usDos, nullptr, nullptr);
	if (bRslt == true) {
		fnCopyUnicodeString(&usNt, &usDos); fnFreeUnicodeString(&usDos);
	} return bRslt;
}


