#ifndef __NX_VERILOG_H__
#define __NX_VERILOG_H__

#include <string>
#include <vector>
#include <map>
#include <utility>

#define _NX_DEBUG

#ifdef _NX_DEBUG
#include <iostream>
#endif

#include "circuit.h"

namespace circuit::verilog {
    std::string dump(::circuit::circuit& _circuit) {
        using namespace std;
        string out = "";
        map<string,string> module_type;
        map<string,::circuit::portmap_t> module_ports;
        for (int i = 0; i < _circuit.module_list.size(); i++)
            std::cout << _circuit.module_list[i].name << " " << &_circuit.module_list[i] << std::endl;
        for (auto& _module : _circuit.module_list) {
            module_type[_module.name] = _module.type;
            std::cout << _module.name << " " << &_module << std::endl;
        }
        for (int i = 0; i < _circuit.module_list.size(); i++)
            std::cout << _circuit.module_list[i].name << " " << &_circuit.module_list[i] << std::endl;
        for (auto v : _circuit.net_list) {
            for (auto up : _circuit.adj(v)) {
                auto u = up.first;
                auto p = up.second;
                cout << u->name << " " << p << endl;
            }
        }
        return out;
    }
}

#endif
