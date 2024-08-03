#include "commands.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include <deque> // Include the deque header
#include <map>
#include <ctime>

using namespace std;
#include <utime.h>
class Debut {
    public:
        int execute(Tokens& obj);
};




int Cat::cat_command(string x) {
    input = x;
    ifstream catFile(input);
    stringstream stream;
    if (!catFile) {
        output = "cat: " + input + ": No such file or directory\n";
        return -1;
    }
    stream << catFile.rdbuf();
    output = stream.str();
    catFile.close();
    return 0;
}

int Cat::cat_command(string x, string y) {
    input = y;
    if (x == "-n") {
        ifstream catfile(input);
        if (!catfile) {
            output = "cat: " + input + ": No such file or directory\n";
            return -1;
        }
        int i = 1;
        stringstream stream;
        while (getline(catfile, myText)) {
            stream << i << " " << myText << endl;
            i++;
        }
        output = stream.str();
        catfile.close();
    } else {
        ifstream catFile1(x);
        if (!catFile1) {
            output = "cat: " + x + ": No such file or directory\n";
            return -1;
        }
        stringstream stream;
        while (getline(catFile1, myText)) {
            stream << myText << endl;
        }
        ifstream catFile2(input);
        if (!catFile2) {
            output = "cat: " + y + ": No such file or directory\n";
            return -1;
        }
        while (getline(catFile2, myText)) {
            stream << myText << endl;
        }
        output = stream.str();
        catFile2.close();
        catFile1.close();
    }
    return 0;
}

int Cat::execute(Tokens &obj) {
    if (obj.num_args() < 1) return -1;
    string file;
    int ind = 1;
    int n = obj.num_args();
    int res = 0;
    for (; ind <= n; ind++) {
        file = obj[ind];
        res = cat_command(file);
    }
    return res;
}

string Cat::getOutput() const { // Ensure this method is defined as const
    return output;
}

int WordCount::process_args(Tokens& obj) {
    int ind = -1;
    if (obj.num_args() == 1) {
        w_flag = c_flag = l_flag = true;
        if (obj[1][0] != '-')
            ind = 1;
        input = obj.tokens[1];
        return ind;
    } else {
        for (int i = 0; i < obj.num_args(); i++) {
            if (obj[i + 1] == "-w" || obj[i + 1] == "-words")
                w_flag = true;
            else if (obj[i + 1] == "-l" || obj[i + 1] == "-lines")
                l_flag = true;
            else if (obj[i + 1] == "-c" || obj[i + 1] == "-characters")
                c_flag = true;
            else if (obj[i + 1][0] != '-')
                ind = i + 1;
        }
        if (ind != -1)
            input = obj.tokens[ind];
    }
    return ind;
}
int WordCount::execute( Tokens& obj) {
    int ind = process_args(obj);
    if(ind == -1) {
        cout << "wc: invalid arguments passed" << endl;
        return -1;
    }
    string x = obj.tokens[ind];
    char *str = new char[x.size() + 1];
    ifstream wcFile(input);
    if(!wcFile) {
        cout << "wc: " + input + ": No such file or directory" << endl;
        return -1;
    }
    stringstream stream;
    stream << wcFile.rdbuf();
    string file_content;
    file_content = stream.str();

    int lines = 0;
    int words = 0;
    int characters = 0;
    for(auto c: file_content) {
        characters++;
        if(c == '\n') {
            lines++;
        }
    }
    if(w_flag) {
        string word;
        while(stream >> word) {
            words++;
        }
    }
    stringstream stream_op;
    if(l_flag) stream_op << lines << " ";
    if(w_flag) stream_op << words << " ";
    if(c_flag) stream_op << characters;
    // stream_op << endl;
    wcFile.close();
    output = stream_op.str();
    return 0;
}

