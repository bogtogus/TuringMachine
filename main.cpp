#include <iostream>
#include <locale>
#include <codecvt>
#include "turing.h"

int main() {
    if (!setlocale(LC_ALL, "")) std::cout << "Error setting locale." << std::endl;
    std::ios_base::sync_with_stdio(false);

    std::locale utf8( std::locale(), new std::codecvt_utf8_utf16<wchar_t> );
    std::wcout.imbue(utf8);
    TM tm;
    tm.mult(2,3);
    return 0;
}
