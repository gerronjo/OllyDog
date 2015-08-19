
#include "CmdLog.hpp"



////////////////////////////////////////////////////////////////////////////////////////

// Security version for memset ..
errno_t memset_s(void* dst, size_t max, int val, size_t size) {
	memset(dst, val, max > size ? size : max);
	if (max < size) return -1;
	return 0;
} 

////////////////////////////////////////////////////////////////////////////////////////

HANDLE	CCmdLog::m_hPipeCb = INVALID_HANDLE_VALUE;	// The pipe thread handle.
DWORD	CCmdLog::m_dPipeCb = 0;						// The pipe thread id .
HANDLE	CCmdLog::m_hPipeRd = nullptr;				// The pipe handle to read message ..
HANDLE	CCmdLog::m_hPipeWr = nullptr;				// The pipe handle to write message .
HANDLE CCmdLog::m_hNotify = nullptr;

CRITICAL_SECTION CCmdLog::m_csShare = {0};			// The critical section object .

size_t CCmdLog::m_nReffer = 0;						// The refference counter ..
ulong	CCmdLog::m_uStatus = CMDLOG_INVALID;
ulong	CCmdLog::m_nNextId = 1;						// ..

ulong	 CCmdLog::m_nRows  = 0;
t_table CCmdLog::m_Table  = {0};
t_menu* CCmdLog::m_pMenu  = nullptr;

////////////////////////////////////////////////////////////////////////////////////////

CCmdLog::CCmdLog()
{
	if (m_nReffer <= 0) {
		static t_menu tmenu[] = {	// It's the menu our logger uses .
			{// KK_DIRECT | KK_CTRL | 'Z'
				TEXT("Clear logger"), 
				TEXT("Empty log winow. (doesn't influence log file)"),  
				K_LOGCLEAR, CCmdLog::MenuProc, nullptr, MENU_ITEM_CLEAR },
			{// KK_DIRECT | KK_CTRL | 'S'
				TEXT("Save logger to file"),
				TEXT("Start writing new log data to file."),
				K_NONE, CCmdLog::MenuProc, nullptr, MENU_ITEM_STORE },
			{// KK_DIRECT | KK_CTRL | 'C'	- No shortcut ..
				TEXT("Copy to the clipboard"),
				TEXT("Copy logger data to the clipboard."),
				K_NONE, CCmdLog::MenuProc, nullptr, MENU_ITEM_GROUP },
			{// KK_DIRECT | KK_CTRL | VK_F1	- No shortcut ..
				TEXT("|>STANDARD"),
				TEXT("Forwarder to standard menus"),
				K_NONE, CCmdLog::MenuProc, nullptr, MENU_ITEM_GROUP },
			{ nullptr, nullptr, K_NONE, nullptr, nullptr, MENU_ITEM_EMPTY },
			
			{// KK_DIRECT | KK_CTRL | 'C'
				TEXT("The whole line"),
				TEXT("Copy the selected line to clipboard."),
				K_COPY, CCmdLog::MenuProc, nullptr, MENU_ITEM_CPYLN },
			{// KK_DIRECT | KK_CTRL | 'C'	- No shortcut ..
				TEXT("The whole table"),
				TEXT("Copy the the whole table to clipboard, preserving column widths"),
				K_COPYALL, CCmdLog::MenuProc, nullptr, MENU_ITEM_CPYTB },
			{// KK_DIRECT | KK_CTRL | 'C'	- No shortcut ..
				TEXT("The whole logger"),
				TEXT("Copy the selected logger to clipboard."),
				K_NONE, CCmdLog::MenuProc, nullptr, MENU_ITEM_CPYLG },
			{ nullptr, nullptr, K_NONE, nullptr, nullptr, MENU_ITEM_EMPTY }
		};	tmenu[2].submenu = tmenu + 5;
		// Initialize our class member ..
		m_pMenu = tmenu + 0; m_nRows = 0; 
		memset(&m_Table, 0, sizeof(m_Table));
	}
	if (m_uStatus == CMDLOG_INVALID)
		if (InitLog() == true) m_uStatus = CMDLOG_WORKING;
	++ m_nReffer;			// Increment the counter ...
}
CCmdLog::~CCmdLog() 
{
	if (--m_nReffer <= 0) {
		FreeLog(); m_uStatus = CMDLOG_INVALID;
	}
}


bool CCmdLog::InitLog()
{
	// Create sorted data ..
	if (Issortedinit(&(m_Table.sorted)) == 0) {
		if ( Createsorteddata (
			&(m_Table.sorted),				// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			SDM_NOSIZE|SDM_INDEXED
		) != 0 ) {
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("Fail to create logger sorted data .")
			); return false;
		} else InitializeCriticalSection(&m_csShare);
	}

	// Initialize pipe logging system ..
	if (initcon() == 0) {
		Addtolist(		// We don't care if it success ...
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_NORMAL, 
			text("Fail to initialize pipe connect")
		); return false;
	}

	// Create the logger window ..
	if ( Create() == false ) {
		Addtolist(		// We don't care if it success ...
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_NORMAL, 
			text("Fail to create the logger window")
		);
	}

	// All done ....
	return true;
}
void CCmdLog::FreeLog()
{
	Destroy();		// Destory the log window first .
	freecon();		// Destory pipe logging system ..
	Destroysorteddata(&m_Table.sorted);
	DeleteCriticalSection(&m_csShare);
}


bool CCmdLog::create()
{
	if (m_Table.hw != nullptr) return true;
	if ( Createtablewindow(
		&m_Table, 0, m_Table.bar.nbar, NULL, TEXT("ICO_L"), LOGNAME
	) == nullptr ) {
		Addtolist(		// .................
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Fail to create logger window .")
		);	return false;
	} else return true;
}
void CCmdLog::destroy()
{
	
}


