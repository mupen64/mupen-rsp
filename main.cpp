#include <windows.h>
#include "./winproject/resource.h"
#include "./win/win.h"
#include <stdio.h>

#include <spec/Rsp_#1.1.h>
#include "hle.h"

#include <spec/Audio_#1.1.h>

#include <Config.h>

#define PLUGIN_NAME "Mupen64 HLE RSP Plugin 0.2.1"

RSP_INFO rsp;


extern void (*processAList)();
static BOOL firstTime = TRUE;
void loadPlugin();

void disasm(FILE* f, unsigned long t[0x1000 / 4]);

__declspec(dllexport) void CloseDLL(void)
{
}

__declspec(dllexport) void DllAbout(HWND hParent)
{
    auto message =
        "Made using Azimer's code by Hacktarux.\r\nMaintained by Aurumaker72\r\nhttps://github.com/Aurumaker72/hacktarux-azimer-rsp-hle";
    MessageBox(NULL, message, PLUGIN_NAME, MB_OK);
}

__declspec(dllexport) void DllConfig(HWND hParent)
{
    if (!firstTime)
    {
        MessageBox(hParent, "Close the ROM before configuring the RSP plugin.", PLUGIN_NAME, MB_OK);
        return;
    }

    DialogBox(g_instance, MAKEINTRESOURCE(IDD_RSPCONFIG), hParent, ConfigDlgProc);
}

__declspec(dllexport) void DllTest(HWND hParent)
{
}

static int audio_ucode_detect(OSTask_t* task)
{
    if (*(unsigned long*)(rsp.RDRAM + task->ucode_data + 0) != 0x1)
    {
        if (*(rsp.RDRAM + task->ucode_data + (0 ^ (3 - S8))) == 0xF)
            return 4;
        else
            return 3;
    }
    else
    {
        if (*(unsigned long*)(rsp.RDRAM + task->ucode_data + 0x30) == 0xF0000F00)
            return 1;
        else
            return 2;
    }
}

extern void (*ABI1[0x20])();
extern void (*ABI2[0x20])();
extern void (*ABI3[0x20])();

void (*ABI[0x20])();

u32 inst1, inst2;

static int audio_ucode(OSTask_t* task)
{
    unsigned long* p_alist = (unsigned long*)(rsp.RDRAM + task->data_ptr);
    unsigned int i;

    switch (audio_ucode_detect(task))
    {
    case 1: // mario ucode
        memcpy(ABI, ABI1, sizeof(ABI[0]) * 0x20);
        break;
    case 2: // banjo kazooie ucode
        memcpy(ABI, ABI2, sizeof(ABI[0]) * 0x20);
        break;
    case 3: // zelda ucode
        memcpy(ABI, ABI3, sizeof(ABI[0]) * 0x20);
        break;
    default:
        {
            /*		char s[1024];
                    sprintf(s, "unknown audio\n\tsum:%x", sum);
                    MessageBox(NULL, s, "unknown task", MB_OK);
            */
            return -1;
        }
    }

    //	data = (short*)(rsp.RDRAM + task->ucode_data);

    for (i = 0; i < (task->data_size / 4); i += 2)
    {
        inst1 = p_alist[i];
        inst2 = p_alist[i + 1];
        ABI[inst1 >> 24]();
    }

    return 0;
}

