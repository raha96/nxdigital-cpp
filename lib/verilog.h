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
#include "string_utils.h"

namespace circuit::verilog {
    const std::string indent = "    ";
    typedef std::vector<::circuit::portmap_t> portvec;

    // Used for ordering ports in case they are mapped by name
    void reorder(std::string module_type, portvec& portmap) {
        // Only do this for standard primitives
        if (module_type == "xor" || module_type == "xnor" || module_type == "and" || module_type == "nand" || 
            module_type == "or" || module_type == "nor" || module_type == "not") {
            // It would be at least as efficient to erase the whole thing and rewrite it in the correct order
            portvec _out;
            _out.push_back(portmap_t("", ""));
            for (auto pair : portmap) {
                auto port = pair.port;
                if (port == "y")
                    _out[0] = pair;
                else
                    _out.push_back(pair);
            }
            portmap = _out;
        }
    }

    std::string dump(::circuit::circuit& _circuit, bool namedmap = true, void (*port_order)(std::string, portvec&) = reorder) {
        using namespace std;
        map<string,string> module_type;
        map<string,portvec> module_ports;
        vector<string> pis, pos, wires;
        // List all modules
        for (auto& _module : _circuit.module_list) {
            module_type[_module.name] = _module.type;
        }
        // Generate port map
        for (auto v : _circuit.net_list) {
            for (auto up : _circuit.adj(v)) {
                auto u = up.first;
                auto p = up.second;
                module_ports[u->name].push_back(portmap_t(p, v.name));
            }
        }
        for (auto v : _circuit.module_list) {
            for (auto up : _circuit.adj(v)) {
                auto u = up.first;
                auto p = up.second;
                module_ports[v.name].push_back(portmap_t(p, u->name));
            }
        }
        // List all nets and seperate PIs, POs and internal wires
        for (auto& _net : _circuit.net_list) {
            switch (_net.type) {
                case ::circuit::IN:
                    pis.push_back(_net.name); break;
                case ::circuit::OUT:
                    pos.push_back(_net.name); break;
                default:
                    assert(_net.type == ::circuit::INT);    // TODO: Implement the rest
                    wires.push_back(_net.name);
            }
        }
        string out = "";
        // Module declaration line
        out += "module "  + _circuit.name() + " (";
        out += string_utils::join(", ", pis);
        if (pis.size() > 0 && pos.size() > 0)
            out += ", ";
        out += string_utils::join(", ", pos);
        out += ");\n";
        // Input, output, wire definitions
        if (pis.size())
            out += indent + "input " + string_utils::join(", ", pis) + ";\n";
        if (pos.size())
            out += indent + "output " + string_utils::join(", ", pos) + ";\n";
        if (wires.size())
            out += indent + "wire " + string_utils::join(", ", wires) + ";\n";
        // Create each module instance using the port maps we built earlier
        for (auto _module : module_type) {
            auto name = _module.first;
            auto type = _module.second;
            string instanceline = indent + type + " " + name + " (";
            bool first = true;
            // Fix the port order
            port_order(type, module_ports[name]);
            for (auto portmap : module_ports[name]) {
                auto port = portmap.port;
                auto net = portmap.net;
                if (!first) {
                    instanceline += ", ";
                }
                if (namedmap)
                    instanceline += "." + port + "(" + net + ")";
                else
                    instanceline += net;
                first = false;
            }
            instanceline += ");\n";
            out += instanceline;
        }
        out += "endmodule\n";
        return out;
    }
}

#endif
