
#include "Perl.hpp"

////////////////////////////////////////////////////////////////////////////////////////

size_t	CPerl::m_nReffer = 0;	// The refference counter.
ulong	CPerl::m_uStatus = 0;	// The status of this component.

////////////////////////////////////////////////////////////////////////////////////////

CPerl::CPerl()
{
	if (m_uStatus == PERL_INVALID) 
		if (init() == true) m_uStatus = PERL_WORKING;
	++ m_nReffer;
}
CPerl::~CPerl()
{
	if ( --m_nReffer <= 0 ) {
		free(); m_uStatus = PERL_INVALID;
	}
}


int CPerl::ExecFile(const std::vector<std::wstring>&, std::wstring&, HANDLE hfile)
{
	return 0;
}
int CPerl::ExecCmds(const std::wstring&, std::wstring&, HANDLE hfile)
{
	return 0;
}
int CPerl::ExecStop(unsigned long)
{
	return 0;
}


bool CPerl::init()
{
	return true;
}
void CPerl::free()
{
	
}
