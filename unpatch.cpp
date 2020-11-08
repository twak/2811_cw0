#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include<iostream>
#include<string>
#include<regex>
#include<string>
#include <dirent.h> // this might be tricky, but mostly there for gnu compilers.
#include <iterator>

#include "cave.h"
#include "coin.h"
#include "location.h"
#include "mushroom.h"

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#endif

using namespace std;

class rezult {
public:
    std::string output;
    int code;
};

rezult * exec(string in) { //https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po

    rezult* r = new rezult;

    const char* cmd = in.c_str();
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        r->code = pclose(pipe);
        r->output = result;
        throw;
    }

    r->code = pclose(pipe);
    r->output = result;

    return r;


//    std::array<char, 128> buffer;
//    std::string result;
//    FILE* file = popen(cmd, "r");
//    std::unique_ptr<FILE, decltype(&pclose)> pipe(file, pclose);
//    std::unique_ptr<FILE> pipe(file);

//    if (!pipe) {
//        throw std::runtime_error("popen() failed!");
//    }

//    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
//        result += buffer.data();
//    }

//    int ret = pclose(file);
//    if(WIFEXITED(ret))
//      printf("%d\n", WEXITSTATUS(ret));

//    cout << rc/8 << endl;

//    pipe.get()->
//    int exitStatus = WEXITSTATUS(pclose(file));
//    if ( exitStatus != 0 )
//        return to_string(exitStatus > 0 ? -exitStatus : exitStatus);


//    return result;
}

void readClaimed (string& dir, string& username, ofstream& csv) {

    std::ifstream t(dir+"/"+username+".patch");
    std::stringstream buffer;

    buffer << t.rdbuf();

    string fileStr = buffer.str();
    cout << username << endl;
    csv << username << ",";

    double claimed[6] = {0,0,0,0,0,0};

    for (int i = 0; i < 6; i++) {
        smatch m;
        if ( regex_search(fileStr, m, regex( to_string(i+1)+ "\\. ([0-9])"))) {
            claimed[i] = stoi(m[1].str());
            cout << "claimed " << to_string(i+1) << "/ " + m[1].str() << endl;
            csv <<  m[1].str() << ",";
        }
        else
            csv << ",";
    }
}