int ListDir::execute(Tokens& obj) {
    int status;
    struct stat stats;
    std::stringstream stream;
    DIR *dirp;
    struct dirent *file_p;
    bool l = false, a = false;
    int ind = -1;

    for (int i = 0; i < obj.num_args(); i++) {
        if (obj[i + 1] == "-a" || obj[i + 1] == "-show_all_files")
            a = true;
        else if (obj[i + 1] == "-l" || obj[i + 1] == "-detailed_list")
            l = true;
        else if (obj[i + 1] == "-al" || obj[i + 1] == "detailed_list_of_all_files")
            a = l = true;
        else if (obj[i + 1][0] != '-')
            ind = i + 1;
    }
    std::string p_str;
    if (ind != -1)
        p_str = obj.tokens[ind];
    else {
        p_str = filesystem::current_path();
    }

    std::string path(p_str);

    std::cout << "Debug: Listing directory: " << path << std::endl; // Debug print

    dirp = opendir(path.c_str());
    if (dirp == NULL) {
        stream << "No such File or Directory." << std::endl;
        output = stream.str(); // Set the output here
        std::cout << "Debug: Directory not found: " << path << std::endl; // Debug print
        return -1;
    }
    if (l) {
        stream << std::left
               << std::setw(10) << "Permissions"
               << std::right
               << std::setw(8) << "Size "
               << std::left
               << std::setw(15) << " Access Time"
               << "File Name" << std::endl;
    }
    file_p = new struct dirent();
    do {
        file_p = readdir(dirp);
        if (file_p != NULL) {
            if (!strcmp(file_p->d_name, "."))
                continue;
            if (!strcmp(file_p->d_name, ".."))
                continue;
            if (!a && file_p->d_name[0] == '.')
                continue;
            std::string sl = "/";
            std::string file_path = path + sl + std::string(file_p->d_name);
            std::string perm;
            if (l) {
                status = stat(file_path.c_str(), &stats);
                struct tm file_time = *(localtime(&stats.st_atime));
                mode_str(stats.st_mode, perm);
                stream << std::left
                       << std::setw(10) << perm
                       << std::right
                       << std::setw(8) << (long)stats.st_size
                       << "  "
                       << std::left
                       << std::setw(15) << getTimeString(file_time)
                       << file_p->d_name << std::endl;
            } else {
                stream << file_p->d_name << "\t";
            }
        }
    } while (file_p != NULL);

    output = stream.str(); // Set the output here
    std::cout << "Debug: Directory listing output: " << output << std::endl; // Debug print
    closedir(dirp);
    delete file_p;
    file_p = NULL;
    return 0;
}

std::string ListDir::getOutput() const { // Ensure this method is defined as const
    return output;
}
std::string ListDir::getTimeString(struct tm &file_time) {
    std::string month;
    std::stringstream timestrm;
    switch (file_time.tm_mon) {
        case 0: month = "Jan"; break;
        case 1: month = "Feb"; break;
        case 2: month = "Mar"; break;
        case 3: month = "Apr"; break;
        case 4: month = "May"; break;
        case 5: month = "Jun"; break;
        case 6: month = "Jul"; break;
        case 7: month = "Aug"; break;
        case 8: month = "Sep"; break;
        case 9: month = "Oct"; break;
        case 10: month = "Nov"; break;
        case 11: month = "Dec"; break;
    }

    timestrm << month + " "
             << std::setw(2) << std::setfill('0') << file_time.tm_mday
             << " "
             << std::setw(2) << std::setfill('0') << file_time.tm_hour
             << ":"
             << std::setw(2) << std::setfill('0') << file_time.tm_min;
    return timestrm.str();
}

void ListDir::mode_str(mode_t m, std::string &str) {
    str.push_back((m & S_IRUSR) ? 'r' : '-');
    str.push_back((m & S_IWUSR) ? 'w' : '-');
    str.push_back((m & S_IRGRP) ? 'r' : '-');
    str.push_back((m & S_IXUSR) ? 'x' : '-');
    str.push_back((m & S_IWGRP) ? 'w' : '-');
    str.push_back((m & S_IXGRP) ? 'x' : '-');
    str.push_back((m & S_IROTH) ? 'r' : '-');
    str.push_back((m & S_IWOTH) ? 'w' : '-');
    str.push_back((m & S_IXOTH) ? 'x' : '-');
}
int PrintDir::execute( Tokens& obj) {
    if( obj.num_args() > 1 )
    {
        cout << "pwd does not accept arguments." << endl;
        return -1;
    }
    output = filesystem::current_path();
    return 0;
};

