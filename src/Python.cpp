
#include "Python.hpp"

////////////////////////////////////////////////////////////////////////////////////////

size_t	CPython::m_nReffer = 0;	// The refference counter.
ulong	CPython::m_uStatus = 0;	// The status of this component.

// MSVCR90 interface ..
HMODULE CPython::m_vcModule												= nullptr;
CPython::__fclose CPython::fclose										= nullptr;
CPython::__fdopen CPython::fdopen										= nullptr;
CPython::__setvbuf CPython::setvbuf										= nullptr;
CPython::__open_osfhandle CPython::open_osfhandle						= nullptr;

// Python interface ...
HMODULE CPython::m_pyModule												= nullptr;
CPython::py_finalize CPython::Py_Finalize								= nullptr;
CPython::py_initialize CPython::Py_Initialize							= nullptr;
CPython::py_setpythonhome CPython::Py_SetPythonHome					= nullptr;
CPython::py_setprogramname CPython::Py_SetProgramName					= nullptr;

CPython::pyfile_asfile CPython::PyFile_AsFile							= nullptr;
CPython::pyfile_fromfile CPython::PyFile_FromFile						= nullptr;
CPython::pyfile_fromstring CPython::PyFile_FromString					= nullptr;

CPython::pyeval_initthreads CPython::PyEval_InitThreads				= nullptr;

CPython::pysys_setargv CPython::PySys_SetArgv							= nullptr;
CPython::pysys_getobject CPython::PySys_GetObject						= nullptr;
CPython::pysys_setobject CPython::PySys_SetObject						= nullptr;

CPython::pyimport_addmodule CPython::PyImport_AddModule				= nullptr;
CPython::pyimport_importmodule CPython::PyImport_ImportModule		= nullptr;

CPython::pyobject_setattrstring	CPython::PyObject_SetAttrString		= nullptr;
CPython::pyobject_print CPython::PyObject_Print						= nullptr;

CPython::pyrun_simplestring CPython::PyRun_SimpleString				= nullptr;
CPython::pyrun_simplestringflags CPython::PyRun_SimpleStringFlags	= nullptr;
CPython::pyrun_simplefile CPython::PyRun_SimpleFile					= nullptr;
CPython::pyrun_simplefileflags CPython::PyRun_SimpleFileFlags		= nullptr;
CPython::pyrun_simplefileex CPython::PyRun_SimpleFileEx				= nullptr;
CPython::pyrun_simplefileexflags CPython::PyRun_SimpleFileExFlags	= nullptr;

////////////////////////////////////////////////////////////////////////////////////////

CPython::CPython()
{
	if (m_uStatus == PYTHON_INVALID) 
		if (init() == true) m_uStatus = PYTHON_WORKING;
	++ m_nReffer;
}
CPython::~CPython()
{
	if ( --m_nReffer <= 0 ) {
		free(); m_uStatus = PYTHON_INVALID;
	}
}


