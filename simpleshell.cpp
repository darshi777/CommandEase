#include "simpleshell.hpp"
#include <map>
// std::map<std::string, std::string> commandMap = {
//     --{"list everything", "ls"},
//     {"print working directory", "pwd"},
//     {"make directory", "mkdir"},
//     {"change directory", "cd"},
//     {"clear screen", "clear"},
//     {"move file", "mv"},
//     --{"word count", "wc"},
//     {"concatenate", "cat"},
//     {"create file", "touch"},
//     {"exit shell", "exit"}
// };

int isSpace( string str) {
    int res = str.find_first_not_of(" \t\n\v\f\r");
    return res==-1;
}

void SimpleShell::printPrompt() {
    current_dir = filesystem::current_path();
    cout << "\e[1m" << current_dir << prompt << "\e[0m";
}
void SimpleShell::execute() {
    int flag = 1;
    char ch;
    int len, i;
    string cmd;
    len = history.size();
    i = len - 1;
    while(flag) {
        printPrompt();
        getline(cin, cmd);
        // cmd.append("\0");
        // cout << "cmd:"<<cmd<<"here"<<endl;
        flag = processCmd(cmd);
        history.push_back(cmd);
        len = history.size();
        i = len -1;
    }
}
int SimpleShell::processCmd(string cmd_chain) {
    string output;
    int res;
    if (cmd_chain.empty() || isSpace(cmd_chain)) {
        return 1;
    }
    if( cmd_chain.find('|') == -1 ) {
        res = executeCommand( cmd_chain, output, 1);
        if(res == -1 || res == 1)
            return 1;
        if(output != "")
            cout << output << endl;
    }
    else {
        int ind;
        int next = 0;
        string cmd;
        while ((ind = cmd_chain.find('|', next)) != std::string::npos) {
            cmd = cmd_chain.substr(next, ind - next);
            res = executeCommand(cmd, output);
            if( res == -1 ) {
                cout << output << endl;
                return -1;
            }
            input = output;
            next = ind + 1;
        }
        if( next < (cmd_chain.size() -1) ) {
            cmd = cmd_chain.substr(next, cmd_chain.size() - 1);
            res = executeCommand(cmd, output);
            if( res == -1 || res == 1) {
                cout << output << endl;
                return -1;
            }
        }
        if(output!="")
            cout << output << endl;
    }
    return 1;
}     
int SimpleShell::executeCommand(string cmd, string& out, int first) {
    out = "";
    int flag = 0;
    Tokens tokens(cmd);
    if (tokens[0] == "\0") {
        return 1;
    }

    if (!first && !input.empty()) {
        tokens.tokens.push_back(input);
    }

    if (tokens[0] == "concatenate") {
        if (tokens.num_args() < 1) return -1;
        Cat c1;
        int res = -1;
        if (tokens.num_args() == 1) {
            res = c1.cat_command(tokens[1]);
            out = c1.getOutput();
        } else if (tokens.num_args() == 2) {
            res = c1.cat_command(tokens[1], tokens[2]);
            out = c1.getOutput();
        } else {
            res = c1.execute(tokens);
            out = c1.getOutput();
        }
        if (!out.empty() && out.back() == '\n') {
            out.pop_back();
        }
        if (res == -1) return -1;
    } else if (tokens[0].rfind("./", 0) == 0) { // execute program
        Fork().execute(tokens);
    } else if (tokens[0] == "change" && tokens[1] == "directory") { // cd command
        if (tokens.num_args() < 2) { // Ensure there are at least 2 arguments
            std::cerr << "Error: Insufficient arguments for change directory command" << std::endl;
            return -1;
        }
        std::cout << "Changing directory to: " << tokens[2] << std::endl; // Debug print
        ChangeDir cd;
        int res = cd.execute(tokens[2]); // Use tokens[2] for the directory path
        if (res == -1) return 1;
        out = "";
    } else if (tokens[0] == "clear") { // clear command
        Clear clr;
        clr.execute(tokens);
    } else if (tokens[0] == "print" && tokens[1] == "directory") { // pwd command
        PrintDir pwd;
        int res = pwd.execute(tokens);
        if (res == -1) return 1;
        out = pwd.getOutput() + "\n";
    } else if (tokens[0] == "list_everything") { // ls command
        ListDir ls;
        if (tokens.num_args() == 0) {
            tokens.tokens.push_back(current_dir);
        }
        int res = ls.execute(tokens); // Ensure res is set
        std::cout << "Debug: ListDir execute result: " << res << std::endl; // Debug print
        if (res == -1) return 1;
        out = ls.getOutput();
        std::cout << "Debug: ListDir output: " << out << std::endl; // Debug print
    } 
    else if (tokens[0] == "word_count") { // wc command
        WordCount wc;
        int res;
        if (tokens.num_args() < 1) {
            res = -1;
            cout << "error: Insufficient Arguments" << endl;
        } else {
            try {
                wc.execute(tokens);
            } catch (const exception& e) {
                cout << "error: " << e.what();
                cout << endl;
                return -1;
            }
            if (res == -1) return 1;
            out = wc.getOutput();
        }
    } else if (tokens[0] == "make_directory") { // mkdir command
        MakeDirectory mdir;
        int res;
        if (tokens.num_args() < 1) {
            res = -1;
            cout << "error: Insufficient Arguments" << endl;
        } else {
            try {
                mdir.execute(tokens);
            } catch (const exception& e) {
                cout << "error: " << e.what();
                cout << endl;
                return -1;
            }
            if (res == -1) return 1;
            out = mdir.getOutput();
        }
    } else if (tokens[0] == "move_file") { // mv command
        Move mv;
        int res;
        if (tokens.num_args() < 2) {
            res = -1;
            cout << "error: Insufficient Arguments" << endl;
        } else {
            try {
                mv.execute(tokens);
            } catch (const exception& e) {
                cout << "error: " << e.what();
                cout << endl;
                return -1;
            }
            if (res == -1) return 1;
        }
    } else if (tokens[0] == "create_file") { // touch command
        class Touch A;
        A.execute(tokens);
    } else if (tokens[0] == "copy") { // cp command
        Copy cp;
        int res = cp.execute(tokens);
        if (res == -1) return 1;
    } else if (tokens[0] == "match_pattern") { // grep command
        Grep grep;
        int res = grep.execute(tokens);
        if (res == -1) return 1;
        out = grep.getOutput();
    } else if (tokens[0] == "change_mode") { // chmod command
        Chmod chmod;
        int res = chmod.execute(tokens);
        if (res == -1) return 1;
        out = chmod.getOutput(); // Retrieve and set the output
    } else if (tokens[0] == "remove") {
        Remove rm;
        int status = rm.execute(tokens);
        out = rm.getOutput();
    } else if (tokens[0] == "print_head") {
        Head head;
        int status = head.execute(tokens);
        out = head.getOutput();
    } else if (tokens[0] == "print_tail") {
        Tail tail;
        int status = tail.execute(tokens);
        out = tail.getOutput();
    } else if (tokens[0] == "difference") {
        Diff diff;
        int status = diff.execute(tokens);
        out = diff.getOutput();
        std::cout << "Debug: Diff command output: " << out << std::endl;
    } else if (tokens[0] == "reverse") { // reverse command
        Reverse rev;
        int res = rev.execute(tokens);
        if (res == -1) return 1;
        out = "File content reversed successfully.";
    } else if (tokens[0] == "uppercase") { // uppercase command
        Uppercase up;
        int res = up.execute(tokens);
        if (res == -1) return 1;
        out = "File content converted to uppercase successfully.";
    } else if (tokens[0] == "lowercase") { // lowercase command
        Lowercase low;
        int res = low.execute(tokens);
        if (res == -1) return 1;
        out = "File content converted to lowercase successfully.";
    } else if (tokens[0] == "findreplace") { // find and replace command
        FindReplace fr;
        int res = fr.execute(tokens);
        if (res == -1) return 1;
        out = "Find and replace operation completed successfully.";
    } 
    else if (tokens[0] == "encrypt") { // encrypt command
        Encrypt enc;
        int res = enc.execute(tokens);
        if (res == -1) return 1;
        out = enc.getOutput(); // Retrieve and set the output
    } else if (tokens[0] == "decrypt") { // decrypt command
        Decrypt dec;
        int res = dec.execute(tokens);
        if (res == -1) return 1;
        out = dec.getOutput(); // Retrieve and set the output
    } 
    else if (tokens[0] == "compress") { // compress command
        Compress comp;
        int res = comp.execute(tokens);
        if (res == -1) return 1;
        out = "File content compressed successfully.";
    } 
    else if (tokens[0] == "decompress") { // decompress command
        Decompress decomp;
        int res = decomp.execute(tokens);
        if (res == -1) return 1;
        out = "File content decompressed successfully.";
    } 
    else if (tokens[0] == "filehistory") { // file history command
        static FileHistory fh; // Ensure the history is loaded and saved properly
        int res = fh.execute(tokens);
        std::cout << "Debug: FileHistory execute result: " << res << std::endl; // Debug print
        if (res == -1) {
            out = fh.getOutput(); // Retrieve and set the output even on error
            std::cout << "Debug: FileHistory error output: " << out << std::endl; // Debug print
            return 1;
        }
        out = fh.getOutput(); // Retrieve and set the output
        std::cout << "Debug: FileHistory output: " << out << std::endl; // Debug print
    }
    
     else if (tokens[0] == "filesplit") { // file split command
        FileSplit fs;
        int res = fs.execute(tokens);
        std::cout << "Debug: FileSplit execute result: " << res << std::endl; // Debug print
        if (res == -1) {
            out = fs.getOutput(); // Retrieve and set the output even on error
            std::cout << "Debug: FileSplit error output: " << out << std::endl; // Debug print
            return 1;
        }
        out = fs.getOutput(); // Retrieve and set the output
        std::cout << "Debug: FileSplit output: " << out << std::endl; // Debug print
    } 
    else if (tokens[0] == "filejoin") { // file join command
        FileJoin fj;
        int res = fj.execute(tokens);
        std::cout << "Debug: FileJoin execute result: " << res << std::endl; // Debug print
        if (res == -1) {
            out = fj.getOutput(); // Retrieve and set the output even on error
            std::cout << "Debug: FileJoin error output: " << out << std::endl; // Debug print
            return 1;
        }
        out = fj.getOutput(); // Retrieve and set the output
        std::cout << "Debug: FileJoin output: " << out << std::endl; // Debug print
    } 
    else if (tokens[0] == "exit") { // exit command
        exit(0);
    } else {
        // sys call here
        try {
            if (!first) {
                ofstream outFile("tempfile");
                outFile.close();
                cmd = cmd + " tempfile";
                system(cmd.c_str());
                system("rm tempfile");
            } else {
                // Use popen to capture the output of the command
                FILE* pipe = popen(cmd.c_str(), "r");
                if (!pipe) {
                    out = "Failed to execute command.";
                    return -1;
                }

                char buffer[128];
                std::ostringstream ss;
                while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                    ss << buffer;
                }
                pclose(pipe);

                out = ss.str();
            }
        } catch (const std::exception& e) {
            cout << "error: " << e.what();
            cout << endl;
            return -1;
        }
        return 0;
    }
    return 0;
}