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

std::wstring TuringMachine::printNextCmd() const {
    const wchar_t curSymb = tape.at(curPosition);
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

int TuringMachine::setAlphabet(const std::vector<wchar_t>& al) {
    if (al.empty()) {
        return -1;
    }
    this->alphabet.clear();
    std::vector<wchar_t>::const_iterator it = al.cbegin();
    bool containsEmpty = false;
    while (it != al.cend()) {
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
    std::map<wchar_t, TuringMachine::qCmd::Cmd>::const_iterator itcmd;
    std::map<wchar_t, TuringMachine::qCmd::Cmd>::const_iterator itcmdend;
    while (itprg != prg.cend()) {
        if (qstates.find(itprg->first) == qstates.end()) { // if command contains forbidden current state
            return -1;
        }
        itcmd = itprg->second.getCmdBegin();
        itcmdend = itprg->second.getCmdEnd();
        while (itcmd != itcmdend) { // check program valid
            if (alphabet.find(itcmd->first) == alphabet.end()) { // if command contains forbidden current symbol
                return -1;
            } else if (alphabet.find(itcmd->second.symbolToWrite) == alphabet.end()) { // if command contains forbidden symbol
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
    int tapeSize = tape.size();
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
        std::wcout << printNextCmd() << std::endl;
        e = execute();
    }
    return 0;
}


/* ================================ */

TuringMachine::qCmd::qCmd(const std::map<wchar_t, Cmd>& cs) {
    std::map<wchar_t, Cmd>::const_iterator itc = cs.cbegin();
    while (itc != cs.cend()) {
        cmds.insert({itc->first, itc->second});
        ++itc;
    }
}

TuringMachine::qCmd::qCmd(const qCmd& other) {
    this->cmds = other.cmds;
}

TuringMachine::qCmd& TuringMachine::qCmd::operator=(const TuringMachine::qCmd& other) {
    this->cmds = other.cmds;
    return *this;
}

TuringMachine::qCmd::qCmd(qCmd&& other) {
    this->cmds = std::map(other.cmds);
    other.cmds.clear();
}

TuringMachine::qCmd& TuringMachine::qCmd::operator=(qCmd&& other) {
    this->cmds = std::map(other.cmds);
    other.cmds.clear();
    return *this;
}

TuringMachine::qCmd::Cmd TuringMachine::qCmd::getCmd(const wchar_t& symbol) const {
    std::map<wchar_t, Cmd>::const_iterator cmdit = cmds.find(symbol);
    if (cmdit == cmds.cend()) {
        return Cmd{0,Action::N,0};
    }
    return cmdit->second;
}

const std::map<wchar_t, TuringMachine::qCmd::Cmd>::const_iterator TuringMachine::qCmd::getCmdBegin() const {
    return cmds.cbegin();
}

const std::map<wchar_t, TuringMachine::qCmd::Cmd>::const_iterator TuringMachine::qCmd::getCmdEnd() const {
    return cmds.cend();
}

