#ifndef __NX_CIRCUIT_H__
#define __NX_CIRCUIT_H__

#include <string>
#include <cassert>

#include "circuit_utils.h"

namespace circuit {
    enum _net_type {
        IN, OUT, INOUT, STATIC0, STATIC1
    };
    enum _net_val {
        NV_U, NV_Z, NV_X, NV_0, NV_1
    };
    struct portmap_t {
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
            { assert(is_valid(name)); net_list.data.push_back(net_t(name, type)); regnode(name, net_list.last()); }
        void add_module(std::string name, std::string type) 
            { assert(is_valid(name)); module_list.data.push_back(module_t(name, type)); regnode(name, module_list.last()); std::cout<<module_list.last() << " " << name << std::endl; }
        // Instead of exposing the maps themselves as is the case in the Python version, here we use generator functions
        // so we can overload.
        auto adj(net_t& net) {
            std::map<module_t*,std::string> out;
            std::cout << net.name << std::endl;
            for (auto edge : edges[net.name]) {
                out[(module_t*)name_registration[edge.first]] = edge.second;
                std::cout << edge.first << " " << edge.second << std::endl;
                std::cout << ((module_t*)name_registration[edge.first]) << std::endl;
                std::cout << ((module_t*)name_registration[edge.first])->name << std::endl;
            }
            std::cout << "DONE" << std::endl;
            return out;
        }
        void add_connection(net_t& net, module_t& module, std::string port) { edges[net.name][module.name] = port; }
        void add_connection(module_t& module, net_t& net, std::string port) { edges[module.name][net.name] = port; }
        void add_connection(std::string u, std::string v, std::string port) { edges[u][v] = port; }
        // Get the *local* port name corresponding to a net (the name of the port of the module connected to the net)
        std::string get_port(std::string u, std::string v) { return edges[u][v]; }
    //private:
        std::string _name;
        // name_registration is meant to keep account of all nodes by name to allow higher levels of abstraction with low cost.
        std::map<std::string,void*> name_registration;
        void regnode(std::string name, auto node) { name_registration[name] = node; }
        // used_name and is_valid must be called for adding each node to ensure unique names in an efficient manner.
        std::map<std::string,bool> used_name;
        bool is_valid(std::string name) { if (used_name[name]) return false; used_name[name] = true; return true; }
        // The connection data is stored in edges as v -> {u -> port(u)}
        std::map<std::string,std::map<std::string,std::string>> edges;
    };
}

#endif

// NOTE:
// <overline>It seems like use of vector iterators alters the addresses to std::vector items, causing segfault in some cases.</overline>
// The culprit seems to be pass by reference? Probably I'm wrong.