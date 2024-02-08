#include <cstdio>
#include <fstream>
#include <string>
#include <list>
#include "stringUtils.h"

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
        // Read each line from the file and store it in the 
        // 'line' variable. 
        while (getline(file, line)) {
            printf("%s\n", line.c_str());
        }

        // Close the file stream once all lines have been 
        // read. 
        file.close();
    }
    else {
        // Print an error message to the standard error 
        // stream if the file cannot be opened. 
        printf("Unable to open file!\n");
    }


    std::list<int> testList;
    testList.push_back(1);
    testList.push_back(4);
    testList.push_back(5);

    std::list<int>::iterator iter = testList.begin();

    for (int i = 0; i < 4; i++)
    {
        if (iter != testList.end())
        {
            printf("ITER IS %i\n", *iter);
        }
        else
        {
            printf("ITER IS NULL\n");
            break;
        }
        std::advance(iter, 2);
    }
    return 0;
}