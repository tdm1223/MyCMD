#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<tchar.h>
#include<locale.h>
#include<windows.h> 

const int CMD_TOKEN_NUM = 10;
const TCHAR ERROR_CMD[] = _T("'%s'은(는) 실행할 수 있는 프로그램이 아닙니다. \n");

TCHAR cmdString[MAX_PATH];
TCHAR cmdTokenList[CMD_TOKEN_NUM][MAX_PATH];
TCHAR seps[] = _T(" ,\t\n");

int CmdProcessing(int tokenNum);
TCHAR* StrLower(TCHAR*);
int CmdReadTokenize();
int main(int argc, TCHAR* argv[])
{
    // 한글 입력을 가능케 하기 위해.
    _tsetlocale(LC_ALL, _T("Korean"));

    if (argc > 2)
    {
        for (int i = 1; i < argc; i++)
        {
            _tcscpy(cmdTokenList[i - 1], argv[i]);
        }
        CmdProcessing(argc - 1);
    }
    DWORD isExit = NULL;
    while (1)
    {
        int tokenNum = CmdReadTokenize();

        // 명령의 미입력
        if (tokenNum == 0)
        {
            continue;
        }

        isExit = CmdProcessing(tokenNum);
        if (isExit == TRUE)
        {
            _fputts(_T("명령어 처리를 종료합니다. \n"), stdout);
            break;
        }
    }

    return 0;
}

// 명령을 입력받아 토큰화하는 함수
// 토큰 개수 반환
int CmdReadTokenize()
{
    TCHAR* token;

    _fputts(_T("Command Prompt>> "), stdout);
    _getts_s(cmdString, sizeof(cmdString));

    token = _tcstok(cmdString, seps);
    int tokenNum = 0;
    while (token != NULL)
    {
        // 토큰 리스트에 대소문자 구별없이 차례대로 저장
        _tcscpy(cmdTokenList[tokenNum++], StrLower(token));
        token = _tcstok(NULL, seps);
    }

    return tokenNum;
}

int CmdProcessing(int tokenNum)
{
    BOOL isRun;
    STARTUPINFO si = { 0, };
    PROCESS_INFORMATION pi;

    TCHAR cmdStringWithOptions[MAX_PATH] = { 0, };
    TCHAR optString[MAX_PATH] = { 0, };

    si.cb = sizeof(si);

    if (!_tcscmp(cmdTokenList[0], _T("exit")))
    {
        return TRUE;
    }
    else if (!_tcscmp(cmdTokenList[0], _T("start")))
    {
        // start를 제외한 나머지 문자열을 재구성
        if (tokenNum > 1)
        {
            for (int i = 1; i < tokenNum; i++)
            {
                _stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);
                _stprintf(cmdStringWithOptions, _T("%s %s"), _T("windows.exe"), optString);
            }
        }
        // start만 입력
        else
        {
            _stprintf(cmdStringWithOptions, _T("%s"), _T("windows.exe"));
        }

        isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else if (!_tcscmp(cmdTokenList[0], _T("echo")))
    {
        for (int i = 1; i < tokenNum; i++)
        {
            _stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);

        }
        _tprintf(_T("echo message : %s \n"), optString);
    }
    else
    {
        // 프로그램 실행
        _tcscpy(cmdStringWithOptions, cmdTokenList[0]);

        for (int i = 1; i < tokenNum; i++)
        {
            _stprintf(cmdStringWithOptions, _T("%s %s"), cmdStringWithOptions, cmdTokenList[i]);
        }

        isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (isRun == FALSE)
        {
            _tprintf(ERROR_CMD, cmdTokenList[0]);
        }
    }

    return 0;
}

/*
    문자열의 내에 존재하는 모든 대문자를 소문자로 변경한다.
    변경된 문자열의 포인터를 반환한다.
*/
TCHAR* StrLower(TCHAR* pStr)
{
    TCHAR* ret = pStr;
    while (*pStr)
    {
        if (_istupper(*pStr))
        {
            *pStr = _totlower(*pStr);
        }
        pStr++;
    }
    return ret;
}