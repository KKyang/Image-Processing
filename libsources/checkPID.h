#ifndef CHECKPID_H
#define CHECKPID_H

#define _UNICODE
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <tchar.h>
#include <locale>
#include <codecvt>
#include <string>

namespace checkPID
{
    bool isRunning(std::string name);
    bool matchProcessName( DWORD processID, std::wstring processName);
}

#endif // CHECKPID_H
