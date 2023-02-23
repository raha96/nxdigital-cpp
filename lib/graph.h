#ifndef __NX_GRAPH_H__
#define __NX_GRAPH_H__

#include <vector>
#include <map>
#include <utility>
#include <string>
#include <any>

namespace circuit {

    // This class is intended to encapsulate a vector and make it read-only to the public. 
    // To use it, a child class is inherited and made friends with the class that uses vector_interface.
    template <typename T>
    class vector_interface {
    public:
        inline T operator [] (unsigned index) { return data[index]; }
        inline auto size() { return data.size(); }
    protected:
        std::vector<T> data;
    };
    template <typename Ti, typename To>
    class map_interface {
    public:
        inline To operator [] (Ti key) { return data[key]; }
    protected:
        std::map<Ti,To> data;
    };

    template <typename T>
    class _vec_graph : public vector_interface<T> {
        friend class graph_base;
    };
    template <typename Ti, typename To>
    class _map_graph : public map_interface<Ti,To> {
        friend class graph_base;
    };

    typedef std::pair<std::string,std::string> property_t;
    inline property_t property(std::string prop, std::string val) { return std::make_pair(prop, val); }

    class node_base {
    public:
        std::string name;
    };

    //template <typename T> 
    class graph_base {
    protected: 
        typedef node_base* NT;
        typedef _vec_graph<NT> _vec;
        typedef _map_graph<NT,property_t> _prop;
    public:
        _map_graph<NT,_prop> adj, pred;
        //inline void add_child(T* item) { adj.data.push_back(item); }
        //inline void add_parent(T* item) { pred.data.push_back(item); }
        inline void add_node(auto node) {  }
    protected:
        std::vector<std::any> nodes;
        // Must be overriden on inheritance
        enum types { _INT };
        // Used for casting - _typemap keeps account of the type of each element
        std::map<void*,types> _typemap;
        // _ptrcast casts each element based on the corresponding _typemap value.
        // These functions are conceptually virtual, but virtual functions can't return auto, so.
        auto _ptrcast(void* node) { return node; };
    };
}

#endif
