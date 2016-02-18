#include "StdAfx.h"
#include "manager.h"
#include "svc.h"
#include "vst.h"
#include "editor.h"
#include "shared.h"
#include "vst_config.h"

using namespace stringcvt;

template<unsigned short N>
svc<N>::svc(dsp_preset const & in) : m_vst(NULL)
{
	m_vst = vst::g_acquire_vst(in, manager::get_vst_entry(N), vst::owner_dspsvc);
}

template<unsigned short N>
svc<N>::~svc(void)
{
	m_vst->declick();
	vst::g_release_vst(m_vst, vst::owner_dspsvc);
}

template<unsigned short N>
void svc<N>::g_get_name(pfc::string_base & p_out)
{
	p_out = manager::get_vst_entry(N).get_product();
}

template<unsigned short N>
bool svc<N>::on_chunk(audio_chunk * chunk, abort_callback & p_abort_cb)
{
	return m_vst->process_chunk(chunk, p_abort_cb);
}

template<unsigned short N>
bool svc<N>::g_get_default_preset(dsp_preset & p_out)
{
	// :TRICKY: We need all dsp_presets to be always unique
	// since they are the only way to identify vst instances
	dsp_preset_builder builder;
	srand(GetTickCount());
	builder << rand();
	builder.finish(g_get_guid(), p_out);
	return true;
}

template<unsigned short N>
void svc<N>::g_show_config_popup(const dsp_preset & p_data,
	HWND p_parent, dsp_preset_edit_callback & p_callback)
{
	// :TRICKY: Although we have randomized data in presets to identify
	// VST instances, the same presets can be recalled from chain presets.
	// So if we're in the converter window, we need a new instance.
	// This is error-prone but is the lesser of two evils.
	string8 wnd = string_utf8_from_window(GetAncestor(p_parent, GA_ROOT));
	bool force_new = (wnd.find_first("Conv") != ~0);

	vst* v = NULL;
	if (force_new)
	{
		v = new vst(manager::get_vst_entry(N));
		v->set_preset(p_data);
	}
	else
	{
		v = vst::g_acquire_vst(
			p_data, manager::get_vst_entry(N), vst::owner_editor);
	}
	
	if (!v->is_editor_opened() && v->is_ok())
	{
		if (v->has_editor())
		{
			v->set_editor_opened(true);
			editor e(p_data, manager::get_vst_entry(N), p_callback, v);
			e.DoModal(p_parent);
			v->set_editor_opened(false);
		}
		else
		{
			popup_message::g_show(
				"This feature isn't implemented for non-GUI plug-ins yet. Sorry.", "Not implemented");
		}
	}
	else if (!v->is_ok())
	{
		popup_message::g_complain(v->get_msg());
	}
	/*vst_config * gui = new vst_config();
	gui->Create(0, CWindow::rcDefault, L"Image Test",
		WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
	gui->ShowWindow(SW_SHOWNORMAL);
	Sleep(5000);*/
	if (force_new)
	{
		delete v;
	}
	else
	{
		vst::g_release_vst(v, vst::owner_editor);
	}
}

template<unsigned short N>
bool svc<N>::g_have_config_popup()
{
	return true;
}

template<unsigned short N>
GUID svc<N>::g_get_guid()
{
	return manager::get_vst_entry(N).get_guid();
}


/*BOOL WINAPI DllMain(HINSTANCE p_hinstdll, DWORD p_reason, LPVOID p_reserved)
{
	if (p_reason == DLL_PROCESS_ATTACH)
	{
		// It was done here previously.
		// We can't instantiate services at this point.
	}
	else if (p_reason == DLL_PROCESS_DETACH)
	{
	}
	return true;
}*/

//---------->8---------->8---------->8---------->8---------->8---------->8------
////////////////////////////////////////////////////////////////////////////////
/////////////////// CUSTOM PLUG-IN ENTRY POINT GOES HERE ///////////////////////
////////////////////////////////////////////////////////////////////////////////
//----------8<----------8<----------8<----------8<----------8<----------8<------
//
// Must replace component_client.cpp

#include "..\..\foobar2000\SDK\component.h"

static HINSTANCE g_hIns;
static pfc::string_simple g_name,g_full_path;
static bool g_services_available = false, g_initialized = false;

