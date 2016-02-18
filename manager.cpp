#include "StdAfx.h"
#include "manager.h"
#include "entry.h"
#include "vst.h"
#include "shared.h"

using namespace WTL;
using namespace stringcvt;
static const wchar_t* reg_key = L"Software\\Yego\\F2K VST adapter";
static const wchar_t* filter_str = L"VST plug-in DLL file (*.dll)\0*.dll\0";
static const wchar_t* cfg_filename_v0 = L"vst_entries.bin";
list_t<entry*> manager::vst_entries;

// Unfortunately, filesystem service isn't ready here yet

void manager::load_vsts(list_base_t<entry*> & p_list)
{
	p_list.remove_all();
	string8 cfgdir = core_api::get_profile_path();
	if (cfgdir.find_first("://") != ~0)
	{
		cfgdir.remove_chars(0, cfgdir.find_first("://") + 3);
	}
	wchar_t fname[MAX_PATH] = {0};
	PathCombine(fname, string_os_from_utf8(cfgdir), cfg_filename_v0);
	FILE* pfile = _wfopen(fname, L"rb");
	if (pfile == NULL) return;
	int n = 0;
	fread(&n, sizeof(int), 1, pfile);
	for (int i = 0; i < n; i++)
	{
		string_list_impl strings;
		for (int i = 0; i < 7; i++)
		{
			wchar_t str[MAX_PATH] = {0};
			fgetws(str, MAX_PATH, pfile);
			size_t len = wcslen(str);
			if (str[len - 1] == L'\n') str[len - 1] = L'\0';
			strings.add_item(string_utf8_from_os(str));
		}
		p_list.add_item(new entry(strings));
	}
	fclose(pfile);
	/*service_ptr_t<file> vstl;
	abort_callback_dummy cb;
	cfgdir += "\\vst_entries2.bin";
	filesystem::g_open_read(vstl, cfgdir, cb);
	t_int32 hdr = 0, n = 0;
	vstl->read_bendian_t(hdr, cb);
	if (hdr != 'VSTL') return;
	vstl->read_bendian_t(n, cb);
	for (int i = 0; i < n; i++)
	{
		p_list.add_item(new entry(vstl));
	}*/
}

void manager::store_vsts(const list_base_t<entry*> & p_list)
{
	string8 cfgdir = core_api::get_profile_path();
	if (cfgdir.find_first("://") != ~0)
	{
		cfgdir.remove_chars(0, cfgdir.find_first("://") + 3);
	}
	wchar_t fname[MAX_PATH] = {0};
	PathCombine(fname, string_os_from_utf8(cfgdir), cfg_filename_v0);
	FILE* pfile = _wfopen(fname, L"wb");
	if (pfile == NULL) return;
	int n = p_list.get_count();
	fwrite(&n, sizeof(int), 1, pfile);
	for (int i = 0; i < n; i++)
	{
		string_list_impl strings;
		p_list.get_item(i)->to_string_list(strings);
		for (int j = 0; j < 7; j++)
		{
			fwprintf(pfile, L"%s\n", string_os_from_utf8(strings[j]));
		}
	}
	fclose(pfile);
	/*service_ptr_t<file> vstl;
	abort_callback_dummy cb;
	cfgdir += "\\vst_entries2.bin";
	filesystem::g_open_write_new(vstl, cfgdir, cb);
	vstl->write_bendian_t('VSTL', cb);
	int n = p_list.get_count();
	vstl->write_bendian_t(n, cb);
	for (int i = 0; i < n; i++)
	{
		p_list.get_item(i)->write(vstl);
	}*/
}

int manager::init(void)
{
	load_vsts(vst_entries);
	return vst_entries.get_count();
}

void manager::dispose(void)
{
	// :TODO: Crashes
	/*for (int i = 0, j = vst_entries.get_count(); i < j; i++)
	{
		delete vst_entries.get_item(i);
	}*/
}

const entry & manager::get_vst_entry(int n)
{
	return *vst_entries.get_item(n);
}

const entry * manager::get_vst_entry(const GUID & g)
{
	entry* result = NULL;
	for (t_size i = 0, j = vst_entries.get_count(); i < j; i++)
	{
		entry* e = vst_entries.get_item(i);
		if (e->get_guid() == g)
		{
			result = e;
			break;
		}
	}
	return result;
}

void manager::apply()
{
	list_t<entry*> vst_list;
	for (int i = 0, j = m_list->GetItemCount(); i < j; i++)
	{
		vst_list.add_item(reinterpret_cast<entry*>(m_list->GetItemData(i)));
	}
	store_vsts(vst_list);
}

// Compatibility
void load_reg(list_base_t<entry*> & p_list)
{
	RegKey key(HKEY_CURRENT_USER);
	key.Create(reg_key, KEY_READ);
	for (RegValueForwardIterator i(key), e; i != e; ++i) {
		LPCTSTR name = (*i).first;
		RegValue value = (*i).second;
		if (value.Type == REG_MULTI_SZ && name[0] == L'{')
		{
			string_list_impl strings;
			for (int i = 0; i < 7; i++)
			{
				strings.add_item(string_utf8_from_os(value.at(i)));
			}
			p_list.add_item(new entry(strings));
		}
	}
}

