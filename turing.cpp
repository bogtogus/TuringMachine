#include "turing.h"
#include <iostream>

TuringMachine::TuringMachine() {
    for (int i = 0; i < lineSize; i++) {
        tape.push_back(EMPTY);
    }
}

TuringMachine::~TuringMachine() {
    tape.clear();
    alphabet.clear();
}

TuringMachine::Error TuringMachine::execute() {
    const wchar_t curSymb = tape.at(curPosition);
    const qCmd qcmd = program.at(curState);
    if (qcmd.find(curSymb) == qcmd.end()) {
        return Error::NoError;
    }
    const Cmd cmd = qcmd.at(curSymb);
    Error e = Error::NoError;
    if (cmd.symbolToWrite != '\0') {
        e = writeSymbol(cmd.symbolToWrite);
        if (e != Error::NoError) {
            return e;
        }
        e = move(cmd.action);
        curState = cmd.nextState;
    } else {
        e = move(cmd.action);
    }
    return e;
}

void TuringMachine::resetTape() {
    std::vector<wchar_t>::iterator it = tape.begin();
    while (it != tape.end()) {
        *it = EMPTY;
        ++it;
    }
    curPosition = 0;
}

TuringMachine::Error TuringMachine::move(const Action& act) {
    switch (act) {
        case Action::N:
            break;
        case Action::L:
            if (curPosition == 0 && tape.size() < maxTapeSize) {
                tape.insert(tape.begin(), EMPTY);
            } else if (tape.size() >= maxTapeSize) {
                return Error::Overflow;
            } else {
                --curPosition;
            }
            break;
        case Action::R:
            if (curPosition == tape.size() - 1 && tape.size() < maxTapeSize) {
                tape.push_back(EMPTY);
            } else if (tape.size() >= maxTapeSize) {
                return Error::Overflow;
            }
            ++curPosition;
            break;
        case Action::E:
            return Error::EndOfProgram;
        default:
            break;
    }
    return Error::NoError;
}

TuringMachine::Error TuringMachine::writeSymbol(const wchar_t& symb) {
    if (curPosition > tape.size()) {
        return Error::Overflow;
    }
    tape[curPosition] = symb;
    return Error::NoError;
}

std::wstring TuringMachine::viewNextCmd() const {
    const wchar_t curSymb = tape.at(curPosition);
    const qCmd qcmd = program.at(curState);
    if (qcmd.find(curSymb) == qcmd.end()) {
        return L"";
    }
    const Cmd cmd = qcmd.at(curSymb);
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
    if (temp == -1) {
        temp = curState;
    }
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
    if (cmd.symbolToWrite != '\0') {
        output.push_back(cmd.symbolToWrite);
    } else {
        output.push_back(tape.at(curPosition));
    }
    output.push_back(action);
    return output;
}

std::wstring TuringMachine::viewTape() const {
    std::wstring output;
    int viewPos = 0;
    int leftBorder = 0;
    int rightBorder = 0;
    while (viewPos < maxTapeSize && (tape.at(viewPos) == '\0' || tape.at(viewPos) == EMPTY)) {
        ++viewPos;
    }
    leftBorder = viewPos;
    
    output.push_back(' ');
    const size_t tapeSize = tape.size();
    while (viewPos < tapeSize && tape.at(viewPos) != '\0' && tape.at(viewPos) != EMPTY) {
        output.push_back(tape.at(viewPos));
        ++viewPos;
    }
    if (viewPos < tapeSize && tape.at(viewPos) != '\0') {
        output.push_back(tape.at(viewPos));
        rightBorder = viewPos;
    } else {
        rightBorder = viewPos - 1;
    }
    output.append(L" \n");
    if (curPosition >= leftBorder && curPosition <= rightBorder) {
        int temp = curPosition - leftBorder + 1;
        while (temp > 0) {
            output.push_back(' ');
            --temp;
        }
        output.push_back('^');
    } else if (curPosition < leftBorder) {
        output.push_back('<');
    } else {
        int temp = rightBorder - leftBorder + 2;
        while (temp > 0) {
            output.push_back(' ');
            --temp;
        }
        output.push_back('>');
    }
    return output;
}