bool CCmdLog::Create()
{
	::wcscpy_s(m_Table.name, SHORTNAME, LOGNAME);	// name .
	m_Table.mode = TABLE_SAVEALL|TABLE_USERDEF|TABLE_RIGHTSEL;
	m_Table.subtype			= 0;	// not figure out yet ..
	m_Table.bottomspace		= m_Table.minwidth = 0;
	m_Table.tabfunc			= CCmdLog::TableProc;
	m_Table.drawfunc			= CCmdLog::DrawProc;
	//m_Table.updatefunc		= CCmdLog::UpdateProc;
	//m_Table.tableselfunc		= CCmdLog::TabSelProc;
	m_Table.menu				= m_pMenu + 0;
	m_Table.custommode		= 0;
	m_Table.customdata		= this;
	// Now, initialize the t_bar ..
	m_Table.bar.nbar			= 5;
	m_Table.bar.visible		= 1;
	m_Table.bar.name[0]		= TEXT("No");
	m_Table.bar.expl[0]		= TEXT("The logging item");
	m_Table.bar.mode[0]		= BAR_SORT;
	m_Table.bar.defdx[0]		= 5 * 1;
	m_Table.bar.name[1]		= TEXT("Time");
	m_Table.bar.expl[1]		= TEXT("The logging time");
	m_Table.bar.mode[1]		= BAR_SORT;
	m_Table.bar.defdx[1]		= 5 * 3;
	m_Table.bar.name[2]		= TEXT("Type");
	m_Table.bar.expl[2]		= TEXT("The logging type");
	m_Table.bar.mode[2]		= BAR_SORT;
	m_Table.bar.defdx[2]		= 5 * 2;
	m_Table.bar.name[3]		= TEXT("Code");
	m_Table.bar.expl[3]		= TEXT("The replied error code");
	m_Table.bar.mode[3]		= BAR_SORT;
	m_Table.bar.defdx[3]		= 5 * 1;
	m_Table.bar.name[4]		= TEXT("Echo");
	m_Table.bar.expl[4]		= TEXT("The replied message");
	m_Table.bar.mode[4]		= BAR_BUTTON;
	m_Table.bar.defdx[4]		= 5 * 50;

	if ( Createtablewindow(
		&m_Table, 0, m_Table.bar.nbar, NULL, TEXT("ICO_L"), LOGNAME
	) == nullptr ) {
		Addtolist(		// .................
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Fail to create logger window .")
		);	return false;
	} else return true;
}
void CCmdLog::Destroy()
{
}

DWORD WINAPI CCmdLog::PipeProc(LPVOID lpParam)
{
	std::string str; std::wstring wstr;
	DWORD count = 0; char buffer[TEXTLEN*2] = {0};
	PIPEMSG msge = {-1, 0, nullptr, -1}; PPIPEMSG pmsg;
	CCmdLog *pthis = reinterpret_cast<CCmdLog*>(lpParam);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar>> cvt;

	Addtolist (
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Enter thread 'CCmdLog::PipeProc'") 
	);

	for (size_t pos = std::string::npos; ;) {
		// Sleep(10000); continue;
		bool brslt = ReadFile (	// ....
			m_hPipeRd, buffer, sizeof(buffer), &count, nullptr 
		) == TRUE; 
		if (brslt == false || count == 0) break; 
		str.append(buffer,count); if (count>=sizeof(buffer)) continue;
		// 
		// Search it each time means it's a terrible synchronization 
		// design, but it seems that anonymous pipe is not suitable 
		// to do this...
		//
		if (str.length() >= sizeof(msge)) {	
			// Usually, the read/write operation is not too frequent, 
			// and the size of output data is less than TEXTLEN*2, 
			// the beginning of buffer is actually the pipe message .
			// This checking can make it faster ..
			char *pbuffer = const_cast<char*>(str.c_str());
			pmsg = reinterpret_cast<PPIPEMSG>(pbuffer);
			if (pmsg->start != msge.start || pmsg->end != msge.end) {
				pos = str.find(	// Try to find a pipe message.
					reinterpret_cast<char*>(&msge.start), 1, sizeof(msge.start)
				);
				if (pos == std::string::npos) pmsg = nullptr;
				else {
					pmsg = reinterpret_cast<PPIPEMSG>(pbuffer+pos);
					if (pmsg->end != msge.end) pmsg = nullptr;
				}
			} else pos = 0;
			
			// We're requested to exit the loop ..
			if (pmsg != nullptr && (pmsg->id==0 || pmsg->id==-1)) {
				Addtolist (
					reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
					text("Request to exit the pipe thread ..")
				);	SetEvent(pmsg->event); break;
			}
			
			// Handle the input message ...
			if (pos == 0 && pmsg != nullptr) {
				wstr = cvt.from_bytes(str.substr(sizeof(msge)));
				msge.id = pmsg->id;		// Update the new id ..
				if (wstr.length()) pthis->msg(msge.id, wstr);
			} else if (pos != std::string::npos && pmsg != nullptr) {
				wstr = cvt.from_bytes(str.substr(0,pos));
				if (wstr.length()) pthis->msg(msge.id, wstr);
				msge.id = pmsg->id;		// Update the new id ..
				wstr = cvt.from_bytes(str.substr(pos+sizeof(msge)));
				if (wstr.length()) pthis->msg(msge.id, wstr);
			} else { 
				wstr = cvt.from_bytes(str); pthis->msg(msge.id,wstr); 
			}
			
			// We're requested to set the notification event .
			if (pos != std::string::npos && pmsg) {
				Addtolist (
					reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
					text("Request to set the notification event ..")
				);	SetEvent(pmsg->event);
			}
		//
		// Pipe message can't be less then sizeof(msge) ..
		//
		} else { 
			wstr = cvt.from_bytes(str); pthis->msg(msge.id,wstr,true); 
		}
		//
		// Clear the string buffer ..
		//
		str.clear();
	}

	Addtolist(
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Leave thread 'CCmdLog::PipeProc'")
	);

	return 0;
}


int  CCmdLog::MenuProc(t_table* table, wchar* name, ulong index, int mode)
{
	HGLOBAL hMen = nullptr; 
	int count = static_cast<int>(table->custommode); 
	if (table->mode & TABLE_STDSCR) count = table->sorted.n;
	CCmdLog *pthis = reinterpret_cast<CCmdLog*>(table->customdata);

	switch (mode) {
	case MENU_VERIFY: return MENU_NORMAL; 
	case MENU_EXECUTE: 
		switch(index) {
		case MENU_ITEM_CLEAR: pthis->clear(); break;
		case MENU_ITEM_STORE: pthis->store() ; break;
		case MENU_ITEM_CPYLN: 
			if (table->sorted.n <= 0) break;
			if (table->sorted.selected < 0) break;
			if (table->sorted.selected >= count) break;
			if ((hMen = pthis->CopyRow(table->sorted.selected, 1)) == nullptr) break;
			if (!OpenClipboard(table->hparent)) {GlobalFree(hMen); break;}
			if (!EmptyClipboard()) { GlobalFree(hMen); break; }
			SetClipboardData(CF_UNICODETEXT, hMen); CloseClipboard();
			break;
		case MENU_ITEM_CPYLG: 
			if (table->sorted.n <= 0) break;
			if (table->sorted.selected < 0) break;
			if (table->sorted.selected >= count) break;
			if ((hMen = pthis->CopyLog(table->sorted.selected)) == nullptr) break;
			if (!OpenClipboard(table->hparent)) {GlobalFree(hMen); break;}
			if (!EmptyClipboard()) { GlobalFree(hMen); break; }
			SetClipboardData(CF_UNICODETEXT, hMen); CloseClipboard();
			break;
		case MENU_ITEM_CPYTB: 
			if (table->sorted.n <= 0) break;
			if ((hMen = pthis->CopyTab()) == nullptr) break;
			if (!OpenClipboard(table->hparent)) { GlobalFree(hMen); break; }
			if (!EmptyClipboard()) { GlobalFree(hMen); break; }
			SetClipboardData(CF_UNICODETEXT, hMen); CloseClipboard();
			break;
		default: break;
		}
		return MENU_NOREDRAW;
	default: return MENU_ABSENT;
	}
	return 0;
}

