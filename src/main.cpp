#include <windows.h>
#include <string>
#include <format>
#include <iostream>
#include <TlHelp32.h>
#include <winnt.h>

// process : make a snapshot, check if theres any processes, iterate through
// snapshot using GetFirstProcess, check if next exists using GetNextProcess,
// grab handle and return if it exists

DWORD getProcessId(std::wstring &processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    DWORD processId = 0;
    PROCESSENTRY32W process{};
    std::cout << process.dwSize << std::endl;
    process.dwSize = sizeof(process);

    if (Process32FirstW(snapshot, &process)) {
        do {
            if (process.szExeFile == processName) {
                processId = process.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &process));
    }

    CloseHandle(snapshot);
    return processId;
}

// LoadLibraryA -> using this function to load the dll.
// We get a handle to the process -> OpenProcess with the proper flags
// PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD Reserve
// enough space for the dll path into the exe:  MEM_COMMIT | MEM_RESERVE with a
// parameter PAGE_READWRITE, with the returned address we WriteMemory and we
// write the apth We fetch the LoadLibraryA from kernerl32 (GetProcessAddress
// and GetModuleHandleA)

BOOL InjectDll(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
                                           PROCESS_CREATE_THREAD,
                                       FALSE, processId);

    if (processHandle == INVALID_HANDLE_VALUE)
        return 0;

    return 1;
}

int main() {
    std::wstring processName = L"Notepad.exe";
    DWORD processId = getProcessId(processName);
    std::wcout << std::format(L"Found {} at {:#08X}", processName, processId);
    // std::printf("", processName.data(), processId);
    return 0;
}