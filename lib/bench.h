#ifndef __NX_BENCH_H__
#define __NX_BENCH_H__

#include <fstream>
#include <regex>

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
        const std::regex re_gate("\\s*(\\w+)\\s*=\\s*(\\w+)\\s*\\(\\s*(\\w+)\\s*(,\\s*(\\w+)\\s*)*\\)\\s*", std::regex_constants::icase);

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
                // The first input
                out.conns.push_back(match[3].str());
                // The rest of the inputs
                // In this loop we skip every other net name, as each ", [NET]" generates two regex matches ("NET" & ", NET")
                for (int i = 5; i < match.size(); i += 2)
                    out.conns.push_back(match[i].str());
                if (match[1].str() == "G980gat") {
                    std::cout << "===============\n";
                    for (auto s : match) {
                        std::cout << s.str() << std::endl;
                    }
                    std::cout << "===============\n";
                }
                if (match.size() > 7)
                    std::cout << match.size() << std::endl;
            }
            return out;
        }
    }

    ::circuit::circuit load(std::string path) {
        ::circuit::circuit _circuit;
        std::fstream fin(path);
        std::string line;
        int cnt = 5;
        while (std::getline(fin, line)) {
            auto isio = _private::process_io(line);
            auto isgate = _private::process_gate(line);
            if (isio.name != "") {
                _circuit.add_net(isio.name, isio.type);
            }
            if (isgate.type != "") {
                if (isgate.conns.size() > 2) {
                    //std::cout << isgate.type << std::endl;
                    //std::cout << ::circuit::string_utils::join("|", isgate.conns) << std::endl;
                }
            }
        }
        fin.close();
        return _circuit;
    }
}

#endif