// Logger destructor, a function that frees resources allocated in the
// descriptor of the bookmark. There are no such resources, so this function
// can be NULL.
void CCmdLog::DestProc(t_sorthdr* sort)
{
	PLOGGER logger = reinterpret_cast<PLOGGER>(sort);
	if (logger != nullptr) {
		if (logger->msge != nullptr) {
			delete logger->msge; logger->msge = nullptr;
		}
	}
}


// Function that compares two bookmarks according to the specified criterium.
// Returns -1 if first item is "lower" (should precede second on the screen),
// +1 if first item is "higher" and 0 if they are equal. Criterium n is the
// index of the column in the table.
int  CCmdLog::SortProc(const t_sorthdr* sh1, const t_sorthdr* sh2, const int sort)
{
	const LOGGER* lg1 = reinterpret_cast<const LOGGER*>(sh1);
	const LOGGER* lg2 = reinterpret_cast<const LOGGER*>(sh2);
	switch(sort) {
	case 0:						// Sort by id
		if (lg1->addr < lg2->addr) return -1;
		else if (lg1->addr > lg2->addr) return 1;
		else return 0;
	case 1:						// Sort by time
		if (lg1->time < lg2->time) return -1;
		else if (lg1->time > lg2->time) return 1;
		else return 0;
	case 2:						// Sort by type
		if (lg1->type < lg2->type) return -1;
		else if (lg1->type > lg2->type) return 1;
		else return 0;
	case 3:						// Sort by code
		if (lg1->code < lg2->code) return -1;
		else if (lg1->code > lg2->code) return 1;
		else return 0;
	default: return 0;
	}
	return 0;
}



// If column is DF_CACHESIZE, this function must return the requested 
// size of the data cache. If column is DF_FILLCACHE, DF_FREECACHE or 
// DF_NEWROW, it must return 0. In all other cases, it must return the 
// length of the displayed string s in wide characters (and, if mask is 
// used, the length of the mask in bytes)
int  CCmdLog::DrawProc(
	wchar* str, uchar* mask, int* select, t_table* table, 
	t_sorthdr* sort, int column, void* cache
) {
	static wchar * cmdtype[] = {	// Command types we supported .
		text("unknow"),
		text("bat"), text("lua"), 
		text("java"), text("perl"), text("ruby"), text("tool"), 
		text("inline"), text("python"), text("system"), text("multicmd")
	};
	//
	// Note: 
	// For table with style TABLE_SIMPLE or TABLE_USERDEF,
	// "t_sorthdr *sort" is the pointer to the index of drawing loop.
	//
	LPLOGGER plog;
	std::wstring *pmsg; 
	std::vector<t_sorthdr> *rows;
	int len, color, max = TEXTLEN * 2;
	ulong irow = sort->addr + table->offset;
	t_sorthdr* thdr = reinterpret_cast<t_sorthdr*>(cache);
	t_sorthdr* msge = reinterpret_cast<t_sorthdr*>(&thdr[1]);
	LPLOGGER logger = reinterpret_cast<LPLOGGER>(&msge[1]);

	// The logger can't be null while drawing ..
	if (column > DF_NEWROW && logger == nullptr) return 0;
	if (column > DF_NEWROW && logger->addr == 0) return 0;

	// The common mask flag .
	if (irow == table->sorted.selected) *select |= DRAW_SELECT;
	*select |= DRAW_MASK|DRAW_NEW|DRAW_EXTSEL|DRAW_COLOR;

	switch (column) {
	case DF_CACHESIZE:				// Request for draw cache size
		// We require cache for outputing.
		//-----------------------------------------------------
		//ulong		addr;	// std::vector<t_sorthdr> *
		//ulong		size;	// The row count of handled logger
		//ulong		type;	// The selection index ..
		//-----------------------------------------------------
		//ulong		addr;	// The string offset .
		//ulong		size;	// The size of this string buffer
		//ulong		type;	// The 0-based index of this line
		//-----------------------------------------------------
		//ulong		addr;	// Nothing changed .
		//ulong		size;	// Nothing changed .
		//ulong		type;	// Nothing changed .
		//ulong		rows;	// Nothing changed .
		//time_t		time;	// Nothing changed .
		//long			code;	// Nothing changed .
		//void*		msge;	// The splited multiline array .
		return sizeof(t_sorthdr)+sizeof(t_sorthdr)+sizeof(LOGGER);

	case DF_FILLCACHE:				// Request to fill draw cache
		// Now we need to initialize cache when drawing begins.
		max = sizeof(t_sorthdr)+sizeof(t_sorthdr)+sizeof(LOGGER);
		memset_s(cache, max, 0, max);		// Zero the cache ..
		// Needn't allocate the array if it uses single line ..
		if (table->mode & TABLE_STDSCR) ;
		else rows = new std::vector<t_sorthdr>;
		thdr->addr = reinterpret_cast<ulong>(rows);
		return 0;

	case DF_FREECACHE:				// Request to free cached resources
		// We need to free cached resources when drawing ends.
		rows = reinterpret_cast<std::vector<t_sorthdr>*>(thdr->addr);
		if (table->mode & TABLE_STDSCR) ;
		else if (rows != nullptr) delete rows;
		return 0;

	case DF_NEWROW:					// Request to start new row in window
		//
		// ---------------> For single line drawing ..
		//
		if (table->mode & TABLE_STDSCR) {
			plog = reinterpret_cast<LPLOGGER>(
				Getsortedbyselection(&table->sorted, irow)
			);
			if (plog != nullptr && plog->addr != 0) {
				*logger = *plog; thdr->size = irow; 
				pmsg = reinterpret_cast<std::wstring*>(plog->msge);
				msge->addr = msge->type = 0;
				msge->size = pmsg->find_first_of(text('\n'));
				if (msge->size == std::wstring::npos)
					msge->size = pmsg->length();
			} else logger->addr = 0;	// Return a null logger .
			return 0;	// Finish ....
		}

		//
		// ---------------> For multi-line drawing ..
		//
		// Find the logger by the row number ..
		for (plog = logger; irow >= thdr->size + plog->rows; ) {
			thdr->size += plog->rows; // Goto the next level.
			plog = reinterpret_cast<LPLOGGER>(
				Getsortedbyselection(&table->sorted,thdr->type++)
			);
			// Already output all loggers or it's a invalid logger
			if (!plog || !plog->addr) { logger->addr = 0; return 0; }
		}
		// Get the row list ..
		rows = reinterpret_cast<std::vector<t_sorthdr>*>(thdr->addr);
		// Save this logger to the cache if it's new ..
		if (plog != nullptr && plog != logger && plog->addr) {
			// Copy as a new logger cache and clear old logger rows.
			*logger = *plog; rows->clear();
			// Now parsing message information ..
			msge->addr = msge->size = msge->type = 0;
			pmsg = reinterpret_cast<std::wstring*>(plog->msge);
			std::wstring::const_iterator npos, nend = pmsg->cend();
			for (npos = pmsg->cbegin(); npos != nend; ++npos) {
				if (*npos == TEXT('\n')) {	// The line ending ..
					msge->type  = rows->size(); rows->push_back(*msge);
					msge->addr += msge->size+1; msge->size = 0;
				} else ++msge->size;			// The normal chars ..
			} if (pmsg->back() != text('\n')) {
				msge->type  = rows->size(); rows->push_back(*msge);
			} while (rows->size() < plog->rows) {
				msge->type = rows->size(); msge->size = 0;
				rows->push_back(*msge);		// Insert null logger .
			}
		}
		// Now, get the target row and put it into the cache .
		msge->type = irow-thdr->size; *msge = rows->at(msge->type);
		return 0;
		
	case 0:							// The number of logging item .
		// Do nothing if it's not the first line .
		if (irow > thdr->size) return 0;
		len = ::swprintf_s(str, max, TEXT("%d"), logger->addr);
		memset_s(mask, max, DRAW_GRAY, len);
		break;
	case 1:							// The time when logging happen.
		struct tm stm; localtime_s(&stm, &logger->time);
		len = ::wcsftime(str, max, TEXT("%I:%M:%S %p"), &stm);
		memset_s(mask, max, DRAW_NORMAL, len);
		break;
	case 2:							// The cmd type of execution .
		// Do nothing if it's not the first line .
		if (irow > thdr->size) return 0;
		if (logger->type >= _countof(cmdtype)) break;
		len = ::wcsnlen_s(cmdtype[logger->type], 10);
		::wcsncpy_s(str, max, cmdtype[logger->type], 10);
		color = logger->code == 0 ? DRAW_NORMAL : DRAW_HILITE;
		memset_s(mask, max, color, len);
		break;
	case 3:							// The error code of execution .
		// Do nothing if it's not the first line .
		if (irow > thdr->size) return 0;
		len = ::swprintf_s(str, max, TEXT("%i"), logger->code);
		color = logger->code == 0 ? DRAW_NORMAL : DRAW_HILITE;
		memset_s(mask, max, color, len);
		break;
	case 4:							// The answer of execution .
		pmsg = reinterpret_cast<std::wstring*>(logger->msge);
		// Get the color ...
		color = logger->code == 0 ? DRAW_NORMAL : DRAW_HILITE;
		// For the single line, need to add G_SINGLE.
		if (logger->rows == 1) {
			color = DRAW_HILITE;	// Always highlight the first line
			*str++ = G_CALLDEST; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_CALLDEST; *mask++ = DRAW_GRAPH; --max;
		// For multi-line, but doesn't support multi-line outputing
		} else if (table->mode & TABLE_STDSCR) {
			color = DRAW_HILITE;	// Always highlight the first line
			*str++ = G_SINGLE; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_CALLDEST; *mask++ = DRAW_GRAPH; --max;
		// For the first line, need to add G_BEGIN ..
		} else if (msge->type == 0) {
			color = DRAW_HILITE;	// Always highlight the first line
			*str++ = G_BEGIN; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_CALLDEST; *mask++ = DRAW_GRAPH; --max;
		// For the last line, need to add G_END ..
		} else if (msge->type+1 >= logger->rows) {
			//*str++ = msge->size == 0 ? G_END : G_ENTRY;
			*str++ = G_END;   *mask++ = DRAW_GRAPH; --max;
			*str++ = G_POINT; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_SPACE; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_SPACE; *mask++ = DRAW_GRAPH; --max;
		// For the body line  ...
		} else {
			*str++ = msge->size == 0 ? G_BODY : G_ENTRY;
			*mask++ = DRAW_GRAPH; --max;
			*str++ = G_POINT; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_SPACE; *mask++ = DRAW_GRAPH; --max;
			*str++ = G_SPACE; *mask++ = DRAW_GRAPH; --max;
		}
		*str++ = G_SPACE; *mask++ = DRAW_GRAPH; --max;
		::wcsncpy_s(str, max, pmsg->c_str()+msge->addr, msge->size);
		memset_s(mask, max, color, msge->size);
		len = msge->size + TEXTLEN * 2 - max;
		break;
	default: return 0;
	}
	return len;			// Return the length of the text ..
}


