#pragma once
#include <tchar.h>
class LoggerMember;

class Logger
{
public:
    Logger(const TCHAR* filename, const TCHAR* filepath = _T(".."), unsigned int threadcount = 2);
    ~Logger();

public:
    virtual void const            Log(const TCHAR* format, ...);                    // log�� �ۼ��� ���� �Է�

protected:
    virtual void                LoggerFunction();                                // log�� �ۼ��ϴ� ���� �Լ�
    virtual void const            PrintLog(const TCHAR* log);                        // log�� ����ϴ� �Լ�( ������ ���� )

protected:
    LoggerMember* m_pLoggerMember;        // �α� ������ ����� ��� ���� Ŭ����
    const TCHAR* m_filePath;                // �α� ������ ������ ���
    const TCHAR* m_fileName;                // �α� ������ �̸�
    bool                m_bExit;                // �ΰ� ���� �÷���
    const unsigned int    m_iThreadCount;            // �ΰ��� ������ ����
};
