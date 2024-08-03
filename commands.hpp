#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include <ctime>
#include <cstring>
#include <sys/wait.h>
#include <dirent.h>

#include "tokens.hpp"

class Cat {
public:
    int cat_command(string x);
    int cat_command(string x, string y);
    int execute(Tokens &obj);
    string getOutput() const; // Ensure this method is declared as const

private:
    string input;
    string output; // Add this member variable
    string myText;
};

class MakeDirectory{
    string output;
    public:
        int execute(Tokens& obj);
        int createDir(string);
        string getOutput();
};

class ChangeDir{
    public:
        int execute(string dir_path);
        string getOutput();
};

class Clear{
    public:
        Clear() {
        }
        void execute( Tokens& obj);
};

class WordCount{
    string output;
    string input;
    bool w_flag = false;
    bool c_flag = false;
    bool l_flag = false;

    public:
        int process_args(Tokens& obj);
        int execute( Tokens& obj);
        string getOutput() {
            return output;
        }
};

class Touch{
    string output;
    public:
        int execute( Tokens& obj);
        string& getOutput() {
            return output;
        }
};

class ListDir {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    std::string getTimeString(struct tm &file_time);
    void mode_str(mode_t m, std::string &str);
};

class PrintDir{
    string output;
    public:
        int execute( Tokens& obj);
        string& getOutput() {
            return output;
        }
};

class Move {
    public:
        int execute(Tokens& obj);
        int renameFile(const string& source, const string& destination) {
            return rename(source.c_str(), destination.c_str());
        }
        string getFileName(const string& path);
};

class Fork {
    public:
        int execute(Tokens& obj);
};

class Copy {
public:
    int execute(Tokens& tokens);
};

class Grep {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
};

class Chmod {
public:
    int execute(Tokens& tokens);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
};

class Remove {
public:
    int execute(Tokens& tokens);
    std::string getOutput() const;
private:
    std::string output; // Declare the output member variable
};

class Head {
public:
    int execute(Tokens& tokens);
    std::string getOutput() const;
private:
    std::string output;
};


class Tail {
public:
    int execute(Tokens& tokens);
    std::string getOutput() const;
private:
    std::string output;
};

class Diff {
public:
    int execute(Tokens& tokens);
    std::string getOutput() const;
private:
    std::string output;
};

class Reverse {
public:
    int execute(Tokens& obj);
private:
    void reverseFileContent(const string& filePath);
};

class Uppercase {
public:
    int execute(Tokens& obj);
private:
    void uppercaseFileContent(const string& filePath);
};

class Lowercase {
public:
    int execute(Tokens& obj);
private:
    void lowercaseFileContent(const string& filePath);
};

class FindReplace {
public:
    int execute(Tokens& obj);
private:
    void findAndReplaceInFile(const string& filePath, const string& findStr, const string& replaceStr);
};

class Encrypt {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    void encryptFileContent(const std::string& filePath, int shift);
    std::string caesarCipher(const std::string& text, int shift);
};

class Decrypt {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    void decryptFileContent(const std::string& filePath, int shift);
    std::string caesarCipher(const std::string& text, int shift);
};


class Compress {
public:
    int execute(Tokens& obj);
private:
    void compressFileContent(const string& filePath);
    string runLengthEncode(const string& text);
};

class Decompress {
public:
    int execute(Tokens& obj);
private:
    void decompressFileContent(const string& filePath);
    string runLengthDecode(const string& text);
};

class FileHistory {
public:
    FileHistory() {
        loadHistory();
    }
    ~FileHistory() {
        saveHistory();
    }
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    void trackChange(const std::string& filePath);
    void displayHistory(const std::string& filePath);
    void saveHistory();
    void loadHistory();
    std::map<std::string, std::vector<std::string>> history;
};
class FileSplit {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    void splitBySize(const std::string& filePath, size_t size);
    void splitByLines(const std::string& filePath, size_t lines);
};

class FileJoin {
public:
    int execute(Tokens& obj);
    std::string getOutput() const; // Ensure this method is declared as const

private:
    std::string output; // Add this member variable
    void joinFiles(const std::vector<std::string>& fileNames, const std::string& outputFilePath);
};