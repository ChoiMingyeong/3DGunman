#include "Logger.h"

#include <vector>
#include <queue>
#include <thread>
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "Logger.h"

class LoggerMember
{
    friend Logger;            // MGLogger에서는 private 변수에 쉽게 접근할 수 있도록 friend 키워드 사용
private:
    LoggerMember()
    {
        // 크리티컬 섹션 초기화
        InitializeCriticalSection(&CS_LogQueue);

        hEventLogQueue = nullptr;
        hLogFile = nullptr;
    }
    ~LoggerMember()
    {
        // Logger 종료 신호 보냄
        if (nullptr != hEventLogQueue)
        {
            SetEvent(hEventLogQueue);
        }

        // Logger 쓰레드 종료
        size_t threadCount = LoggerThreadVec.size();
        for (size_t i = 0; i < threadCount; ++i)
        {
            LoggerThreadVec[i].join();
        }
        LoggerThreadVec.clear();

        // 크리티컬 섹션 삭제
        DeleteCriticalSection(&CS_LogQueue);

        // 이벤트 핸들 닫기
        CloseHandle(hEventLogQueue);
        hEventLogQueue = nullptr;

        // 파일 핸들 닫기
        CloseHandle(hLogFile);
        hLogFile = nullptr;
    }

    std::queue<const TCHAR*>     LogQueue;             // 로그를 선입선출식으로 저장하기 위한 로그 큐
    std::vector<std::thread>    LoggerThreadVec;      // 로거 쓰레드를 한번에 관리하기 위한 벡터
    CRITICAL_SECTION            CS_LogQueue;          // 로그큐의 크리티컬 섹션
    HANDLE                      hEventLogQueue;       // 로그큐의 이벤트 핸들
    HANDLE                      hLogFile;             // 로그 파일의 핸들
};

Logger::Logger(const TCHAR* filename, const TCHAR* filepath, unsigned int threadcount)
    :m_iThreadCount(threadcount), m_bExit(false)
{
    int nameLength = _tcslen(filename);
    TCHAR* tempName = new TCHAR[nameLength + sizeof(TCHAR)];
    strcpy_s(tempName, nameLength + sizeof(TCHAR), filename);
    m_fileName = tempName;

    TCHAR tempPath[1024] = { 0, };
    sprintf_s(tempPath, sizeof(tempPath), _T("%s/log"), filepath);
    m_filePath = tempPath;

    // 로그 파일 경로 생성
    int nResult = _mkdir(m_filePath);

    // 로거 관련 멤버 변수 생성
    m_pLoggerMember = new LoggerMember;

    // 이벤트 핸들 생성
    m_pLoggerMember->hEventLogQueue = CreateEvent(NULL, FALSE, FALSE, m_fileName);

    // 파일 경로 및 이름 형식 만들기
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szTimeLog[1024] = { 0, };
    sprintf_s(szTimeLog, sizeof(szTimeLog), _T("%s/%04d-%02d-%02dT%02d-%02d-%02d_%s.txt"),
        m_filePath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, m_fileName);

    // 이 파일 핸들이 닫히지 않았어도 외부에서 이 파일을 읽을 수는 있게 함
    m_pLoggerMember->hLogFile = CreateFile(szTimeLog, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    // Logger 쓰레드 생성
    for (size_t i = 0; i < m_iThreadCount; ++i)
    {
        m_pLoggerMember->LoggerThreadVec.emplace_back(std::thread(&Logger::LoggerFunction, this));
    }
}

Logger::~Logger()
{
    // Logger 종료 신호 보냄
    m_bExit = true;

    // 로거 멤벼 변수 소멸자 호출
    delete m_pLoggerMember;
    m_pLoggerMember = nullptr;
}

void const Logger::Log(const TCHAR* format, ...)
{
    // 로그 형식 만들기
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szTimeLog[1024] = { 0, };

    // 문자열 포맷팅
    va_list vl;
    va_start(vl, format);
    int len = _vsctprintf(format, vl) + 1;
    TCHAR* _buffer = new TCHAR[len + 1];
    vsprintf_s(_buffer, len + 1, format, vl);
    va_end(vl);

    sprintf_s(szTimeLog, sizeof(szTimeLog),
        _T("%04d-%02d-%02d %02d:%02d:%02d - %s\n"),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, _buffer);

    delete[] _buffer;
    _buffer = nullptr;

    // 임계 영역
    {
        EnterCriticalSection(&m_pLoggerMember->CS_LogQueue);

        m_pLoggerMember->LogQueue.push(szTimeLog);

        LeaveCriticalSection(&m_pLoggerMember->CS_LogQueue);
    }

    // 이벤트 셋팅
    SetEvent(m_pLoggerMember->hEventLogQueue);
}

void Logger::LoggerFunction()
{
    while (!m_bExit)
    {
        // 로그 이벤트가 발생할 때 까지 대기
        WaitForSingleObject(m_pLoggerMember->hEventLogQueue, INFINITE);

        // 임계 영역
        {
            EnterCriticalSection(&m_pLoggerMember->CS_LogQueue);

            if (m_pLoggerMember->LogQueue.empty())
            {
                ResetEvent(m_pLoggerMember->hEventLogQueue);
                LeaveCriticalSection(&m_pLoggerMember->CS_LogQueue);
                continue;
            }

            const TCHAR* nowLog = m_pLoggerMember->LogQueue.front();
            m_pLoggerMember->LogQueue.pop();

            // 파일에 로그 작성
            DWORD dwWrite;
            WriteFile(m_pLoggerMember->hLogFile, nowLog, static_cast<DWORD>(_tcslen(nowLog)), &dwWrite, nullptr);

            PrintLog(nowLog);            // 뭔가 현재 작성한 로그를 출력..

            LeaveCriticalSection(&m_pLoggerMember->CS_LogQueue);
        }

        // 파일의 출력 버퍼 내용을 바로 디스크에 기록
        // 성능을 더 잡아 먹지만, 파일 기록의 유실 확률을 낮추기 위해서 사용
        FlushFileBuffers(m_pLoggerMember->hLogFile);
    }
}

void const Logger::PrintLog(const TCHAR* log)
{
    printf_s(_T("%s"), log);
}
