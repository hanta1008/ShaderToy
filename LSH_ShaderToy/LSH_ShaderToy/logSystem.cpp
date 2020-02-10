#include "logSystem.h"

void showLog(const char* str)
{
	printf("%s\n", str);
}

void showLog(int num)
{
	printf("%d\n", num);
}

void showWndCtrlInfo(WndCtrl* wndCtrl)
{
	int ID = wndCtrl->wndID;

	printf("ID : %d\n", ID);
}

void gitTest()
{
	//깃허브 커밋 테스트용
}