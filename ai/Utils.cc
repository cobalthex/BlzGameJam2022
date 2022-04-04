#include "Utils.hh"

#ifdef WIN32

extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char*);
#define OutputDebugString OutputDebugStringA


void DebugLog(const char* log)
{
    OutputDebugStringA(log);
}

#endif