namespace core_api
{
	HINSTANCE get_my_instance() { return g_hIns; }
	HWND get_main_window()
	{
		PFC_ASSERT( g_foobar2000_api != NULL );
		return g_foobar2000_api->get_main_window();
	}
	const char* get_my_file_name() { return g_name;}
	const char* get_my_full_path() { return g_full_path;}
	bool are_services_available() {	return g_services_available; }
	bool assert_main_thread()
	{
		return (g_services_available && g_foobar2000_api) ?
			g_foobar2000_api->assert_main_thread() : true;
	}
	void ensure_main_thread() {	if (!is_main_thread()) uBugCheck();	}
	bool is_main_thread()
	{
		return (g_services_available && g_foobar2000_api) ?
			g_foobar2000_api->is_main_thread() : true;
	}
	const char* get_profile_path()
	{
		PFC_ASSERT( g_foobar2000_api != NULL );
		return g_foobar2000_api->get_profile_path();
	}
	bool is_shutting_down()
	{
		return (g_services_available && g_foobar2000_api) ?
			g_foobar2000_api->is_shutting_down() : g_initialized;
	}
	bool is_initializing()
	{
		return (g_services_available && g_foobar2000_api) ?
			g_foobar2000_api->is_initializing() : !g_initialized;
	}
	bool is_portable_mode_enabled() {
		PFC_ASSERT( g_foobar2000_api != NULL );
		return g_foobar2000_api->is_portable_mode_enabled();
	}
	bool is_quiet_mode_enabled() {
		PFC_ASSERT( g_foobar2000_api != NULL );
		return g_foobar2000_api->is_quiet_mode_enabled();
	}
}

namespace {
	class foobar2000_client_impl : public foobar2000_client, private foobar2000_component_globals
	{
	public:
		t_uint32 get_version() {return FOOBAR2000_CLIENT_VERSION;}
		pservice_factory_base get_service_list() {return service_factory_base::__internal__list;}
		void get_config(stream_writer * p_stream,abort_callback & p_abort) {
			cfg_var::config_write_file(p_stream,p_abort);
		}
		void set_config(stream_reader * p_stream,abort_callback & p_abort) {
			cfg_var::config_read_file(p_stream,p_abort);
		}
		void set_library_path(const char * path,const char * name) {
			g_full_path = path;
			g_name = name;
		}
		void services_init(bool val) {
			if (val) g_initialized = true;
			g_services_available = val;
		}
		bool is_debug() { return __debug; }
	};
}

static foobar2000_client_impl g_client;

extern "C"
{
	__declspec(dllexport) foobar2000_client *
		_cdecl foobar2000_get_interface(foobar2000_api * p_api,HINSTANCE hIns)
	{
		g_hIns = hIns;
		g_foobar2000_api = p_api;
		if (p_api != NULL)
		{
			int n = manager::init();
			/* :TRICKY:
			Template abuse as workaround against FB2K API limitations.
			___
			This is the only way to get the same class to work
			as many different DSP entries. Delegates copypaste
			job to the compiler. */
			if (n >  0) static dsp_factory_t<svc< 0>> entry;
			if (n >  1) static dsp_factory_t<svc< 1>> entry;
			if (n >  2) static dsp_factory_t<svc< 2>> entry;
			if (n >  3) static dsp_factory_t<svc< 3>> entry;
			if (n >  4) static dsp_factory_t<svc< 4>> entry;
			if (n >  5) static dsp_factory_t<svc< 5>> entry;
			if (n >  6) static dsp_factory_t<svc< 6>> entry;
			if (n >  7) static dsp_factory_t<svc< 7>> entry;
			if (n >  8) static dsp_factory_t<svc< 8>> entry;
			if (n >  9) static dsp_factory_t<svc< 9>> entry;
			if (n > 10) static dsp_factory_t<svc<10>> entry;
			if (n > 11) static dsp_factory_t<svc<11>> entry;
			if (n > 12) static dsp_factory_t<svc<12>> entry;
			if (n > 13) static dsp_factory_t<svc<13>> entry;
			if (n > 14) static dsp_factory_t<svc<14>> entry;
			if (n > 15) static dsp_factory_t<svc<15>> entry;
			if (n > 16) static dsp_factory_t<svc<16>> entry;
			if (n > 17) static dsp_factory_t<svc<17>> entry;
			if (n > 18) static dsp_factory_t<svc<18>> entry;
			if (n > 19) static dsp_factory_t<svc<19>> entry;
		}
		return &g_client;
	}
}