#ifndef __NX_BENCH_H__
#define __NX_BENCH_H__

#include <fstream>
#include <regex>

#include "circuit.h"

#define _NX_DEBUG

#ifdef _NX_DEBUG
#include <iostream>
#endif

namespace circuit::bench {
    namespace _private {
        const std::regex re_io("\\s*(input|output)\\s*\\((\\w+)\\)", std::regex_constants::icase);

        struct _ret_io {
            ::circuit::_net_type type;
            std::string name = "";
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
    }

    ::circuit::circuit load(std::string path) {
        ::circuit::circuit _circuit;
        std::fstream fin(path);
        std::string line;
        int cnt = 5;
        while (std::getline(fin, line)) {
            auto isio = _private::process_io(line);
            if (isio.name != "") {
                _circuit.add_net(isio.name, isio.type);
                if (cnt > 0)
                    std::cout << isio.name << " " << isio.type << std::endl;
                cnt--;
            }
        }
        fin.close();
        return _circuit;
    }
}

#endif
