#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
// ... existing code ...
using namespace std;
class Tokens {
    public:
        vector <string> tokens;
        Tokens(string);
        string operator[](int index);
        int num_args();
        int isSpace(string);
};