// Custom table function of bookmarks window. Here it is used only to process
// doubleclicks (custom message WM_USER_DBLCLK). This function is also called
// on WM_DESTROY, WM_CLOSE (by returning -1, you can prevent window from
// closing), WM_SIZE (custom tables only), WM_CHAR (only if TABLE_WANTCHAR is
// set) and different custom messages WM_USER_xxx (depending on table type).
// See documentation for details.
long CCmdLog::TableProc(t_table* table, HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParm)
{
	int count = static_cast<int>(table->custommode), value = 0; 
	if (table->mode & TABLE_STDSCR) count = table->sorted.n;
	switch (uMsg) {
	case WM_USER_CREATE:
		break;
	case WM_USER_CHGALL:
		break;
	case WM_USER_UPD:
		break;
	case WM_USER_FOCUS:
		break;
	case WM_USER_CNTS:
		value = static_cast<int>(lParm) + table->offset;
		if (table->sorted.selected == value) return 0;
		if (value > count-1) return 0;
		if (value < 0) return 0;
		table->sorted.selected = value;
		return 1;
	case WM_USER_SETS: 
		value = static_cast<int>(lParm) + table->offset;
		if (table->sorted.selected == value) return 0;
		if (value > count-1) value = count-1;
		if (value < 0) value = 0;
		table->sorted.selected = value;
		return 1;
	case WM_USER_VSCR:
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO); 
		si.fMask = SIF_RANGE|SIF_PAGE|SIF_POS;
		si.nPage = wParm; si.nPos = table->offset;
		si.nMin = 0; si.nMax = count; si.nTrackPos = 0;
		SetScrollInfo(table->hw, SBS_VERT, &si, TRUE);
		return 1;
	case WM_USER_HSCR:
		break;
	case WM_USER_VINC:
		if (table->offset == 0 && lParm < 0) return 0;
		value = table->offset + static_cast<int>(wParm);
		if (value > count+1 && lParm > 0) return 0;
		table->offset += static_cast<int>(lParm); 
		if (table->offset < 0) table->offset = 0; 
		value = count - wParm + 1; if (value<0) value = 0;
		if (table->offset > value) table->offset = value;
		return 1; 
	case WM_USER_VPOS:
		table->offset = static_cast<int>(lParm);
		if (table->offset < 0) table->offset = 0; 
		if (table->offset > count-1) table->offset = count-1;
		return 1;
	case WM_USER_VBYTE:
		break;
	case WM_USER_BAR: 
		if (wParm == 4) table->mode ^= TABLE_STDSCR;
		table->offset = 0;  // Jump to top ..
		return 1;
	case WM_USER_DBLCLK: break;
	default: break;
	}
	return 0;
}


