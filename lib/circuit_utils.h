#ifndef __NX_CIRCUIT_UTILS_H__
#define __NX_CIRCUIT_UTILS_H__

#include <vector>
#include <map>

namespace circuit {
    template <typename T>
    class protected_vector {
        friend class circuit;
    public:
        T& operator [] (int index) { return data[index]; }
        auto size() { return data.size(); }
        T* last() { return &(*this)[size() - 1]; }
        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
    protected:
        std::vector<T> data;
    };

    template <typename Ti, typename To>
    class protected_map {
        friend class circuit;
    public:
        To operator [] (Ti key) { return data[key]; }
        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
    protected:
        std::map<Ti,To> data;
    };
}

#endif
