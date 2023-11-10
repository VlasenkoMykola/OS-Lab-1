//include <iostream>
//include <vector>

using namespace std;

//pragma once

#include <iostream>
#include <string>
#include <cstring>
//include <fstream>
//include "unittest.h"

#include <time.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int func_2 = 0;
    int return_value = 0;
    int send_exit_value = 0;
    int fail_by_division = 0;
    int x;
    if (argc < 3) {
	cout << "syntax error: not enough arguments: " << argc;
	cout << std::endl;
	return 1;
    }
    if (argv[1][0] == 'f') {
	func_2 = 0;
	fail_by_division = 1;
    } else if (argv[1][0] == 'g') {
	func_2 = 1;
	fail_by_division = 1;
    } else {
	cout << "syntax error: 1st argument: " << argv[1];
	cout << std::endl;
	return 1;
    }
    x = atoi(argv[2]);
    if (x==0 && 0 != strncmp("0", argv[2], 2)) {
	cout << "syntax error: 3nd argument not a number: " << argv[3];
	cout << std::endl;
	return 1;
    }
    // 2^0 bitmask -- f fails, 2^2 bitmask -- g fails
    if ((! func_2 && (x & 1)) || (func_2 && (x & 4))) {
	// divide by 0; complex code to avoid compiler warnings
	return_value = x/(fail_by_division - 1);
    }
    // 2^1 bitmask -- f hangs, 2^3 bitmask -- g hangs
    if ((! func_2 && (x & 2)) || (func_2 && (x & 8))) {
	// hangs
	for(;;);
    }
    // when x = 0 both f, g return 0
    if (x > 0) {
	if (!func_2) {
	    // f
	    return_value = (x - 1) % 256;
	} else {
	    // g
	    return_value = (x + 151) % 256;
	}
    }
    return return_value;
}