int  CCmdLog::UpdateProc(t_table* table)
{
	return 0;
}
void CCmdLog::TabSelProc(t_table* table, int column, int row)
{
}


ulong CCmdLog::setidle(ulong id)
{
	LPLOGGER logger; ulong flag; if (id <= 0) return 0;
	EnterCriticalSection(&m_csShare);

	logger = reinterpret_cast<LPLOGGER>(
		Findsorteddata(&m_Table.sorted, id, 0)
	);
	if (logger == nullptr) {
		flag = LOGGER_FLAG_NONE;
		Addtolist (
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Can't find the logger %d"), id
		);
	} else {
		flag = logger->flag; 
		logger->flag = LOGGER_FLAG_IDLE;
		Addtolist (
			reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
			text("Set idle for %d"), id
		);
	}

	LeaveCriticalSection(&m_csShare);
	return flag;
}
ulong CCmdLog::setbusy(ulong id)
{
	LPLOGGER logger; ulong flag; if (id <= 0) return 0;
	EnterCriticalSection(&m_csShare);

	Addtolist(
		reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
		text("Set busy for %d"), id
	);
	logger = reinterpret_cast<LPLOGGER>(
		Findsorteddata(&m_Table.sorted, id, 0)
	);
	if (logger == nullptr) flag = LOGGER_FLAG_NONE;
	else {
		flag = logger->flag; 
		logger->flag = LOGGER_FLAG_BUSY;
	}

	LeaveCriticalSection(&m_csShare);
	return flag;
}


