#ifndef __RSP_1_1_H__
#define __RSP_1_1_H__

#if defined(__cplusplus)

extern "C" {
#endif

enum class PluginType {
    Video = 2,
    Audio = 3,
    Input = 4,
    RSP = 1,
};

enum class ControllerExtension {
    None = 1,
    Mempak = 2,
    Rumblepak = 3,
    Transferpak = 4,
    Raw = 5
};

enum class SystemType {
    NTSC,
    PAL,
    MPAL
};

typedef struct
{
    uint16_t Version;
    uint16_t Type;
    char Name[100];

    /* If DLL supports memory these memory options then set them to TRUE or FALSE
       if it does not support it */
    int32_t NormalMemory; /* a normal uint8_t array */
    int32_t MemoryBswaped; /* a normal uint8_t array where the memory has been pre
                             bswap on a dword (32 bits) boundry */
} PLUGIN_INFO;

typedef struct
{
    void* hInst;
    // Whether the memory has been pre-byteswapped on a uint32_t boundary
    int32_t MemoryBswaped;
    uint8_t* RDRAM;
    uint8_t* DMEM;
    uint8_t* IMEM;

    uint32_t* MI_INTR_REG;

    uint32_t* SP_MEM_ADDR_REG;
    uint32_t* SP_DRAM_ADDR_REG;
    uint32_t* SP_RD_LEN_REG;
    uint32_t* SP_WR_LEN_REG;
    uint32_t* SP_STATUS_REG;
    uint32_t* SP_DMA_FULL_REG;
    uint32_t* SP_DMA_BUSY_REG;
    uint32_t* SP_PC_REG;
    uint32_t* SP_SEMAPHORE_REG;

    uint32_t* DPC_START_REG;
    uint32_t* DPC_END_REG;
    uint32_t* DPC_CURRENT_REG;
    uint32_t* DPC_STATUS_REG;
    uint32_t* DPC_CLOCK_REG;
    uint32_t* DPC_BUFBUSY_REG;
    uint32_t* DPC_PIPEBUSY_REG;
    uint32_t* DPC_TMEM_REG;

    void(__cdecl* CheckInterrupts)(void);
    void(__cdecl* ProcessDlistList)(void);
    void(__cdecl* ProcessAlistList)(void);
    void(__cdecl* ProcessRdpList)(void);
    void(__cdecl* ShowCFB)(void);
} RSP_INFO;

typedef struct
{
    void* hWnd; /* Render window */
    void* hStatusBar;
    /* if render window does not have a status bar then this is NULL */

    int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
    //   bswap on a dword (32 bits) boundry
    //	eg. the first 8 bytes are stored like this:
    //        4 3 2 1   8 7 6 5

    uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom
    // This will be in the same memory format as the rest of the memory.
    uint8_t* RDRAM;
    uint8_t* DMEM;
    uint8_t* IMEM;

    uint32_t* MI_INTR_REG;

    uint32_t* DPC_START_REG;
    uint32_t* DPC_END_REG;
    uint32_t* DPC_CURRENT_REG;
    uint32_t* DPC_STATUS_REG;
    uint32_t* DPC_CLOCK_REG;
    uint32_t* DPC_BUFBUSY_REG;
    uint32_t* DPC_PIPEBUSY_REG;
    uint32_t* DPC_TMEM_REG;

    uint32_t* VI_STATUS_REG;
    uint32_t* VI_ORIGIN_REG;
    uint32_t* VI_WIDTH_REG;
    uint32_t* VI_INTR_REG;
    uint32_t* VI_V_CURRENT_LINE_REG;
    uint32_t* VI_TIMING_REG;
    uint32_t* VI_V_SYNC_REG;
    uint32_t* VI_H_SYNC_REG;
    uint32_t* VI_LEAP_REG;
    uint32_t* VI_H_START_REG;
    uint32_t* VI_V_START_REG;
    uint32_t* VI_V_BURST_REG;
    uint32_t* VI_X_SCALE_REG;
    uint32_t* VI_Y_SCALE_REG;

    void(__cdecl* CheckInterrupts)(void);
} GFX_INFO;

typedef struct
{
    void* hwnd;
    void* hinst;

    int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
    //   bswap on a dword (32 bits) boundry
    //	eg. the first 8 bytes are stored like this:
    //        4 3 2 1   8 7 6 5
    uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom
    // This will be in the same memory format as the rest of the memory.
    uint8_t* RDRAM;
    uint8_t* DMEM;
    uint8_t* IMEM;

    uint32_t* MI_INTR_REG;

    uint32_t* AI_DRAM_ADDR_REG;
    uint32_t* AI_LEN_REG;
    uint32_t* AI_CONTROL_REG;
    uint32_t* AI_STATUS_REG;
    uint32_t* AI_DACRATE_REG;
    uint32_t* AI_BITRATE_REG;

    void(__cdecl* CheckInterrupts)(void);
} AUDIO_INFO;

typedef struct
{
    int32_t Present;
    int32_t RawData;
    int32_t Plugin;
} CONTROL;

typedef union {
    uint32_t Value;

    struct
    {
        unsigned R_DPAD : 1;
        unsigned L_DPAD : 1;
        unsigned D_DPAD : 1;
        unsigned U_DPAD : 1;
        unsigned START_BUTTON : 1;
        unsigned Z_TRIG : 1;
        unsigned B_BUTTON : 1;
        unsigned A_BUTTON : 1;

        unsigned R_CBUTTON : 1;
        unsigned L_CBUTTON : 1;
        unsigned D_CBUTTON : 1;
        unsigned U_CBUTTON : 1;
        unsigned R_TRIG : 1;
        unsigned L_TRIG : 1;
        unsigned Reserved1 : 1;
        unsigned Reserved2 : 1;

        signed Y_AXIS : 8;

        signed X_AXIS : 8;
    };
} BUTTONS;

typedef struct
{
    void* hMainWindow;
    void* hinst;

    int32_t MemoryBswaped; // If this is set to TRUE, then the memory has been pre
    //   bswap on a dword (32 bits) boundry, only effects header.
    //	eg. the first 8 bytes are stored like this:
    //        4 3 2 1   8 7 6 5
    uint8_t* HEADER; // This is the rom header (first 40h bytes of the rom)
    CONTROL* Controls; // A pointer to an array of 4 controllers .. eg:
    // CONTROL Controls[4];
} CONTROL_INFO;

typedef struct s_rom_header {
    uint8_t init_PI_BSB_DOM1_LAT_REG;
    uint8_t init_PI_BSB_DOM1_PGS_REG;
    uint8_t init_PI_BSB_DOM1_PWD_REG;
    uint8_t init_PI_BSB_DOM1_PGS_REG2;
    uint32_t ClockRate;
    uint32_t PC;
    uint32_t Release;
    uint32_t CRC1;
    uint32_t CRC2;
    uint32_t Unknown[2];
    uint8_t nom[20];
    uint32_t unknown;
    uint32_t Manufacturer_ID;
    uint16_t Cartridge_ID;
    uint16_t Country_code;
    uint32_t Boot_Code[1008];
} t_rom_header;


/******************************************************************
  Function: CloseDLL
  Purpose:  This function is called when the emulator is closing
            down allowing the dll to de-initialise.
  input:    none
  output:   none
*******************************************************************/
__declspec(dllexport) void CloseDLL(void);

/******************************************************************
  Function: DllAbout
  Purpose:  This function is optional function that is provided
            to give further information about the DLL.
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
__declspec(dllexport) void DllAbout(void* hParent);

/******************************************************************
  Function: DllConfig
  Purpose:  This function is optional function that is provided
            to allow the user to configure the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
__declspec(dllexport) void DllConfig(void* hParent);

/******************************************************************
  Function: DllTest
  Purpose:  This function is optional function that is provided
            to allow the user to test the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/
__declspec(dllexport) void DllTest(void* hParent);

/******************************************************************
  Function: DoRspCycles
  Purpose:  This function is to allow the RSP to run in parrel with
            the r4300 switching control back to the r4300 once the
            function ends.
  input:    The number of cylces that is meant to be executed
  output:   The number of cycles that was executed. This value can
            be greater than the number of cycles that the RSP
            should have performed.
            (this value is ignored if the RSP is stoped)
*******************************************************************/
__declspec(dllexport) uint32_t DoRspCycles(uint32_t Cycles);

/******************************************************************
  Function: GetDllInfo
  Purpose:  This function allows the emulator to gather information
            about the dll by filling in the PluginInfo structure.
  input:    a pointer to a PLUGIN_INFO stucture that needs to be
            filled by the function. (see def above)
  output:   none
*******************************************************************/
__declspec(dllexport) void GetDllInfo(PLUGIN_INFO* PluginInfo);

/******************************************************************
  Function: InitiateRSP
  Purpose:  This function is called when the DLL is started to give
            information from the emulator that the n64 RSP
            interface needs
  input:    Rsp_Info is passed to this function which is defined
            above.
            CycleCount is the number of cycles between switching
            control between the RSP and r4300i core.
  output:   none
*******************************************************************/
__declspec(dllexport) void InitiateRSP(RSP_INFO Rsp_Info, uint32_t* CycleCount);

/******************************************************************
  Function: RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/
__declspec(dllexport) void RomClosed(void);

#if defined(__cplusplus)
}
#endif

#endif // __RSP_1_1_H__