bool CPython::init()
{
	//
	// Initialize the python environment
	//
	std::wstring path; int iRslt = 0; DWORD nrslt = 0;
	std::vector<wchar> vetbuf; wchar buffer[MAX_PATH+1] = {0};

	// First of all, get the python path from options .
	for (; GetOption(text("Python Path"), path); ) {
		nrslt = ::GetFullPathName (	// ...
			path.c_str(), _countof(buffer), buffer, nullptr
		);
		if (nrslt == 0) { path.clear(); break; }
		if (nrslt < _countof(buffer)) { path = buffer; break; }
		vetbuf.resize(nrslt+1);		// Allocate buffer ...
		nrslt = ::GetFullPathName (	// ...
			path.c_str(), vetbuf.size(), vetbuf.data(), nullptr
		);
		if (!nrslt || nrslt >= vetbuf.size()) path.clear();
		else path.assign(vetbuf.begin(), vetbuf.end()); break;
	}

	// Use the directory of the exe file if we fail to get python 
	// path from options.
	for (std::size_t pos = 0; path.length() <= 0; ) {
		nrslt = GetModuleFileName (	// Try the first time .......
			nullptr, buffer, _countof(buffer)
		);
		if (nrslt == 0) { path.clear(); break; }
		if (nrslt < _countof(buffer)) { path = buffer; 
			pos = path.find_last_not_of(text("\\/"));
			pos = path.find_last_of(text("\\/"),pos);
			path.replace( pos, -1, text("\\python"));
			break;
		}
		vetbuf.resize(nrslt*2);		// Allocate buffer ..........
		nrslt = GetModuleFileName (	// Try the second time ......
			nullptr, vetbuf.data(), vetbuf.size()
		);
		if (nrslt != 0 && nrslt <= vetbuf.size()) {
			path.assign(vetbuf.begin(), vetbuf.end());
			pos = path.find_last_not_of(text("\\/"));
			pos = path.find_last_of(text("\\/"),pos);
			path.replace( pos, -1, text("\\python"));
		} else path.clear(); break;
	}

	// Use current directory if we still can't get the python path .
	for (; path.length() <= 0; ) {
		nrslt = ::GetCurrentDirectory(_countof(buffer), buffer);
		if (nrslt == 0) { path.clear(); break; }
		if (nrslt < _countof(buffer)) { 
			path = buffer; path += text("\\python");
		}
		vetbuf.resize(nrslt+1);		// Allocate buffer ...
		nrslt = ::GetCurrentDirectory(vetbuf.size(),vetbuf.data());
		if (nrslt != 0 && nrslt <= vetbuf.size()) {
			path.assign(vetbuf.begin(), vetbuf.end());
			path.append(text("\\python"));
		} else path.clear(); break;
	}

	// We return false if we still can't get the python path ...
	if(path.length()<=0) return false; path.push_back(text('\\')); 

	// Now, We overwrite the environment variable PYTHONHOME..
	// It's not a necessuary operation ..
	::SetEnvironmentVariable(text("PYTHONHOME"), path.c_str());

	// Locate the python kernel file "pythonxx.dll" ..
	WIN32_FIND_DATA fData = {0}; HANDLE hFile = nullptr; 
	hFile = FindFirstFile((path+text("*.dll")).c_str(), &fData); 
	if (hFile != INVALID_HANDLE_VALUE) {
		do {
			if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				continue;		// We skip all directory .
			_wcslwr_s(fData.cFileName, _countof(fData.cFileName));
			if (wcsstr(fData.cFileName, text("python"))) break;
			else fData.cFileName[0] = text('\0');
		} while (FindNextFile(hFile, &fData));
		FindClose(hFile);		// Finish finding ..
	} else fData.cFileName[0] = text('\0');

	///
	// Now, initialize all python interface dynamically.
	// The reason we query python interface dynamically is to 
	// make sure our plugin can work without python ..
	///
	m_pyModule = ::GetModuleHandle(fData.cFileName);
	if (m_pyModule == nullptr) {
		m_pyModule = ::LoadLibrary((path+fData.cFileName).c_str());
		if (m_pyModule == nullptr) {
			m_pyModule = ::GetModuleHandle(text("python27.dll"));
			if (m_pyModule == nullptr) {
				m_pyModule = ::LoadLibrary(text("python27.dll"));
				if (m_pyModule == nullptr) return false;
			}
		}
	}

	if ( (Py_Finalize = reinterpret_cast<py_finalize>(
		::GetProcAddress(m_pyModule, "Py_Finalize")
	)) == nullptr ) return false;
	if ( (Py_Initialize = reinterpret_cast<py_initialize>(
		::GetProcAddress(m_pyModule, "Py_Initialize")
	)) == nullptr ) return false;
	if ( (Py_SetPythonHome = reinterpret_cast<py_setpythonhome>(
		::GetProcAddress(m_pyModule, "Py_SetPythonHome")
	)) == nullptr ) return false;
	if ( (Py_SetProgramName = reinterpret_cast<py_setprogramname>(
		::GetProcAddress(m_pyModule, "Py_SetProgramName")
	)) == nullptr ) return false;

	if ( (PyFile_AsFile = reinterpret_cast<pyfile_asfile>(
		::GetProcAddress(m_pyModule, "PyFile_AsFile")
	)) == nullptr ) return false;
	if ( (PyFile_FromFile = reinterpret_cast<pyfile_fromfile>(
		::GetProcAddress(m_pyModule, "PyFile_FromFile")
	)) == nullptr ) return false;
	if ( (PyFile_FromString = reinterpret_cast<pyfile_fromstring>(
		::GetProcAddress(m_pyModule, "PyFile_FromString")
	)) == nullptr ) return false;

	if ( (PyEval_InitThreads = reinterpret_cast<pyeval_initthreads>(
		::GetProcAddress(m_pyModule, "PyEval_InitThreads")
	)) == nullptr ) return false;

	if ( (PySys_SetArgv = reinterpret_cast<pysys_setargv>(
		::GetProcAddress(m_pyModule, "PySys_SetArgv")
	)) == nullptr ) return false;
	if ( (PySys_GetObject = reinterpret_cast<pysys_getobject>(
		::GetProcAddress(m_pyModule, "PySys_GetObject")
	)) == nullptr ) return false;
	if ( (PySys_SetObject = reinterpret_cast<pysys_setobject>(
		::GetProcAddress(m_pyModule, "PySys_SetObject")
	)) == nullptr ) return false;
	
	if ( (PyObject_SetAttrString = reinterpret_cast<pyobject_setattrstring>(
		::GetProcAddress(m_pyModule, "PyObject_SetAttrString")
	)) == nullptr ) return false;

	if ( (PyImport_ImportModule = reinterpret_cast<pyimport_importmodule>(
		::GetProcAddress(m_pyModule, "PyImport_ImportModule")
	)) == nullptr ) return false;
	if ( (PyImport_AddModule = reinterpret_cast<pyimport_addmodule>(
		::GetProcAddress(m_pyModule, "PyImport_AddModule")
	)) == nullptr ) return false;

	if ( (PyRun_SimpleString = reinterpret_cast<pyrun_simplestring>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleString")
	)) == nullptr ) return false;
	if ( (PyRun_SimpleStringFlags = reinterpret_cast<pyrun_simplestringflags>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleStringFlags")
	)) == nullptr ) return false;
	if ( (PyRun_SimpleFile = reinterpret_cast<pyrun_simplefile>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleFile")
	)) == nullptr ) return false;
	/*if ( (PyRun_SimpleFileFlags = reinterpret_cast<pyrun_simplefileflags>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleFileFlags")
	)) == nullptr ) return false;*/
	if ( (PyRun_SimpleFileEx = reinterpret_cast<pyrun_simplefileex>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleFileEx")
	)) == nullptr ) return false;
	if ( (PyRun_SimpleFileExFlags = reinterpret_cast<pyrun_simplefileexflags>(
		::GetProcAddress(m_pyModule, "PyRun_SimpleFileExFlags")
	)) == nullptr ) return false;

	// Initialize the python environment, prepare the hooks
	//if (Py_SetPythonHome) Py_SetPythonHome(cvt.to_bytes(path).c_str());
	//if (Py_SetProgramName) Py_SetProgramName(cvt.to_bytes(buffer).c_str());
	if (Py_Initialize) Py_Initialize();
	if (PyEval_InitThreads) PyEval_InitThreads();
	// ...

	///
	// Now, initialize all MSVCR90 interface dynamically.
	///
	ACTCTX ctx = {sizeof(ACTCTX), 0}; ULONG_PTR actToken = 0;
	ctx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID|ACTCTX_FLAG_HMODULE_VALID;
	ctx.hModule = m_pyModule; ctx.lpResourceName = MAKEINTRESOURCE(2);
	HANDLE hCtx = CreateActCtx(&ctx);
	if(hCtx != INVALID_HANDLE_VALUE) {
		if ( ActivateActCtx(hCtx, &actToken) ) {
			m_vcModule = ::GetModuleHandle(text("MSVCR90.dll"));
			if (m_vcModule == NULL) {
				m_vcModule = ::LoadLibrary(text("MSVCR90.dll"));
				if (m_vcModule == NULL) return false;
			}
			if ( (open_osfhandle = reinterpret_cast<__open_osfhandle>(
				::GetProcAddress(m_vcModule, "_open_osfhandle")
			)) == nullptr ) return false;
			if ( (setvbuf = reinterpret_cast<__setvbuf>(
				::GetProcAddress(m_vcModule, "setvbuf")
			)) == nullptr ) return false;
			if ( (fdopen = reinterpret_cast<__fdopen>(
				::GetProcAddress(m_vcModule, "_fdopen")
			)) == nullptr ) return false;
			if ( (fclose = reinterpret_cast<__fclose>(
				::GetProcAddress(m_vcModule, "fclose")
			)) == nullptr ) return false;
			// Deactive active context after finished ..
			DeactivateActCtx(0, actToken);
		}
		// Now close context ...
		ReleaseActCtx(hCtx); 
	}

	return true;	// All done, return true ...
}
void CPython::free()
{
	// Free the python module .
	if (Py_Finalize) Py_Finalize();
	Py_Finalize						= nullptr;
	Py_Initialize						= nullptr;
	PyFile_AsFile						= nullptr;
	Py_SetPythonHome					= nullptr;
	Py_SetProgramName					= nullptr;
	PyFile_FromString					= nullptr;
	PyEval_InitThreads				= nullptr;
	PyRun_SimpleStringFlags			= nullptr;
	PyRun_SimpleFileExFlags			= nullptr;
	if (m_pyModule) FreeLibrary(m_pyModule); 
	m_pyModule							= nullptr;
}