ulong CCmdLog::set(ulong id, int code, int type, ulong flag)
{
	LPLOGGER logger = nullptr; if (id <= 0) return 0;
	
	EnterCriticalSection(&m_csShare);
	
	// Initialize the sorted data if it hasn't initialize yet ..
	if (Issortedinit(&m_Table.sorted) == 0) {
		if (Createsorteddata (				// Make sure it's already initialized.
			&m_Table.sorted,			// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			0
		) != 0) { LeaveCriticalSection(&m_csShare); return 0; }
	}
	
	// Get the logger ..
	void *temp = Findsorteddata(&m_Table.sorted, id, 0);
	logger = reinterpret_cast<LPLOGGER>(temp);
	if (logger != nullptr) { 
		logger->code = code; id = logger->addr;
		if (type) logger->type = type;
		if (flag) logger->flag = flag;
	} else id = 0;
	
	LeaveCriticalSection(&m_csShare);
	
	// Now redraw the window and return ...
	Updatetable(&m_Table, false); return id;
}
ulong CCmdLog::log(ulong id, int code, int type, const std::wstring &msg)
{
	std::wstring *pmsg = nullptr; void *temp = nullptr;
	LPLOGGER plog = nullptr; ulong ulNewId = 0;

	EnterCriticalSection(&m_csShare);

	if (Issortedinit(&m_Table.sorted) == 0) {
		if (Createsorteddata (				// Make sure it's already initialized.
			&m_Table.sorted,					// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			0
		) != 0) { LeaveCriticalSection(&m_csShare); return 0; }
	}
	
	if (id <= 0) {							// Add new record ..
		// Can't add logger any more ...
		if (m_Table.custommode < MAXINT) {

			// Find a new id ..
			while (Findsorteddata(&m_Table.sorted, m_nNextId, 0))
				++ m_nNextId;

			// Initialize a new item ...
			time_t timer; time(&timer); LOGGER logger = {
				m_nNextId, 1, type, 0, LOGGER_FLAG_IDLE, 
				timer, code, nullptr
			};
			// Initialize a new string.. (it's suck without c++11)
			std::wstring *pmsg = new std::wstring();
			std::wstring::const_iterator itr, eitr = msg.cend();
			for (itr = msg.cbegin(); itr != eitr; ++itr) {
				// We don't support old mac format ...
				if (*itr == text('\r')) continue;
				if (*itr == text('\n')) ++logger.rows;
				pmsg->push_back(*itr);
			} if (pmsg->back() != text('\n')) {
				pmsg->push_back(text('\n')); ++logger.rows;
			}	logger.msge = pmsg;	// Set the string ....

			// Add a new item and return it's id ..
			temp = Addsorteddata(&m_Table.sorted, &logger);;
			plog = reinterpret_cast<LPLOGGER>(temp);
			if (plog != nullptr) ulNewId = plog->addr;
			else { ulNewId = 0; delete logger.msge; }
			
			// Update the line count ..
			if (plog) m_Table.custommode += plog->rows;
			
		}
		
	} else {									// Modify existed records .
		temp = Findsorteddata(&m_Table.sorted, id, 0);
		if ((plog = reinterpret_cast<LPLOGGER>(temp)) != nullptr) {

			m_Table.custommode -= plog->rows;

			// Update the logger ..
			plog->code = code; plog->type = type; plog->rows = 0;
			pmsg = reinterpret_cast<std::wstring*>(plog->msge);
			if (pmsg == nullptr) pmsg = new std::wstring();
			else pmsg->clear();		// Clear the message first..
			std::wstring::const_iterator itr, eitr = msg.cend();
			for (itr = msg.cbegin(); itr != eitr; ++itr) {
				// We don't support old mac format ...
				if (*itr == text('\r')) continue;
				if (*itr == text('\n')) ++plog->rows;
				pmsg->push_back(*itr);
			} if (pmsg->back() != text('\n')) {
				pmsg->push_back(text('\n')); ++plog->rows;
			}	plog->msge = pmsg;		// Set the string ....

			m_Table.custommode += plog->rows;

			// Return this id .
			ulNewId = plog->addr;
		
		}
	}

	LeaveCriticalSection(&m_csShare);

	// Redrawing the logger window and return ..
	if (m_Table.offset <= 0) Updatetable(&m_Table, false);
	else PostMessage(m_Table.hw,WM_VSCROLL,SB_PAGEDOWN,0); 
	return ulNewId;
}
ulong CCmdLog::log(ulong id, int code, int type, const wchar *msg)
{
	std::wstring *pmsg = nullptr; void *temp = nullptr;
	LPLOGGER plog = nullptr; ulong ulNewId = 0;

	EnterCriticalSection(&m_csShare);

	if (Issortedinit(&m_Table.sorted) == 0) {
		if (Createsorteddata (				// Initialize the sorted data
			&m_Table.sorted,					// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			0
		) != 0) { LeaveCriticalSection(&m_csShare); return 0; }
	}
	if (id <= 0) {							// Add new record ..
		// Can't add logger any more ...
		if (m_Table.custommode < MAXINT) {

			// Find a new id ..
			while (Findsorteddata(&m_Table.sorted,m_nNextId,0))
				++ m_nNextId;

			// Initialize a new item ...
			time_t timer; time(&timer); LOGGER logger = {
				m_nNextId, 1, type, 0, LOGGER_FLAG_IDLE, 
				timer, code, nullptr
			};
			// Initialize a new string.. (it's suck without c++11)
			std::wstring *pmsg = new std::wstring();
			for (size_t npos = 0; msg[npos]; ++npos) {
				// We don't support old mac format ...
				if (msg[npos] == text('\r')) continue;
				if (msg[npos] == text('\n')) ++logger.rows;
				pmsg->push_back(msg[npos]);
			} if (pmsg->back() != text('\n')) {
				pmsg->push_back(text('\n')); ++logger.rows;
			}	logger.msge = pmsg;	// Set the string ....

			// Add a new item and return it's id ..
			temp = Addsorteddata(&m_Table.sorted, &logger);
			plog = reinterpret_cast<LPLOGGER>(temp);
			if (plog != nullptr) ulNewId = plog->addr;
			else { ulNewId = 0; delete logger.msge; } 

			// Update the line count ..
			if (plog) m_Table.custommode += plog->rows;

		}
	} else {								// Modify existed record .
		temp = Findsorteddata(&m_Table.sorted, id, 0);
		if ((plog = reinterpret_cast<LPLOGGER>(temp)) != nullptr) {

			m_Table.custommode -= plog->rows;

			// Update the logger ..
			plog->code = code; plog->type = type; plog->rows = 0;
			pmsg = reinterpret_cast<std::wstring*>(plog->msge);
			if (pmsg == nullptr) pmsg = new std::wstring();
			else pmsg->clear();		// Clear the message first..
			for (size_t npos = 0; msg[npos]; ++npos) {
				// We don't support old mac format ...
				if (msg[npos] == text('\r')) continue;
				if (msg[npos] == text('\n')) ++plog->rows;
				pmsg->push_back(msg[npos]);
			} if (pmsg->back() != text('\n')) {
				pmsg->push_back(text('\n')); ++plog->rows;
			}	plog->msge = pmsg;	// Set the string ........

			m_Table.custommode += plog->rows;

			// Return this id .
			ulNewId = plog->addr;

		}
	}

	LeaveCriticalSection(&m_csShare);

	// Redrawing the logger window and return ..
	if (m_Table.offset <= 0) Updatetable(&m_Table, false);
	else PostMessage(m_Table.hw,WM_VSCROLL,SB_PAGEDOWN,0); 
	return ulNewId;
}
ulong CCmdLog::msg(ulong id, const std::wstring& msg, bool append)
{
	std::wstring *pmsg; LPLOGGER plog; if (id <= 0) return 0;

	EnterCriticalSection(&m_csShare);

	// Initialize the sorted data if it hasn't initialize yet ..
	if (Issortedinit(&m_Table.sorted) == 0) {
		if (Createsorteddata (				// Make sure it's already initialized.
			&m_Table.sorted,			// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			0
		) != 0)  { LeaveCriticalSection(&m_csShare); return 0; }
	}
	
	
	// Get the logger ..
	void *temp = Findsorteddata(&m_Table.sorted, id, 0);
	if ((plog = reinterpret_cast<LPLOGGER>(temp)) != nullptr) {

		m_Table.custommode -= plog->rows;

		pmsg = reinterpret_cast<std::wstring*>(plog->msge);
		if (pmsg == nullptr) pmsg = new std::wstring();
		if (append == false) { pmsg->clear(); plog->rows = 0; }
		else if (pmsg->back() != text('\n')) --plog->rows;
		// Extract the message ...
		std::wstring::const_iterator itr, eitr = msg.cend();
		for (itr = msg.cbegin(); itr != eitr; ++itr) {
			// We don't support old mac format ...
			if (*itr == text('\t')) { pmsg->append(8, text(' ')); continue; }
			if (*itr == text('\r')) continue;
			if (*itr == text('\n')) ++ plog->rows;
			pmsg->push_back(*itr);
		} if (pmsg->back() != text('\n')) ++ plog->rows;
		plog->msge = pmsg;			// Set the string ...

		m_Table.custommode += plog->rows;

	} else id = 0;	// return 0 if the logger doesn't exist.

	LeaveCriticalSection(&m_csShare);

	// Redrawing the logger window and return ..
	if (m_Table.offset <= 0) Updatetable(&m_Table, false);
	else PostMessage(m_Table.hw,WM_VSCROLL,SB_PAGEDOWN,0); 
	return id;
}
ulong CCmdLog::msg(ulong id, const wchar* msg, bool append)
{
	std::wstring *pmsg; LPLOGGER plog; if (id <= 0) return 0;

	EnterCriticalSection(&m_csShare);

	if (Issortedinit(&m_Table.sorted) == 0) {
		if (Createsorteddata (				// Make sure it's already initialized.
			&m_Table.sorted,					// Descriptor of sorted data
			sizeof(LOGGER),					// Size of single data item
			10,									// Initial number of allocated items
			(SORTFUNC *)CCmdLog::SortProc,	// Sorting function
			(DESTFUNC *)CCmdLog::DestProc,	// Data destructor
			0
		) != 0) { LeaveCriticalSection(&m_csShare); return 0; }
	}
	
	
	// Get the logger ..
	void *temp = Findsorteddata(&m_Table.sorted, id, 0);
	if ((plog = reinterpret_cast<LPLOGGER>(temp)) != nullptr) {

		m_Table.custommode -= plog->rows;

		// Append or replace the message .
		pmsg = reinterpret_cast<std::wstring*>(plog->msge);
		if (pmsg == nullptr) pmsg = new std::wstring();
		if (append == false) { pmsg->clear(); plog->rows = 0; }
		else if (pmsg->back() != text('\n')) --plog->rows;
		// Extract the message ...
		for (size_t npos = 0; msg[npos]; ++npos) {
			// We don't support old mac format ...
			if (msg[npos] == text('\t')) { pmsg->append(8, text(' ')); continue; }
			if (msg[npos] == text('\r')) continue;
			if (msg[npos] == text('\n')) ++plog->rows;
			pmsg->push_back(msg[npos]);
		} if (pmsg->back() != text('\n')) ++plog->rows;
		plog->msge = pmsg;			// Set the string ....

		m_Table.custommode += plog->rows;

	} else id = 0;	// return 0 if the logger doesn't exist.

	LeaveCriticalSection(&m_csShare);

	// Redrawing the logger window and return ..
	if (m_Table.offset <= 0) Updatetable(&m_Table, false);
	else PostMessage(m_Table.hw,WM_VSCROLL,SB_PAGEDOWN,0); 
	return id;
}


