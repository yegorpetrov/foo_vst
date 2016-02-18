#include "StdAfx.h"
#include "entry.h"

using namespace stringcvt;

entry::entry(string_list_const & data) :
	m_has_editor(false)
{
	m_path = data[0];
	m_product = data[1];
	m_version = data[2];
	m_vendor = data[3];
	m_io = data[4];
	m_guid = GUID_from_text(data[5]);
	m_has_editor = (string8(data[6]) == "config:true");
}

entry::entry(const string_base & path)
{
	m_path = path;
}

entry::entry(const entry& e)
{
	m_product = e.m_product;
	m_version = e.m_version;
	m_vendor = e.m_vendor;
	m_io = e.m_io;
	m_path = e.m_path;
	memcpy(&m_guid, &e.m_guid, sizeof(GUID));
	m_has_editor = e.m_has_editor;
}

entry::~entry(void)
{
}

void entry::to_string_list(string_list_impl & data) const
{
	data.remove_all();
	data.add_item(m_path);
	data.add_item(m_product);
	data.add_item(m_version);
	data.add_item(m_vendor);
	data.add_item(m_io);
	data.add_item(print_guid(m_guid));
	data.add_item(m_has_editor ? "config:true" : "config:false");
}