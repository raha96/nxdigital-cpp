#include <iostream>

#define _NX_DEBUG

#include "lib/circuit.h"
#include "lib/verilog.h"
#include "lib/bench.h"

using namespace std;

circuit::circuit ha() {
    circuit::circuit _ha;
    _ha.add_net("a", circuit::IN);
    _ha.add_net("b", circuit::IN);
    _ha.add_net("s", circuit::OUT);
    _ha.add_net("c", circuit::OUT);
    _ha.add_module("g1", "xor");
    _ha.add_module("g2", "and");

    _ha.add_connection("a", "g1", "a");
    _ha.add_connection("b", "g1", "b");
    _ha.add_connection("g1", "s", "y");
    
    _ha.add_connection("a", "g2", "a");
    _ha.add_connection("b", "g2", "b");
    _ha.add_connection("g2", "c", "y");

    return _ha;
}

int main() {
    //circuit::circuit cir = ha();
    //std::string verilog_code = circuit::verilog::dump(cir, false);
    //std::cout << verilog_code;

    circuit::circuit cir = circuit::bench::load("sample/c1355.bench");

    return 0;
}