ulong CCmdLog::connect(ulong id)
{
	if (id == 0) return 0;
	PIPEMSG msg = {-1, id, m_hNotify, -1}; DWORD dwSize;
	WriteFile(m_hPipeWr, &msg, sizeof(msg), &dwSize, nullptr);
	WaitForSingleObject(m_hNotify, INFINITE);
	return id ;
}
pvoid CCmdLog::getpipe()
{
	return reinterpret_cast<pvoid>(m_hPipeWr);
}
ulong CCmdLog::sendmsg(pvoid msg, ulong size)
{
	DWORD dwSize = 0;
	WriteFile(m_hPipeWr, msg, size, &dwSize, nullptr);
	return dwSize;
}
ulong CCmdLog::disconn(ulong id)
{
	// For now, the code is same as 'connect', 
	if (id == 0) return 0;
	PIPEMSG msg = {-1, id, m_hNotify, -1}; DWORD dwSize;
	WriteFile(m_hPipeWr, &msg, sizeof(msg), &dwSize, nullptr);
	WaitForSingleObject(m_hNotify, INFINITE);
	return id ;
}
bool  CCmdLog::initcon()
{
	// Create notification event ..
	if (m_hNotify == nullptr) {
		if ( (m_hNotify = CreateEvent(nullptr, true, false, nullptr)
		) == nullptr) {
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("Fail to create notification event .")
			); m_hNotify = nullptr; return false;
		}
	}

	// Create name pipe to capture the output of tools .
	if (m_hPipeRd == nullptr && m_hPipeWr == nullptr) {
		SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES)}; 
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = true; sa.lpSecurityDescriptor = nullptr; 
		if (!CreatePipe(&m_hPipeRd, &m_hPipeWr, &sa, 0)) {
			m_hPipeRd = m_hPipeWr = nullptr; 
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("Fail to create pipe to redirect output")
			); return false;
		}
		SetHandleInformation(m_hPipeRd, HANDLE_FLAG_INHERIT, 0);
	}

	// Create a thread to capture output ..
	if (m_hPipeCb == INVALID_HANDLE_VALUE && m_dPipeCb == 0) {
		if ( (m_hPipeCb = CreateThread (
			nullptr, 0, CCmdLog::PipeProc, this, 0, &m_dPipeCb
		)) == INVALID_HANDLE_VALUE ) {
			m_hPipeCb = INVALID_HANDLE_VALUE; m_dPipeCb = 0;
			Addtolist(		// .................
				reinterpret_cast<ulong>(GetRetAddr()), DRAW_HILITE, 
				text("Fail to create thread 'CCmdLog::PipeProc'")
			); return false;
		}
	}

	return true;
}
void  CCmdLog::freecon()
{
	PIPEMSG msg = {-1, -1, m_hNotify, -1}; DWORD dwSize = 0;
	WriteFile(m_hPipeWr, &msg, sizeof(msg), &dwSize, nullptr);
	//WaitForSingleObject(m_hNotify, INFINITE);
	WaitForSingleObject(m_hPipeCb, INFINITE);
	CloseHandle(m_hPipeCb); m_hPipeCb = INVALID_HANDLE_VALUE;
	
	CloseHandle(m_hPipeWr); CloseHandle(m_hPipeRd);
	m_hPipeWr = m_hPipeRd = INVALID_HANDLE_VALUE;
	
	CloseHandle(m_hNotify); m_hNotify = INVALID_HANDLE_VALUE;
}


ulong CCmdLog::erase(ulong id)
{
	EnterCriticalSection(&m_csShare);
	void *temp = Findsorteddata(&m_Table.sorted, id, 0);
	LPLOGGER plog = reinterpret_cast<LPLOGGER>(temp);
	if (plog != nullptr) {
		Deletesorteddata(&m_Table.sorted, plog->addr, 0);
		Updatetable(&m_Table, false);
	} else id = 0;
	LeaveCriticalSection(&m_csShare);
	return id ;
}
ulong CCmdLog::store(void)
{
	wchar path[MAXPATH] = {0}; int irslt = 0;
	::wcsncpy_s(path, _countof(path), text("logger.log"), 10);
	if ( (irslt = Browsefilename(
		text("Command Bar Logger File"), path, nullptr, nullptr, 
		text("*.log"), m_Table.hparent, BRO_SAVE
	)) == 0 ) return 0;

	HGLOBAL hMem = CopyTab(); if (hMem == nullptr) return 0;
	wchar *buffer = reinterpret_cast<wchar*>(GlobalLock(hMem));
	if (buffer == nullptr) return 0;
	const std::locale utf8 = std::locale(
		std::locale(), new std::codecvt_utf8<wchar>()
	);
	std::wfstream file(path, std::fstream::trunc|std::fstream::out);
	file.imbue(utf8); file << buffer << std::endl; file.close();
	GlobalUnlock(hMem); GlobalFree(hMem); 
	return 0;
}
ulong CCmdLog::clear(void)
{
	m_Table.offset = m_Table.sorted.selected = m_Table.xshift = 0;

	EnterCriticalSection(&m_csShare);

	ulong begin = -1, count = 0, end; LPLOGGER plog; m_nNextId = 0;
	for (int pos = 0; pos < m_Table.sorted.n; ++pos) {
		plog = reinterpret_cast<LPLOGGER> (
			Getsortedbyindex(&m_Table.sorted, pos)
		);	// logger->addr = pos;
		if (plog->flag & LOGGER_FLAG_IDLE) {
			m_Table.custommode -= plog->rows; ++ count;
		}
		if (begin == -1) {
			if (plog->flag & LOGGER_FLAG_IDLE) {
				begin = plog->addr;
			}
		} else {
			if (plog->flag & LOGGER_FLAG_BUSY) {
				end = plog->addr; pos -= count + 1;
				Addtolist(1,1,text("begin: %x, end: %x"),begin,end);
				Deletesorteddatarange(&m_Table.sorted, begin, end);
				if (!m_nNextId) m_nNextId = begin; begin = -1;
			}
		}
	} if (begin != -1) {
		if (!m_nNextId) m_nNextId = begin; end = plog->addr + 1;
		Deletesorteddatarange(&m_Table.sorted, begin, end);
	}

	LeaveCriticalSection(&m_csShare);

	Updatetable(&m_Table, false); return count;
}