bool endsWith2(string const & value, string const & ending)
{
    if (ending.size() > value.size()) return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void mkdir_ (string sPath) {  // https://stackoverflow.com/a/35109823/708802
    int nError = 0;
    #if defined(_WIN32)
      nError = _mkdir(sPath.c_str()); // can be used on Windows
    #else
      nError = mkdir(sPath.c_str(), S_IRWXU); // can be used on non-Windows
    #endif
    if (nError != 0) {
      cout << "error creating directory "<< sPath <<". quitting.";
      exit (-2);
    }
}

string unpatch (const string& dir, const string& username, const string& outdir) {

    std::ifstream t(dir);
    std::stringstream buffer;
    buffer << t.rdbuf();
    string fileStr = buffer.str();

    fileStr.erase(std::remove(fileStr.begin(), fileStr.end(), '\357'), fileStr.end()); // dirty hack to remove BOM...
    fileStr.erase(std::remove(fileStr.begin(), fileStr.end(), '\273'), fileStr.end());
    fileStr.erase(std::remove(fileStr.begin(), fileStr.end(), '\277'), fileStr.end());


//    string* username = new string ( fileStr.substr(6, fileStr.find(".patch***")-6) );
    cout << username << endl;

    mkdir_( string ( outdir+"/"+username ).c_str() );

    std::regex rgx("~~~~~~([a-zA-Z0-9\\.]+?)~~~~~~");

    string lastName;
    int startPos = -1;

    ofstream* outfile = 0;

    for(std::sregex_iterator i = std::sregex_iterator(fileStr.begin(), fileStr.end(), rgx);
                             i != std::sregex_iterator();
                             ++i) {
        smatch m = *i;

        string filename = m[1].str();

        if ( endsWith2 ( filename, ".cpp") || endsWith2 ( filename, ".h") || endsWith2 ( filename, ".pro") ) {

            if (outfile) {
                *outfile << fileStr.substr(startPos, m.position() - startPos);
                outfile -> close();
            }

            startPos = m.position() + m.length();

            cout << "extracting " << filename << endl;
//            cout << m.position() << " " << m.length() << endl;

            outfile = new ofstream();
            outfile -> open( outdir+"/"+username+"/"+filename, ios::out | ios::trunc );

//            if (endsWith2 ( filename, ".cpp") || endsWith2 ( filename, ".h")) {
//                *outfile << "/** unpatched file from " << *username << " **/\n";
//                *outfile << "/**" + m.str() +" **/";
//            }
        }
    }

    if (outfile) {
        *outfile << fileStr.substr(startPos, fileStr.length()-startPos);
        outfile -> close();
    }

    outfile = new ofstream();
    outfile -> open( outdir+"/"+username+"/"+username+".patch", ios::out | ios::trunc );
    *outfile << fileStr;
    outfile -> close();
}

string toWin(string nix) {
    std::replace(nix.begin(), nix.end(), '/', '\\');
    return nix;
}

bool build (const string& dir, const string& thisFile, ofstream& results) {

    // copy files that should not be changed

    #if defined(_WIN32)
    exec ("copy /y \"" + toWin ( thisFile)+"\\grade_files\\*\" \"" + toWin(dir)+"\"" );
    #else
    exec ("cp " + thisFile+"/grade_files/* " + dir );
    #endif

    // build
    cout << exec("qmake -o "+dir+"/Makefile "+dir+"/CavePlusPlus.pro" )->output;
    cout << endl;

    #if defined(_WIN32)
    rezult* br = exec("mingw32-make.exe -C "+toWin(dir) );
    #else
    rezult* br = exec("make -C "+toWin(dir) );
    #endif

    cout << br->output;
    cout << endl;

    if (br->code) {
        cout << " ** build failure " << br->code << endl;
        results << "0,build failed,0,build failed,0,build failed,0,build failed,0,build failed,0,build failed,";
        return false;
    }
    else
        return true;


}

void test (string& dir, ofstream& results) {

    const int tries = 5; // number of crashes shown
    for (int i = 1; i <= 6; i++) { // 6 questions

        int bestScore = 0;
        int crashes = 0;
        string bestString;

        for (int c = 0; c< tries; c++) {

#if defined(_WIN32)
            rezult* res = exec( (toWin(dir)+"\\release\\CavePlusPlus "+ to_string(i) ).c_str());
#else
            rezult* res = exec( (dir+"/CavePlusPlus "+ to_string(i) ).c_str());
#endif

            if (res->output[0] != '\0' && res->code == 0) {
                int score = res->output[0] - '0';
                bestScore = score;
                bestString = res->output.substr(1, res->output.size()-2) ;
            }

            if (res -> code != 0)
                crashes++;

            delete res;
        }

        if (crashes !=0) {
            bestScore = 0;
            if (crashes == tries)
                bestString = " crashed everytime :(";
            else
                bestString = " crashed sometimes ("+to_string(crashes)+") :/";
        }

        results << bestScore << "," << bestString << ",";
        cout << "awarded " << i <<"/ " << bestScore << "  comments: " << bestString << endl;

    }
}

void unpatchAll()
{
    // where are the student's patch files? (all in one directory from minerva).
    string dir = "/home/twak/Downloads/cw1_submissions/gradebook_202021_32871_COMP2811_Coursework2013a20Cave20Plus20Plus_2020-10-28-18-38-24";
    // where should the student's files be unpatched to?
    const string outdir = "/home/twak/Downloads/out";

    const std::regex usrReg("Plus\\_([0-9a-z]+)\\_attempt");

    if (auto submissionFolder = opendir(dir.c_str())) {
        while (auto f = readdir(submissionFolder)) {

            if (!f->d_name || f->d_name[0] == '.')
                continue;

            string name = string (f->d_name);

            string username = name;
            smatch m;

            if (regex_search(name, m, usrReg))
                username = m[1].str();

            if (endsWith2(name, ".patch")) {

                cout << "*** starting unpatch " + username << endl;
                unpatch(dir+"/"+name, username, outdir);
                cout << "*** finished unpatch " << username << endl;
            }
         }
      }
}

void testAll()
{
    ofstream resultsfile;

    // location to write final grades:
    resultsfile.open( "C:/Users/twak/Downloads/unpatched_all_on_time/out_problematic.csv", ios::out | ios::trunc );
    // directory of this unpatch.cpp file:
    const string thisfile = "C:/Users/twak/Documents/comp2811_20_lectures/courseworks/2811_cw0";
    // location of the unpatched submissions:
    const string dir = "C:/Users/twak/Downloads/unpatched_all_on_time/problematic";

    if (auto submissionFolder = opendir(dir.c_str())) {
        while (auto f = readdir(submissionFolder)) {

            if (!f->d_name || f->d_name[0] == '.')
                continue;

            string username = string (f->d_name);
            string userDir = dir +"/" + username;

            cout << "*** starting test " << username << endl;

            readClaimed(userDir, username, resultsfile);
            if (build(userDir, thisfile, resultsfile))
                test(userDir, resultsfile);

            resultsfile << endl;

            cout << "*** finished test " << username << endl;
         }
      }

    resultsfile.close();
}

int main(int argc, char** argv)
{
    unpatchAll(); // extract all the patch files
                   // (add any student's submissions who didn't use the patch format here)
    testAll();    // build and test all files
}
