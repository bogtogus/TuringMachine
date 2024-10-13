#include "turing.h"
#include <iostream>

TuringMachine::TuringMachine() {
    for (int i = 0; i < lineSize; i++) {
        tape.push_back(EMPTY);
    }
    alphabet.insert(alphabet.begin(), {'1', '*', 'A', 'B', EMPTY});
}

TuringMachine::~TuringMachine() {
    tape.clear();
    alphabet.clear();
}

TuringMachine::Error TuringMachine::next() {
    const wchar_t curSymb = tape.at(position);
    const qCmd::Cmd cmd = program.at(curState).getCmd(curSymb);
    if (cmd.symbolToWrite == '\0') {
        return Error::WrongChar;
    }
    Error e = writeSymbol(cmd.symbolToWrite);
    if (e != Error::NoError) {
        return e;
    }
    e = move(cmd.action);
    curState = cmd.nextState;
    return e;
}

void TuringMachine::resetTape() {
    std::vector<wchar_t>::iterator it = tape.begin();
    while (it != tape.end()) {
        *it = EMPTY;
        ++it;
    }
}

int TuringMachine::mult(const int& x, const int& y) {
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
         TuringMachine::qCmd(
                std::map<wchar_t, qCmd::Cmd>{ // commands on current q state
                                             {'1', // current symbol
                                              TuringMachine::qCmd::Cmd{'1', // symbol to write
                                                            Action::R, // action
                                                            0 // next q curState
                                                           }
                                             },
                                             {'*', // current symbol
                                              TuringMachine::qCmd::Cmd{'*', // symbol to write
                                                            Action::E, // action
                                                            1 // next q curState
                                                           }
                                             },
                                            }
                 )
        },
    };
    Error e = Error::NoError;
    while (e != Error::EndOfProgram) {
        std::wcout << printNextCmd() << std::endl;
        e = next();
    }
    return 0;
}

TuringMachine::Error TuringMachine::move(const Action& act) {
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

TuringMachine::Error TuringMachine::writeSymbol(const wchar_t& symb) {
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

std::wstring TuringMachine::printNextCmd() const {
    const wchar_t curSymb = tape.at(position);
    const qCmd::Cmd cmd = program.at(curState).getCmd(curSymb);
    std::wstring qnum;
    unsigned int temp = curState;
    if (temp == 0) {
        qnum = L'\x2080';
    } else {
        while (temp > 0) {
            qnum.insert(qnum.begin(), (wchar_t)(L'\x2080' + (wchar_t)(temp % 10)));
            temp /= 10;
        }
    }
    std::wstring qnextnum;
    temp = cmd.nextState;
    if (temp == 0) {
        qnextnum = L'\x2080';
    } else {
        while (temp > 0) {
            qnextnum.insert(qnextnum.begin(), (wchar_t)(L'\x2080' + (wchar_t)(temp % 10)));
            temp /= 10;
        }
    }
    wchar_t action = '\0';
    switch(cmd.action) {
        case TuringMachine::Action::N:
            action = 'N';
            break;
        case TuringMachine::Action::L:
            action = 'L';
            break;
        case TuringMachine::Action::R:
            action = 'R';
            break;
        case TuringMachine::Action::E:
            action = 'E';
            break;
    }
    std::wstring output(L"q");
    output.append(qnum);
    output.push_back(curSymb);
    output.append(L" → q");
    output.append(qnextnum);
    output.push_back(cmd.symbolToWrite);
    output.push_back(action);
    return output;
}

