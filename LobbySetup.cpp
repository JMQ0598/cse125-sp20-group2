#include <string>
#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <tchar.h>
#include <iostream>
#include <cstdio>

// Filepath
std::string EXEC_FILE = "gg.exe"; 

// BG color
const COLORREF BG_COLOR = RGB(215, 55, 55);
HBRUSH hbrBkgnd = NULL;

// Control IDs
const int PORT_BOX_ID = 100;
const int INFO_TEXT_ID = 101;
const int JOIN_BTN_ID = 102;
const int HOST_BTN_ID = 103;
const int IP_BOX_ID = 104;
const int STATUS_TEXT_ID = 105;
const int PORT_TEXT_ID = 106;
const int IP_TEXT_ID = 106;
const int DM_TEXT_ID = 107;
const int DM_BOX_ID = 108;
const int KM_TEXT_ID = 109;
const int KM_BOX_ID = 110;
const int LM_TEXT_ID = 111;
const int LM_BOX_ID = 112;
const int MAPINFO_TEXT_ID = 113;

// Controls
HWND hInfo = NULL;      // Lets players know what to do
HWND hStatus = NULL;    // To indicate processes running/errors
HWND hHost = NULL;      // Host button
HWND hJoin = NULL;      // Join button
HWND hIP = NULL;        // Text box to enter IP
HWND hPort = NULL;      // Text box to enter port
HWND hPortText = NULL;  // Text to indicate where to type the port
HWND hIPText = NULL;    // Text to indicate where to type the IP
HWND hDMText = NULL;    // Text to indicate where to type dungeon map file
HWND hDM = NULL;        // Text box to enter dungeon map file
HWND hKMText = NULL;    // Text to indicate where to type kitchen map file
HWND hKM = NULL;        // Text box to enter kitchen map file
HWND hLMText = NULL;    // Text to indicate where to type lobby map file
HWND hLM = NULL;        // Text box to enter lobby map file
HWND hMapInfo = NULL;   // Lets players now only hosts can pick maps

// Port/IP
std::string port;
std::string hostIP;

// Map values
std::string dm;
std::string km;
std::string lm;

// For window setup - window class and window name
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Lobby Setup");

// Command line received upon process startup. Used for status text's initial value.
LPSTR cmdLine;

// Error strings
const char* ERR_E2BIG = "The space required for the arguments and environment settings exceeds 32 KB.";
const char* ERR_EACCES = "The specified file has a locking or sharing violation.";
const char* ERR_EINVAL = "Invalid parameter";
const char* ERR_EMFILE = "Too many files open (the specified file must be opened to determine whether it is executable).";
const char* ERR_ENOENT = "The file or path not found.";
const char* ERR_ENOEXEC = "The specified file is not executable or has an invalid executable-file format.";
const char* ERR_ENOMEM = "Not enough memory is available to execute the new process; the available memory has been corrupted; or an invalid block exists, indicating that the calling process was not allocated properly.";
const char* ERR_GENERIC = "Unable to spawn game process.";
const char* ERR_BADDM_STR = "Dungeon map not found. Try again.";
const char* ERR_BADKM_STR = "Kitchen map not found. Try again.";
const char* ERR_BADLM_STR = "Lobby map not found. Try again.";

/**
 * Used for debugging when a process fails to spawn.
 */
void errorSpawningProcess() {
    std::cerr << "Error has occurred." << std::endl;
    switch(errno) {
        case E2BIG:
            std::cerr << ERR_E2BIG << std::endl;
            break;
        case EACCES:
            std::cerr << ERR_EACCES << std::endl;
            break;
        case EINVAL:
            std::cerr << ERR_EINVAL << std::endl;
            break;
        case EMFILE:
            std::cerr << ERR_EMFILE << std::endl;
            break;
        case ENOENT:
            std::cerr << ERR_ENOENT << std::endl;
            break;
        case ENOEXEC:
            std::cerr << ERR_ENOEXEC << std::endl;
            break;
        case ENOMEM:
            std::cerr << ERR_ENOMEM << std::endl;
            break;
    }
}

void clearStatusWarning() {
    SetWindowText(hStatus, "");
}

/**
 * Stores map files into strings to send later as command args
 */
void collectMapStrings() {
    char buf[32];
    GetWindowText(hDM, buf, 32);
    dm = buf;
    GetWindowText(hKM, buf, 32);
    km = buf;
    GetWindowText(hLM, buf, 32);
    lm = buf;
}

bool checkMapsExist() {

    // Check DM file
    FILE* f = fopen(dm.c_str(), "r");
    if (!f)
    {
        SetWindowText(hStatus, ERR_BADDM_STR);
        return false;
    }
    fclose(f);

    // Check KM file
    f = fopen(km.c_str(), "r");
    if (!f)
    {
        SetWindowText(hStatus, ERR_BADKM_STR);
        return false;
    }
    fclose(f);

    // Check LM file
    f = fopen(lm.c_str(), "r");
    if (!f)
    {
        SetWindowText(hStatus, ERR_BADLM_STR);
        return false;
    }
    fclose(f);

    // All checks passed.
    clearStatusWarning();
    return true;
}

