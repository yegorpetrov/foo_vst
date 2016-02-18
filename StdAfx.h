#pragma once
#ifndef H_STDAFX
#define H_STDAFX

#define _CRT_SECURE_NO_WARNINGS
#include "pluginterfaces/vst2.x/aeffectx.h"
#include "..\..\foobar2000\ATLHelpers\ATLHelpers.h"
#include "Helpers\regkey.h"

using namespace pfc;

// {ED53CCA9-658F-421f-8393-D362CEB5520B}
static const GUID guid_adv_vst =
	{ 0xed53cca9, 0x658f, 0x421f, { 0x83, 0x93, 0xd3, 0x62, 0xce, 0xb5, 0x52, 0xb } };

// uLoadString from shared.h doesn't work for some reason
BOOL LoadStringRsrc(HINSTANCE p_inst, UINT p_id, pfc::string_base & p_out);
string8 get_rsrc_string(UINT p_id);

#ifdef _DEBUG
	const static bool __debug = true;
#else
	const static bool __debug = false;
#endif

static const char* version_str = "0.9.0.4";
static const int version_int = 90;

#endif