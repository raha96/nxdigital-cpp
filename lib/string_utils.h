#ifndef __NX_STRING_UTILS_H__
#define __NX_STRING_UTILS_H__

#include <string>
#include <vector>

namespace circuit::string_utils {
    std::string join(std::string separator, std::vector<std::string>& list) {
        std::string out = "";
        bool first = true;
        for (std::string item : list) {
            if (!first)
                out += separator;
            out += item;
            first = false;
        }
        return out;
    }
};

#endif
