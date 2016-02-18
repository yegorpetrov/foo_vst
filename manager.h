#pragma once
#ifndef H_MANAGER
#define H_MANAGER

#include "Helpers/ToolTipDialog.h"
#include "resource.h"

class entry;

// Preferences dialog and VST entries load/save manager
class manager :
	public ATL::CDialogImpl<manager>,
	public WTL::CToolTipDialog<manager>,
	public preferences_page_instance
{
public:
	manager(preferences_page_callback::ptr callback) :
	  m_callback(callback), m_changed(false) {}
	static int init();
	static void dispose();
	static const entry & get_vst_entry(int n);
	static const entry * get_vst_entry(const GUID & g);
	static bool is_portable(string8* p_exedir = NULL);
	static void load_vsts(list_base_t<entry*> & p_list);
	static void store_vsts(const list_base_t<entry*> & p_list);
	void apply();
	void reset(bool from_reg);
	void reset() { reset(false); }
	t_uint32 get_state();
	
	enum {IDD = IDD_VSTMANAGER};
	BEGIN_MSG_MAP(vst_manager)
		CHAIN_MSG_MAP(CToolTipDialog<manager>)
		COMMAND_HANDLER_EX( IDC_BTNADD, BN_CLICKED, on_button )
		COMMAND_HANDLER_EX( IDC_BTNREMOVE, BN_CLICKED, on_button )
		MSG_WM_INITDIALOG(on_init_dialog)
		MSG_WM_DESTROY(on_destroy);
	END_MSG_MAP()

private:
	static list_t<entry*> vst_entries;
	const preferences_page_callback::ptr m_callback;
	BOOL on_init_dialog(CWindow, LPARAM);
	void on_destroy();
	void on_button(UINT, int id, ATL::CWindow);

	CListViewCtrl* m_list;
	bool m_changed;
};

#endif