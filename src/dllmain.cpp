#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <memcury.h>

#define WIN32_LEAN_AND_MEAN

const char* logo =
R"( _____  _____  ________  ________  ____  _____
|_   _||_   _||_   __  ||_   __  ||_   \|_   _|
  | |    | |    | |_ \_|  | |_ \_|  |   \ | |
  | '    ' |    |  _| _   |  _|     | |\ \| |
   \ \__/ /    _| |__/ | _| |_     _| |_\   |_
    `.__.'    |_________|_____|   |_____|_____|
 ____________         __                 __
|_   _||_   _|       [  |               [  |  _
  | |    | | _ .--.   | |  .--.   .---.  | | / ] .---.  _ .--.
  | '    ' |[ `.-. |  | |/ .'`\ \/ /'`\] | '' < / /__\\[ `/'`\]
   \ \__/ /  | | | |  | || \__. || \__.  | |`\ \| \__., | |
    `.__.'  [___||__][___]'.__.' '.___.'[__|  \_]'.__.'[___]

)";

static auto currentProcess = GetCurrentProcess();

inline void writeMemory(const uintptr_t address, const std::vector<BYTE> toWrite) {
    WriteProcessMemory(currentProcess, (LPVOID)address, toWrite.data(), toWrite.size(), NULL);
}

void Main(const HMODULE hModule) {
    AllocConsole();
    SetConsoleTitleA("UEFN Unlocker by gamerbross v1.1 among us");
    FILE* pFile;
    freopen_s(&pFile, ("CONOUT$"), "w", stdout);

    std::cout << logo << "Made by @gamerbross_ on X/Twitter!\n";

    // rel16/32
    static const std::vector<BYTE> jeBytes  = { 0x0F, 0x84 };
    static const std::vector<BYTE> jneBytes = { 0x0F, 0x85 };
    static const std::vector<BYTE> jlBytes  = { 0x0F, 0x8C };

    static const std::vector<BYTE> jnoBytes = { 0x0F, 0x81 }; // using jump if not overflow cus easier and should always work
    static const std::vector<BYTE> nopBytes = { 0x90, 0x90 };

    // rel8
    static const std::vector<BYTE> xorByte  = { 0x32 };
    static const std::vector<BYTE> je8Byte  = { 0x74 };
    static const std::vector<BYTE> jb8Byte  = { 0x72 };
    static const std::vector<BYTE> jmp8Byte = { 0x71 };

    for (auto str : { L"Error_CannotModifyCookedAssets", L"Unable to Edit Cooked asset" } )
        writeMemory(Memcury::Scanner::FindStringRef(str).ScanFor(xorByte).Get(), nopBytes);

    writeMemory(
        Memcury::Scanner::FindStringRef(L"Folder '{0}' is read only and its contents cannot be edited")
        .ScanFor(jneBytes, false).Get(),
        jnoBytes
    );
    writeMemory(
        Memcury::Scanner::FindStringRef(L"Alias asset '{0}' is in a read only folder. Unable to edit read only assets.")
        .ScanFor(jeBytes, false).Get(),
        jnoBytes
    );

    writeMemory(
        Memcury::Scanner::FindStringRef(L"CannotDuplicateCooked").FindFunctionBoundary().ScanFor(jlBytes).Get(),
        jnoBytes
    );

    writeMemory(
        Memcury::Scanner::FindStringRef(L"Package is cooked or missing editor data\n").ScanFor(je8Byte, false).Get(),
        jmp8Byte
    );

    std::cout << "Done!\n";
}

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD dwReason, const LPVOID lpReserved)
{
    if (dwReason != DLL_PROCESS_ATTACH)
        return TRUE;

    auto mainThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, NULL);
    if (mainThread)
        CloseHandle(mainThread);

    return TRUE;
}