int Fork::execute(Tokens& obj) {
    vector <string> cmd_tok=obj.tokens;
    
    pid_t p;
    int res;
    p = fork();
    if(p > 0) {
        res = wait(NULL);
    }
    else if(!p)
    {                
        char *args[cmd_tok.size()+1];
        int index=0;int i=0;int k=0;
        for (i=0;i<cmd_tok.size();i++)
        {   
            char *exec_name= new char[cmd_tok[i].size()+1];
            for (index=0;index<cmd_tok[0].size();index++)
            {
                exec_name[index]=cmd_tok[i][index];
            }
            args[i]=exec_name;
        }
        args[i]=NULL;
        
        execvp(args[0],args);
        exit(0);
    }
    return res;
}

int MakeDirectory::execute(Tokens& obj) {
    if(obj.num_args() < 1)
        return -1;
    int res = 0;
    int n = obj.num_args();
    int ind = 1;
    for(;ind <= n; ind++) {
        int result = createDir(obj[ind]);
        if( result != 0 ) res = result;
    }
    return res;
}

int MakeDirectory::createDir(string str) {
    DIR *dirp;
    string p_str(str);
    string path = "";
    string dir_name = "";

    int n = p_str.size();
    bool flag = false;
    int i = n -1;
    while( i >= 0) {
        char c = p_str[i];
        if(c == '/') {
            flag = true;
            break;
        }
        i--;
        dir_name = c + dir_name;
    }
    if(flag && i>0) {
        int j = 0;
        while( j <= i) {
            path.push_back(p_str[j]);
            j++;
        }
        if(path[0] != '/') {
            path = "/" + path;
            string cur = filesystem::current_path();
            path = cur + path;
        }
    }
    else {
        path = filesystem::current_path();
    }
    
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {   
        cout << dir_name + ": cannot create directory." << endl;
        return -1;
    }

    string path_dir = "";
    if(flag)
        path_dir = path + dir_name;
    else
        path_dir = path + "/" + dir_name;

    if (stat(path_dir.c_str(), &st) == 0) {
        cout << dir_name + ": directory already exists" << endl;
        return -1;
    }

    filesystem::create_directory(path_dir.c_str());

    return 0;
}

string MakeDirectory::getOutput() {
    return output;
}

int ChangeDir::execute(string dir_path)
{
    try {
        filesystem::current_path(dir_path);
    }
    catch ( const filesystem::filesystem_error& exception) {
            cout << "error: " << exception.what();
            cout << endl;
            return -1;
    }
    return 0;
};
//------------

//------
void Clear::execute( Tokens& obj)
{
    if( obj.num_args() > 0 )
    {
        cout << "clear does not accept any arguments." << endl;
        return;
    }

    // ANSI Escape Sequences used
    cout << "\033[2J\033[1;1H";
};

