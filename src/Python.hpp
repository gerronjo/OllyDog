
#include <vector>
#include <string>
#include <codecvt>
#include <algorithm>

#include <fcntl.h>
#include <stdio.h>
#include <windows.h>

#include <python.h>

#include "Options.hpp"

class CPython : public COptions
{
public:
private:
	#define PYTHON_INVALID			0x00000000
	#define PYTHON_WORKING			0x00000000
	static size_t						m_nReffer;
	static	ulong						m_uStatus;
protected:
	// The main module of python2.x ..
	static HMODULE					m_pyModule;
	// Py_xxxxx infterface ...
	typedef void (*py_finalize) (void);
	static py_finalize				Py_Finalize;
	typedef void (*py_initialize) (void);
	static py_initialize				Py_Initialize;
	typedef void (*py_setpythonhome) (const char*);
	static py_setpythonhome			Py_SetPythonHome;
	typedef void (*py_setprogramname) (const char*);
	static py_setprogramname			Py_SetProgramName;
	// PyFile_xxxx interfaces ..
	typedef FILE* (*pyfile_asfile) (PyObject*); 
	static pyfile_asfile				PyFile_AsFile; 
	typedef PyObject* (*pyfile_fromfile) (FILE*, char*, char*, int (*)(FILE*)); 
	static pyfile_fromfile			PyFile_FromFile;
	typedef PyObject* (*pyfile_fromstring) (const char*, const char*);
	static pyfile_fromstring			PyFile_FromString;
	// PyEval_xxxx interfaces ..
	typedef void (*pyeval_initthreads) (void);
	static pyeval_initthreads		PyEval_InitThreads;
	// PySys_xxxx interfaces ..
	typedef void (*pysys_setargv) (int, char**);
	static pysys_setargv				PySys_SetArgv;
	typedef PyObject* (*pysys_getobject) (char*);
	static pysys_getobject			PySys_GetObject;
	typedef int (*pysys_setobject) (char*, PyObject*);
	static pysys_setobject			PySys_SetObject;
	// PyObject_xxxx interfaces ..
	typedef int (*pyobject_setattrstring) (PyObject*, const char*, PyObject*);
	static pyobject_setattrstring	PyObject_SetAttrString;
	typedef int (*pyobject_print) (PyObject*, FILE*, int);
	static pyobject_print				PyObject_Print;
	// PyImport_xxxx interfaces ..
	typedef PyObject* (*pyimport_importmodule)(const char*);
	static pyimport_importmodule		PyImport_ImportModule;
	typedef PyObject* (*pyimport_addmodule)(const char*);
	static pyimport_addmodule		PyImport_AddModule;
	// PyRun_xxxx interfaces ..
	typedef int (*pyrun_simplestring) (const char*);
	static pyrun_simplestring		PyRun_SimpleString;
	typedef int (*pyrun_simplestringflags) (const char*, PyCompilerFlags*);
	static pyrun_simplestringflags	PyRun_SimpleStringFlags;
	typedef int (*pyrun_simplefile) (FILE*, const char*);
	static pyrun_simplefile			PyRun_SimpleFile;
	typedef int (*pyrun_simplefileflags) (FILE*, const char*, PyCompilerFlags*);
	static pyrun_simplefileflags		PyRun_SimpleFileFlags;
	typedef int (*pyrun_simplefileex) (FILE*, const char*, int);
	static pyrun_simplefileex		PyRun_SimpleFileEx;
	typedef int (*pyrun_simplefileexflags) (FILE*, const char*, int, PyCompilerFlags*);
	static pyrun_simplefileexflags	PyRun_SimpleFileExFlags;

	// The main module of MSVCR90 ..
	static HMODULE					m_vcModule;
	// ....
	typedef int (__cdecl *__open_osfhandle) (intptr_t, int);
	static __open_osfhandle			open_osfhandle;
	typedef FILE* (__cdecl *__fdopen) (int, const char*);
	static __fdopen					fdopen;
	typedef int (__cdecl *__fclose) (FILE*);
	static __fclose					fclose;
	typedef int (__cdecl *__setvbuf) (FILE*, char*, int, size_t);
	static __setvbuf					setvbuf;

public:
	CPython(); ~CPython(); 
	virtual int ExecStop(unsigned long);	// Not suport yet ..
	virtual int ExecEcho(const std::wstring&, std::wstring&, HANDLE hfile = nullptr);
	virtual int ExecModl(const std::wstring&, std::wstring&, HANDLE hfile = nullptr);
	virtual int ExecCmds(const std::wstring&, std::wstring&, HANDLE hfile = nullptr);
	virtual int ExecFunc(const std::vector<std::wstring>&, std::wstring&, HANDLE hfile = nullptr);
	virtual int ExecFile(const std::vector<std::wstring>&, std::wstring&, HANDLE hfile = nullptr);
private:
protected:
	virtual bool init(); virtual void free();
};


