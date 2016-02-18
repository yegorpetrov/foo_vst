#pragma once
#ifndef H_EDITOR
#define H_EDITOR

class editor : public CDialogImpl<editor>, public instanceTrackerV2<editor>
{
	vst* m_vst;
	AEffect* m_effect;
	dsp_preset_edit_callback & m_callback;
	dsp_preset_impl m_initial;
	bool m_edited;
	bool m_prests_loaded;
	CUpDownCtrl* m_updown;
	CListBox* m_presets;
	string8 m_dir;
	HANDLE m_quithandle;
	HHOOK m_hook;
	DWORD m_threadid;

	LRESULT on_timer(UINT, WPARAM wParam, LPARAM, BOOL&);
	void on_command(UINT nID, CPoint lParam);
	void on_key_down(UINT, UINT, UINT);
	LRESULT deltapos(int wParam, LPNMHDR lParam, BOOL &bHandled);
	LRESULT open_folder(int wParam, LPNMHDR lParam, BOOL &bHandled);
	LRESULT outinfo(int wParam, LPNMHDR lParam, BOOL &bHandled);
	BOOL on_init_dialog(CWindow, LPARAM);
	void on_viewpresets(UINT, int, ATL::CWindow);
	void on_bypass(UINT, int, ATL::CWindow);
	void on_add(UINT, int, ATL::CWindow);
	void on_delete(UINT, int, ATL::CWindow);
	void on_listdblclick(UINT, int, ATL::CWindow);
	BOOL on_close();
	void on_destroy();
	void save_config();
	void update_lim_display(int val);
	void load_presets();
	static DWORD __stdcall watch_dir(void* p_arg);
	static LRESULT __stdcall hook_proc(int p_ncode, WPARAM p_wp, LPARAM p_lp);

public:
	enum { IDD = IDD_DSP };
	editor(const dsp_preset&, const entry&, dsp_preset_edit_callback&, vst*);
	~editor(void);
	void resize_editor(int w, int h);
	void preset_edited() { m_edited = true; }

	BEGIN_MSG_MAP(editor)
		MSG_WM_DESTROY(on_destroy)
		MSG_WM_KEYDOWN(on_key_down)
		MSG_WM_INITDIALOG(on_init_dialog)
		MSG_WM_CLOSE(on_close)
		MESSAGE_HANDLER(WM_TIMER, on_timer)
		NOTIFY_HANDLER(IDC_OUTNSPIN, UDN_DELTAPOS, deltapos);
		NOTIFY_HANDLER(IDC_OPENFOLDER, NM_CLICK, open_folder);
		NOTIFY_HANDLER(IDC_OUTINFO, NM_CLICK, outinfo);
		MSG_WM_SYSCOMMAND(on_command)
		COMMAND_HANDLER_EX(IDC_VIEWPRESETS, BN_CLICKED, on_viewpresets)
		COMMAND_HANDLER_EX(IDC_BTNADDPRESET, BN_CLICKED, on_add)
		COMMAND_HANDLER_EX(IDC_BTNDELPRESETS2, BN_CLICKED, on_delete)
		COMMAND_HANDLER_EX(IDC_BYPASS, BN_CLICKED, on_bypass)
		COMMAND_HANDLER_EX(IDC_PRESETLIST, LBN_DBLCLK, on_listdblclick)
	END_MSG_MAP()
};

#endif