/**
 * Creates a thread to host a server and detaches the thread from this one.
 */
void spawnHost() 
{    
    // Process/Startup info setup
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, strdup(std::string(EXEC_FILE + " server " + port + " " + dm + " " + km + " " + lm).c_str()),
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        SetWindowText(hStatus, ERR_GENERIC);
    } else {
        ExitProcess(0);
    }
}

/**
 * Creates a thread to join a server and detaches the thread from this one.
 */
void spawnClient() 
{
    // Process/Startup info setup
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, strdup(std::string(EXEC_FILE + " client " + port + " " + hostIP).c_str()),
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        SetWindowText(hStatus, ERR_GENERIC);
    } else {
        ExitProcess(0);
    }
}

void invalidPortWarning() {
    SetWindowText(hStatus, "Please enter a valid, nonzero port.");
}


bool getPort() 
{
    // Extract port from text box
    char buf[6];
    GetWindowText(hPort, buf, 6);
    long val = strtol(buf, nullptr, 10);

    // Validation check
    if (val == 0) {
        invalidPortWarning();
        return false;
    }

    // Success. Set port and exit.
    clearStatusWarning();
    port = buf;
    return true;
}

bool isValidIP(std::string str) {
    int dgtCnt = 0;
    int dotCnt = 0;
    int j = 0;
    for (unsigned int i = 0; i < str.size(); i++) {
        if (str[i] == '.') {
            if (dgtCnt < 1) return false;
            if (i == str.size() - 1) return false;
            int octet = strtol(str.substr(j, i - j).c_str(), nullptr, 10);
            if (octet == 0 || octet > 255)  return false;
            dgtCnt = 0;
            dotCnt++;
            j = i + 1;

        } else if (isdigit(str[i])) {
            dgtCnt++;
            if (dgtCnt > 3) return false;
        
        } else {
            return false;
        }
    }
    if (dotCnt != 3) return false;
    int octet = strtol(str.substr(j, str.size() - j).c_str(), nullptr, 10);
    if (octet == 0 || octet > 255)  return false;
    return true;
}

void invalidIPWarning() {
    SetWindowText(hStatus, "Please enter a valid host IP.");
}

bool getHostIP() 
{
    // Extract IP from text box
    char buf[16];
    GetWindowText(hIP, buf, 16);
    std::string val = std::string(buf);

    // Edge case: localhost
    if (val.compare("localhost") == 0) {
        hostIP = val;
        return true;
    }

    // Validation check
    if (!isValidIP(val)) {
        invalidIPWarning();
        return false;
    }

    // Success. Set IP and exit.
    clearStatusWarning();
    hostIP = val;
    return true;
}

/**
 * Window message handling
 */
