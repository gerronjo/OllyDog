

#include "Options.hpp"

////////////////////////////////////////////////////////////////////////////////////////

ulong								COptions::m_uStatus = 0;
size_t								COptions::m_nReffer = 0;
COptions::MapIntOpts				COptions::m_intOpts;
COptions::MapStrOpts				COptions::m_strOpts;

////////////////////////////////////////////////////////////////////////////////////////

COptions::COptions()
{
	if (m_nReffer <= 0) {
		// Set all default int options .
		m_intOpts[text("Show CmdBar")] = 1;
		// Set all default str options .
		m_strOpts[text("Python Path")] = std::wstring(text("./python"));
		m_strOpts[text("Tool Path")] = std::wstring(text("./tool"));
		m_strOpts[text("Perl Path")] = std::wstring(text("./perl"));
		m_strOpts[text("Lua Path")] = std::wstring(text("./lua"));
		// Read all options from the option file .
		ReadOptionsFile();
	}	m_nReffer ++;
}

COptions::~COptions()
{
	if (--m_nReffer <= 0) {
		// Save all options to the option file .
		SaveOPtionsFile();
		// Clear all options in the contianer .
		m_intOpts.clear();
		m_strOpts.clear();
	}
}


bool COptions::GetOptionsFile(std::wstring &file)
{
	file = ollyfile;
	wchar buffer[MAX_PATH+1] = {0};
	std::size_t pos = 0, len = 0; DWORD nrslt = 0;

	while (file.length() < 8) {	// ollyfile is not valid ..
		nrslt = GetModuleFileName(nullptr, buffer, _countof(buffer));
		if (nrslt == 0) return false;
		if (nrslt < _countof(buffer)) { file = buffer; break; }
		file.assign(nrslt * 2, text('\0'));
		nrslt = GetModuleFileName (
			nullptr, const_cast<wchar*>(file.data()), file.size()
		);
		if (nrslt == 0 || nrslt >= file.size()) return false;
		pos = file.find_last_not_of(text('\0'), std::wstring::npos);
		if (pos != std::wstring::npos) 
			file.erase(pos+1, std::wstring::npos); break;
	}

	if ((pos = file.rfind(text(".exe"))) == file.length()-4) {
		file.replace(pos, std::wstring::npos, text(".ini"));
		return true;					// Success ..
	} else return false;				// Failure ..
}


bool COptions::ReadOptionsFile()
{
	std::wstring file; std::vector<wchar> buffer;
	if (GetOptionsFile(file) == false) return false;
	
	// Read all int options .
	COptions::MapIntOpts::iterator iitr, eiitr;
	iitr = m_intOpts.begin(); eiitr = m_intOpts.end();
	for(iitr; iitr != eiitr; ++iitr) {
		iitr->second = GetPrivateProfileInt(
			text("OllyDOG"), iitr->first, iitr->second, file.c_str()
		);
	}

	// Read all str options .
	COptions::MapStrOpts::iterator sitr, seitr;
	sitr = m_strOpts.begin(); seitr = m_strOpts.end();
	for( ; sitr != seitr; ++sitr) {
		for(int index=1; index <= 10; ++index) {
			DWORD nSize = index * MAX_PATH; buffer.reserve(nSize);
			DWORD nRslt = GetPrivateProfileString(
				text("OllyDOG"), sitr->first, sitr->second.c_str(), 
				buffer.data(), nSize, file.c_str()
			);
			if (nRslt == 0) break;
			if (nRslt+2 >= nSize) continue;
			sitr->second = buffer.data(); break;
		}
	}

	// Read all stt options. (Not support yet !)
	return true;		// For now, we always return success .
}
bool COptions::SaveOPtionsFile()
{
	std::wstring file; wchar buffer[MAX_PATH+1];
	if (GetOptionsFile(file) == false) return false;

	// Save all int options .
	COptions::MapIntOpts::iterator iitr, eiitr;
	iitr = m_intOpts.begin(); eiitr = m_intOpts.end();
	for(iitr; iitr != eiitr; ++iitr) {
		int iRslt = swprintf_s (
			buffer, _countof(buffer), text("%d"), iitr->second
		);
		if (iRslt > 0 && iRslt < _countof(buffer)) {
			WritePrivateProfileString(
				text("OllyDOG"), iitr->first, buffer, file.c_str()
			);
		}
	}

	// Save all str options .
	COptions::MapStrOpts::iterator sitr, esitr;
	sitr = m_strOpts.begin(); esitr = m_strOpts.end();
	for(sitr; sitr != esitr; ++sitr) {
		WritePrivateProfileString(
			text("OllyDOG"), sitr->first, sitr->second.c_str(), 
			file.c_str()
		);
	}

	// Save all stt options. (Not support yet !)
	return true;		// For now, we always return success .
}

