#include "checkPID.h"
#include <iostream>
using namespace checkPID;

bool checkPID::isRunning(std::string name)
{
    // Get the list of process identifiers.
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wname = converter.from_bytes(name);
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return false;
    }

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            if(matchProcessName( aProcesses[i], wname))
            {
                return true;
            }
        }
    }
    return false;
}

bool checkPID::matchProcessName( DWORD processID, std::wstring processName)
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod),
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName,
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }

    // Compare process name with your string
    bool matchFound = !_tcscmp(szProcessName, processName.c_str() );
    // Release the handle to the process.
    CloseHandle( hProcess );

    return matchFound;
}

//TCHAR* 2 CHAR*
//int size= wcslen(szProcessName);
//char ansi_string[30];
//wcstombs(ansi_string, szProcessName, size+1);