int TuringMachine::setAlphabet(const std::vector<wchar_t>& al) {
    if (al.empty()) {
        return -1;
    }
    this->alphabet.clear();
    std::vector<wchar_t>::const_iterator it = al.cbegin();
    bool containsEmpty = false;
    while (it != al.cend()) {
        if (*it == '\0') {
            return -2;
        }
        this->alphabet.insert(*it);
        if (!containsEmpty && *it == EMPTY) {
            containsEmpty = true;
        }
        ++it;
    }
    if (!containsEmpty) {
        alphabet.insert(EMPTY);
    }
    return 0;
}

int TuringMachine::setStates(const unsigned int& lastState) {
    for (int st = 0; st <= lastState; st++) {
        qstates.insert(st);
    }
    return 0;
}

int TuringMachine::setProgram(const std::map<q_, qCmd>& prg) {
    if (prg.empty()) {
        return -1;
    }
    std::map<q_, qCmd>::const_iterator itprg = prg.cbegin();
    qCmd::const_iterator itcmd;
    qCmd::const_iterator itcmdend;
    while (itprg != prg.cend()) {
        if (qstates.find(itprg->first) == qstates.end()) { // if command contains forbidden current state
            return -1;
        }
        itcmd = itprg->second.cbegin();
        itcmdend = itprg->second.cend();
        while (itcmd != itcmdend) { // check program valid
            if (alphabet.find(itcmd->first) == alphabet.end()) { // if command contains forbidden current symbol
                return -1;
            } else if (itcmd->second.symbolToWrite != '\0' && alphabet.find(itcmd->second.symbolToWrite) == alphabet.end()) { // if command contains forbidden symbol
                return -1;
            } else if (qstates.find(itcmd->second.nextState) == qstates.end()) { // if command contains forbidden next state
                return -1;
            }
            ++itcmd;
        }
        program.insert(*itprg);
        ++itprg;
    }
    return 0;
}

int TuringMachine::writeOnTape(const std::vector<wchar_t>& data) {
    if (data.empty() || data.size() > maxTapeSize) {
        return -1;
    }
    std::vector<wchar_t>::const_iterator itdata = data.cbegin();
    std::vector<wchar_t>::iterator ittape = tape.begin();
    while(ittape != tape.end()) {
        *ittape = *itdata;
    }
    return 0;
}

int TuringMachine::writeOnTape(const std::wstring& data) {
    if (data.empty() || data.size() > maxTapeSize) {
        return -1;
    }
    std::wstring::const_iterator itdata = data.cbegin();
    int writePos = curPosition;
    size_t tapeSize = tape.size();
    while(writePos < maxTapeSize && itdata != data.cend()) {
        if (writePos >= tapeSize && tapeSize < maxTapeSize) {
            tape.push_back(*itdata);
            ++tapeSize;
        } else if (tapeSize >= maxTapeSize) {
            break;
        } else {
            tape[writePos] = *itdata;
        }
        ++writePos;
        ++itdata;
    }
    return 0;
}

int TuringMachine::run() {
    if (alphabet.empty() || qstates.empty() || program.empty()) {
        return -1;
    }
    Error e = Error::NoError;
    while (e != Error::EndOfProgram) {
        std::wcout << viewTape() << std::endl;
        std::wcout << viewNextCmd() << std::endl;
        e = execute();
        std::wcout << viewTape() << std::endl;
    }
    return 0;
}


TuringMachine::Cmd::Cmd(const wchar_t& s, const Action& a, const q_& n) 
    : symbolToWrite(s), action(a), nextState(n) {

}

TuringMachine::Cmd::Cmd(const Action& a) 
    : symbolToWrite('\0'), action(a), nextState(0)
{

}