string Move::getFileName(const string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}
int Move::execute(Tokens& obj) {
        if (obj.num_args() != 2) {
            cout << "mv: missing source and/or destination arguments" << endl;
            return -1;
        }

        string source = obj[1];
        string destination = obj[2];

        struct stat sourceStat, destStat;
        if (stat(source.c_str(), &sourceStat) != 0) {
            cout << "mv: cannot access '" << source << "': No such file or directory" << endl;
            return -1;
        }

        if (stat(destination.c_str(), &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
            // Destination is a directory, move the source into the directory
            string newDestination = destination + "/" + getFileName(source);
            if (renameFile(source, newDestination) == 0) {
                cout << "Moved '" << source << "' to '" << newDestination << "'" << endl;
                return 0;
            } else {
                cout << "mv: error moving file" << endl;
                return -1;
            }
        } else {
            // Destination is a file or a non-existent path, rename the source
            if (renameFile(source, destination) == 0) {
                cout << "Renamed '" << source << "' to '" << destination << "'" << endl;
                return 0;
            } else {
                cout << "mv: error renaming file" << endl;
                return -1;
            }
        }
    }

int Touch::execute( Tokens& obj) {

    vector<string>input_tok=obj.tokens;
    string dir;

    int count=1;
    int flag=0;
    int args = obj.num_args();
    int max = args;

    count=1;
    int res = 0;
    while (count<=max)
    {
        string file=obj.tokens[count];
        struct stat st;
        string path(file);
        
        if (stat(path.c_str(), &st) == 0) {
            struct timespec ts;    
            clock_gettime(CLOCK_REALTIME, &ts);
            struct utimbuf updated_time;
            updated_time.actime = ts.tv_sec;
            updated_time.modtime = ts.tv_sec;
            if (utime(path.c_str(), &updated_time) != 0) {
                return -1;
            }
        }
        else {
            string p_str(file);
            string path = "";
            string file_name = "";

            bool flag = false;
            int n = p_str.size();
            int i = n -1;
            while( i >= 0) {
                char c = p_str[i];
                if(c == '/') {
                    flag = true;
                    break;
                }
                i--;
                file_name = c + file_name;
            }
            i++;
            if(flag && i > 0) {
                int j = 0;
                while( j < i) {
                    path.push_back(p_str[j]);
                    j++;
                }
                if(path[0] != '/') {
                    path = "/" + path;
                    string cur = filesystem::current_path();
                    path = cur + path;
                }
            }
            else path = filesystem::current_path();

            if(stat(path.c_str(), &st) != 0) {
                cout << "touch: directory " + path + " does not exist" << endl;
                res = -1;
            }
            else {
                string path_dir = "";
                if(flag)
                    path_dir = path + file_name;
                else
                    path_dir = path + "/" + file_name;

                try {
                    ofstream Mfile(path_dir);
                    Mfile.close();
                }
                catch( const exception & e) {
                    cout << e.what() ;
                    res = -1;
                }
            }
        }
        count++;
    }
        
    return res;
}

int Copy::execute(Tokens& tokens) {
    if (tokens.num_args() < 2) {
        std::cerr << "Error: Insufficient arguments for copy command" << std::endl;
        return -1;
    }

    std::string src = tokens[1];
    std::string dest = tokens[2];

    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile) {
        std::cerr << "Error: Source file " << src << " does not exist" << std::endl;
        return -1;
    }

    std::ofstream destFile(dest, std::ios::binary);
    if (!destFile) {
        std::cerr << "Error: Destination file " << dest << " cannot be created" << std::endl;
        return -1;
    }

    destFile << srcFile.rdbuf();

    srcFile.close();
    destFile.close();

    return 0;
}

int Grep::execute(Tokens& obj) {
    if (obj.num_args() < 2) {
        output = "grep: missing pattern and/or file arguments\n";
        return -1;
    }

    std::string pattern = obj[1];
    std::string file = obj[2];
    std::ifstream inFile(file);
    if (!inFile) {
        output = "grep: " + file + ": No such file or directory\n";
        return -1;
    }

    std::string line;
    std::stringstream stream;
    while (std::getline(inFile, line)) {
        if (line.find(pattern) != std::string::npos) {
            stream << line << std::endl;
        }
    }
    inFile.close();
    output = stream.str();
    return 0;
}

std::string Grep::getOutput() const { // Ensure this method is defined as const
    return output;
}

int Chmod::execute(Tokens& tokens) {
    // Check if the number of arguments is correct
    if (tokens.num_args() != 2) {
        output = "Error: chmod requires exactly 2 arguments\n";
        return -1;
    }

    std::string mode_str = tokens[1];
    std::string filename = tokens[2];

    // Convert mode string to mode_t
    mode_t mode = 0;
    for (char c : mode_str) {
        mode = mode << 3;
        if (c >= '0' && c <= '7') {
            mode |= (c - '0');
        } else {
            output = "Error: Invalid mode string\n";
            return -1;
        }
    }

    // Change file permissions
    if (::chmod(filename.c_str(), mode) != 0) {
        output = "Error: Failed to change file permissions\n";
        return -1;
    }

    output = "Permissions of '" + filename + "' changed to " + mode_str + "\n";
    return 0;
}

