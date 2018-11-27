#pragma once
#include <set>
#include <ShObjIdl.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <winrt/base.h>

#include "config.hpp"
#include "eventhook.hpp"
#include "windowshook.hpp"
#include "messagewindow.hpp"
#include "swcadata.hpp"

class TaskbarAttributeWorker : public MessageWindow {
private:
	static const swca::pSetWindowCompositionAttribute SetWindowCompositionAttribute;

	struct MonitorInfo {
		Window TaskbarWindow;
		std::set<Window> MaximisedWindows;
	};

	// todo:
	// dynamic peek button
	// better aero peek support: detect current peeked to window and include in calculation
	// support windows that are immune to peek (with some extended/dwm flag iirc)
	// dynamic cortana and task view
	// color preview cpicker (internal color override?)
	// apply settings & blacklist changes instantly. (PropertyChanged?)
	// correct virtual desktop switch detection
	//     (currently relying on the fact that cloacking status changes)
	//     not sure if an undocumented COM interface is any better
	//     maybe EVENT_SYSTEM_DESKTOPSWITCH
	// support alt-tab? EVENT_SYSTEM_SWITCH{START,END}
	// handle EVENT_OBJECT_SHOW (but what is the reverse that signals it for window objects, EVENT_OBJECT_HIDE doesn't works)
	// on current desktop not working after explorer restart?
	// explorer slows to a crawl when using normal mode. sounds like we have recursion problems

	// Maximised window
	bool m_PeekActive;
	EventHook m_PeekUnpeekHook;
	EventHook m_CloakUncloakHook;
	EventHook m_MinimizeRestoreHook;
	EventHook m_ResizeMoveHook;
	void OnAeroPeekEnterExit(DWORD event, ...);
	void OnWindowStateChange(bool skipCheck, DWORD, Window window, LONG idObject, ...);
	static bool IsWindowMaximised(Window window);
	static BOOL CALLBACK EnumWindowsProcess(HWND hWnd, LPARAM lParam);

	// Start menu
	HMONITOR m_CurrentStartMonitor;
	winrt::com_ptr<IAppVisibility> m_IAV;
	DWORD m_IAVECookie;
	void OnStartVisibilityChange(bool state);
	HMONITOR GetStartMenuMonitor();

	// Taskbar find & hook
	std::unordered_map<HMONITOR, MonitorInfo> m_Taskbars;
	std::vector<WindowsHook> m_Hooks;
	EventHook m_CreateDestroyHook;
	void OnWindowCreateDestroy(DWORD event, Window window, LONG idObject, ...);
	void RefreshTaskbars();
	void HookTaskbar(Window window);

	// Taskbar appearance refresh
	void Poll();
	bool SetAttribute(Window window, Config::TASKBAR_APPEARANCE config);
	Config::TASKBAR_APPEARANCE GetConfigForMonitor(HMONITOR monitor, bool skipCheck = false);
	bool RefreshAttribute(HMONITOR monitor, bool skipCheck = false);
	long OnRequestAttributeRefresh(WPARAM, LPARAM lParam);

	// Other
	bool m_returningToStock;
	void ReturnToStock();
	EventHook::callback_t BindHook();

public:
	TaskbarAttributeWorker(HINSTANCE hInstance);

	void ResetState();

	~TaskbarAttributeWorker();
};