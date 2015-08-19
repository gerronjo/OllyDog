

#include <vector>
#include <string>
#include <codecvt>
#include <algorithm>

#include <python.h>

#include "Options.hpp"

class CPerl : public COptions
{
public:
private:
	#define PERL_INVALID				0x00000000
	#define PERL_WORKING				0x00000000
	static size_t						m_nReffer;
	static	ulong						m_uStatus;
protected:
		
public:
	CPerl(); ~CPerl(); 
	virtual int ExecStop(unsigned long);
	//virtual int ExecCmds(const wchar *cmds, HANDLE hfile);
	virtual int ExecCmds(const std::wstring&, std::wstring&, HANDLE hfile = nullptr);
	//virtual int ExecFile(const wchar *file, HANDLE hfile);
	virtual int ExecFile(const std::vector<std::wstring>&, std::wstring&, HANDLE hfile = nullptr);
private:
protected:
	virtual bool init(); virtual void free();
};
