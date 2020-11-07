/***
 * Do not change this file
 *
 * A script very much like this will be used to grade your code.
 *
 * I may add additional checks to ensure that no cheating takes place!
 *
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include<iostream>
#include<string>
#include <dirent.h> // this might be tricky, but mostly there for gnu compilers.

#include "cave.h"
#include "coin.h"
#include "location.h"
#include "mushroom.h"

using namespace std;

int Location::count;
int Thing::count;

string test1()
{

    Cave c(8,8);
    c.getTom()->setLocation(&c, 4, 6);

    bool goodA = true;

    // let's walk in a circle and check we get the results we expect
    c.command("move north");
    goodA &= (c.getTom()->getY() == 5);

    c.command("move west");
    goodA &= (c.getTom()->getX() == 3);

    c.command("move north");
    goodA &= (c.getTom()->getY() == 4);

    c.command("move east");
    goodA &= (c.getTom()->getX() == 4);

    c.command("move north");
    goodA &= (c.getTom()->getY() == 3);

    c.command("move east");
    goodA &= (c.getTom()->getX() == 5);

    c.command("move south");
    goodA &= (c.getTom()->getY() == 4);

    c.command("move east");
    goodA &= (c.getTom()->getX() == 6);

    c.command("move south");
    goodA &= (c.getTom()->getY() == 5);

    c.command("move west");
    goodA &= (c.getTom()->getX() == 5);

    c.command("move north");
    goodA &= (c.getTom()->getY() == 4);

    c.command("move west");
    goodA &= (c.getTom()->getX() == 4);


    bool goodB = true;

    // walk into wall to south
    for (int i = 0; i < 11; i++)
        c.command("move south");

    goodB &= (c.getTom()->getX() == 4);
    goodB &= (c.getTom()->getY() == 6);

    // walk into wall to east
    for (int i = 0; i < 11; i++)
        c.command("move east");

    goodB &= (c.getTom()->getX() == 6);
    goodB &= (c.getTom()->getY() == 6);

    string out = to_string(  (goodA ? 1 : 0) + (goodB ? 1 : 0) );

    if (!goodA)
            out+=" failed to move south or west.";

    if (!goodB)
            out+=" rocks did not block tom to south or west.";

    return out;
}

string test2()
{
    bool goodA = true, goodB = true, caught_error = false;

    int i, j;

    try {

        for (i = 5; i < 20; i+=3)
            for (j = 5; j < 30; j+=2) {
                Cave c(i,j);

                goodA &= c.getWidth() == i;
                goodA &= c.getHeight() == j;

                for (int x = 0; x < i; x++)
                    for (int y = 0; y < j; y++)
                        if (x > 0 && x < i-1 && y> 0 && y < j-1)
                            goodB &= !c.getMap()[x][y]->isBlocking();
                        else
                            goodB &= c.getMap()[x][y]->isBlocking();
            }

    }
    catch (...) // all errors (including logic_error) fall through here
    {
        goodA = goodB = false;
        caught_error = true;
    }

    string out = to_string ( (goodA ? 1 : 0) + (goodB ? 1 : 0) );

    if (!goodA)
        out += " failed to set width/height variable in cave.";

    if (!goodB)
        out += " failed to place blocking rocks on resized caves.";

    if ( caught_error )
        out += " caught an error at " + to_string(i) +"," + to_string(j) +".";

    return out;
}

//void* operator new(std::size_t sz) {
//    void *ptr = std::malloc(sz);
//    if (ptr)
//        return ptr;
//    else
//        throw std::bad_alloc{};
//}

//void operator delete(void* ptr) noexcept
//{
//    std::free(ptr);
//}

string test3()
{

    Location::count = 0; // reset the counters in location and thing
    Thing::count = 0;

    bool goodA = true, goodB = true;

    {
        int x = 8, y= 8;

        Cave c(x, y);

        goodA &= Location::count == x*y;
    }

    goodA &= Location::count == 0;
    goodA &= Thing::count == 0;

    string out = to_string ( (goodA ? 1 : 0)  + (goodB ? 1 : 0) );

    if (!goodA) {
        out += " Location/thing count not 0.";

        if (Thing::count != 0)
             out+= (" thing count "+ to_string ( Thing::count) +".");
        if (Location::count != 0)
             out+= (" location count was "+ to_string ( Thing::count) + ".");
    }

    if (!goodB)
        out += (" number of new's did not match number of deletes.");//fixme

    return out;
}

void test4CheckPointers(Cave *a, Cave &b, bool& goodA)
{
    goodA &= b.getMap() != a -> getMap(); // check that we created a new map
    goodA &= b.getMap()[0][0] != a -> getMap()[0][0]; // has the vector in Location been copied?
    goodA &= b.getMap()[0][0]->getThings()->at(0) != a -> getMap()[0][0]->getThings()->at(0); // has the Rock been copied
}

bool hasCoin (Cave &c, int x, int y) {

    for (auto t : *c.getMap()[x][y]->getThings())
        if ( dynamic_cast<Coin*>(t) )
            return true;

    return false;
}

bool hasMushroom (Cave &c, int x, int y) {

    for (auto t : *c.getMap()[x][y]->getThings())
        if ( dynamic_cast<Mushroom*>(t) )
            return true;

    return false;
}

string test4() {

    bool goodA = true, goodB = true;

    Cave *a = new Cave(8, 8); // so we don't destroy any (accidently) shallow copied copies twice

    a->getTom()->setLocation(a, 1, 1);
    a->command("place mushroom");

    Cave b(*a);

    test4CheckPointers(a, b, goodA);
    bool construtorPointer = goodA;
    bool constructorMushTest = hasMushroom(b, 1, 1);
    goodA &= constructorMushTest;


    Cave *c = new Cave(8,8);

    c->getTom()->setLocation(a, 2, 2);
    c->command("place mushroom");

    Cave d(8,8);
    d = *c;

    test4CheckPointers(c, d, goodB);
    bool operatorPointer = goodB;
    bool operatorMushTest = hasMushroom(d, 2, 2);
    goodB &= operatorMushTest;

    string out = to_string ( (goodA ? 1 : 0) + (goodB ? 1 : 0) );

    if (!construtorPointer)
        out += " copy constructor failed to deep copy.";

    if (!constructorMushTest)
        out += " copy constructor failed to copy a placed mushroom.";

    if (!operatorPointer)
        out += " copy asignment operator failed to deep copy.";

    if (!operatorMushTest)
        out += " copy asignment operator failed to copy a placed mushroom.";

    return out;
}


string test5() {

    bool goodA = true, goodB = true;

    Cave c(8,8);
    c.getTom()->setLocation(&c, 4, 4);
    c.command("throw mushroom east");
    goodA &= hasMushroom(c, 5,4);

    c.command("throw coin south");
    goodA &= hasCoin(c, 4,5);

    c.getTom()->setLocation(&c, 6,6);

    c.command("throw coin west");
    goodA &= hasCoin(c, 5,6);

    c.command("throw coin east");
    goodB &= !hasCoin(c, 7,6); // can't throw - rock
    goodB &= goodA; // no marks for rock-blocking if throwing didn't work

    string out = to_string ( (goodA ? 1 : 0) + (goodB ? 1 : 0) );

    if (!goodA)
        out += " failed to throw objects.";

    if (goodA && !goodB)
        out += " thrown objects were not blocked by rocks.";

    return out;
}


bool hasBomb (Cave &c, int x, int y)
{

    for (auto t : *c.getMap()[x][y]->getThings())
        if ( t->getName().compare("bomb") == 0 )
            return true;

    return false;
}

int countBombs(Cave *c)
{
    int count = 0;
    for (int x = 0; x < c->getWidth(); x++)
        for (int y = 0; y < c->getHeight(); y++)
            if (hasBomb(*c, x, y))
                count++;

    return count;
}

string test6()
{

    bool goodA = true, goodB = true, goodC = true, goodD = true, goodE = true;

    Cave c(8,8);

    goodA &= countBombs(&c) == 0;

    const int bombCount = 9;
    int bombs[bombCount][2] = {{1,1},{2,2},{4,1},{1,6},{1,5},{2,5},{2,6},{2,7},{0,7}};

    for (int x = 0; x < bombCount; x++) {
        c.getTom()->setLocation(&c,bombs[x][0],bombs[x][1]);
        c.command("place bomb");
    }

    goodA &= countBombs(&c) == bombCount;

    c.getTom()->setLocation(&c,1,4);
    c.command("place mushroom");
    c.getTom()->setLocation(&c,1,5);
    c.command("place coin");

    c.getTom()->setLocation(&c,1,2);

    c.command("explode");

    int lastBombCount = countBombs(&c);
    goodB &= lastBombCount == 7;

    c.getTom()->setLocation(&c,1,5);
    c.command("explode");
    goodE &= (countBombs(&c) ==  2 || countBombs(&c) == lastBombCount - 5);

    bool cA = true, cB = true;

    cA &= !c.getMap()[2][7]->isBlocking(); // a bomb inside the rock should destroy the rock
    cA &=  c.getMap()[1][7]->isBlocking(); // other rocks remain untouched

    goodD &= cA;

    cB &=  hasMushroom ( c, 1, 4 ); // mushroom should not be destroyed
    cB &= !hasCoin( c, 1, 5 );    // coin should be destroyed

    goodC &= cB;

    c.command("place bomb");

    int score = (goodA ? 1 : 0) + (goodD ? 1 : 0) + (goodB ? 1 : 0) +(goodE ? 1 : 0) + (goodC ? 1 : 0);
    string out = to_string ( score );

    if (!goodA)
        out += " failed to place bombs.";

    if (score > 0) {
        if (!goodB)
            out += " failed to detonate correct number of bombs for first explosion.";

        if (!goodE)
            out += " failed to detonate correct number of bombs for second explosion.";

        if (!cA)
            out += " bombs failed to interact with rocks correctly.";

        if (!cB)
            out += " bombs failed to interact with mushroom/coin correctly.";
    }

    return out;
}

bool isEmpty(const string& file)  { // https://stackoverflow.com/questions/2424138/portable-way-to-get-file-size-in-c-c
    ifstream ifile(file);
    ifile.seekg(0, ios_base::end);
    return ifile.tellg() == 0;
}

bool endsWith(string const & value, string const & ending)
{
    if (ending.size() > value.size()) return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

string runTest(int test)
{
    if (test == 1)
        return test1();
    if (test == 2)
        return test2();
    if (test == 3)
        return test3();
    if (test == 4)
        return test4();
    if (test == 5)
        return test5();
    if (test == 6)
        return test6();
    return "~";
}


void test()
{
    cerr.setstate(ios_base::failbit); // no error output while testing please!

    cout << "Enter folder containing all source files, or enter to only run tests:";
    string folder;
    getline(cin, folder); // comment out this line to test quickly

    stringstream patch;
    string username = "";

    if (folder.length() > 0) {
        cout << endl <<"Enter leeds username (sc19xxx):";
        getline(cin, username);
        username += ".patch";

        if (auto dir = opendir(folder.c_str())) {
            while (auto f = readdir(dir)) {
                if (!f->d_name || f->d_name[0] == '.')
                    continue;

                string name = string (f->d_name);
                if (endsWith(name, ".cpp") || endsWith(name, ".pro") || endsWith(name, ".h") ) {
                    printf("Adding file: %s\n", f->d_name);

                    string fileName = folder+"/"+name;

                    if (isEmpty(fileName)) {
                        cout <<"...warning - empty file, please remove" << endl;
                        continue;
                    }

                    ifstream file(fileName );
                    stringstream tmp;

                    patch << "~~~~~~"<<name<<"~~~~~~\n";
                    patch << file.rdbuf();
                }
            }
            closedir(dir);
        }
        else {
            cout <<"folder not found: " << folder << endl;
            return;
        }
    }

    stringstream testResults;

    testResults << "test results" << endl;  // fixme: separate patch and test
    testResults << "1. " << test1() << endl;
    testResults << "2. " << test2() << endl;
    testResults << "3. " << test3() << endl;
    testResults << "4. " << test4() << endl;
    testResults << "5. " << test5() << endl;
    testResults << "6. " << test6() << endl;

    cout << testResults.str();

    if (folder.length() > 0) {
        ofstream outfile;
        outfile.open(folder+"/"+username, ios::out | ios::trunc );
        outfile << "******" << username << "******\n";
        outfile << testResults.str();
        outfile << patch.str();
        outfile.close();
    }
}