std::string Chmod::getOutput() const { // Ensure this method is defined as const
    return output;
}

int Remove::execute(Tokens& tokens) {
    if (tokens.num_args() < 1) {
        string output = "rm: missing operand";
        std::cerr << output << std::endl;
        return -1;
    }

    for (int i = 1; i <= tokens.num_args(); ++i) {
        std::string file = tokens[i];
        if (remove(file.c_str()) != 0) {
            output = "rm: cannot remove '" + file + "': No such file or directory";
            std::cerr << output << std::endl;
            return -1;
        }
    }

    output = "Files removed successfully";
    return 0;
}

std::string Remove::getOutput() const {
    return output;
}

int Head::execute(Tokens& tokens) {
    //std::cout << "Debug: Head command execute called" << std::endl;
    if (tokens.num_args() < 1) {
        output = "head: missing operand";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string file = tokens[1];
    std::ifstream infile(file);
    if (!infile.is_open()) {
        output = "head: cannot open '" + file + "': No such file or directory";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string line;
    std::ostringstream oss;
    int line_count = 0;
    while (std::getline(infile, line) && line_count < 5) {
        oss << line << "\n";
        line_count++;
    }
    infile.close();

    output = oss.str();

    std::cout << output << std::endl;
   // std::cout << "Debug: Head command output generated" << std::endl;
    return 0;
}

std::string Head::getOutput() const {
    return output;
}

int Tail::execute(Tokens& tokens) {
    if (tokens.num_args() < 1) {
        output = "tail: missing operand";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string file = tokens[1];
    std::ifstream infile(file);
    if (!infile.is_open()) {
        output = "tail: cannot open '" + file + "': No such file or directory";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string line;
    std::deque<std::string> lines; // Correctly declare the deque
    while (std::getline(infile, line)) {
        if (lines.size() == 5) {
            lines.pop_front();
        }
        lines.push_back(line);
    }
    infile.close();

    std::ostringstream oss;
    for (const auto& l : lines) {
        oss << l << "\n";
    }

    output = oss.str();

    std::cout << output << std::endl;
    return 0;
}

std::string Tail::getOutput() const {
    return output;
}

int Diff::execute(Tokens& tokens) {
    if (tokens.num_args() < 2) {
        output = "diff: missing operand";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string file1 = tokens[1];
    std::string file2 = tokens[2];
    std::ifstream infile1(file1);
    std::ifstream infile2(file2);

    if (!infile1.is_open()) {
        output = "diff: cannot open '" + file1 + "': No such file or directory";
        std::cerr << output << std::endl;
        return -1;
    }

    if (!infile2.is_open()) {
        output = "diff: cannot open '" + file2 + "': No such file or directory";
        std::cerr << output << std::endl;
        return -1;
    }

    std::string line1, line2;
    std::ostringstream oss;
    int line_number = 1;
    bool differences_found = false;

    while (std::getline(infile1, line1) && std::getline(infile2, line2)) {
        if (line1 != line2) {
            oss << "Line " << line_number << ":\n";
            oss << "< " << line1 << "\n";
            oss << "> " << line2 << "\n";
            differences_found = true;
        }
        line_number++;
    }

    // Check for any remaining lines in file1
    while (std::getline(infile1, line1)) {
        oss << "Line " << line_number << ":\n";
        oss << "< " << line1 << "\n";
        oss << "> \n";
        differences_found = true;
        line_number++;
    }

    // Check for any remaining lines in file2
    while (std::getline(infile2, line2)) {
        oss << "Line " << line_number << ":\n";
        oss << "< \n";
        oss << "> " << line2 << "\n";
        differences_found = true;
        line_number++;
    }

    infile1.close();
    infile2.close();

    if (!differences_found) {
        oss << "Files are identical\n";
    }

    output = oss.str();

    std::cout << output << std::endl;
    return 0;
}

std::string Diff::getOutput() const {
    return output;
}

int Reverse::execute(Tokens& obj) {
    if (obj.num_args() != 1) {
        cout << "reverse: requires exactly one argument" << endl;
        return -1;
    }
    string input = obj[1];
    ifstream inFile(input);
    if (!inFile) {
        cout << "reverse: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    reverseFileContent(input);
    return 0;
}

void Reverse::reverseFileContent(const string& filePath) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    reverse(content.begin(), content.end());

    ofstream outFile(filePath);
    outFile << content;
    outFile.close();
}

int Uppercase::execute(Tokens& obj) {
    if (obj.num_args() != 1) {
        cout << "uppercase: requires exactly one argument" << endl;
        return -1;
    }
    string input = obj[1];
    ifstream inFile(input);
    if (!inFile) {
        cout << "uppercase: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    uppercaseFileContent(input);
    return 0;
}

void Uppercase::uppercaseFileContent(const string& filePath) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    transform(content.begin(), content.end(), content.begin(), ::toupper);

    ofstream outFile(filePath);
    outFile << content;
    outFile.close();
}

int Lowercase::execute(Tokens& obj) {
    if (obj.num_args() != 1) {
        cout << "lowercase: requires exactly one argument" << endl;
        return -1;
    }
    string input = obj[1];
    ifstream inFile(input);
    if (!inFile) {
        cout << "lowercase: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    lowercaseFileContent(input);
    return 0;
}

void Lowercase::lowercaseFileContent(const string& filePath) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    transform(content.begin(), content.end(), content.begin(), ::tolower);

    ofstream outFile(filePath);
    outFile << content;
    outFile.close();
}

int FindReplace::execute(Tokens& obj) {
    if (obj.num_args() != 3) {
        cout << "findreplace: requires exactly three arguments" << endl;
        return -1;
    }
    string input = obj[1];
    string findStr = obj[2];
    string replaceStr = obj[3];
    ifstream inFile(input);
    if (!inFile) {
        cout << "findreplace: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    findAndReplaceInFile(input, findStr, replaceStr);
    return 0;
}

void FindReplace::findAndReplaceInFile(const string& filePath, const string& findStr, const string& replaceStr) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    size_t pos = 0;
    while ((pos = content.find(findStr, pos)) != string::npos) {
        content.replace(pos, findStr.length(), replaceStr);
        pos += replaceStr.length();
    }

    ofstream outFile(filePath);
    outFile << content;
    outFile.close();
}
int Encrypt::execute(Tokens& obj) {
    if (obj.num_args() != 2) {
        output = "encrypt: requires exactly two arguments\n";
        return -1;
    }
    std::string input = obj[1];
    int shift = std::stoi(obj[2]);
    std::ifstream inFile(input);
    if (!inFile) {
        output = "encrypt: " + input + ": No such file or directory\n";
        return -1;
    }
    inFile.close();
    encryptFileContent(input, shift);
    output = "File content encrypted successfully.\n";
    return 0;
}

void Encrypt::encryptFileContent(const std::string& filePath, int shift) {
    std::ifstream inFile(filePath);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    std::string encryptedContent = caesarCipher(content, shift);

    std::ofstream outFile(filePath);
    outFile << encryptedContent;
    outFile.close();
}

std::string Encrypt::caesarCipher(const std::string& text, int shift) {
    std::string result = text;
    for (char& c : result) {
        if (std::isalpha(c)) {
            char base = std::islower(c) ? 'a' : 'A';
            c = (c - base + shift) % 26 + base;
        }
    }
    return result;
}

std::string Encrypt::getOutput() const { // Ensure this method is defined as const
    return output;
}

int Decrypt::execute(Tokens& obj) {
    if (obj.num_args() != 2) {
        output = "decrypt: requires exactly two arguments\n";
        return -1;
    }
    std::string input = obj[1];
    int shift = std::stoi(obj[2]);
    std::ifstream inFile(input);
    if (!inFile) {
        output = "decrypt: " + input + ": No such file or directory\n";
        return -1;
    }
    inFile.close();
    decryptFileContent(input, shift);
    output = "File content decrypted successfully.\n";
    return 0;
}

void Decrypt::decryptFileContent(const std::string& filePath, int shift) {
    std::ifstream inFile(filePath);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    std::string decryptedContent = caesarCipher(content, -shift);

    std::ofstream outFile(filePath);
    outFile << decryptedContent;
    outFile.close();
}

std::string Decrypt::caesarCipher(const std::string& text, int shift) {
    std::string result = text;
    for (char& c : result) {
        if (std::isalpha(c)) {
            char base = std::islower(c) ? 'a' : 'A';
            c = (c - base + shift + 26) % 26 + base; // +26 to handle negative shifts
        }
    }
    return result;
}

std::string Decrypt::getOutput() const { // Ensure this method is defined as const
    return output;
}

int Compress::execute(Tokens& obj) {
    if (obj.num_args() != 1) {
        cout << "compress: requires exactly one argument" << endl;
        return -1;
    }
    string input = obj[1];
    ifstream inFile(input);
    if (!inFile) {
        cout << "compress: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    compressFileContent(input);
    return 0;
}

void Compress::compressFileContent(const string& filePath) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    string compressedContent = runLengthEncode(content);

    ofstream outFile(filePath);
    outFile << compressedContent;
    outFile.close();
}

string Compress::runLengthEncode(const string& text) {
    string encoded;
    int n = text.length();
    for (int i = 0; i < n; i++) {
        int count = 1;
        while (i < n - 1 && text[i] == text[i + 1]) {
            i++;
            count++;
        }
        encoded += text[i];
        if (count > 1) {
            encoded += to_string(count);
        }
    }
    return encoded;
}

int Decompress::execute(Tokens& obj) {
    if (obj.num_args() != 1) {
        cout << "decompress: requires exactly one argument" << endl;
        return -1;
    }
    string input = obj[1];
    ifstream inFile(input);
    if (!inFile) {
        cout << "decompress: " + input + ": No such file or directory" << endl;
        return -1;
    }
    inFile.close();
    decompressFileContent(input);
    return 0;
}

void Decompress::decompressFileContent(const string& filePath) {
    ifstream inFile(filePath);
    stringstream buffer;
    buffer << inFile.rdbuf();
    string content = buffer.str();
    inFile.close();

    string decompressedContent = runLengthDecode(content);

    ofstream outFile(filePath);
    outFile << decompressedContent;
    outFile.close();
}

string Decompress::runLengthDecode(const string& text) {
    string decoded;
    int n = text.length();
    for (int i = 0; i < n; i++) {
        char ch = text[i];
        int count = 0;
        while (i + 1 < n && isdigit(text[i + 1])) {
            count = count * 10 + (text[i + 1] - '0');
            i++;
        }
        if (count == 0) {
            decoded += ch;
        } else {
            decoded.append(count, ch);
        }
    }
    return decoded;
}
int FileHistory::execute(Tokens& obj) {
    if (obj.num_args() != 2) {
        output = "filehistory: requires exactly two arguments\n";
        return -1;
    }
    std::string command = obj[1];
    std::string filePath = obj[2];
    if (command == "track") {
        trackChange(filePath);
    } else if (command == "display") {
        displayHistory(filePath);
    } else {
        output = "filehistory: unrecognized command\n";
        return -1;
    }
    return 0;
}

void FileHistory::trackChange(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        output = "filehistory: " + filePath + ": No such file or directory\n";
        return;
    }
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    time_t now = time(0);
    std::string timestamp = ctime(&now);
    history[filePath].push_back("Timestamp: " + timestamp + "Content:\n" + content);
    output = "Change tracked for " + filePath + "\n";
    saveHistory();
}

void FileHistory::displayHistory(const std::string& filePath) {
    if (history.find(filePath) == history.end()) {
        output = "filehistory: no history found for " + filePath + "\n";
        return;
    }
    std::stringstream ss;
    for (const auto& entry : history[filePath]) {
        ss << entry << "\n";
    }
    output = ss.str();
}

void FileHistory::saveHistory() {
    std::ofstream outFile("filehistory.dat", std::ios::binary);
    for (const auto& [filePath, entries] : history) {
        outFile << filePath << '\n';
        outFile << entries.size() << '\n';
        for (const auto& entry : entries) {
            outFile << entry.size() << '\n' << entry;
        }
    }
    outFile.close();
}

void FileHistory::loadHistory() {
    std::ifstream inFile("filehistory.dat", std::ios::binary);
    if (!inFile) return;
    std::string filePath;
    while (std::getline(inFile, filePath)) {
        size_t entryCount;
        inFile >> entryCount;
        inFile.ignore();
        std::vector<std::string> entries(entryCount);
        for (size_t i = 0; i < entryCount; ++i) {
            size_t entrySize;
            inFile >> entrySize;
            inFile.ignore();
            entries[i].resize(entrySize);
            inFile.read(&entries[i][0], entrySize);
        }
        history[filePath] = std::move(entries);
    }
    inFile.close();
}

std::string FileHistory::getOutput() const { // Ensure this method is defined as const
    return output;
}

int FileSplit::execute(Tokens& obj) {
    std::cout << "Debug: FileSplit execute called with " << obj.num_args() << " arguments" << std::endl; // Debug print
    if (obj.num_args() != 3) {
        output = "filesplit: requires exactly three arguments\n";
        return -1;
    }
    std::string filePath = obj[1];
    std::string mode = obj[2];
    size_t value = std::stoi(obj[3]);
    std::cout << "Debug: filePath = " << filePath << ", mode = " << mode << ", value = " << value << std::endl; // Debug print
    if (mode == "size") {
        splitBySize(filePath, value);
    } else if (mode == "lines") {
        splitByLines(filePath, value);
    } else {
        output = "filesplit: unrecognized mode\n";
        return -1;
    }
    output = "File split operation completed successfully.\n";
    return 0;
}

void FileSplit::splitBySize(const std::string& filePath, size_t size) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        output = "filesplit: " + filePath + ": No such file or directory\n";
        return;
    }
    char* buffer = new char[size];
    int part = 1;
    while (inFile.read(buffer, size) || inFile.gcount() > 0) {
        std::ofstream outFile(filePath + ".part" + std::to_string(part++), std::ios::binary);
        outFile.write(buffer, inFile.gcount());
        outFile.close();
    }
    delete[] buffer;
    inFile.close();
    output = "File split into " + std::to_string(part - 1) + " parts\n";
}

void FileSplit::splitByLines(const std::string& filePath, size_t lines) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        output = "filesplit: " + filePath + ": No such file or directory\n";
        return;
    }
    std::string line;
    int part = 1;
    size_t lineCount = 0;
    std::ofstream outFile(filePath + ".part" + std::to_string(part++));
    while (std::getline(inFile, line)) {
        if (lineCount++ == lines) {
            outFile.close();
            outFile.open(filePath + ".part" + std::to_string(part++));
            lineCount = 1;
        }
        outFile << line << std::endl;
    }
    outFile.close();
    inFile.close();
    output = "File split into " + std::to_string(part - 1) + " parts\n";
}

