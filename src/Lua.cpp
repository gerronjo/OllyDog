
#include "Lua.hpp"

////////////////////////////////////////////////////////////////////////////////////////

size_t	CLua::m_nReffer = 0;	// The refference counter.
ulong	CLua::m_uStatus = 0;	// The status of this component.

////////////////////////////////////////////////////////////////////////////////////////

CLua::CLua()
{
	if (m_uStatus == LUA_INVALID) 
		if (init() == true) m_uStatus = LUA_WORKING;
	++ m_nReffer;
}
CLua::~CLua()
{
	if ( --m_nReffer <= 0 ) {
		free(); m_uStatus = LUA_INVALID;
	}
}


int CLua::ExecStop(unsigned long)
{
	return 0;
}
int CLua::ExecCmds(const std::wstring&, std::wstring&, HANDLE hfile)
{
	return 0;
}
int CLua::ExecFile(const std::vector<std::wstring>&, std::wstring&, HANDLE hfile)
{
	return 0;
}


bool CLua::init()
{
	return 0;
}
void CLua::free()
{
}