#include <iostream>
#include <iomanip>
#include <cmath>
#include <ostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

void getProcessList();
void displayHeaders();
void displayInfo(PROCESSENTRY32 pe32, DWORD dwPriorityClass, PROCESS_MEMORY_COUNTERS pmc);
void displayValue(SIZE_T valueInBytes, int width);

int main(void)
{
    getProcessList();
    return 0;
}

void getProcessList()
{
    HANDLE hProcessSnap, hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    PROCESS_MEMORY_COUNTERS pmCounters;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "Unable to create ToolHelp32 snapshot of processes \nError: "
                  << GetLastError();
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32))
    {
        std::cout << "Failed to retrieve info about [System process] \nError: "
                  << GetLastError();
        CloseHandle(hProcessSnap);
        return;
    }

    displayHeaders();

    do
    {
        dwPriorityClass = 0;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL)
        {
            continue;
        }

        dwPriorityClass = GetPriorityClass(hProcess);
        if (GetProcessMemoryInfo(hProcess, &pmCounters, sizeof(pmCounters)) == 0)
        {
            std::cout << "Failed to get information regarding process memory for process with pid: " << pe32.th32ProcessID
                      << " | Error: " << GetLastError() << "\n";
            continue;
        }

        displayInfo(pe32, dwPriorityClass, pmCounters);
        CloseHandle(hProcess);

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return;
}

void displayInfo(PROCESSENTRY32 pe32, DWORD dwPriorityClass, PROCESS_MEMORY_COUNTERS pmc)
{
    std::cout << std::left << std::setw(60) << std::setfill(' ') << pe32.szExeFile
              << std::right << std::setw(6) << std::setfill(' ') << pe32.th32ProcessID
              << std::right << std::setw(14) << std::setfill(' ') << pe32.cntThreads
              << std::right << std::setw(13) << std::setfill(' ') << pe32.th32ParentProcessID
              << std::right << std::setw(16) << std::setfill(' ') << pe32.pcPriClassBase
              << std::right << std::setw(17) << std::setfill(' ') << dwPriorityClass
              << std::right << std::setw(15) << std::setfill(' ') << pmc.PageFaultCount;

    displayValue(pmc.WorkingSetSize, 11);
    displayValue(pmc.PeakWorkingSetSize, 15);
    displayValue(pmc.PagefileUsage, 14);
    displayValue(pmc.PeakPagefileUsage, 20);
    std::cout << "\n";
}

void displayValue(SIZE_T valueInBytes, int width)
{
    if (valueInBytes > 1048576)
    {
        std::cout << std::right << std::setw(width) << std::setfill(' ') << round(double(valueInBytes / 1048576)) << "MB";
    }
    else if (valueInBytes > 1024)
    {
        std::cout << std::right << std::setw(width) << std::setfill(' ') << round(double(valueInBytes / 1024)) << "KB";
    }
    else
    {
        std::cout << std::right << std::setw(width + 1) << std::setfill(' ') << valueInBytes << "B";
    }
}

void displayHeaders()
{
    std::cout << std::left << std::setw(60) << std::setfill(' ') << "Process Name"
              << std::right << std::setw(6) << std::setfill(' ') << "PID"
              << std::right << std::setw(14) << std::setfill(' ') << "Thread count"
              << std::right << std::setw(13) << std::setfill(' ') << "Parent PID"
              << std::right << std::setw(16) << std::setfill(' ') << "Priority base"
              << std::right << std::setw(17) << std::setfill(' ') << "Priority class"
              << std::right << std::setw(15) << std::setfill(' ') << "Page Faults"
              << std::right << std::setw(13) << std::setfill(' ') << "Mem Usage"
              << std::right << std::setw(17) << std::setfill(' ') << "Peak Mem Usage"
              << std::right << std::setw(16) << std::setfill(' ') << "Pagefile Usage"
              << std::right << std::setw(23) << std::setfill(' ') << "Peak Pagefile Usage\n"
              << std::left << std::setw(209) << std::setfill('=') << "=" << "\n";
}
