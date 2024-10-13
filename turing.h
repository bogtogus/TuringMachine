#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <string>

#define EMPTY L'λ'
typedef unsigned int q_;

class TuringMachine {
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
        q_ curState;
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
            public:
                struct Cmd {
                    wchar_t symbolToWrite;
                    Action action;
                    unsigned int nextState;
                };
                qCmd(const std::map<wchar_t, Cmd>& cs) {
                    std::map<wchar_t, Cmd>::const_iterator itc = cs.cbegin();
                    while (itc != cs.cend()) {
                        cmds.insert({itc->first, itc->second});
                        ++itc;
                    }
                }
                /*friend std::wostream& operator<<(std::wostream& os, const qCmd& dt) {
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
                    os << 'q' << qnum << curSymb << L" → q" << qnextnum << cmd.symbolToWrite << action;
                    return os;
                }*/
                qCmd(const qCmd& other) {
                    this->cmds = other.cmds;
                }
                qCmd& operator=(const qCmd& other) {
                    this->cmds = other.cmds;
                    return *this;
                }
                qCmd(qCmd&& other) {
                    this->cmds = std::map(other.cmds);
                    other.cmds.clear();
                }
                qCmd& operator=(qCmd&& other) {
                    this->cmds = std::map(other.cmds);
                    other.cmds.clear();
                    return *this;
                }
                Cmd getCmd(const wchar_t& symbol) const {
                    std::map<wchar_t, Cmd>::const_iterator cmdit = cmds.find(symbol);
                    if (cmdit == cmds.cend()) {
                        return Cmd{0,Action::N,0};
                    }
                    return cmdit->second;
                }
            private:
                std::map<wchar_t, Cmd> cmds; // {current symbol: cmd, ...}
        };
        std::map<q_, qCmd> program;
        Error move(const Action& act);
        Error next();
        Error writeSymbol(const wchar_t& symb);
        void resetTape();
        std::wstring printNextCmd() const;
    public:
        TuringMachine();
        ~TuringMachine();
        int mult(const int& x, const int& y);
        
};