LRESULT CALLBACK WndProc(
   _In_ HWND   hWnd,
   _In_ UINT   message,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
) {
    switch(message)
    {
        // Handle window creation
        case WM_CREATE:

            // Create child windows
            hInfo =     CreateWindowEx(0, "STATIC", "Enter port to host a lobby. Enter an IP AND port to join a lobby.", WS_CHILD | WS_VISIBLE | SS_CENTER, 45, 10, 210, 40, hWnd, (HMENU) INFO_TEXT_ID, GetModuleHandle(NULL), NULL);
            hPortText = CreateWindowEx(0, "STATIC", "Port:", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 60, 70, 30, hWnd, (HMENU) PORT_TEXT_ID, GetModuleHandle(NULL), NULL);
            hPort =     CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "9000", WS_CHILD | WS_VISIBLE, 65, 55, 185, 30, hWnd, (HMENU) PORT_BOX_ID, GetModuleHandle(NULL), NULL);
            hIPText =   CreateWindowEx(0, "STATIC", "IP:", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 95, 50, 30, hWnd, (HMENU) IP_TEXT_ID, GetModuleHandle(NULL), NULL);
            hIP =       CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "localhost", WS_CHILD | WS_VISIBLE, 65, 90, 185, 30, hWnd, (HMENU) IP_BOX_ID, GetModuleHandle(NULL), NULL);
            hMapInfo =  CreateWindowEx(0, "STATIC", "Enter map file names below. Only hosts can choose the map.", WS_CHILD | WS_VISIBLE | SS_CENTER, 45, 130, 210, 40, hWnd, (HMENU) MAPINFO_TEXT_ID, GetModuleHandle(NULL), NULL);
            hDMText =   CreateWindowEx(0, "STATIC", "Dungeon Map:", WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 170, 95, 30, hWnd, (HMENU) DM_TEXT_ID, GetModuleHandle(NULL), NULL);
            hDM =       CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "maps/BasicDungeon.dm", WS_CHILD | WS_VISIBLE, 105, 170, 180, 30, hWnd, (HMENU) DM_BOX_ID, GetModuleHandle(NULL), NULL);
            hKMText =   CreateWindowEx(0, "STATIC", "Kitchen Map:", WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 205, 95, 30, hWnd, (HMENU) KM_TEXT_ID, GetModuleHandle(NULL), NULL);
            hKM =       CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "maps/BasicKitchen.km", WS_CHILD | WS_VISIBLE, 105, 205, 180, 30, hWnd, (HMENU) KM_BOX_ID, GetModuleHandle(NULL), NULL);
            hLMText =   CreateWindowEx(0, "STATIC", "Lobby Map:  ", WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 240, 95, 30, hWnd, (HMENU) LM_TEXT_ID, GetModuleHandle(NULL), NULL);
            hLM =       CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "maps/BasicLobby.lm", WS_CHILD | WS_VISIBLE, 105, 240, 180, 30, hWnd, (HMENU) LM_BOX_ID, GetModuleHandle(NULL), NULL);
            hHost =     CreateWindowEx(0, "BUTTON", "Host", WS_CHILD | WS_VISIBLE, 95, 280, 110, 30, hWnd, (HMENU) HOST_BTN_ID, GetModuleHandle(NULL), NULL);
            hJoin =     CreateWindowEx(0, "BUTTON", "Join", WS_CHILD | WS_VISIBLE, 95, 315, 110, 30, hWnd, (HMENU) JOIN_BTN_ID, GetModuleHandle(NULL), NULL);
            hStatus =   CreateWindowEx(0, "STATIC", cmdLine, WS_CHILD | WS_VISIBLE | SS_CENTER, 30, 360, 230, 60, hWnd, (HMENU) STATUS_TEXT_ID, GetModuleHandle(NULL), NULL);
            break;

        // Handle command messages to window
        case WM_COMMAND:
            
            // Handle if host button is clicked
            if (LOWORD(wParam) == HOST_BTN_ID) {
                collectMapStrings();
                if (getPort() && checkMapsExist()) spawnHost();
            } else if (LOWORD(wParam) == JOIN_BTN_ID) {
                if (getPort() && getHostIP()) spawnClient();
            }
            break;

        // Handles when a static control is about to be drawn (can be used to set text foreground and bg colors)
        case WM_CTLCOLORSTATIC:

            // Setting BG color
            SetBkColor((HDC)wParam, BG_COLOR);

            // Creating the HBRUSH background object - necessary for changing bg color
            if (hbrBkgnd == NULL)
            {
                hbrBkgnd = CreateSolidBrush(BG_COLOR);
            }
            return (INT_PTR)hbrBkgnd;
            break;

        // Handle window close
        case WM_DESTROY:
            PostQuitMessage(0);
            DeleteObject(hbrBkgnd);     // Cleaning up unneeded brush
            break;
        
        // Default window processing
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }

    // Finished handling message.
    return 0;
}

/**
 * Helper function called by WinMain to generate a WNDCLASSEX structure
 * for window class info
 */
WNDCLASSEX GenerateWindowClassInfo(_In_ HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);                           // Size of the WCEX structure
    wcex.style          = CS_HREDRAW | CS_VREDRAW;                      // Class style(s)
    wcex.lpfnWndProc    = WndProc;                                      // Window procedure pointer (handles window events)
    wcex.cbClsExtra     = 0;                                            // # of extra bytes to allocate the structure
    wcex.cbWndExtra     = 0;                                            // Extra bytes to allocate to the window instance
    wcex.hInstance      = hInstance;                                    // Handle to instance of application
    wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);         // Handle to class icon
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);                  // Handle to class cursor
    wcex.hbrBackground  = CreateSolidBrush(BG_COLOR);           // Handle to background brush (can be used for painting bg)
    wcex.lpszMenuName   = NULL;                                         // Pointer to resource name of class menu
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);    // Handle to icon associated with window class
    return wcex;
}

/**
 * Entry point - WinAPI's equivalent of the main method.
 */
int CALLBACK WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
) {
    // Generate window class info
    WNDCLASSEX wcex = GenerateWindowClassInfo(hInstance);

    // Make sure that the WNDCLASSEX structure is registered 
    if (!RegisterClassEx(&wcex))
    {
        // Error dialogue box for register failure
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Error"),
            MB_OK);
        return 1;
    }

    // Store command line value
    cmdLine = lpCmdLine;

    // Create a window with these properties
    HWND hWnd = CreateWindow(
        szWindowClass,                  // Application name
        szTitle,                        // Title text
        WS_OVERLAPPED | WS_SYSMENU,     // Type of window to create
        CW_USEDEFAULT, CW_USEDEFAULT,   // Initial position of window
        300, 450,                       // Initial size of window
        NULL,                           // Parent of this window
        NULL,                           // Menu bar to be used with this window (NULL for class menu)
        hInstance,                      // An instance of this module
        NULL
    );

    // Handle if the window failed to create
    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Error"),
            MB_OK);

        return 1;
    }

    // Display the window
    ShowWindow(hWnd, nCmdShow);     // Window object + command line args as parameters
    UpdateWindow(hWnd);

    // Main message loop
    // Loop will terminate when WM_QUIT is received and GetMessage returns false.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Return information about a WM_QUIT message (which is when the window is closed and the message loop ends)
    return (int) msg.wParam;
}

