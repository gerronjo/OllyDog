

#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm> 

#include <stdio.h>
#include <windows.h>

#include <plugin.h>

class COptions 
{
public:
	// For int type .
	bool SetOption(const wchar*, int&, bool flush=true);
	bool GetOption(const wchar*, int&, bool cache=true);
	// For str type .
	bool SetOption(const wchar*, std::wstring&, bool flush=true);
	bool GetOption(const wchar*, std::wstring&, bool cache=true);
	// Access other options .
	bool SetOption(const wchar*, const wchar*, int &value);
	bool GetOption(const wchar*, const wchar*, int &value);
	bool SetOption(const wchar*, const wchar*, std::wstring &value);
	bool GetOption(const wchar*, const wchar*, std::wstring &value);

public:
	#define OPTION_INVALID							0x00000000
	#define OPTION_WORKING							0x00000000
private:
	class COptKeyComp {					// For key comparison.
		public: bool operator() (
			const wchar* lpsDst, const wchar* lpsSrc) const {
			return wcscmp(lpsDst, lpsSrc) < 0;
		};
	};
	typedef std::map<const wchar*,std::wstring,COptions::COptKeyComp> MapStrOpts;
	typedef std::map<const wchar*,int,COptions::COptKeyComp> MapIntOpts;
	static	ulong							COptions::m_uStatus;
	static size_t							COptions::m_nReffer;
	static MapStrOpts						COptions::m_strOpts;
	static MapIntOpts						COptions::m_intOpts;
protected:
	COptions(); ~COptions();
	bool GetOptionsFile(std::wstring &tsFile);
	bool ReadOptionsFile(); bool SaveOPtionsFile();
};


