#include <iostream>

#define _NX_DEBUG

#include "lib/circuit.h"
#include "lib/verilog.h"

using namespace std;

int main() {
    circuit::circuit ha;
    ha.add_net("a", circuit::IN);
    ha.add_net("b", circuit::IN);
    ha.add_net("s", circuit::OUT);
    ha.add_net("c", circuit::OUT);
    ha.add_module("g1", "xor");
    std::cout << __LINE__ << ": " << &ha.module_list[0] << std::endl;
    std::cout << __LINE__ << ": " << ha.name_registration["g1"] << std::endl;
    std::cout << __LINE__ << ": " << ((circuit::module_t*)ha.name_registration["g1"])->name << std::endl;
    ha.add_module("g2", "and");
    
    std::cout << __LINE__ << ": " << &ha.module_list[0] << std::endl;
    std::cout << __LINE__ << ": " << ha.name_registration["g1"] << std::endl;
    std::cout << __LINE__ << ": " << ((circuit::module_t*)ha.name_registration["g1"])->name << std::endl;

    ha.add_connection("a", "g1", "a");
    ha.add_connection("b", "g1", "b");
    ha.add_connection("g1", "s", "y");
    std::cout << __LINE__ << ": " << &ha.module_list[0] << std::endl;
    std::cout << __LINE__ << ": " << ha.name_registration["g1"] << std::endl;
    std::cout << __LINE__ << ": " << ((circuit::module_t*)ha.name_registration["g1"])->name << std::endl;
    
    ha.add_connection("a", "g2", "a");
    ha.add_connection("b", "g2", "b");
    ha.add_connection("g2", "c", "y");
    std::cout << __LINE__ << ": " << &ha.module_list[0] << std::endl;
    std::cout << __LINE__ << ": " << ha.name_registration["g1"] << std::endl;
    std::cout << __LINE__ << ": " << ((circuit::module_t*)ha.name_registration["g1"])->name << std::endl;

    circuit::verilog::dump(ha);

    return 0;
}