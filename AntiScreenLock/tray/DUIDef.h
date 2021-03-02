// def.h: 所有UI中使用的共同定义
//
//////////////////////////////////////////////////////////////////////
#ifndef _DUI_DEF_H
#define _DUI_DEF_H

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

//atl
#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>

// gdi++
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// 
// stl
// 
#include <map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <memory>


#ifndef tstring
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif
#endif

#ifndef SIZEOF
#define SIZEOF _countof
#endif

#define ASSERT assert

typedef CPoint DUI_Point;
typedef CSize  DUI_Size;
typedef CRect  DUI_Rect;

#include <atlimage.h>

typedef enum
{
	DUI_BT_NORMAL = 0,
	DUI_BT_TRANS,
	DUI_BT_ALPHA
}DUI_BITMAP_TYPE;

typedef enum
{
	DUI_ALIGNMENT_NONE = 0,
	DUI_ALIGNMENT_LEFT,
	DUI_ALIGNMENT_TOP,
	DUI_ALIGNMENT_RIGHT,
	DUI_ALIGNMENT_BOTTOM,
	DUI_ALIGNMENT_CENTER
}DUI_ALIGNMENT;

typedef enum
{
	DUI_STRETCH_NONE = 0,
	DUI_STRETCH_SCALE,
	DUI_STRETCH_FILL,
	DUI_STRETCH_HORIZONTAL,
	DUI_STRETCH_VERTICAL
}DUI_STRETCH;

typedef enum
{
	DUI_STATE_NORMAL = 0,
	DUI_STATE_DOWN,
	DUI_STATE_HOVER,
	DUI_STATE_DISABLE,
	DUI_STATE_NORMAL_DISABLE,
	DUI_STATE_DOWN_DISABLE,
	DUI_STATE_HOVER_DISABLE,
	DUI_STATE_SELECT,
}DUI_BUTTON_STATE;

enum SEP_LINE_TYPE
{
	SEP_LINE_NORMAL = 0,
	SEP_LINE_DOT
};

#define TRANSPARENT_COLOR RGB(255, 0, 255)

//UI库中使用的消息
#define WM_UI_TIPS					WM_USER + 0xF000
#define WM_UI_CURSOR				WM_USER + 0xF001
#define WM_UI_ACTIVE				WM_USER + 0xF002
#define WM_UI_ASYNC_CALL_LUP		WM_USER + 0xF020
#define WM_UI_ASYNC_CALL_RUP		WM_USER + 0xF021
#define WM_UI_ASYNC_CALL_DROPUP		WM_USER + 0xF022
#define WM_UI_ASYNC_CALL_ENTERKEY	WM_USER + 0xF023
#define WM_UI_ASYNC_CALL_TABKEY		WM_USER + 0xF024
#define WM_UI_VER_SCROLL			WM_USER + 0xF100 //定义滚动消息，给其他窗口发送滚动, wparam:滚动距离  lparam:是否到达顶端
#define WM_UI_VER_SCROLL_PERCENT	WM_USER + 0xF101
#define WM_UI_BEGIN_SCROLL			WM_USER + 0xF102 //开始滚动
#define WM_UI_TREE_SCROLLBAR		WM_USER + 0xF103 //创建滚动条消息
#define WM_UI_TRAY_NOTIFY			WM_USER + 0xF200
#define WM_UI_MENU_COMMAND			WM_USER + 0xF201	//点击菜单命令
#define WM_UI_THEME_LOAD			WM_USER + 0xF301		// 读主题资源
#define WM_UI_CHANGE_BACKGROUND	    WM_USER + 0xF302		// 更换背景
#define WM_UI_RESTOR_BACKGROUND	    WM_USER + 0xF303		
//#ifndef WM_MOUSEHWHEEL
#define WM_UI_MOUSEHWHEEL		    0x020A
//#endif

void TrackMouseLeave(HWND hWnd);


#define ACTIVE_TIMER	0xC000			//控件激活绘制定时器

#endif
