#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include "turing.h"

using TM = TuringMachine;
using qCmd = TuringMachine::qCmd;
using Cmd = TuringMachine::qCmd::Cmd;
using Action = TuringMachine::Action;

int main() {
    if (!setlocale(LC_ALL, "")) std::cout << "Error setting locale." << std::endl;
    std::ios_base::sync_with_stdio(false);
    if (SetConsoleCP(CP_UTF8) == FALSE) std::cout << "Error setting win locale." << std::endl;
    if (SetConsoleOutputCP(CP_UTF8) == FALSE) std::cout << "Error setting win output locale." << std::endl;

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);
    TuringMachine tm;
    tm.setAlphabet({'1', '*', 'A', 'B', EMPTY});
    tm.setStates(1);
    std::map<q_, qCmd> program = {
    /*   qᵢaᵢ → aₓMqₓ   */
        {0, // current state qᵢ
         qCmd( // set of commands for current state
                std::map<wchar_t, Cmd>{ // commands on current q state
                        /* aᵢ - current symbol; 
                           aₓ - symbol to write; 
                           M - action; 
                           qₓ - next state.

                        { aᵢ,    { aₓ,         M, qₓ}}    */
                        {'1', Cmd{'1', Action::R, 0 }},
                        {'*', Cmd{'*', Action::E, 1 }},
                }
         )
        },
    };
    if (tm.setProgram(program) != 0) {
        std::cout << "Ошибка программирования машины!" << std::endl;
    }
    const std::wstring data = L"111*11=";
    tm.writeOnTape(data);
    tm.run();
    return 0;
}
