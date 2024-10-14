#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <filesystem>
#include "turing.h"

using TM = TuringMachine;
using Cmd = TuringMachine::Cmd;
using Action = TuringMachine::Action;

int main() {
    if (!setlocale(LC_ALL, "")) std::cout << "Error setting locale." << std::endl;
    std::ios_base::sync_with_stdio(false);
    if (SetConsoleCP(CP_UTF8) == FALSE) std::cout << "Error setting win locale." << std::endl;
    if (SetConsoleOutputCP(CP_UTF8) == FALSE) std::cout << "Error setting win output locale." << std::endl;

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);
    TuringMachine tm;
    //std::filesystem::path cwd = std::filesystem::current_path();
    //std::cout << cwd << std::endl;
    //tm.readConfFromFile("C:/Users/BOGDAN/projects/7sem/Theory of calculating processes/lab2/mult.conf");
    tm.setAlphabet({'1', '*', 'A', 'B', '=', EMPTY});
    tm.setStates(7);
    std::map<q_, qCmd> program = {
    /*   qᵢaᵢ → aₓMqₓ   */
        {0, // current state qᵢ
         qCmd{ // commands on current q state
                 /* aᵢ - current symbol; 
                    aₓ - symbol to write; 
                    M - action; 
                    qₓ - next state.
    
                 { aᵢ,    { aₓ,         M, qₓ}}    */
                 {'1', Cmd{     Action::R    }},
                 {'*', Cmd{'*', Action::L, 1 }},
                 {EMPTY, Cmd{   Action::E    }}
         }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {1 , qCmd{{'1', Cmd{'A', Action::R, 2 }},
                  {'A', Cmd{     Action::L    }},
                  {EMPTY, Cmd{   Action::E    }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {2 , qCmd{{'1', Cmd{     Action::R    }},
                  {'*', Cmd{     Action::R    }},
                  {'=', Cmd{'=', Action::L, 3 }},
                  {'A', Cmd{     Action::R    }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {3 , qCmd{{'1', Cmd{'B', Action::R, 3 }},
                  {'=', Cmd{'=', Action::R, 4 }},
                  {'B', Cmd{     Action::R    }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {4 , qCmd{{'1', Cmd{     Action::R    }},
                  {EMPTY, Cmd{'1', Action::L, 5 }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {5 , qCmd{{'1', Cmd{     Action::L    }},
                  {'=', Cmd{     Action::L    }},
                  {'B', Cmd{'B', Action::L, 6 }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {6 , qCmd{{'1', Cmd{'1', Action::N, 3 }},
                  {'*', Cmd{'*', Action::R, 7 }},
                  {'B', Cmd{     Action::L,   }}
                 }
        },
    /*  {qᵢ,      { aᵢ,    { aₓ,         M, qₓ}}*/
        {7 , qCmd{{'1', Cmd{     Action::L    }},
                  {'*', Cmd{'*', Action::L, 1 }},
                  {'=', Cmd{     Action::L    }},
                  {'B', Cmd{'1', Action::R, 7 }}
                 }
        }
    };
    if (tm.setProgram(program) != 0) {
        std::cout << "Ошибка программирования машины!" << std::endl;
    } else {
        int size = sizeof(program);
        std::map<q_, qCmd>::const_iterator it = program.cbegin();
        while(it != program.cend()) {
            size += sizeof(it->first);
            size += sizeof(qCmd);
            size += it->second.size() * sizeof(TM::Cmd);
            ++it;
        }
        std::cout << "Размер программы " << size << " байт." << std::endl;
    }
    const std::wstring data = L"111*11=";
    tm.writeOnTape(data);
    tm.run();
    return 0;
}