__declspec(dllexport) DWORD DoRspCycles(DWORD Cycles)
{
    OSTask_t* task = (OSTask_t*)(rsp.DMEM + 0xFC0);
    unsigned int i, sum = 0;

    if (firstTime)
    {
        firstTime = FALSE;
        if (config.audio_external)
            loadPlugin();
    }

    if (task->type == 1 && task->data_ptr != 0 && config.graphics_hle)
    {
        if (rsp.ProcessDlistList != NULL)
        {
            rsp.ProcessDlistList();
        }
        *rsp.SP_STATUS_REG |= 0x0203;
        if ((*rsp.SP_STATUS_REG & 0x40) != 0)
        {
            *rsp.MI_INTR_REG |= 0x1;
            rsp.CheckInterrupts();
        }

        *rsp.DPC_STATUS_REG &= ~0x0002;
        return Cycles;
    }
    else if (task->type == 2 && config.audio_hle)
    {
        if (config.audio_external)
            processAList();
        else if (rsp.ProcessAlistList != NULL)
        {
            rsp.ProcessAlistList();
        }
        *rsp.SP_STATUS_REG |= 0x0203;
        if ((*rsp.SP_STATUS_REG & 0x40) != 0)
        {
            *rsp.MI_INTR_REG |= 0x1;
            rsp.CheckInterrupts();
        }
        return Cycles;
    }
    else if (task->type == 7)
    {
        rsp.ShowCFB();
    }

    *rsp.SP_STATUS_REG |= 0x203;
    if ((*rsp.SP_STATUS_REG & 0x40) != 0)
    {
        *rsp.MI_INTR_REG |= 0x1;
        rsp.CheckInterrupts();
    }

    if (task->ucode_size <= 0x1000)
        for (i = 0; i < (task->ucode_size / 2); i++)
            sum += *(rsp.RDRAM + task->ucode + i);
    else
        for (i = 0; i < (0x1000 / 2); i++)
            sum += *(rsp.IMEM + i);


    if (task->ucode_size > 0x1000)
    {
        switch (sum)
        {
        case 0x9E2: // banjo tooie (U) boot code
            {
                int i, j;
                memcpy(rsp.IMEM + 0x120, rsp.RDRAM + 0x1e8, 0x1e8);
                for (j = 0; j < 0xfc; j++)
                    for (i = 0; i < 8; i++)
                        *(rsp.RDRAM + ((0x2fb1f0 + j * 0xff0 + i) ^ S8)) = *(rsp.IMEM + ((0x120 + j * 8 + i) ^ S8));
            }
            return Cycles;
            break;
        case 0x9F2: // banjo tooie (E) + zelda oot (E) boot code
            {
                int i, j;
                memcpy(rsp.IMEM + 0x120, rsp.RDRAM + 0x1e8, 0x1e8);
                for (j = 0; j < 0xfc; j++)
                    for (i = 0; i < 8; i++)
                        *(rsp.RDRAM + ((0x2fb1f0 + j * 0xff0 + i) ^ S8)) = *(rsp.IMEM + ((0x120 + j * 8 + i) ^ S8));
            }
            return Cycles;
            break;
        }
    }
    else
    {
        switch (task->type)
        {
        case 2: // audio
            if (audio_ucode(task) == 0)
                return Cycles;
            break;
        case 4: // jpeg
            switch (sum)
            {
            case 0x278: // used by zelda during boot
                *rsp.SP_STATUS_REG |= 0x200;
                return Cycles;
                break;
            case 0x2e4fc: // uncompress
                jpg_uncompress(task);
                return Cycles;
                break;
            default:
                {
                    char s[1024];
                    sprintf(s, "unknown jpeg:\n\tsum:%x", sum);
                    MessageBox(NULL, s, "unknown task", MB_OK);
                }
            }
            break;
        }
    }

    {
        char s[1024];
        FILE* f;
        sprintf(s, "unknown task:\n\ttype:%d\n\tsum:%x\n\tPC:%x", task->type, sum, rsp.SP_PC_REG);
        MessageBox(NULL, s, "unknown task", MB_OK);

        if (task->ucode_size <= 0x1000)
        {
            f = fopen("imem.dat", "wb");
            fwrite(rsp.RDRAM + task->ucode, task->ucode_size, 1, f);
            fclose(f);

            f = fopen("dmem.dat", "wb");
            fwrite(rsp.RDRAM + task->ucode_data, task->ucode_data_size, 1, f);
            fclose(f);

            f = fopen("disasm.txt", "wb");
            memcpy(rsp.DMEM, rsp.RDRAM + task->ucode_data, task->ucode_data_size);
            memcpy(rsp.IMEM + 0x80, rsp.RDRAM + task->ucode, 0xF7F);
            disasm(f, (unsigned long*)(rsp.IMEM));
            fclose(f);
        }
        else
        {
            f = fopen("imem.dat", "wb");
            fwrite(rsp.IMEM, 0x1000, 1, f);
            fclose(f);

            f = fopen("dmem.dat", "wb");
            fwrite(rsp.DMEM, 0x1000, 1, f);
            fclose(f);

            f = fopen("disasm.txt", "wb");
            disasm(f, (unsigned long*)(rsp.IMEM));
            fclose(f);
        }
    }

    return Cycles;
}

__declspec(dllexport) void GetDllInfo(PLUGIN_INFO* PluginInfo)
{
    PluginInfo->Version = 0x0101;
    PluginInfo->Type = PLUGIN_TYPE_RSP;
    strcpy(PluginInfo->Name, PLUGIN_NAME);
    PluginInfo->NormalMemory = TRUE;
    PluginInfo->MemoryBswaped = TRUE;
}

__declspec(dllexport) void InitiateRSP(RSP_INFO Rsp_Info, DWORD* CycleCount)
{
    rsp = Rsp_Info;
}

__declspec(dllexport) void RomClosed(void)
{
    int i;
    for (i = 0; i < 0x1000; i++)
    {
        rsp.DMEM[i] = rsp.IMEM[i] = 0;
    }
    /*   init_ucode1();
       init_ucode2();*/
    firstTime = TRUE;
}