int CPython::ExecStop(unsigned long timeout)
{
	return 0;
}
int CPython::ExecEcho(const std::wstring &cmds, std::wstring &err, HANDLE hfile)
{
	int id = 0; FILE *pfile = nullptr; DWORD count = 0;
	PyObject *poldout, *polderr, *pnewout, *pnewerr; 
	if (cmds.length() <= 0) {
		err = text("No python command found"); return 1;
	}
	if (DuplicateHandle ( 
		GetCurrentProcess(), hfile, GetCurrentProcess(), &hfile, 
		0, false, DUPLICATE_SAME_ACCESS 
	)) { 
		id = open_osfhandle((intptr_t)hfile, _O_WRONLY); 
		pfile = fdopen(id,"w"); setvbuf(pfile,nullptr,_IONBF,1024); 
		poldout = PySys_GetObject("stdout"); 
		polderr = PySys_GetObject("stderr"); 
		pnewout = PyFile_FromFile(pfile, "logger", "w", nullptr); 
		pnewerr = PyFile_FromFile(pfile, "logger", "w", nullptr); 
		PySys_SetObject("stdout", pnewout); 
		PySys_SetObject("stderr", pnewerr); 
	} else poldout = polderr = pnewout = pnewerr = nullptr; 

	std::wstring_convert<std::codecvt_utf8_utf16<wchar>> cvt; 
	std::string str = cvt.to_bytes(cmds); 
	std::size_t pos = str.find(text('.')); 
	if (pos != std::string::npos) { 
		std::string mod = str.substr(0, pos); 
		PyObject* dest = PyImport_ImportModule(mod.c_str()); 
		PyObject* main = PyImport_AddModule("__main__"); 
		PyObject_SetAttrString(main, mod.c_str(), dest); 
	} 
	str.insert(0, "print ");
	int irslt = PyRun_SimpleString(str.c_str());
	if (irslt != 0) err = text("Internal error that PyRun_SimpleString fail"); 
	else err = text("Execute python expression successfully .."); 

	if (pnewout != nullptr) PySys_SetObject("stdout", poldout); 
	if (pnewerr != nullptr) PySys_SetObject("stderr", polderr); 
	if (pfile != nullptr) fclose(pfile); return irslt; 
}
int CPython::ExecModl(const std::wstring&, std::wstring&, HANDLE hfile)
{
	return 0;
}
int CPython::ExecCmds(const std::wstring &cmds, std::wstring &err, HANDLE hfile)
{
	int id = 0; FILE *pfile = nullptr; DWORD count = 0;
	PyObject *poldout, *polderr, *pnewout, *pnewerr; 
	if (cmds.length() <= 0) {
		err = text("No python command found"); return 1;
	}
	if (DuplicateHandle (
		GetCurrentProcess(), hfile, GetCurrentProcess(), &hfile, 
		0, false, DUPLICATE_SAME_ACCESS
	)) {
		id = open_osfhandle((intptr_t)hfile, _O_WRONLY);
		pfile = fdopen(id,"w"); setvbuf(pfile,nullptr,_IONBF,1024);
		poldout = PySys_GetObject("stdout"); 
		polderr = PySys_GetObject("stderr"); 
		pnewout = PyFile_FromFile(pfile, "logger", "w", nullptr);
		pnewerr = PyFile_FromFile(pfile, "logger", "w", nullptr);
		PySys_SetObject("stdout", pnewout);
		PySys_SetObject("stderr", pnewerr);
	} else poldout = polderr = pnewout = pnewerr = nullptr;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar>> cvt; 
	std::string str = cvt.to_bytes(cmds); 
	int irslt = PyRun_SimpleString(str.c_str());
	if (irslt != 0) err = text("Internal error that PyRun_SimpleString fail"); 
	else err = text("Execute python commands successfully .."); 

	if (pnewout != nullptr) PySys_SetObject("stdout", poldout); 
	if (pnewerr != nullptr) PySys_SetObject("stderr", polderr); 
	if (pfile != nullptr) fclose(pfile); return irslt; 
}
int CPython::ExecFunc(const std::vector<std::wstring> &argv, std::wstring &err, HANDLE hfile)
{
	std::vector<std::wstring>::const_iterator itr, eitr; 
	itr = argv.cbegin(); eitr = argv.cend(); err = *itr; 
	if (argv.size() <= 0) {
		err = text("No python command function found"); return 1;
	}
	err.push_back(text('('));
	for (++itr; itr != eitr; ++itr) { 
		err.append(*itr); err.push_back(text(',')); 
	} 
	if (err.back() == text(',')) err.pop_back(); 
	err.push_back(text(')')); 
	Addtolist(1,1,text("Execute Function: %s"),err.c_str());

	int id = 0; FILE *pfile = nullptr; DWORD count = 0;
	PyObject *poldout, *polderr, *pnewout, *pnewerr; 
	if (DuplicateHandle (
		GetCurrentProcess(), hfile, GetCurrentProcess(), &hfile, 
		0, false, DUPLICATE_SAME_ACCESS
	)) {
		id = open_osfhandle((intptr_t)hfile, _O_WRONLY); 
		pfile = fdopen(id,"w"); setvbuf(pfile,nullptr,_IONBF,1024);
		poldout = PySys_GetObject("stdout");
		polderr = PySys_GetObject("stderr");
		pnewout = PyFile_FromFile(pfile, "logger", "w", nullptr);
		pnewerr = PyFile_FromFile(pfile, "logger", "w", nullptr);
		PySys_SetObject("stdout", pnewout);
		PySys_SetObject("stderr", pnewerr);
	} else poldout = polderr = pnewout = pnewerr = nullptr;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar>> cvt; 
	std::string str = cvt.to_bytes(err); 
	std::size_t pos = str.find(text('.')); 
	if (pos != std::string::npos) { 
		std::string mod = str.substr(0, pos); 
		PyObject* dest = PyImport_ImportModule(mod.c_str()); 
		PyObject* main = PyImport_AddModule("__main__"); 
		PyObject_SetAttrString(main, mod.c_str(), dest); 
	} 
	str.insert(0, "print ");
	int irslt = PyRun_SimpleString(str.c_str());
	if (irslt != 0) err = text("Internal error that PyRun_SimpleString fail"); 
	else err = text("Execute python function successfully .."); 

	if (pnewout != nullptr) PySys_SetObject("stdout", poldout);
	if (pnewerr != nullptr) PySys_SetObject("stderr", polderr);
	if (pfile != nullptr) fclose(pfile); return irslt;
}
int CPython::ExecFile(const std::vector<std::wstring> &argv, std::wstring &err, HANDLE hfile)
{
	int id = 0; FILE *pfile = nullptr; DWORD count = 0; 
	PyObject *poldout, *polderr, *pnewout, *pnewerr; 
	if (argv.size() <= 0) { 
		err = text("No python script file found"); return 1; 
	} 
	if (DuplicateHandle (
		GetCurrentProcess(), hfile, GetCurrentProcess(), &hfile, 
		0, false, DUPLICATE_SAME_ACCESS
	)) {
		id = open_osfhandle((intptr_t)hfile, _O_WRONLY); 
		pfile = fdopen(id,"w"); setvbuf(pfile,nullptr,_IONBF,1024);
		poldout = PySys_GetObject("stdout"); 
		polderr = PySys_GetObject("stderr");
		pnewout = PyFile_FromFile(pfile, "logger", "w", nullptr);
		pnewerr = PyFile_FromFile(pfile, "logger", "w", nullptr);
		PySys_SetObject("stdout", pnewout);
		PySys_SetObject("stderr", pnewerr);
	} else poldout = polderr = pnewout = pnewerr = nullptr;

	// Pack up the arguments ..
	std::vector<char*> args; int irslt = 0; 
	std::vector<std::wstring>::const_iterator itr, eitr; 
	std::wstring_convert<std::codecvt_utf8_utf16<wchar>> cvt; 
	itr = argv.cbegin(); eitr = argv.cend(); 
	for (size_t len = 0; itr != eitr; ++itr) { 
		// Allocate buffer each time, not good ..
		std::string str = cvt.to_bytes(*itr); 
		len = str.length(); char *arg = new char[len+1]; 
		strncpy_s(arg,len+1,str.data(),len); arg[len] = '\0'; 
		args.push_back(arg);
	}

	PySys_SetArgv(args.size(), args.data());	// pass args .
	PyObject *pobj = PyFile_FromString(args.at(0), "r");
	if (pobj == nullptr) {
		err = text("Internal error that PyFile_FromString fail"); 
		irslt = -1;
	} else {
		PyRun_SimpleFileEx(PyFile_AsFile(pobj), args.at(0), true);
		err = text("Execute python script file successfully .."); 
		irslt = 00;
	}

	// Free resource ...
	std::vector<char*>::iterator sitr, seitr;
	sitr = args.begin(); seitr = args.end();
	for (sitr; sitr != seitr; ++sitr) { if (*sitr) delete [] *sitr; }
	args.clear();

	if (pnewout != nullptr) PySys_SetObject("stdout", poldout);
	if (pnewerr != nullptr) PySys_SetObject("stderr", polderr);
	if (pfile != nullptr) fclose(pfile); return irslt;
}

