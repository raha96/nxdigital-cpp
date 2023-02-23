#ifndef __NX_CIRCUIT_H__
#define __NX_CIRCUIT_H__

#include <cassert>

#include "graph.h"

namespace circuit {
    class net_t : public node_base {
    };

    class module_t : public node_base {
    };

    class circuit : public graph_base {
    private:
        enum types { _NET, _MODULE };
        inline auto _ptrcast(void* node) {
            if (_typemap[node] == _NET)
                return (net_t*) node;
            return (module_t*) node;
        }
    };
}

#endif
