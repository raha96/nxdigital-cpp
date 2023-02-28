#ifndef __NX_BENCH_H__
#define __NX_BENCH_H__

#include <fstream>
#include <regex>
#include <algorithm>
#include <cassert>

#include "circuit.h"

// Is this necessary?
#include "string_utils.h"

#define _NX_DEBUG

#ifdef _NX_DEBUG
#include <iostream>
#endif

namespace circuit::bench {
    namespace _private {
        const std::regex re_io("\\s*(input|output)\\s*\\((\\w+)\\)\\s*", std::regex_constants::icase);
        const std::regex re_gate("\\s*(\\w+)\\s*=\\s*(\\w+)\\s*\\(([\\w\\s,]+)\\)\\s*", std::regex_constants::icase);
        const std::regex re_gate_inputs("\\w+", std::regex_constants::icase);
        const std::regex re_file_basename("(\\w+)\\.bench", std::regex_constants::icase);

        struct _ret_io {
            ::circuit::_net_type type;
            std::string name = "";
        };

        struct _ret_gate {
            std::string type = "";
            std::vector<std::string> conns;
        };

        std::string tolower(std::string str) {
            std::string out = "";
            for (char ch : str) {
                out += std::tolower(ch);
            }
            return out;
        }

        _ret_io process_io(const std::string line) {
            // Returns a _ret_io containing net name and net type (IN/OUT) if a match, 
            // otherwise a _ret_io with empty name string.
            std::cmatch match;
            std::regex_match(line.c_str(), match, re_io);
            _ret_io out;
            if (!match.empty()) {
                std::string type = tolower(match[1].str());
                if (type == "input")
                    out.type = ::circuit::IN;
                else if (type == "output")
                    out.type = ::circuit::OUT;
                else
                    assert(0);
                out.name = match[2].str();
            }
            return out;
        }

        _ret_gate process_gate(const std::string line) {
            // Similar to process_io, but for gates
            std::cmatch match;
            std::regex_match(line.c_str(), match, re_gate);
            _ret_gate out;
            if (!match.empty()) {
                out.type = tolower(match[2].str());
                // The output
                out.conns.push_back(match[1].str());
                // Traverse the input list
                std::string inputs = match[3].str();
                std::smatch sm;
                while (std::regex_search(inputs, sm, _private::re_gate_inputs)) {
                    out.conns.push_back(sm.str());
                    inputs = sm.suffix();
                }
            }
            return out;
        }

        std::string gatename(int gatenum, int digits=4) {
            std::string out = "";
            for (int i = 0; i < digits; i++) {
                out += char('0' + (gatenum % 10));
                gatenum /= 10;
            }
            std::reverse(out.begin(), out.end());
            return "G" + out;
        }

        std::string file_basename(std::string name) {
            // Extract base name. Used for choosing the module's name.
            std::cmatch match;
            std::regex_search(name.c_str(), match, re_file_basename);
            return match[1].str();
        }
    }

    ::circuit::circuit load(std::string path) {
        ::circuit::circuit _circuit;
        std::fstream fin(path, std::ios::in);
        std::string line;
        std::map<std::string,bool> wire_declared;
        std::vector<std::string> ionames;
        _circuit.set_name(_private::file_basename(path));
        int gatecnt = 1;
        while (std::getline(fin, line)) {
            auto isio = _private::process_io(line);
            auto isgate = _private::process_gate(line);
            if (isio.name != "") {
                assert(isgate.type == "");
                _circuit.add_net(isio.name, isio.type);
                ionames.push_back(isio.name);
            }
            if (isgate.type != "") {
                assert(isio.name == "");
                std::string name = _private::gatename(gatecnt);
                char port = 'a';
                gatecnt++;
                _circuit.add_module(name, isgate.type);
                _circuit.add_connection(name, isgate.conns[0], "y");
                wire_declared[isgate.conns[0]] = false;
                for (int i = 1; i < isgate.conns.size(); i++) {
                    _circuit.add_connection(isgate.conns[i], name, std::string(1, port));
                    wire_declared[isgate.conns[i]] = false;
                    port++;
                }
            }
        }
        fin.close();
        // Explicitly define the implicit wires
        for (int i = 0; i < ionames.size(); i++)
            wire_declared[ionames[i]] = true;
        for (auto isdecl : wire_declared) {
            if (!isdecl.second)
                _circuit.add_net(isdecl.first, ::circuit::INT);
        }
        return _circuit;
    }
}

#endif
