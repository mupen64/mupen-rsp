BOOL CALLBACK ConfigDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern HINSTANCE dll_hInstance;
void SaveSettings();
void LoadSettings();
extern char AppPath[MAX_PATH];
