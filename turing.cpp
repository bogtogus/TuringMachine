#include "turing.h"
#include <iostream>

TM::TM() {
    for (int i = 0; i < lineSize; i++) {
        tape.push_back(EMPTY);
    }
    alphabet.insert(alphabet.begin(), {'1', '*', 'A', 'B', EMPTY});
}

TM::~TM() {
    tape.clear();
    alphabet.clear();
}

void TM::resetTape() {
    std::vector<wchar_t>::iterator it = tape.begin();
    while (it != tape.end()) {
        *it = EMPTY;
        ++it;
    }
}

int TM::mult(const int& x, const int& y) {
    const int sizeNeeded = x + y + 2 + x * y;
    if (x < 1 || y < 1 || sizeNeeded > maxLineSize) {
        return -1;
    }
    position = 0;
    resetTape();
    int i = 0;
    for (; i < x; ++i) {
        tape[i] = '1';
    }
    tape[i++] = '*';
    for (; i < x+y+1; ++i) {
        tape[i] = '1';
    }
    tape[i] = '=';

    qstates = {0, 1, 2, 3, 4, 5, 6, 7};
    program = {
        {0, // current state
         TM::qCmd(this, // parent
                  std::map<wchar_t, qCmd::Cmd>{ // commands on current state q
                                               {'1', // current symbol
                                                TM::qCmd::Cmd{'1', Action::R, 0} // command on current state and current symbol
                                               },
                                               {'*', // current symbol
                                                TM::qCmd::Cmd{'*', Action::E, 1} // command on current state and current symbol
                                               },
                                              }
                 )
        },
    };
    Error e = Error::NoError;
    while (e != Error::EndOfProgram) {
        std::wcout << program.at(state) << std::endl;
        e = program[state].run();
        std::wcout << program.at(state) << std::endl;
    }
    return 0;
}

TM::Error TM::move(const Action& act) {
    switch (act) {
        case Action::N:
            break;
        case Action::L:
            if (position == 0 && tape.size() < maxLineSize) {
                tape.insert(tape.begin(), EMPTY);
            } else if (tape.size() >= maxLineSize) {
                return Error::Overflow;
            }
            break;
        case Action::R:
            if (position == tape.size() - 1 && tape.size() < maxLineSize) {
                tape.push_back(EMPTY);
            } else if (tape.size() >= maxLineSize) {
                return Error::Overflow;
            }
            ++position;
            break;
        case Action::E:
            return Error::EndOfProgram;
        default:
            break;
    }
    return Error::NoError;
}

TM::Error TM::writeSymbol(const wchar_t& symb) {
    std::vector<wchar_t>::const_iterator ita = alphabet.cbegin();
    while (ita != alphabet.cend()) {
        if (*ita == symb) {
            break;
        }
        ++ita;
    }
    if (ita == alphabet.cend()) {
        return Error::WrongChar;
    }
    return Error::NoError;
}

