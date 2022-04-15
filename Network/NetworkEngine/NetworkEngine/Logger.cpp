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
    friend Logger;            // MGLogger������ private ������ ���� ������ �� �ֵ��� friend Ű���� ���
private:
    LoggerMember()
    {
        // ũ��Ƽ�� ���� �ʱ�ȭ
        InitializeCriticalSection(&CS_LogQueue);

        hEventLogQueue = nullptr;
        hLogFile = nullptr;
    }
    ~LoggerMember()
    {
        // Logger ���� ��ȣ ����
        if (nullptr != hEventLogQueue)
        {
            SetEvent(hEventLogQueue);
        }

        // Logger ������ ����
        size_t threadCount = LoggerThreadVec.size();
        for (size_t i = 0; i < threadCount; ++i)
        {
            LoggerThreadVec[i].join();
        }
        LoggerThreadVec.clear();

        // ũ��Ƽ�� ���� ����
        DeleteCriticalSection(&CS_LogQueue);

        // �̺�Ʈ �ڵ� �ݱ�
        CloseHandle(hEventLogQueue);
        hEventLogQueue = nullptr;

        // ���� �ڵ� �ݱ�
        CloseHandle(hLogFile);
        hLogFile = nullptr;
    }

    std::queue<const TCHAR*>     LogQueue;             // �α׸� ���Լ�������� �����ϱ� ���� �α� ť
    std::vector<std::thread>    LoggerThreadVec;      // �ΰ� �����带 �ѹ��� �����ϱ� ���� ����
    CRITICAL_SECTION            CS_LogQueue;          // �α�ť�� ũ��Ƽ�� ����
    HANDLE                      hEventLogQueue;       // �α�ť�� �̺�Ʈ �ڵ�
    HANDLE                      hLogFile;             // �α� ������ �ڵ�
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

    // �α� ���� ��� ����
    int nResult = _mkdir(m_filePath);

    // �ΰ� ���� ��� ���� ����
    m_pLoggerMember = new LoggerMember;

    // �̺�Ʈ �ڵ� ����
    m_pLoggerMember->hEventLogQueue = CreateEvent(NULL, FALSE, FALSE, m_fileName);

    // ���� ��� �� �̸� ���� �����
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szTimeLog[1024] = { 0, };
    sprintf_s(szTimeLog, sizeof(szTimeLog), _T("%s/%04d-%02d-%02dT%02d-%02d-%02d_%s.txt"),
        m_filePath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, m_fileName);

    // �� ���� �ڵ��� ������ �ʾҾ �ܺο��� �� ������ ���� ���� �ְ� ��
    m_pLoggerMember->hLogFile = CreateFile(szTimeLog, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    // Logger ������ ����
    for (size_t i = 0; i < m_iThreadCount; ++i)
    {
        m_pLoggerMember->LoggerThreadVec.emplace_back(std::thread(&Logger::LoggerFunction, this));
    }
}

Logger::~Logger()
{
    // Logger ���� ��ȣ ����
    m_bExit = true;

    // �ΰ� �⺭ ���� �Ҹ��� ȣ��
    delete m_pLoggerMember;
    m_pLoggerMember = nullptr;
}

void const Logger::Log(const TCHAR* format, ...)
{
    // �α� ���� �����
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szTimeLog[1024] = { 0, };

    // ���ڿ� ������
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

    // �Ӱ� ����
    {
        EnterCriticalSection(&m_pLoggerMember->CS_LogQueue);

        m_pLoggerMember->LogQueue.push(szTimeLog);

        LeaveCriticalSection(&m_pLoggerMember->CS_LogQueue);
    }

    // �̺�Ʈ ����
    SetEvent(m_pLoggerMember->hEventLogQueue);
}

void Logger::LoggerFunction()
{
    while (!m_bExit)
    {
        // �α� �̺�Ʈ�� �߻��� �� ���� ���
        WaitForSingleObject(m_pLoggerMember->hEventLogQueue, INFINITE);

        // �Ӱ� ����
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

            // ���Ͽ� �α� �ۼ�
            DWORD dwWrite;
            WriteFile(m_pLoggerMember->hLogFile, nowLog, static_cast<DWORD>(_tcslen(nowLog)), &dwWrite, nullptr);

            PrintLog(nowLog);            // ���� ���� �ۼ��� �α׸� ���..

            LeaveCriticalSection(&m_pLoggerMember->CS_LogQueue);
        }

        // ������ ��� ���� ������ �ٷ� ��ũ�� ���
        // ������ �� ��� ������, ���� ����� ���� Ȯ���� ���߱� ���ؼ� ���
        FlushFileBuffers(m_pLoggerMember->hLogFile);
    }
}

void const Logger::PrintLog(const TCHAR* log)
{
    printf_s(_T("%s"), log);
}
