#pragma once
#ifndef H_ENTRY
#define H_ENTRY

class entry
{
public:
	entry(string_list_const & data);
	entry(const string_base & path);
	entry(const entry&);
	virtual ~entry(void);
	void to_string_list(string_list_impl & data) const;

	const string8 & get_product() const
	{
		return m_product;
	}

	const string8 & get_version() const
	{
		return m_version;
	}

	const string8 & get_vendor() const
	{
		return m_vendor;
	}

	const string8 & get_io() const
	{
		return m_io;
	}

	const string8 & get_path() const
	{
		return m_path;
	}

	const GUID & get_guid() const
	{
		return m_guid;
	}

	bool has_editor() const
	{
		return m_has_editor;
	}

	// Unfortunately, filesystem service isn't ready here yet

	/*void write(service_ptr_t<file> & p_writer)
	{
		abort_callback_dummy cb;
		p_writer->write_bendian_t(ehdr, cb);
		p_writer->write_bendian_t((t_int32)eprd, cb);
		p_writer->write_string(m_product, cb);
		p_writer->write_bendian_t((t_int32)evrs, cb);
		p_writer->write_string(m_version, cb);
		p_writer->write_bendian_t((t_int32)evnd, cb);
		p_writer->write_string(m_vendor, cb);
		p_writer->write_bendian_t((t_int32)eioc, cb);
		p_writer->write_string(m_io, cb);
		p_writer->write_bendian_t((t_int32)epth, cb);
		p_writer->write_string(m_path, cb);
		p_writer->write_bendian_t((t_int32)egid, cb);
		p_writer->write_string(print_guid(m_guid).get_ptr(), cb);
		p_writer->write_bendian_t((t_int32)eedt, cb);
		p_writer->write_string(m_has_editor ? "true" : "false", cb);
		p_writer->write_bendian_t((t_int32)eeof, cb);
	}

	entry(service_ptr_t<file> & p_reader)
	{
		abort_callback_dummy cb;
		t_int32 hdr = 0;
		p_reader->read_bendian_t(hdr, cb);
		if (hdr != ehdr) return;
		for (t_int32 id = 0; id != eeof; p_reader->read_bendian_t(id, cb))
		{
			switch (id)
			{
				case eprd:
					p_reader->read_string(m_product, cb);
					break;
				case evrs:
					p_reader->read_string(m_version, cb);
					break;
				case evnd:
					p_reader->read_string(m_vendor, cb);
					break;
				case eioc:
					p_reader->read_string(m_io, cb);
					break;
				case epth:
					p_reader->read_string(m_path, cb);
					break;
				case egid:
					{
						string8 s_guid;
						p_reader->read_string(s_guid, cb);
						m_guid = GUID_from_text(s_guid);
					}
					break;
				case eedt:
					{
						string8 edt;
						p_reader->read_string(edt, cb);
						m_has_editor = edt == "true";
					}
					break;
			}
		}
	}*/

protected:
	string8 m_product;
	string8 m_version;
	string8 m_vendor;
	string8 m_io;
	string8 m_path;
	GUID m_guid;
	bool m_has_editor;
private:
	static const t_int32 ehdr = '_VST';
	enum d_type {eprd = 'prdc', evrs = 'vrsn', evnd = 'vndr', eioc = '_io_',
		epth = 'path', egid = 'guid', eedt = 'edit', eeof = '_eof'};
};

#endif