void manager::reset(bool from_reg)
{
	for (int i = 0, j = m_list->GetItemCount(); i < j; i++)
	{
		delete reinterpret_cast<entry*>(m_list->GetItemData(i));
	}
	m_list->DeleteAllItems();
	list_t<entry*> vst_list;
	if (from_reg)
	{
		load_reg(vst_list);
		m_changed = true;
	}
	else
	{
		load_vsts(vst_list);
		m_changed = false;
	}
	for (int i = 0, j = vst_list.get_count(); i < j; i++)
	{
		entry* e = vst_list.get_item(i);
		m_list->AddItem(i, 0, string_os_from_utf8(e->get_product()));
		m_list->AddItem(i, 1, string_os_from_utf8(e->get_vendor()));
		m_list->AddItem(i, 2, string_os_from_utf8(e->get_io()));
		m_list->AddItem(i, 3, string_os_from_utf8(e->get_path()));
		m_list->SetItemData(i, reinterpret_cast<DWORD_PTR>(e));
	}
}

t_uint32 manager::get_state()
{
	::EnableWindow(GetDlgItem(IDC_BTNADD).m_hWnd,
		m_list->GetItemCount() < 20);
	t_uint32 state = 0;
	if (m_changed)
	{
		state |= preferences_state::resettable;
		state |= preferences_state::changed;
		state |= preferences_state::needs_restart;
	}
	return state;
}

BOOL manager::on_init_dialog(CWindow, LPARAM)
{
	m_list = new CListViewCtrl(GetDlgItem(IDC_VSTLIST));
	m_list->SetExtendedListViewStyle(
		LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP |
		LVS_EX_DOUBLEBUFFER | LVS_EX_UNDERLINEHOT);
	SetWindowTheme(m_list->m_hWnd, L"explorer", NULL);
	m_list->AddColumn(L"Product", 0);
	m_list->AddColumn(L"Vendor", 1);
	m_list->AddColumn(L"I/O", 2);
	m_list->AddColumn(L"Module", 3);
	m_list->SetColumnWidth(0, 125);
	m_list->SetColumnWidth(1, 100);
	m_list->SetColumnWidth(2, 40);
	m_list->SetColumnWidth(3, 220);
	reset();
	return true;
}

void manager::on_destroy()
{
	delete m_list;
	dispose();
}

void manager::on_button(UINT, int id, ATL::CWindow)
{
	if (id == IDC_BTNADD)
	{
		// Compatibility
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			reset(true);
			m_callback->on_state_changed();
			return;
		}
		OPENFILENAME ofn;
		wchar_t buf[MAX_PATH] = {0};
		{
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = this->m_hWnd;
			ofn.lpstrFilter = filter_str;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = buf;
			ofn.lpstrFile[0] = 0;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags =
				OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
				OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		}
		if (GetOpenFileName(&ofn) != 0)
		{
			string8 path;
			if (core_api::is_portable_mode_enabled())
			{
				wchar_t exe[MAX_PATH] = {0}, out[MAX_PATH] = {0};
				GetModuleFileName(NULL, exe, MAX_PATH);
				PathRemoveFileSpec(exe);
				if (PathRelativePathTo(out, exe, FILE_ATTRIBUTE_DIRECTORY,
					ofn.lpstrFile, FILE_ATTRIBUTE_NORMAL))
				{
					path << stringcvt::string_utf8_from_os(out);
				}
				else
				{
					path << stringcvt::string_utf8_from_os(ofn.lpstrFile);
				}
			}
			else
			{
				path << stringcvt::string_utf8_from_os(ofn.lpstrFile);
			}
			entry e(path);
			vst* v = new vst(e, true);
			if (!v->is_ok())
			{
				popup_message::g_complain(v->get_msg());
				delete v;
				return;
			}
			for (int i = 0, j = m_list->GetItemCount(); i < j; i++)
			{
				entry* e = reinterpret_cast<entry*>(m_list->GetItemData(i));
				if (e->get_guid() == v->get_guid())
				{
					popup_message::g_complain(
						"This plug-in is already in the list.");
					delete v;
					m_list->SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED,
						LVIS_SELECTED | LVIS_FOCUSED);
					return;
				}
			}
			int c = m_list->GetItemCount();
			m_list->AddItem(c, 0, string_os_from_utf8(v->get_product()));
			m_list->AddItem(c, 1, string_os_from_utf8(v->get_vendor()));
			m_list->AddItem(c, 2, string_os_from_utf8(v->get_io()));
			m_list->AddItem(c, 3, string_os_from_utf8(v->get_path()));
			m_list->SetItemData(c, reinterpret_cast<DWORD_PTR>(v));
			m_changed = true;
			m_callback->on_state_changed();
		}
	}
	else if (id == IDC_BTNREMOVE)
	{
		int i = -1;
		while ((i = m_list->GetNextItem(-1, LVNI_SELECTED)) != -1)
		{
			m_list->DeleteItem(i);
			m_changed = true;
		}
		if (m_changed) m_callback->on_state_changed();
	}
}

class vst_manager_page : public preferences_page_impl<manager>
{
public:
	const char * get_name()
	{
		return "VST plug-ins";
	}

	GUID get_guid()
	{
		static const GUID guid =
		{ 0x89c112f4, 0xcda2, 0x4554,
		{ 0x91, 0x8a, 0xe7, 0x56, 0xea, 0x60, 0x8b, 0x94 } };
		return guid;
	}

	GUID get_parent_guid()
	{
		// Components branch
		static const GUID parent =
		{ 0x0e966267, 0x7dfb, 0x433b,
		{ 0xa0, 0x7c, 0x3f, 0x8c, 0xdd, 0x31, 0xa2, 0x58 } };
		return parent;
	}
};
static preferences_page_factory_t<vst_manager_page> prefpage;