bool COptions::SetOption(const wchar *key, int &value, bool flush)
{
	if (key == nullptr || key[0] == text('\0')) return false;
	m_intOpts[key] = value;	// Set cache value .
	if (flush == true) {					// Save option to ini file .
		std::wstring file; wchar buffer[MAX_PATH+1] = {0};
		if (GetOptionsFile(file) == false) return false;
		int iRslt = swprintf_s (
			buffer, _countof(buffer), text("%d"), value
		);
		if (iRslt > 0 && iRslt < _countof(buffer)) {
			return WritePrivateProfileString (
				text("OllyDOG"), key, buffer, file.c_str()
			) == TRUE;
		} else return false;
	}	return true;
}
bool COptions::GetOption(const wchar *key, int& value, bool cache)
{
	if (key == nullptr || key[0] == text('\0')) return false;
	if (cache == false) {				// Get option from ini file .
		std::wstring file;
		if (GetOptionsFile(file) == false) return false;
		m_intOpts[key] = GetPrivateProfileInt(
			text("OllyDOG"), key, m_intOpts[key], file.c_str()
		);
	}
	value = m_intOpts[key]; return true;
}


bool COptions::SetOption(const wchar *key, std::wstring& value, bool flush)
{
	if (key == nullptr || key[0] == text('\0')) return false;
	m_strOpts[key] = value;			// Set cache value .
	if (flush == true) {			// Save option to ini file .
		std::wstring file;
		if (GetOptionsFile(file) == false) return false;
		return WritePrivateProfileString(
			text("OllyDOG"), key, value.c_str(), file.c_str()
		) == TRUE;
	}
	return true;
}
bool COptions::GetOption(const wchar *key, std::wstring& value, bool cache)
{
	if (key == nullptr || key[0] == text('\0')) return false;
	if (cache == false) {			// Get option from ini file .
		DWORD nRslt = 0, nSize = 0; std::vector<wchar> buffer;
		std::wstring file;	// Get ini file first
		if (GetOptionsFile(file) == false) return false;
		for (int index = 1; index <= 100; ++index) {
			nSize = index * MAX_PATH; buffer.reserve(nSize);
			nRslt = GetPrivateProfileString (
				text("OllyDOG"), key, m_strOpts[key].c_str(), 
				buffer.data(), nSize, file.c_str()
			);
			if (nRslt == 0) break;
			if (nRslt+2 >= nSize) continue;
			m_strOpts[key] = buffer.data(); break;
		}
	}
	value = m_strOpts[key]; return true;
}


// Read OllyDBG option .
bool COptions::SetOption(const wchar *section, const wchar *key, int& value)
{
	std::wstring file; wchar buffer[MAX_PATH+1] = {0};
	if (GetOptionsFile(file) == false) return false;
	int iRslt = swprintf_s (buffer, _countof(buffer), text("%d"), value);
	if (iRslt <= 0 || iRslt >= _countof(buffer)) return false;
	return WritePrivateProfileString (section, key, buffer, file.c_str()) == TRUE;
}
bool COptions::GetOption(const wchar *section, const wchar *key, int& value)
{
	std::wstring file;
	if (GetOptionsFile(file) == false) return false;
	value = GetPrivateProfileInt (section, key, 0, file.c_str());
	return true;
}


bool COptions::SetOption(const wchar *section, const wchar *key, std::wstring& value)
{
	std::wstring file;
	if (GetOptionsFile(file) == false) return false;
	return WritePrivateProfileString(section, key, value.c_str(), file.c_str()) == TRUE;
}
bool COptions::GetOption(const wchar *section, const wchar *key, std::wstring& value)
{
	DWORD nRslt = 0, nSize = 0; std::vector<wchar> buffer;
	std::wstring file;	// Get ini file first
	if (GetOptionsFile(file) == false) return false;
	for(int index=1; index<=100; ++index) {
		nSize = index * MAX_PATH; buffer.reserve(nSize);
		nRslt = GetPrivateProfileString (
			section, key, nullptr, buffer.data(), nSize, file.c_str()
		);
		if (nRslt == 0) break; if (nRslt+2 >= nSize) continue;
		value.assign(buffer.data()); break;
	}	return true;
}


