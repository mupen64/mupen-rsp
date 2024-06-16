#include <windows.h>
#include "win.h"

#include <stdio.h>
#include <string>

#include <Config.h>
#include <spec/Rsp_#1.1.h>
#include <spec/Audio_#1.1.h>
#include "../winproject/resource.h"

extern RSP_INFO rsp;

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
    if (strlen(str) > 3) return str + strlen(str) - 3;
    else return NULL;
}


BOOL APIENTRY
DllMain(
    HINSTANCE hInst /* Library instance handle. */ ,
    DWORD reason /* Reason this function is being called. */ ,
    LPVOID reserved /* Not used. */)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        g_instance = hInst;
        g_app_path = get_app_full_path();
        config_load();

    // FIXME: Are we really sure we want to load the audio plugin here, and not on RomOpen?
    // audiohandle = (HMODULE)get_handle(liste_plugins, config.audio_path);
        break;

    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

static BYTE fake_header[0x1000];
static DWORD fake_AI_DRAM_ADDR_REG;
static DWORD fake_AI_LEN_REG;
static DWORD fake_AI_CONTROL_REG;
static DWORD fake_AI_STATUS_REG;
static DWORD fake_AI_DACRATE_REG;
static DWORD fake_AI_BITRATE_REG;

void plugin_load(HMODULE mod)
{
    AUDIO_INFO info;
    // FIXME: Do we have to provide hwnd?
    info.hwnd = NULL;
    info.hinst = rsp.hInst;
    info.MemoryBswaped = TRUE;
    info.HEADER = fake_header;
    info.RDRAM = rsp.RDRAM;
    info.DMEM = rsp.DMEM;
    info.IMEM = rsp.IMEM;
    info.MI_INTR_REG = rsp.MI_INTR_REG;
    info.AI_DRAM_ADDR_REG = &fake_AI_DRAM_ADDR_REG;
    info.AI_LEN_REG = &fake_AI_LEN_REG;
    info.AI_CONTROL_REG = &fake_AI_CONTROL_REG;
    info.AI_STATUS_REG = &fake_AI_STATUS_REG;
    info.AI_DACRATE_REG = &fake_AI_DACRATE_REG;
    info.AI_BITRATE_REG = &fake_AI_BITRATE_REG;
    info.CheckInterrupts = rsp.CheckInterrupts;
    auto initiateAudio = (BOOL (__cdecl *)(AUDIO_INFO))GetProcAddress(mod, "InitiateAudio");
    g_processAList = (void (__cdecl *)(void))GetProcAddress(mod, "ProcessAList");
    initiateAudio(info);
}


BOOL CALLBACK ConfigDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
        goto refresh;
    case WM_CLOSE:
        config_save();
        EndDialog(hwnd, IDOK);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
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
