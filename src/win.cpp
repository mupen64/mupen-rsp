/*
 * Copyright (c) 2025, hacktarux-azimer-rsp-hle maintainers, contributors, and original authors (Hacktarux, Azimer).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "Config.h"
#include "FrontendService.h"
#include "core_plugin.h"
#include "disasm.h"
#include "hle.h"
#include "win.h"

extern core_rsp_info rsp;
extern bool g_rsp_alive;

HINSTANCE g_instance;
std::string g_app_path;

// ProcessAList function from audio plugin, only populated when audio_external is true
void (*g_processAList)() = nullptr;

t_config prev_config = {};

std::string get_app_full_path()
{
    char ret[MAX_PATH] = {0};
    char drive[_MAX_DRIVE], dirn[_MAX_DIR];
    char path_buffer[_MAX_DIR];
    GetModuleFileName(nullptr, path_buffer, sizeof(path_buffer));
    _splitpath(path_buffer, drive, dirn, nullptr, nullptr);
    strcpy(ret, drive);
    strcat(ret, dirn);

    return ret;
}


char* getExtension(char* str)
{
    if (strlen(str) > 3)
        return str + strlen(str) - 3;
    else
        return NULL;
}

BOOL APIENTRY DllMain(HINSTANCE hinst, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        g_instance = hinst;
        g_app_path = get_app_full_path();
        config_load();
        config_init();
        break;
    default:
        break;
    }

    return TRUE;
}

static uint8_t fake_header[0x1000];
static uint32_t fake_AI_DRAM_ADDR_REG;
static uint32_t fake_AI_LEN_REG;
static uint32_t fake_AI_CONTROL_REG;
static uint32_t fake_AI_STATUS_REG;
static uint32_t fake_AI_DACRATE_REG;
static uint32_t fake_AI_BITRATE_REG;

void* plugin_load(const std::string& path)
{
    const auto module = LoadLibrary(path.c_str());

    if (!module)
    {
        FrontendService::show_error("Failed to load the external audio plugin.\nEmulation will not behave as expected.");
        return nullptr;
    }

    core_audio_info info;
    // FIXME: Do we have to provide hwnd?
    info.main_hwnd = NULL;
    info.hinst = (HINSTANCE)rsp.hinst;
    info.byteswapped = TRUE;
    info.rom = fake_header;
    info.rdram = rsp.rdram;
    info.dmem = rsp.dmem;
    info.imem = rsp.imem;
    info.mi_intr_reg = rsp.mi_intr_reg;
    info.ai_dram_addr_reg = &fake_AI_DRAM_ADDR_REG;
    info.ai_len_reg = &fake_AI_LEN_REG;
    info.ai_control_reg = &fake_AI_CONTROL_REG;
    info.ai_status_reg = &fake_AI_STATUS_REG;
    info.ai_dacrate_reg = &fake_AI_DACRATE_REG;
    info.ai_bitrate_reg = &fake_AI_BITRATE_REG;
    info.check_interrupts = rsp.check_interrupts;
    auto initiateAudio = (BOOL(__cdecl*)(core_audio_info))GetProcAddress(module, "InitiateAudio");
    g_processAList = (void(__cdecl*)(void))GetProcAddress(module, "ProcessAList");
    initiateAudio(info);

    return module;
}


INT_PTR CALLBACK ConfigDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        config_load();
        memcpy(&prev_config, &config, sizeof(t_config));

        if (!config.audio_hle && !config.audio_external)
        {
            CheckDlgButton(hwnd, IDC_ALISTS_INSIDE_RSP, BST_CHECKED);
        }
        if (config.audio_hle && !config.audio_external)
        {
            CheckDlgButton(hwnd, IDC_ALISTS_EMU_DEFINED_PLUGIN, BST_CHECKED);
        }
        if (config.audio_hle && config.audio_external)
        {
            CheckDlgButton(hwnd, IDC_ALISTS_RSP_DEFINED_PLUGIN, BST_CHECKED);
        }
        CheckDlgButton(hwnd, IDC_UCODE_CACHE_VERIFY, config.ucode_cache_verify ? BST_CHECKED : BST_UNCHECKED);
        goto refresh;
    case WM_CLOSE:
        config_save();
        EndDialog(hwnd, IDOK);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            config.ucode_cache_verify = IsDlgButtonChecked(hwnd, IDC_UCODE_CACHE_VERIFY);
            config_save();
            EndDialog(hwnd, IDOK);
            break;
        case IDCANCEL:
            memcpy(&config, &prev_config, sizeof(t_config));
            config_save();
            EndDialog(hwnd, IDCANCEL);
            break;
        case IDC_ALISTS_INSIDE_RSP:
            config.audio_hle = FALSE;
            config.audio_external = FALSE;
            goto refresh;
        case IDC_ALISTS_EMU_DEFINED_PLUGIN:
            config.audio_hle = TRUE;
            config.audio_external = FALSE;
            goto refresh;
        case IDC_ALISTS_RSP_DEFINED_PLUGIN:
            config.audio_hle = TRUE;
            config.audio_external = TRUE;
            goto refresh;
        case IDC_BROWSE_AUDIO_PLUGIN:
            MessageBox(hwnd,
                       "Warning: use this feature at your own risk\n"
                       "It allows you to use a second audio plugin to process alists\n"
                       "before they are sent\n"
                       "Example: jabo's sound plugin in emu config to output sound\n"
                       "        +azimer's sound plugin in rsp config to process alists\n"
                       "Do not choose the same plugin in both place, or it'll probably crash\n"
                       "For more informations, please read the README",
                       "Warning", MB_OK);

            char path[sizeof(config.audio_path)] = {0};
            OPENFILENAME ofn{};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = path;
            ofn.nMaxFile = sizeof(path);
            ofn.lpstrFilter = "DLL Files (*.dll)\0*.dll";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

            if (GetOpenFileName(&ofn))
            {
                strcpy(config.audio_path, path);
            }

            goto refresh;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;

refresh:
    EnableWindow(GetDlgItem(hwnd, IDC_EDIT_AUDIO_PLUGIN), config.audio_external);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSE_AUDIO_PLUGIN), config.audio_external);
    SetDlgItemText(hwnd, IDC_EDIT_AUDIO_PLUGIN, config.audio_path);

    return TRUE;
}

void handle_unknown_task(const OSTask_t* task, const uint32_t sum)
{
    char s[1024];
    FILE* f;
    sprintf(s, "unknown task:\n\ttype:%d\n\tsum:%x\n\tPC:%x", task->type, sum, rsp.sp_pc_reg);
    MessageBox(NULL, s, "unknown task", MB_OK);

    if (task->ucode_size <= 0x1000)
    {
        f = fopen("imem.dat", "wb");
        fwrite(rsp.rdram + task->ucode, task->ucode_size, 1, f);
        fclose(f);

        f = fopen("dmem.dat", "wb");
        fwrite(rsp.rdram + task->ucode_data, task->ucode_data_size, 1, f);
        fclose(f);

        f = fopen("disasm.txt", "wb");
        memcpy(rsp.dmem, rsp.rdram + task->ucode_data, task->ucode_data_size);
        memcpy(rsp.imem + 0x80, rsp.rdram + task->ucode, 0xF7F);
        disasm(f, (unsigned long*)(rsp.imem));
        fclose(f);
    }
    else
    {
        f = fopen("imem.dat", "wb");
        fwrite(rsp.imem, 0x1000, 1, f);
        fclose(f);

        f = fopen("dmem.dat", "wb");
        fwrite(rsp.dmem, 0x1000, 1, f);
        fclose(f);

        f = fopen("disasm.txt", "wb");
        disasm(f, (unsigned long*)(rsp.imem));
        fclose(f);
    }
}

__declspec(dllexport) void DllAbout(void* hParent)
{
    auto message =
    "Made using Azimer's code by Hacktarux.\r\nMaintained by Aurumaker72\r\nhttps://github.com/Aurumaker72/hacktarux-azimer-rsp-hle";
    FrontendService::show_info(message, PLUGIN_NAME, hParent);
}

__declspec(dllexport) void DllConfig(void* hParent)
{
    if (g_rsp_alive)
    {
        FrontendService::show_error("Close the ROM before configuring the RSP plugin.", PLUGIN_NAME, hParent);
        return;
    }

    DialogBox(g_instance, MAKEINTRESOURCE(IDD_RSPCONFIG), (HWND)hParent, ConfigDlgProc);
}

__declspec(dllexport) void DllTest(void* hParent)
{
}
