#include "MGNetwork.h"
#include "LogIn.h"
#include <WinSock2.h>
#include <iostream>

using namespace std;

int main()
{
    LogIn* myLogInServer = new LogIn("127.0.0.1", "root", "bg0823", "gunman3duserdb", 3306);
    while (1);
}