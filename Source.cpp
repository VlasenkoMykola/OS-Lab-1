
//pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
using namespace std;

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <string.h>
#include <tchar.h>
#include "atlstr.h"

enum FuncCallStatus {
    SUCCESS,
    FAILURE,
    UNKNOWN,
    CANCELLED
};

struct FuncCallResult {
    FuncCallStatus status = UNKNOWN;
    int value = 0;
};


class AsyncFuncCall
{
private:
    std::chrono::time_point<std::chrono::system_clock> start;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

#define SIZE 40
public:
    FuncCallResult result;

    AsyncFuncCall(const char* fname, int x):
	result {UNKNOWN, 0}
	{
	start = std::chrono::system_clock::now();
	char cmdline_buffer[SIZE];
	TCHAR wincmdline[SIZE] = {0, };
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	snprintf(cmdline_buffer, SIZE - 1, "func.exe %c %d", fname[0], x);
	//_tcscpy(CMDLINE, A2T(cmdline_buffer));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cmdline_buffer, strlen(cmdline_buffer), wincmdline, SIZE);
	// Start the child process.
	if( !CreateProcess(
		NULL,   // No module name (use command line)
		wincmdline,     // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
	    )
	{
	    printf( "CreateProcess failed (%d).\n", GetLastError() );
	    cout << "Oops! exec failed. fname=" << fname << " buf=" << cmdline_buffer << endl;
	} else {
	    cout << "begot a child. func=" << fname << " pid=" << pi.dwProcessId << endl;
	}
    }

    FuncCallResult wait(void) {
	auto timeout = std::chrono::milliseconds(500);
	DWORD ExitCode;
	if (result.status != UNKNOWN) return result;
	do {
	    // Wait until child process exits.
	    //WaitForSingleObject( pi.hProcess, INFINITE );
	    BOOL status = GetExitCodeProcess( pi.hProcess, &ExitCode );
	    if (!status) {
		printf( "GetExitCodeProcess failed (%d).\n", GetLastError() );
		cout << "waited for a child. pid=" << pi.dwProcessId << endl;
		result.status = FAILURE;
		return result;
	    }
	    if (ExitCode != STILL_ACTIVE) {
		if (ExitCode > 255) {
		    result.status=FAILURE;
		    result.value=0;
		    cout << "child killed. code=" << ExitCode << endl;
		} else {
		    result.status=SUCCESS;
		    result.value=(int) ExitCode;
		    cout << "child exited. code=" << ExitCode << " val=" << result.value << endl;
		}
		// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return result;
	    }
	    this_thread::sleep_for(chrono::milliseconds(100));
	} while ((std::chrono::system_clock::now() - start) < timeout);
	result.status = CANCELLED;
	cout << "cancelling a hanged child. pid=" << pi.dwProcessId << endl;
	TerminateProcess(pi.hProcess, 1);
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return result;
    }
};

FuncCallResult binop_xor(FuncCallResult f, FuncCallResult g) {
    FuncCallResult result = {UNKNOWN, 0};
    if (f.status == SUCCESS && g.status == SUCCESS) {
	result.status = SUCCESS;
	result.value = f.value xor g.value;
	return result;
    } else {
	result.status = FAILURE;
	return result;
    }
    return result;
}

FuncCallResult async_call_binop (int x) {
    AsyncFuncCall f("f", x);
    AsyncFuncCall g("g", x);
    FuncCallResult result = binop_xor(f.wait(), g.wait());
    return result;
}

void inputoutput_test() {
	int x;
	FuncCallResult func_call;
	cout << "Type in value for X:" << endl;
	cin >> x;
	cout << "test x: " << x << endl;
	func_call = async_call_binop(x);
	if (func_call.status == SUCCESS) {
		cout << "func result: " << func_call.value << endl;
	} else if (func_call.status == FAILURE) {
		cout << "x is rejected: " << x << endl;
	} else if (func_call.status == UNKNOWN) {
		cout << "unknown result for x: " << x << endl;
	};

}

int main(int argc, char** argv) {
	inputoutput_test();
	return 0;
}
