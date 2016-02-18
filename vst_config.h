#pragma once

class vst_config : public CWindowImpl<vst_config>
{
	BEGIN_MSG_MAP(CMyWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

public:
	DECLARE_WND_CLASS(L"VSTEditorWindow")

	vst_config(void);
	~vst_config(void);

	void test()
	{
		vst_config v;
	}
};
