/***
 * Do not change this file - but you might edit the arguments passed to main:
 *  - Projects (spanner on right)
 *  - Under "Build and Run", select "Run" (small green arrow)
 *  - Edit "Command line arguments" (main panel)
 *
 * v1: initial release
 * v2: removed std:: from some strings & allowed students to edit the Location class
 */

#include <string>
#include <iostream>

#include "cave.h"
#include "test.cpp"

using namespace std;

int main(int argc, char** argv)
{
    cerr.setstate(ios_base::failbit);
    cout.setstate(ios_base::failbit);

    std::cout.clear();
      
    cout << runTest(stoi(argv[1])) << endl;
    
    return 0;
}
