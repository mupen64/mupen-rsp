#include <shared/services/FrontendService.h>
#include <shared/Config.h>
#include <Windows.h>
#include "win.h"
#include "resource.h"

HWND get_emu_window()
{
    HWND hwnd = nullptr;

    EnumWindows([](const HWND hwnd, const LPARAM l_param) -> BOOL
    {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && hwnd == GetAncestor(hwnd,GA_ROOT))
        {
            *reinterpret_cast<HWND*>(l_param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&hwnd));

    return hwnd;
}

void FrontendService::show_error(const char* str, const char* title, void* hwnd)
{
    MessageBox(static_cast<HWND>(hwnd ? hwnd : get_emu_window()), str, title ? title : PLUGIN_NAME, MB_ICONERROR);
}

void FrontendService::show_info(const char* str, const char* title, void* hwnd)
{
    MessageBox(static_cast<HWND>(hwnd ? hwnd : get_emu_window()), str, title ? title : PLUGIN_NAME, MB_ICONINFORMATION);
}

void FrontendService::show_config_dialog(void* parent)
{
    DialogBox(g_instance, MAKEINTRESOURCE(IDD_RSPCONFIG), (HWND)parent, ConfigDlgProc);
}
