#include "StdAfx.h"
#include "menu.h"

// Main thread APIs wrapper
class dsp_cfg_mainthread : public main_thread_callback {
	dsp_preset_impl new_preset;
	dsp_preset_impl old_preset;
	HANDLE h_event;

public:
	dsp_cfg_mainthread(
		const dsp_preset & n, const dsp_preset & p, HANDLE ev) :
	  new_preset(n), old_preset(p), h_event(ev)	{	}

	~dsp_cfg_mainthread()
	{
		SetEvent(h_event); // It's safe to process the next callback now
	}

	void callback_run() override
	{
		// Find and replace
		dsp_chain_config_impl chain;
		static_api_ptr_t<dsp_config_manager> manager;
		manager->get_core_settings(chain);
		for (int i = 0, j = chain.get_count(); i < j; i++)
		{
			if (chain.get_item(i) == old_preset)
			{
				chain.replace_item(new_preset, i);
				manager->set_core_settings(chain);
				break;
			}
		}
	}
};

static pfc::list_t<dsp_preset_impl> opened_configs;

class dsp_menu_callback : public dsp_preset_edit_callback {
	dsp_preset_impl m_data;
	HANDLE h_event;

public:
	dsp_menu_callback(const dsp_preset & p_data) : m_data(p_data)
	{
		h_event = CreateEvent(NULL, FALSE, TRUE, L"DspMenuCallbackSync");
		opened_configs.add_item(m_data);
		dsp_entry::g_show_config_popup_v2(p_data, GetDesktopWindow(), *this);
		opened_configs.remove_item(m_data);
	}

	~dsp_menu_callback()
	{
		CloseHandle(h_event);
	}

	void on_preset_changed(const dsp_preset & p_data)
	{
		// After callback has been processed
		WaitForSingleObject(h_event, 5000);
		ResetEvent(h_event);
		opened_configs.replace_item(opened_configs.find_item(m_data),p_data);
		service_impl_t<dsp_cfg_mainthread>* cb =
			new service_impl_t<dsp_cfg_mainthread>(p_data, m_data, h_event);
		static_api_ptr_t<main_thread_callback_manager>()->add_callback(cb);
		m_data.copy(p_data);
	}

	static bool is_cfg_opened(const dsp_preset & p_data)
	{
		return opened_configs.find_item(p_data) < INFINITE;
	}
};

DWORD __stdcall run_config(void *arg)
{
	dsp_preset_impl * preset = static_cast<dsp_preset_impl*>(arg);
	dsp_menu_callback dmc(*preset);
	delete preset;
	return 0;
}

static const GUID g_mainmenu_group_id =
	{ 0x44963e7a, 0x4b2a, 0x4588,
	{ 0xb0, 0x17, 0xa8, 0x69, 0x18, 0xcb, 0x8a, 0xa5 } };
static mainmenu_group_popup_factory g_mainmenu_group(g_mainmenu_group_id,
	mainmenu_groups::view, mainmenu_commands::sort_priority_base, "DSP");

class mainmenu_commands_sample : public mainmenu_commands {
	dsp_chain_config_impl chain;

public:
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		get_command_count(); // Gotta refresh first
		if (p_index >= chain.get_count() ||
			!dsp_entry::g_have_config_popup(
				chain.get_item(p_index).get_owner()) ||
				dsp_menu_callback::is_cfg_opened(chain.get_item(p_index)))
		{
			return;
		}
		else
		{
			// Will be deleted in run_config()
			void * preset = static_cast<void*>(new dsp_preset_impl(chain.get_item(p_index)));
			CloseHandle(CreateThread(NULL, 0, run_config, preset, 0, 0));
		}
	}

	static const int cmd_max = 15;

	t_uint32 get_command_count()
	{
		chain.remove_all();
		static_api_ptr_t<dsp_config_manager> manager;
		manager->get_core_settings(chain);
		return cmd_max;
	}
	
	GUID get_parent() {	return g_mainmenu_group_id;	}

	GUID get_command(t_uint32 p_index) {
		// {C047C332-E32C-4833-9690-27E16BE25468}
		GUID base = { 0xc047c332, 0xe32c, 0x4833,
		{ 0x96, 0x90, 0x27, 0xe1, 0x6b, 0xe2, 0x54, 0x68 } };
		base.Data3 = base.Data3 + p_index;
		return base;
	}

	void get_name(t_uint32 p_index, pfc::string_base & p_out) {
		p_out.reset();
		p_out << "DSP #" << p_index + 1 << " config";
	}

	bool get_description(t_uint32 p_index,pfc::string_base & p_out) {
		p_out = "Opens DSP configuration window.";
		return true;
	}
	
	bool get_display(
		t_uint32 p_index,
		pfc::string_base & p_text,
		t_uint32 & p_flags)
	{
		if (p_index >= chain.get_count()) return false;
		bool b = dsp_menu_callback::is_cfg_opened(chain.get_item(p_index));
		if (!dsp_entry::g_have_config_popup(
			chain.get_item(p_index).get_owner()))
		{
			p_flags |= mainmenu_commands::flag_disabled;
		}
		if (dsp_menu_callback::is_cfg_opened(chain.get_item(p_index)))
		{
			p_flags |= mainmenu_commands::flag_disabled |
				mainmenu_commands::flag_checked;
		}
		dsp_entry::g_name_from_guid(p_text,
			chain.get_item(p_index).get_owner());
		return true;
	}
};

static mainmenu_commands_factory_t<mainmenu_commands_sample>
		g_mainmenu_commands_sample_factory;