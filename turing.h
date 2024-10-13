#pragma once

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <string>

#define EMPTY L'λ'
typedef unsigned int q_;

class TuringMachine {
    private:
        enum class Error {
            NoError = 0,
            WrongChar,
            Overflow,
            EndOfProgram
        };
    public:
        enum class Action {
            N = 0, // Stay
            R, // Move right
            L, // Move left
            E // End of program
        };
        class qCmd {
            public:
                struct Cmd {
                    wchar_t symbolToWrite;
                    Action action;
                    unsigned int nextState;
                };
                qCmd(const std::map<wchar_t, Cmd>& cs);
                qCmd(const qCmd& other);
                qCmd& operator=(const qCmd& other);
                qCmd(qCmd&& other);
                qCmd& operator=(qCmd&& other);
                Cmd getCmd(const wchar_t& symbol) const;
                const std::map<wchar_t, Cmd>::const_iterator getCmdBegin() const;
                const std::map<wchar_t, Cmd>::const_iterator getCmdEnd() const;
            private:
                std::map<wchar_t, Cmd> cmds; // {current symbol: cmd, ...}
        };
    private:
        std::set<wchar_t> alphabet;
        std::set<unsigned int> qstates;
        q_ curState = 0;
        std::vector<wchar_t> tape;
        int curPosition = 0;
        int lineSize = 128;
        int maxTapeSize = 4096;
        std::map<q_, qCmd> program;

        Error move(const Action& act);
        Error writeSymbol(const wchar_t& symb);
        Error execute();
        std::wstring printNextCmd() const;
    public:
        TuringMachine();
        ~TuringMachine();

        int setAlphabet(const std::vector<wchar_t>&);
        int setStates(const unsigned int& lastState);
        int setProgram(const std::map<q_, qCmd>&);
        void resetTape();
        int writeOnTape(const std::vector<wchar_t>&);
        int writeOnTape(const std::wstring&);
        int run();
        int mult(const int& x, const int& y);
};