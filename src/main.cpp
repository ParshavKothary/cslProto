#include <cstdio>
#include <fstream>
#include <string>
#include <list>
#include "common/stringUtils.h"
#include "cslProgram/program.h"

using namespace std;

int main(int argc, const char* argv[])
{
    ifstream file("src/script.txt");

    if (file.is_open()) {
        cslProgram::Program program(file);

        program.RunFunction("ON_START");
        program.RunFunction("ON_END");
        // run program

        file.close();
    }
    else {
        // Print an error message to the standard error 
        // stream if the file cannot be opened. 
        printf("Unable to open file!\n");
    }

    return 0;
}