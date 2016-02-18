// vst instances are managed by reference counting implemented
// in a static scope. Something is added to ensure that VSTs
// loading and unloading are performed in the same thread.

#pragma once
#ifndef H_VST
#define H_VST

#include "entry.h"

class vst : public entry, public instanceTrackerV2<vst>
{
	friend void erase_svc_flag(vst* p_vst);
	bool m_pass_svc_flag_removal;
public:
	enum owner_id { owner_dspsvc, owner_instguard, owner_editor };
	vst(const entry & p_entry, bool p_unload = false);
	~vst(void);
	// Simple reference-counting management of VST instances
	static vst* g_acquire_vst(const dsp_preset &, const entry &, owner_id);
	static vst* g_release_vst(vst* p_instance, owner_id);
	bool process_chunk(audio_chunk * chunk, abort_callback &);
	bool is_ok() { return m_ok; }
	const string8 & get_msg() { return m_error_msg; }
	void get_preset(const GUID & p_owner, dsp_preset & out);
	void set_preset(const dsp_preset & in);
	void upd_preset(const dsp_preset & in);
	bool is_editor_opened() { return m_editor_opened; }
	void set_editor_opened(bool state) { m_editor_opened = state; }
	AEffect* get_effect() { return m_effect; }
	void declick();
	void store_fxp(const char* p_path, const string_base * p_custom_name = NULL);
	void load_fxp(const char* p_path);
	int get_out_lim() { return m_out_limit; }
	void set_out_lim(int lim) { m_out_limit = lim; }
	bool get_bypass() { return m_bypass; }
	void set_bypass(bool bypass);

private:
	static VstIntPtr VSTCALLBACK host_callback(
		AEffect*, VstInt32, VstInt32, VstIntPtr, void*, float);
	void unload();
	typedef AEffect* (*vst_entrypoint)(audioMasterCallback audioMaster);

	HMODULE m_module;
	AEffect* m_effect;
	bool m_ok;
	bool m_editor_opened;
	bool m_mainthread;
	string8 m_error_msg;
	string8 m_abspath;
	unsigned m_ref_count;
	dsp_preset_impl m_preset;
	static critical_section acquire_release_sync;
	critical_section preset_sync;
	list_t<owner_id> m_owners;
	bool m_bypass;

	// Processing stuff
	unsigned m_srate;
	unsigned m_channels;
	unsigned m_ins_num;
	unsigned m_outs_num;
	t_size m_data_size;
	t_size m_sample_count;
	float* m_input;
	float* m_output;
	float** m_input_ptrs;
	float** m_output_ptrs;
	void delete_buffers();
	void suspend();
	void resume();
	int m_out_limit;
};

#endif