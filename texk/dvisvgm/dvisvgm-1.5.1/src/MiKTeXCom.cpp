/*************************************************************************
** MiKTeXCom.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include <config.h>
#include <comdef.h>
#include <string>
#include "MessageException.h"
#include "MiKTeXCom.h"
#include "macros.h"

using namespace std;


MiKTeXCom::MiKTeXCom () : _session(0) {
	if (FAILED(CoInitialize(0)))
		throw MessageException("COM library could not be initialized\n");
#ifdef _MSC_VER
	HRESULT hres = _session.CreateInstance(L"MiKTeX.Session");
#elif defined(__WIN64__)
	HRESULT hres = CoCreateInstance(CLSID_MiKTeXSession2_9, 0, CLSCTX_LOCAL_SERVER, IID_ISession2, (void**)&_session);
#else
	HRESULT hres = CoCreateInstance(CLSID_MiKTeXSession2_9, 0, CLSCTX_INPROC_SERVER, IID_ISession2, (void**)&_session);
#endif
	if (FAILED(hres))
		throw MessageException("MiKTeX.Session could not be initialized");
}


MiKTeXCom::~MiKTeXCom () {
#ifdef _MSC_VER
	_session.Release();
#else
	_session->Release();
#endif
	_session = 0; // avoid automatic calling of Release() after CoUninitialize()
	CoUninitialize();
}


string MiKTeXCom::getVersion () {
#ifdef _MSC_VER
	MiKTeXSetupInfo info = _session->GetMiKTeXSetupInfo();
#else
	MiKTeXSetupInfo info;
	_session->GetMiKTeXSetupInfo(&info);
#endif
	_bstr_t version = info.version;
	return string(version);
}


string MiKTeXCom::getBinDir () {
#ifdef _MSC_VER
	MiKTeXSetupInfo info = _session->GetMiKTeXSetupInfo();
#else
	MiKTeXSetupInfo info;
	_session->GetMiKTeXSetupInfo(&info);
#endif
	_bstr_t bindir = info.binDirectory;
	return string(bindir);
}


const char* MiKTeXCom::findFile (const char *fname) {
	try {
		_bstr_t path;
		static string ret;
#ifdef _MSC_VER
		HRESULT hres = _session->FindFile(fname, path.GetAddress());
		bool found = (hres != 0);
#else
		VARIANT_BOOL found_var;
		_session->FindFile(_bstr_t(fname), path.GetAddress(), &found_var);
		bool found = (found_var == VARIANT_TRUE);
#endif
		if (found) {
			ret = _bstr_t(path);
			return ret.c_str();
		}
		return 0;
	}
	catch (_com_error &e) {
		throw MessageException((const char*)e.Description());
	}
}
