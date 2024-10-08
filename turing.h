#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <string>

#define EMPTY L'λ'
typedef unsigned int q_;

class TM {
    public:
        enum class Action {
            N = 0, // Stay
            R, // Move right
            L, // Move left
            E // End of program
        };
    private:
        std::vector<wchar_t> alphabet;
        std::vector<unsigned int> qstates;
        q_ state;
        std::vector<wchar_t> tape;
        int position = 0;
        int lineSize = 128;
        int maxLineSize = 4096;

        enum class Error {
            NoError = 0,
            WrongChar,
            Overflow,
            EndOfProgram
        };
        class qCmd {
            private:
                TM* parent;
            public:
                struct Cmd {
                    wchar_t symbolToWrite;
                    Action action;
                    unsigned int nextState;
                };
                qCmd() {
                    parent = nullptr;
                }
                qCmd(TM* prnt, const std::map<wchar_t, Cmd>& cs) {
                    std::map<wchar_t, Cmd>::const_iterator itc = cs.cbegin();
                    while (itc != cs.cend()) {
                        if (itc->first < 0 || prnt == nullptr) {
                            throw std::exception();
                        } else {
                            parent = prnt;
                        }
                        std::vector<wchar_t>::const_iterator ita = parent->alphabet.cbegin();
                        int foundFlag = 0b00;
                        while (ita != parent->alphabet.cend()) {
                            if (*ita == (itc->first) && (foundFlag & 0b01) != 0b01) {
                                foundFlag |= 0b01;
                            }
                            if (*ita == (itc->second.symbolToWrite) && (foundFlag & 0b10) != 0b10) {
                                foundFlag |= 0b10;
                            }
                            if ((foundFlag & 0b11) == 0b11) {
                                break;
                            }
                            ++ita;
                        }
                        if ((foundFlag & 0b11) != 0b11) {
                            throw std::exception();
                        }
                        std::vector<unsigned int>::const_iterator itq = parent->qstates.cbegin();
                        while (itq != parent->qstates.cend()) {
                            if (*itq == (itc->second.nextState)) {
                                break;
                            }
                            ++itq;
                        }
                        if (itq == parent->qstates.cend()) {
                            throw std::exception();
                        }
                        cmds.insert({itc->first, itc->second});
                        ++itc;
                    }
                }
                Error next() {
                    if (parent == nullptr) {
                        return Error::NoError;
                    }
                    const wchar_t curSymb = currentSymbol();
                    if (cmds.find(curSymb) == cmds.end()) {
                        return Error::NoError;
                    }
                    const Cmd cmd = cmds.at(curSymb);
                    Error e = parent->writeSymbol(cmd.symbolToWrite);
                    if (e != Error::NoError) {
                        return e;
                    }
                    e = parent->move(cmd.action);
                    parent->state = cmd.nextState;
                    return e;
                }
                q_ currentState() const {
                    if (parent == nullptr) {
                        return 0;
                    }
                    return parent->state;
                }
                wchar_t currentSymbol() const {
                    if (parent == nullptr) return '\0';
                    return parent->tape.at(parent->position);
                }
                friend std::wostream& operator<<(std::wostream& os, const qCmd& dt) {
                    const wchar_t curSymb = dt.currentSymbol();
                    if (dt.cmds.find(curSymb) == dt.cmds.end()) {
                        return os;
                    }
                    const Cmd cmd = dt.cmds.at(curSymb);
                    std::wstring qnum;
                    unsigned int temp = dt.currentState();
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
                        case TM::Action::N:
                            action = 'N';
                            break;
                        case TM::Action::L:
                            action = 'L';
                            break;
                        case TM::Action::R:
                            action = 'R';
                            break;
                        case TM::Action::E:
                            action = 'E';
                            break;
                    }
                    os << 'q' << qnum << curSymb << L" → q" << qnextnum << cmd.symbolToWrite << action;
                    return os;
                }
                qCmd(const qCmd& other) {
                    this->parent = other.parent;
                    this->cmds = other.cmds;
                }
                qCmd& operator=(const qCmd& other) {
                    this->parent = other.parent;
                    this->cmds = other.cmds;
                }
                qCmd(qCmd&& other) {
                    this->parent = other.parent;
                    other.parent = nullptr;
                    this->cmds = std::map(other.cmds);
                    other.cmds.clear();
                }
                qCmd& operator=(qCmd&& other) {
                    this->parent = other.parent;
                    other.parent = nullptr;
                    this->cmds = std::map(other.cmds);
                    other.cmds.clear();
                }
            private:
                std::map<wchar_t, Cmd> cmds; // {current symbol: cmd, ...}
        };
        std::map<q_, qCmd> program;
        Error move(const Action& act);
        Error writeSymbol(const wchar_t& symb);
        void resetTape();
    public:
        TM();
        ~TM();
        int mult(const int& x, const int& y);
        
};