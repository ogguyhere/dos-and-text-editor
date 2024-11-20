#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <stack>

using namespace std;

struct Node
{
    string name;
    bool is_dir;
    string owner;
    time_t creation_t;
    vector<Node *> children;
    Node *parent;

    Node(string n, bool i, string o, Node *p = nullptr)
        : name(n), is_dir(i), owner(o), parent(p)
    {
        creation_t = time(nullptr);
    }
};

Node * root = nullptr;
Node * current_dir = nullptr;

//My Functions:
void initialize_shell();
void show_help();
void command_loop();
void create_dir(string dir_n);
void change_dir(string dir_n);
void print_pwd(Node * dir);

int main()
{
    initialize_shell();
    command_loop();
    return 0;
}

void initialize_shell()
{
    cout << "Author: deejz \n";
    cout << "Registration: bscs23128 \n";
    cout << "Course: DSA \n";
    cout << "-------------------------------- \n";

    root = new Node("V:\\", true, "Author");
    current_dir = root;

}

void command_loop()
{
    string command;
    while (true)
    {
        cout << current_dir->name << "> ";
        getline(cin, command);

        string cmd = command.substr(0, command.find(' '));
        string ard = command.substr(command.find(' ') + 1);

        if(cmd == "HELP" || cmd == "help")
        {
            show_help();
        }
        else if(cmd == "EXIT" || cmd == "exit")
        {
            cout << "Exiting deejz shell! \n";
            break;
        }
        else if (cmd == "MKDIR" || cmd == "mkdir")
        {
            

        }
    }
    
}