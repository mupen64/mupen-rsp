#pragma once

#include <string>
#include <hle.h>

extern HINSTANCE g_instance;
extern std::string g_app_path;

BOOL CALLBACK ConfigDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

/**
 * \brief Handles execution of unknown tasks
 */
void handle_unknown_task(const OSTask_t* task, uint32_t sum);