// Copy row data from table ..
// The code is from Copywholetable ..
HGLOBAL CCmdLog::CopyRow(int row, int count)
{
	void* pcache = nullptr; 
	t_table *table = &m_Table;
	int offset = m_Table.offset;	// Save current offset ..
	uchar cache[0x400]; uchar mask[TEXTLEN*2];
	wchar wstr[TEXTLEN*2]; wchar *pbuf, *pstr;
	int width[NBAR]; int size,val,i,j,k,pos; HGLOBAL hMem;
	
	// Not support index at the end ..
	if (count <= 0) return nullptr;
	// Check parameters ..
	if (table->mode & TABLE_STDSCR) {
		size = static_cast<int>(table->sorted.n); 
		if (row+count > size) return nullptr;
	} else {
		size = static_cast<int>(table->custommode); 
		if (row+count > size) return nullptr;
	}

	// Calc the character width array ..
	for (size=i=0; i < table->bar.nbar && i < NBAR; ++i) {
		if ((val = table->bar.dx[i]) > 3) {
			width[i] = val / Getcharacterwidth(table, i);
			if (width[i] >= 0x100) width[i] = 0x0ff;
			size += val + 1;
		} else width[i] = 0;
	} if (size == 0) return nullptr;

	// Allocate the buffer ..
	size = (table->sorted.n+1) * (size+2) + 320;
	hMem = GlobalAlloc(0x2002u, 2 * size); pos = 0;
	if (hMem == nullptr) return nullptr;
	pbuf = reinterpret_cast<wchar*>(GlobalLock(hMem));
	if (pbuf==nullptr) { GlobalFree(hMem); return nullptr; }

	// Get the window title ..
	if (table->hparent != nullptr)
		pos = GetWindowText(table->hparent, pbuf, 128);
	if (pos == 0)
		pos = StrcopyW(pbuf, TEXTLEN, table->name);
	if (pos == 0)
		pos = StrcopyW(pbuf, TEXTLEN, TEXT("Unnamed table"));
	pbuf[pos++] = TEXT('\r'); pbuf[pos++] = TEXT('\n');

	// Require and allocate the cache ...
	val = table->drawfunc(
		wstr, mask, &val, table, 
		reinterpret_cast<t_sorthdr*>(&i), DF_CACHESIZE, 0
	);
	if (val < 0x400) pcache = cache + 0;
	else pcache = Memalloc(val, SILENT);
	if (pcache == nullptr) { GlobalFree(hMem); return 0; }
	// Initialize the cache ..
	if (val > 0) { table->drawfunc(
		wstr, mask, &val, table, 
		reinterpret_cast<t_sorthdr*>(&i), DF_FILLCACHE, pcache
	);}
	
	// Add bar title ..
	for (i = 0; i < table->bar.nbar && i < NBAR; ++i) {
		if (width[i] == 0) continue;
		pstr = table->bar.name[i];
		for (j = 0; j < width[i]; ++j) {
			// The end char '$' or null ..
			if (pstr[j]==0 || pstr[j]==TEXT('$')) break;
			else pbuf[pos++] = pstr[j];
		}
		// Not enough space to display the whole title ..
		// then end with '> '
		if (j >= width[i] && pstr[j] && pstr[j] != TEXT('$')) {
			pbuf[pos++] = TEXT('>'); //pbuf[pos++] = TEXT(' '); 
		// Append ' ' if we have too many space ..
		// It's not avaliable for the last column ..
		} else if (i < table->bar.nbar-1) {
			for ( ; j < width[i]+1; ++j) pbuf[pos++] = TEXT(' ');
		}
	}
	// The ending of title bar ..
	for( ; pos > 0; --pos) {
		if (pbuf[pos-1] != TEXT(' ')) {
			pbuf[pos++] = TEXT('\r'); pbuf[pos++] = TEXT('\n');
			break;
		}
	}

	// Now get the logger data ..
	if (table->mode & TABLE_STDSCR) 
		if (row+count > table->sorted.n) 
			count = table->sorted.n - row;
	m_Table.offset = row;	// We begin with row ..
	for (i = k = 0; i < count; ++i, k = i) {
		table->drawfunc(
			wstr, mask, &val, table, 
			reinterpret_cast<t_sorthdr*>(&k), DF_NEWROW, pcache
		);
		for (j = 0; j < table->bar.nbar && j < NBAR; ++j) {
			val = 0; if (width[j] == 0) continue;
			size = table->drawfunc (
				pbuf+pos, mask, &val, table, 
				reinterpret_cast<t_sorthdr*>(&k), j, pcache
			);
			if ( val & DRAW_ULTEXT ) size = 0;
			else if (size >= 0) size = min(size, 255);
			else size = 0;
			// The width may large than the limit ..
			size = min(size, width[j]);
			// Translate the graph symbols ..
			Replacegraphs(ADDR_SYMMASK, pbuf+pos, mask, val, size);
			pos = pos + size;		// The next position ..
			// Adjust the string ..
			if ( size < width[j] ) {
				if ( j < table->bar.nbar - 1 ) {
					for ( ; size < width[j] + 1; ++size)
						pbuf[pos++] = TEXT(' ');
				}
			} else { 
				pbuf[pos++] = TEXT('>'); //pbuf[pos++] = TEXT(' ');
			}
		}
		// The ending of current line ..
		for( ; pos > 0; --pos) {
			if (pbuf[pos-1] != TEXT(' ')) {
				pbuf[pos++] = TEXT('\r'); pbuf[pos++] = TEXT('\n');
				break;
			}
		}
	}
	
	// The ending of the whole content ..
	pbuf[pos] = 0; size = pos + 1;
	
	// Notify the draw function to free cache ..
	if (pcache != nullptr && pcache != cache+0) {
		table->drawfunc(
			wstr, mask, &val, table, 
			reinterpret_cast<t_sorthdr*>(&i), DF_FREECACHE, pcache
		);
	}
	
	// Cleanup and restore data ...
	table->offset = offset; GlobalUnlock(hMem);
	if (pcache && pcache != cache+0) Memfree(pcache);	
	return GlobalReAlloc(hMem, size*2, GMEM_MOVEABLE);
}
HGLOBAL CCmdLog::CopyLog(int row)
{
	LPLOGGER plog = nullptr; int count = 0; int i;
	if (m_Table.mode & TABLE_STDSCR) {
		for (i = 0; ; ++i, count += plog->rows) {
			plog = reinterpret_cast<LPLOGGER> (
				Getsortedbyselection(&m_Table.sorted, i)
			);
			if (plog == nullptr) return nullptr;
			if (i != row) continue;
			m_Table.mode &= ~TABLE_STDSCR;
			HGLOBAL hMem = CopyRow(count, plog->rows);
			m_Table.mode |=  TABLE_STDSCR;
			return hMem;
		}
	} else {
		for (i = 0; ; ++i, count += plog->rows) {
			plog = reinterpret_cast<LPLOGGER> (
				Getsortedbyselection(&m_Table.sorted, i)
			);
			if (plog == nullptr) return nullptr;
			int end = count+static_cast<int>(plog->rows);
			if (row >= count && row < end) {
				return CopyRow(count, plog->rows);
			}
		}
	}
	// It should not be run here ..
	return nullptr;
}
HGLOBAL CCmdLog::CopyTab(void)
{
	if (m_Table.mode & TABLE_STDSCR) {
		return CopyRow(0, m_Table.sorted.n);
	} else return CopyRow(0, m_Table.custommode);
}


