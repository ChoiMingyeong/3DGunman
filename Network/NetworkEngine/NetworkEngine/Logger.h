#pragma once
#include <tchar.h>
class LoggerMember;

class Logger
{
public:
    Logger(const TCHAR* filename, const TCHAR* filepath = _T(".."), unsigned int threadcount = 2);
    ~Logger();

public:
    virtual void const            Log(const TCHAR* format, ...);                    // log로 작성할 내용 입력

protected:
    virtual void                LoggerFunction();                                // log를 작성하는 메인 함수
    virtual void const            PrintLog(const TCHAR* log);                        // log를 출력하는 함수( 재정의 가능 )

protected:
    LoggerMember* m_pLoggerMember;        // 로그 내에서 사용할 멤버 변수 클래스
    const TCHAR* m_filePath;                // 로그 파일이 생성될 경로
    const TCHAR* m_fileName;                // 로그 파일의 이름
    bool                m_bExit;                // 로거 실행 플래그
    const unsigned int    m_iThreadCount;            // 로거의 쓰레드 개수
};
