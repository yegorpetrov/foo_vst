// svc class is the fb2k dsp service wrapper
// It doesn't use VST directly as it has shorter lifetime.
// Instead, it acquires a _vst_ instance which is managed by
// reference counting.

#pragma once
#ifndef H_SVC
#define H_SVC

class vst;

/* :TRICKY: Template abuse as workaround against FB2K API limitations. */
template<unsigned short N>
class svc : public dsp_impl_base
{
private:
	vst* m_vst;

public:
	svc(dsp_preset const & in);
	~svc(void);

	//---------- Unused stuff --------------
	void on_endofplayback(abort_callback &) {}
	void on_endoftrack(abort_callback &) {}
	void flush() {}
	double get_latency() { return 0; }
	bool need_track_change_mark() {	return false; }
	//---------- Unused stuff --------------

	bool on_chunk(audio_chunk * chunk, abort_callback &);
	static GUID g_get_guid();
	static void g_get_name(pfc::string_base & p_out);
	static bool g_get_default_preset(dsp_preset & p_out);
	static bool g_have_config_popup();
	static void g_show_config_popup(const dsp_preset & p_data,
		HWND p_parent, dsp_preset_edit_callback & p_callback);
};

#endif