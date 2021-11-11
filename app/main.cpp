// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <cstdlib>
#include <cassert>

#include <iostream>
#include <fstream>

//#include "exampleConfig.h"
//#include "example.h"
#include "parser.h"
#include "lexer.h"
#include "debug.h"

/*
 * Simple main program that demontrates how access
 * CMake definitions (here the version number) from source code.
 */
int main(int argc, char** argv) {
    //std::cout << "C++ Boiler Plate v"
    //          << PROJECT_VERSION_MAJOR
    //          << "."
    //          << PROJECT_VERSION_MINOR
    //          << "."
    //          << PROJECT_VERSION_PATCH
    //          << "."
    //          << PROJECT_VERSION_TWEAK
    //          << std::endl;
    //std::system("cat ../LICENSE");

    //// Bring in the dummy class from the example source,
    //// just to show that it is accessible from main.cpp.
    //Dummy d = Dummy();
    //return d.doSomething() ? 0 : -1;


    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" INPUT_FILE"<<std::endl;
        std::terminate();
    }

    std::fstream fs;
    fs.open(argv[1], std::fstream::in);
    
    Lexer lexer(fs);
    AtomicStateParser parser(lexer);
    auto parsed = parser.ParseAtomicStates();
    PrintStates(parsed);
    InterpretStates(parsed, std::cin);
    //volatile auto vec = DriveLexer(lexer);
    //PrintTokens(vec);
}
