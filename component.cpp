#include "stdafx.h"

DECLARE_COMPONENT_VERSION("VST 2.4 adapter", version_str,
stringcvt::string_utf8_from_os(
L"© 2011 Yegor Petrov (entrase@ya.ru)\r\n\
\r\n\
Thanks: Audiophile (beta-testing)\r\n\
\r\n\
VST PlugIn Technology by Steinberg\r\n\
Copyright © 2006 Steinberg Media Technologies GmbH\r\n\
\r\n\
Registry Key wrapper by Keith A. Lewis (kal@kalx.net)\r\n\
Tooltip helper class by Alain Rist (ar@navpoch.com)"));

VALIDATE_COMPONENT_FILENAME("foo_vst.dll");

static advconfig_branch_factory vst_adv_branch("VST", guid_adv_vst, advconfig_branch::guid_root, 0.0);