// ensure.h - throw const char* error on failed assertion
// Copyright (c) 2005 Keith A. Lewis mailto:kal@kalx.net
#pragma once
#ifndef ENSURE_H
#define ENSURE_H

#ifndef WIN32
#define _T(x) x
#endif

#define ENSURE_HASH(x) #x
#define ENSURE_STRING(x) ENSURE_HASH(x)
#pragma warning(disable: 4297)
// like assert, but throws const char*
#define ensure(e) { if (!(e)) throw _T(__FILE__)  \
	_T("(") _T(ENSURE_STRING(__LINE__)) _T(") : ensure: \"") \
	_T(#e) _T("\" failed"); }

#endif // ENSURE_H