std::string FileSplit::getOutput() const { // Ensure this method is defined as const
    return output;
}

int FileJoin::execute(Tokens& obj) {
    if (obj.num_args() < 2) {
        output = "filejoin: requires at least two arguments\n";
        return -1;
    }
    std::vector<std::string> fileNames;
    for (size_t i = 1; i < obj.tokens.size() - 1; ++i) {
        fileNames.push_back(obj[i]);
    }
    std::string outputFilePath = obj[obj.tokens.size() - 1];
    joinFiles(fileNames, outputFilePath);
    output = "File join operation completed successfully.\n";
    return 0;
}

void FileJoin::joinFiles(const std::vector<std::string>& fileNames, const std::string& outputFilePath) {
    std::ofstream outFile(outputFilePath, std::ios::binary);
    if (!outFile) {
        output = "filejoin: could not create output file\n";
        return;
    }
    for (const auto& fileName : fileNames) {
        std::ifstream inFile(fileName, std::ios::binary);
        if (!inFile) {
            output = "filejoin: " + fileName + ": No such file or directory\n";
            outFile.close();
            return;
        }
        outFile << inFile.rdbuf();
        inFile.close();
    }
    outFile.close();
}

std::string FileJoin::getOutput() const { // Ensure this method is defined as const
    return output;
}