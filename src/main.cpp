#include <cstdio>
#include <fstream>
#include <string>
#include <list>
#include "common/stringUtils.h"
#include "cslProgram/program.h"

using namespace std;

int main(int argc, const char* argv[])
{
	printf("Hello World!\n");


	string line = "    TEST , line, 234 , he ,, jlk, kl  lkj, j,, jl,jll ";
	vector<string> splitLine;

	stringUtils::split(line, ',', splitLine);

	for (const string& word : splitLine)
	{
		printf("%s\n", word.c_str());
	}

    // Create an input file stream object named 'file' and 
    // open the file "GFG.txt". 
    ifstream file("src/script.txt");

    if (file.is_open()) {
        cslProgram::Program program(file);

        program.RunFunction("ON_START");
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