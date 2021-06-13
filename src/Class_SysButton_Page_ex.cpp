#include "Class_SysButton_Page_ex.h"

void _SysButton_register()
{
	Ex_ObjRegister(L"SysButton", EOS_VISIBLE, EOS_EX_TOPMOST, 0, 0, 0, 0, _sysbutton_proc);
}

void _Page_register()
{
	Ex_ObjRegister(L"Page", EOS_VISIBLE, EOS_EX_TRANSPARENT, 0, 0, 0, 0, _page_proc);
}

size_t _sysbutton_paint(HWND hWnd, HEXOBJ hObj, obj_s* pObj)
{
	EX_PAINTSTRUCT2 ps;
	EXATOM atomClass = 0;
	EXATOM atomState = 0;
	if (Ex_ObjBeginPaint(hObj, (EX_PAINTSTRUCT2*)&ps))
	{
		if ((ps.dwStyle & EWS_BUTTON_CLOSE) != 0)
		{
			atomClass = ATOM_SYSBUTTON_CLOSE;
		}
		else if ((ps.dwStyle & EWS_BUTTON_MAX) != 0)
		{
			if (_wnd_querystyle(hWnd, WS_MAXIMIZE, FALSE))
			{
				atomClass = ATOM_SYSBUTTON_RESTORE;
			}
			else {
				atomClass = ATOM_SYSBUTTON_MAX;
			}
		}
		else if ((ps.dwStyle & EWS_BUTTON_MIN) != 0)
		{
			if (_wnd_querystyle(hWnd, WS_MINIMIZE, FALSE))
			{
				atomClass = ATOM_SYSBUTTON_RESTORE;
			}
			else {
				atomClass = ATOM_SYSBUTTON_MIN;
			}
		}
		else if ((ps.dwStyle & EWS_BUTTON_MENU) != 0)
		{
			atomClass = ATOM_SYSBUTTON_MENU;
		}
		else if ((ps.dwStyle & EWS_BUTTON_SETTING) != 0)
		{
			atomClass = ATOM_SYSBUTTON_SETTING;
		}
		else if ((ps.dwStyle & EWS_BUTTON_SKIN) != 0)
		{
			atomClass = ATOM_SYSBUTTON_SKN;
		}
		else if ((ps.dwStyle & EWS_BUTTON_HELP) != 0)
		{
			atomClass = ATOM_SYSBUTTON_HELP;
		}

		if ((ps.dwState & STATE_DOWN) != 0)
		{
			atomState = ATOM_DOWN;
		}
		else if ((ps.dwState & STATE_HOVER) != 0)
		{
			atomState = ATOM_HOVER;
		}
		else {
			atomState = ATOM_NORMAL;
		}

		INT left;
		Ex_ThemeDrawControl(ps.hTheme, ps.hCanvas, 0, 0, ps.width, ps.height, atomClass, atomState, 255);
		if ((ps.dwStyle & EWS_TITLE) != 0)
		{
			left = ps.t_left;
			if (((pObj->pWnd_->dwStyle_ & EWS_HASICON) == EWS_HASICON))
			{
				HICON hicon = (HICON)_wnd_geticonhandle(hWnd, FALSE);
				if (hicon != 0)
				{
					HEXIMAGE hImg = 0;
					_img_createfromhicon(hicon, &hImg);
					if (hImg != 0)
					{
						_canvas_drawimagerect(ps.hCanvas, hImg, left, (ps.t_bottom - ps.t_top - 16) / 2, left + 16, (ps.t_bottom - ps.t_top - 16) / 2 + 16, 255);
						_img_destroy(hImg);
						left = left + 20;
					}
				}
			}
			
			if (((pObj->pWnd_->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_canvas_drawtextex(ps.hCanvas, pObj->hFont_, _obj_getcolor(pObj, COLOR_EX_TEXT_NORMAL), pObj->pstrTitle_, -1,
					DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS, left, ps.t_top, ps.t_right, ps.t_bottom, pObj->dwShadowSize_, 0, 0, 0);
			}
		}
		Ex_ObjEndPaint(hObj, (EX_PAINTSTRUCT2*)&ps);
	}
	return 0;
}

void  _sysbutton_remove_proc(obj_s* pObj, INT width, INT height)
{
	HEXOBJ sObj = pObj->objChildFirst_;
	obj_s* psobj = nullptr;
	INT nError = 0;
	BOOL bReCalced = FALSE;
	INT nOffset = 0;
	while (_handle_validate(sObj, HT_OBJECT, (LPVOID*)&psobj, &nError))
	{
		if (!bReCalced)
		{
			bReCalced = TRUE;
			nOffset = width - psobj->right_;
		}
		Ex_ObjSetPos(sObj, 0, psobj->left_ + nOffset, EOP_DEFAULT, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_EX_NODPISCALE);
		sObj = psobj->objNext_;
	}
}

LRESULT CALLBACK _sysbutton_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT nError = 0;
	obj_s* pObj = nullptr;
	if (_handle_validate(hObj, HT_OBJECT, (LPVOID*)&pObj, &nError))
	{
		if (uMsg == WM_NCCREATE)
		{
			LPCWSTR ret = nullptr;
			if (((pObj->dwStyle_ & EWS_BUTTON_MIN) == EWS_BUTTON_MIN))
			{
				ret = copytstr(g_Li.lpstr_min, lstrlenW(g_Li.lpstr_min));
			}
			else if (((pObj->dwStyle_ & EWS_BUTTON_CLOSE) == EWS_BUTTON_CLOSE))
			{
				ret = copytstr(g_Li.lpstr_close, lstrlenW(g_Li.lpstr_close));
			}
			else if (((pObj->dwStyle_ & EWS_BUTTON_HELP) == EWS_BUTTON_HELP))
			{
				ret = copytstr(g_Li.lpstr_help, lstrlenW(g_Li.lpstr_help));
			}
			if (ret != 0)
			{
				pObj->pstrTips_ = ret;
			}
		}
		else if (uMsg == WM_NCHITTEST)
		{
			INT ret = pObj->dwStyle_;
			if ((ret & EWS_TITLE) != 0)
			{
				if ((ret & EWS_HASICON) != 0)
				{
					ret = HTCAPTION;
				}
				else {
					ret = HTCAPTION;
				}
				return ret;
			}
		}
		else if (uMsg == WM_PAINT)
		{
			return _sysbutton_paint(hWnd, hObj, pObj);
		}
		else if (uMsg == WM_MOUSEHOVER)
		{
			LPCWSTR ret = nullptr;
			if (!((pObj->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_obj_setuistate(pObj, STATE_HOVER, FALSE, 0, TRUE, &nError);
				if (((pObj->dwStyle_ & EWS_BUTTON_MAX) == EWS_BUTTON_MAX))
				{
					Ex_MemFree((LPVOID)pObj->pstrTips_);
					if (_wnd_querystyle(hWnd, WS_MAXIMIZE, FALSE))
					{
						ret = copytstr(g_Li.lpstr_res_max, lstrlenW(g_Li.lpstr_res_max));
					}
					else {
						ret = copytstr(g_Li.lpstr_max, lstrlenW(g_Li.lpstr_max));
					}
					pObj->pstrTips_ = ret;
				}
				else if (((pObj->dwStyle_ & EWS_BUTTON_MIN) == EWS_BUTTON_MIN))
				{
					Ex_MemFree((LPVOID)pObj->pstrTips_);
					if (_wnd_querystyle(hWnd, WS_MINIMIZE, FALSE))
					{
						ret = copytstr(g_Li.lpstr_res_min, lstrlenW(g_Li.lpstr_res_min));
					}
					else {
						ret = copytstr(g_Li.lpstr_min, lstrlenW(g_Li.lpstr_min));
					}
					pObj->pstrTips_ = ret;
				}
			}
		}
		else if (uMsg == WM_MOUSELEAVE)
		{
			if (!((pObj->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_obj_setuistate(pObj, STATE_HOVER | STATE_DOWN, TRUE, 0, TRUE, &nError);
			}
		}
		else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
		{
			if (!((pObj->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_obj_setuistate(pObj, STATE_DOWN, FALSE, 0, TRUE, &nError);
			}
		}
		else if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP)
		{
			if (!((pObj->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_obj_setuistate(pObj, STATE_DOWN, TRUE, 0, TRUE, &nError);
			}
		}
		else if (uMsg == WM_EX_LCLICK)
		{
			INT ret = pObj->dwStyle_;
			if ((ret & EWS_BUTTON_CLOSE) != 0)
			{
				if (((pObj->pWnd_->dwStyle_ & EWS_MESSAGEBOX) == EWS_MESSAGEBOX))
				{
					EndDialog(hWnd, IDCANCEL);
				}
				else {
					PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
					_obj_setuistate(pObj, STATE_HOVER | STATE_DOWN, TRUE, 0, TRUE, &nError);
				}
			}
			else if ((ret & EWS_BUTTON_MAX) != 0)
			{
				if (_wnd_querystyle(hWnd, WS_MAXIMIZE, FALSE))
				{
					SendMessageW(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				}
				else {
					SendMessageW(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				}
				_obj_setuistate(pObj, STATE_HOVER | STATE_DOWN, TRUE, 0, TRUE, &nError);
			}
			else if ((ret & EWS_BUTTON_MIN) != 0)
			{
				if (_wnd_querystyle(hWnd, WS_MINIMIZE, FALSE))
				{
					SendMessageW(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				}
				else {
					SendMessageW(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				}
				_obj_setuistate(pObj, STATE_HOVER | STATE_DOWN, TRUE, 0, TRUE, &nError);
			}
		}
		else if (uMsg == WM_SETFOCUS)
		{
			return 0;
		}
		else if (uMsg == WM_SIZE)
		{
			if (((pObj->dwStyle_ & EWS_TITLE) == EWS_TITLE))
			{
				_sysbutton_remove_proc(pObj, LOWORD(lParam), HIWORD(lParam));
			}
		}
	}
	return Ex_ObjDefProc(hWnd, hObj, uMsg, wParam, lParam);
}

size_t _page_paint(HEXOBJ hObj)
{
	EX_PAINTSTRUCT2 ps;
	if (Ex_ObjBeginPaint(hObj, (EX_PAINTSTRUCT2*)&ps))
	{
		Ex_ObjEndPaint(hObj, (EX_PAINTSTRUCT2*)&ps);
	}
	return 0;
}

void _page_onvscrollbar(HWND hWnd, HEXOBJ hObj, obj_s* pObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT nCode = LOWORD(wParam);
	INT oPos = Ex_ObjScrollGetPos(hObj, SB_VERT);
	INT height = pObj->c_bottom_ - pObj->c_top_;
	INT nPos = 0;
	if (nCode == SB_THUMBPOSITION)
	{
		nPos = oPos;
	}
	else if (nCode == SB_PAGEUP)
	{
		nPos = oPos - height;
	}
	else if (nCode == SB_PAGEDOWN)
	{
		nPos = oPos + height;
	}
	else if (nCode == SB_LINEUP)
	{
		nPos = oPos - HIBYTE(LOWORD(pObj->pWnd_->szItemSeparator_));
	}
	else if (nCode == SB_LINEDOWN)
	{
		nPos = oPos + HIBYTE(LOWORD(pObj->pWnd_->szItemSeparator_));
	}
	else if (nCode == SB_TOP)
	{
		nPos = 0;
	}
	else if (nCode == SB_BOTTOM)
	{
		RECT rect{ 0 };
		Ex_ObjGetRect(pObj->objChildFirst_, &rect);
		nPos = rect.bottom - rect.top - height;
	}
	else
	{
		return;
	}
	nPos = Ex_ObjScrollSetPos(hObj, SB_VERT, nPos, TRUE);
	Ex_ObjSetPos(pObj->objChildFirst_, 0, EOP_DEFAULT, (-nPos), 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOACTIVATE | SWP_EX_NODPISCALE);
}


LRESULT CALLBACK _page_proc(HWND hWnd, HEXOBJ hObj, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT nError = 0;
	obj_s* pObj = nullptr;
	if (_handle_validate(hObj, HT_OBJECT, (LPVOID*)&pObj, &nError))
	{
		if (uMsg == WM_VSCROLL || uMsg == WM_HSCROLL)
		{
			_page_onvscrollbar(hWnd, hObj, pObj, uMsg, wParam, lParam);
		}
	}
	return Ex_ObjDefProc(hWnd, hObj, uMsg, wParam, lParam);
}