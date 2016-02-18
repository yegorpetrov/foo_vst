#include "stdafx.h"

BOOL LoadStringRsrc(HINSTANCE p_inst, UINT p_id, pfc::string_base & p_out)
{
	wchar_t * buf = NULL;
	int len = LoadStringW(p_inst, p_id, reinterpret_cast<LPWSTR>(&buf), 0);
	p_out.reset();
	p_out << stringcvt::string_utf8_from_os(buf, len);
	return len > 0 ? TRUE : FALSE;
}

string8 get_rsrc_string(UINT p_id)
{
	string8 str;
	LoadStringRsrc(core_api::get_my_instance(), p_id, str);
	return str;
}