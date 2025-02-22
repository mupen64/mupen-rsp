/*
 * Copyright (c) 2025, hacktarux-azimer-rsp-hle maintainers, contributors, and original authors (Hacktarux, Azimer).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "FrontendService.h"
#include "Config.h"
#include "win.h"

HWND get_emu_window()
{
    HWND hwnd = nullptr;

    EnumWindows([](const HWND hwnd, const LPARAM l_param) -> BOOL {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && hwnd == GetAncestor(hwnd, GA_ROOT))
        {
            *reinterpret_cast<HWND*>(l_param) = hwnd;
            return FALSE;
        }
        return TRUE;
    },
                reinterpret_cast<LPARAM>(&hwnd));

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
