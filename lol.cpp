#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <stack>
using namespace std;

class Node
{
public:
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

class Shell
{
private:
    Node *root;
    Node *current_dir;

public:
    Shell()
    {
        initialize_shell();
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

    void show_help()
    {
        cout << "Available Commands: \n";
        cout << "  HELP     : Display this help message.\n";
        cout << "  EXIT     : Exit the shell.\n";
        cout << "  MKDIR    : Create a new directory. Usage: MKDIR <dirName>\n";
        cout << "  CD       : Change directory. Usage: CD <dirName>\n";
        cout << "  PWD      : Display the current working directory.\n";
        cout << "  DIR      : List contents of the current directory.\n";
        cout << "  TREE     : Display the entire directory structure.\n";
    }

    void create_dir(const string &dir_n)
    {
        if (dir_n.empty())
        {
            cout << "Directory name cannot be empty. \n";
            return;
        }

        for (Node *child : current_dir->children)
        {
            if (child->is_dir && child->name == dir_n)
            {
                cout << "Directory with this name already exists. \n";
                return;
            }
        }

        Node *newDir = new Node(dir_n, true, "Author", current_dir);
        current_dir->children.push_back(newDir);
        cout << "Directory " << dir_n << " has been created successfully. :) \n";
    }

    void change_dir(const string &dir_n)
    {
        if (dir_n == "..")
        {
            if (current_dir->parent)
            {
                current_dir = current_dir->parent;
                cout << "Moved to parent directory. \n";
            }
            else
            {
                cout << "Already at the root directory. \n";
            }
            return;
        }
        else if (dir_n == "\\")
        {
            current_dir = root;
            cout << "Moved to root directory. \n";
            return;
        }
        for (Node *child : current_dir->children)
        {
            if (child->is_dir && child->name == dir_n)
            {
                current_dir = child;
                cout << "Moved to directory " << dir_n << ".\n";
                return;
            }
        }

        cout << "Directory not found: " << dir_n << ".\n";
    }

    void print_pwd(Node *dir)
    {
        if (dir == root)
        {
            cout << dir->name << "\n";
            return;
        }
        stack<string> pathStack;
        while (dir != nullptr)
        {
            pathStack.push(dir->name);
            dir = dir->parent;
        }
        while (!pathStack.empty())
        {
            cout << pathStack.top();
            pathStack.pop();
            if (!pathStack.empty())
                cout << "\\";
        }
        cout << "\n";
    }

    void display_tree(Node *dir, int level = 0)
    {
        for (int i = 0; i < level; i++)
        {
            cout << " ";
        }
        cout << "|-- " << dir->name;
        if (dir->is_dir)
        {
            cout << " (DIR)";
        }
        else
        {
            cout << " (FILE)";
        }
        cout << endl;
        for (Node *child : dir->children)
        {
            display_tree(child, level + 1);
        }
    }

    void tree_command()
    {
        cout << "Directory structure starting from " << current_dir->name << ":\n";
        display_tree(current_dir);
    }

    void command_loop()
    {
        string command;
        while (true)
        {
            cout << current_dir->name << "> ";
            getline(cin, command);

            string cmd = command.substr(0, command.find(' '));
            string arg = command.substr(command.find(' ') + 1);

            if (cmd == "HELP" || cmd == "help")
            {
                show_help();
            }
            else if (cmd == "EXIT" || cmd == "exit")
            {
                cout << "Exiting shell! \n";
                break;
            }
            else if (cmd == "MKDIR" || cmd == "mkdir")
            {
                create_dir(arg);
            }
            else if (cmd == "CD" || cmd == "cd")
            {
                change_dir(arg);
            }
            else if (cmd == "PWD" || cmd == "pwd")
            {
                print_pwd(current_dir);
            }
            else if (cmd == "LS" || cmd == "ls")
            {
                tree_command();
            }
            else
            {
                cout << "Invalid Command. Type HELP for the list of commands. \n";
            }
        }
    }
};

int main()
{
    Shell shell;
    shell.command_loop();
    return 0;
}
