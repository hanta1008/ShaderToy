#pragma once

#include <stdio.h>

#include "iWindow.h"

#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

void showLog(const char* str);
void showLog(int num);