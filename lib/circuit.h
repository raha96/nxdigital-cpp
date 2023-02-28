#ifndef __NX_CIRCUIT_H__
#define __NX_CIRCUIT_H__

#include <string>
#include <cassert>

#include "circuit_utils.h"

namespace circuit {
    enum _net_type {
        INT, IN, OUT, INOUT, STATIC0, STATIC1
    };
    enum _net_val {
        NV_U, NV_Z, NV_X, NV_0, NV_1
    };
    struct portmap_t {
        portmap_t() {}
        portmap_t(std::string port, std::string net) : port(port), net(net) {}
        std::string port, net;
    };

    class node_base {
    public:
        node_base() {}
        node_base (std::string name) : name(name) {}
        std::string name;
    };

    class net_t : public node_base {
    public:
        net_t() {}
        net_t(std::string name, _net_type type) : node_base(name), type(type) {}
        _net_type type;
        _net_val value = NV_U;
        bool locked = false;
    };

    class module_t : public node_base {
    public:
        module_t() {}
        module_t(std::string name, std::string type) : node_base(name), type(type) {}
        std::string type;
        bool commented = false;
    };

    class circuit {
    public:
        protected_vector<net_t> net_list;
        protected_vector<module_t> module_list;
        std::string name() { return _name; }
        void set_name(std::string name) { _name = name; }
        void add_net(std::string name, _net_type type) 
            { assert(is_valid(name)); net_list.data.push_back(net_t(name, type)); regnet(name); }
        void add_module(std::string name, std::string type) 
            { assert(is_valid(name)); module_list.data.push_back(module_t(name, type)); regmodule(name); }
        // Instead of exposing the maps themselves as is the case in the Python version, here we use generator functions
        // so we can overload.
        auto adj(net_t& net) {
            std::map<module_t*,std::string> out;
            for (auto edge : edges[net.name])
                out[(module_t*)getnode(edge.first)] = edge.second;
            return out;
        }
        auto adj(module_t& module) {
            std::map<net_t*, std::string> out;
            for (auto edge : edges[module.name])
                out[(net_t*)getnode(edge.first)] = edge.second;
            return out;
        }
        auto pred(module_t& module) {
            std::map<net_t*, std::string> out;
            //std::cout << module.name << std::endl;
            int cnt = 0;
            for (auto src : edges_reverse[module.name]) {
                out[(net_t*)getnode(src)] = edges[src][module.name];
                cnt++;
            }
            if (cnt > 1)
                std::cerr << "ZX " << cnt << std::endl;
            return out;
        }
        void add_connection(std::string u, std::string v, std::string port) {
            edges[u][v] = port;
            auto it = edges_reverse.find(v);
            if (it == edges_reverse.end()) {
                std::vector<std::string> init;
                edges_reverse[v] = init;
            }
            edges_reverse[v].push_back(u);
        }
        void add_connection(net_t& net, module_t& module, std::string port) {
            add_connection(net.name, module.name, port);
        }
        void add_connection(module_t& module, net_t& net, std::string port) {
            add_connection(module.name, net.name, port);
        }
        // Get the *local* port name corresponding to a net (the name of the port of the module connected to the net)
        std::string get_port(std::string u, std::string v) { return edges[u][v]; }
    private:
        std::string _name;
        // used_name and is_valid must be called for adding each node to ensure unique names in an efficient manner.
        std::map<std::string,bool> used_name;
        bool is_valid(std::string name) { if (used_name[name]) return false; used_name[name] = true; return true; }
        // The connection data is stored in edges as v -> {u -> port(u)}
        std::map<std::string,std::map<std::string,std::string>> edges;
        std::map<std::string,std::vector<std::string>> edges_reverse;
        // name_registration is meant to keep account of all nodes by name to allow higher levels of abstraction with low cost.
        std::map<std::string,int> _name_registration_addr;
        std::map<std::string,bool> _name_registration_isnet;
        void regnet(std::string name) { _name_registration_addr[name] = net_list.size()-1; _name_registration_isnet[name] = true; }
        void regmodule(std::string name) { _name_registration_addr[name] = module_list.size()-1; _name_registration_isnet[name] = false; }
        void* getnode(std::string name) { if (_name_registration_isnet[name]) return &net_list[_name_registration_addr[name]];
                                                                              return &module_list[_name_registration_addr[name]]; }
    };
}

#endif

// NOTE:
// Pointers to std::vector items change when the vector is resized.