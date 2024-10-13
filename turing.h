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
        class Cmd {
            public:
                wchar_t symbolToWrite;
                Action action;
                q_ nextState;
                Cmd(const wchar_t& s, const Action& a, const q_& n);
                Cmd(const Action& a);
                ~Cmd() = default;
        };
    private:
        std::set<wchar_t> alphabet;
        std::set<unsigned int> qstates;
        q_ curState = 0;
        std::vector<wchar_t> tape;
        int curPosition = 0;
        int lineSize = 128;
        int maxTapeSize = 4096;
        std::map<q_, std::map<wchar_t, TuringMachine::Cmd>> program;

        Error move(const Action& act);
        Error writeSymbol(const wchar_t& symb);
        Error execute();
        std::wstring printNextCmd() const;
    public:
        TuringMachine();
        ~TuringMachine();

        int setAlphabet(const std::vector<wchar_t>&);
        int setStates(const unsigned int& lastState);
        int setProgram(const std::map<q_, std::map<wchar_t, TuringMachine::Cmd>>&);
        void resetTape();
        int writeOnTape(const std::vector<wchar_t>&);
        int writeOnTape(const std::wstring&);
        int run();
};

typedef std::map<wchar_t, TuringMachine::Cmd> qCmd;