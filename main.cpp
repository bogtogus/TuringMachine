#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include "turing.h"

int main() {
    if (!setlocale(LC_ALL, "")) std::cout << "Error setting locale." << std::endl;
    std::ios_base::sync_with_stdio(false);
    if (SetConsoleCP(CP_UTF8) == FALSE) std::cout << "Error setting win locale." << std::endl;
    if (SetConsoleOutputCP(CP_UTF8) == FALSE) std::cout << "Error setting win output locale." << std::endl;

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);
    TuringMachine tm;
    tm.mult(2,3);
    return 0;
}
