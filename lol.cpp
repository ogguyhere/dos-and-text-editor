#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <stack>
#include <fstream>
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
    Node(const string &name, bool is_dir) : name(name), is_dir(is_dir) {}
};

class Shell
{
private:
    Node *root;
    Node *current_dir;
    queue<string> print_queue;
    priority_queue<pair<int, string>> print_priority_queue;

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

    void check_file_exist(const string &f_name)
    {
        ifstream file(f_name);
        if (file)
        {
            cout << "File " << f_name << " exists. \n";
        }
        else
        {
            cout << "File " << f_name << " doesnt exist. \n";
        }
        file.close();
    }

    void create_file(const string &f_name, string &content)
    {
        for (Node *child : current_dir->children)
        {
            if (!child->is_dir && child->name == f_name)
            {
                cout << "File with this name already exists. \n";
                return;
            }
        }

        Node *newFile = new Node(f_name, false, "Author", current_dir);
        current_dir->children.push_back(newFile);

        ofstream file(f_name);
        if (!file)
        {
            std::cerr << "Error: couldnt create the file. \n";
            return;
        }
        file << content;
        file.close();
        cout << "File " << f_name << " has been created successfully with the provided content. :) \n";
    }

    Node *find_node(const string &name)
    {
        for (Node *child : current_dir->children)
        {
            if (child->name == name)
            {
                return child;
            }
        }
        return nullptr;
    }

    void attri_file(Node *node)
    {
        if (!node)
        {
            cout << "File not Found. \n";
            return;
        }
        cout << "Name        : " << node->name << endl;
        cout << "Type        : " << (node->is_dir ? "Directory" : "File") << endl;
        cout << "Owner       : " << node->owner << endl;

        char creationTime[20];
        strftime(creationTime, sizeof(creationTime), "%Y-%m-%d %H:%M:%S", localtime(&node->creation_t));
        cout << "Created On : " << creationTime << "\n";
        if (node->parent)
        {
            cout << "Parent      : " << node->parent->name << endl;
        }
        else
        {
            cout << "Parent Dir: None (Root) \n";
        }
    }

    void convert_extension(const string &from_ext, const string &to_ext)
    {
        if (from_ext.empty() || to_ext.empty())
        {
            cout << "Usage : CONVERT <from_ext> <to_ext> \n";
            return;
        }

        bool found = false;
        for (Node *child : current_dir->children)
        {
            if (!child->is_dir)
            {
                size_t extension_pos = child->name.find_last_of('.');
                if (extension_pos != string::npos && child->name.substr(extension_pos + 1) == from_ext)
                {
                    child->name = child->name.substr(0, extension_pos + 1) + to_ext;
                    found = true;
                    cout << "Converted: " << child->name << endl;
                }
            }
        }
        if (!found)
        {
            cout << "No File with extension ." << from_ext << "found. \n";
        }
    }

    void copy_file(const string &file_name, const string &target_dir)
    {
        Node *source_file = find_node(file_name);
        if (!source_file || source_file->is_dir)
        {
            cout << "File not found or its a directory: " << file_name << endl;
            return;
        }
        Node *target = nullptr;
        if (target_dir == "..")
        {
            target = current_dir->parent;
        }
        else if (target_dir == "\\")
        {
            target = root;
        }
        else
        {
            for (Node *child : current_dir->children)
            {
                if (child->is_dir && child->name == target_dir)
                {
                    target = child;
                    break;
                }
            }
        }

        if (!target)
        {
            cout << "Target Directory not found: " << target_dir << endl;
        }

        Node *copy = new Node(source_file->name, false, source_file->owner, target);
        target->children.push_back(copy);
        cout << "File " << file_name << " copied to " << target->name << endl;
    }

    void delete_file(const string &file_name)
    {
        if (file_name.empty())
        {
            cout << "Usage: DEL <file_name> \n";
            return;
        }
        for (auto it = current_dir->children.begin(); it != current_dir->children.end(); it++)
        {
            Node *child = *it;
            if (!child->is_dir && child->name == file_name)
            {
                current_dir->children.erase(it);
                delete child;
                cout << "File " << file_name << " deleted Successfully. \n";
                return;
            }
        }
        cout << "File not found: " << file_name << endl;
    }

    void find_file(const string &file_name)
    {
        if (file_name.empty())
        {
            cout << "Usage: FIND <file_name> \n";
            return;
        }

        for (Node *child : current_dir->children)
        {
            if (!child->is_dir && child->name == file_name)
            {
                cout << "File Found: " << file_name << endl;
                cout << "Owner: " << child->owner << endl;
                cout << "Created on: " << ctime(&child->creation_t);
                return;
            }
        }

        cout << "File not found: " << file_name << endl;
    }

    void findf(const string &file_name, const string &txt)
    {
        if (file_name.empty() || txt.empty())
        {
            cout << "Usage: FINDF <file_name> <text> \n";
            return;
        }

        for (Node *child : current_dir->children)
        {
            if (!child->is_dir && child->name == file_name)
            {
                ifstream file(file_name);
                if (!file)
                {
                    cout << "Error opening file: " << file_name << "\n";
                    return;
                }
                string line;
                bool found = false;
                while (getline(file, line))
                {
                    if (line.find(txt) != string::npos)
                    {
                        cout << "Text found in file " << file_name << ": " << line << endl;
                        found = true;
                    }
                }
                if (!found)
                {
                    cout << "Text not found in file: " << file_name << endl;
                }
                file.close();
                return;
            }
        }
        cout << "File not found: " << file_name << endl;
    }

    void findstr(const string &text)
    {
        if (text.empty())
        {
            cout << "Usage : FINDSTR <text> \n";
            return;
        }

        bool found = false;
        for (Node *child : current_dir->children)
        {
            if (!child->is_dir)
            {
                ifstream file(child->name);
                if (!file)
                {
                    cout << "Error opening fil: " << child->name << endl;
                    continue;
                }
                string line;
                while ((getline(file, line)))
                {
                    if (line.find(text) != string ::npos)
                    {
                        cout << "Text found is file: " << child->name << endl;
                        found = true;
                        break;
                    }
                }
                file.close();
            }
        }
        if (!found)
        {
            cout << "No files in the current Directory contain the text" << text << endl;
        }
    }

    void format_dir(Node *dir)
    {
        for (Node *child : dir->children)
        {
            if (child->is_dir)
            {
                format_dir(child);
            }
            delete child;
        }
        dir->children.clear();
        cout << "Directory " << dir->name << " has been formatted successfully. \n";
    }

    void version(const string &name)
    {
        Node *target = find_node(name);
        for (Node *child : current_dir->children)
        {
            if (child->name == name)
            {
                char creationTime[20];
                strftime(creationTime, sizeof(creationTime), "%Y-%m-%d %H:%M:%S", localtime(&child->creation_t));
                cout << "Created On : " << creationTime << "\n";
                return;
            }
        }
        cout << "File not found: " << name << endl;
    }

    void load_tree(const string &file_name)
    {
        ifstream file(file_name);
        if (!file)
        {
            cout << "Error: cannot open file " << file_name << endl;
            return;
        }

        string line;
        stack<Node *> dir_stack;
        dir_stack.push(root);

        while (getline(file, line))
        {
            size_t level = line.find_first_not_of('\t');
            string name = line.substr(level);
            bool is_directory = (name.back() == '/');
            Node *new_node = new Node(name, is_directory);
            while (dir_stack.size() > level + 1)
                dir_stack.pop();

            if (is_directory)
                dir_stack.push(new_node);
        }

        cout << "Tree successfully loaded from " << file_name << endl;
    }

    void move_File(const string &file_name, Node *source_dir, Node *dest_dir)
    {
        Node *file_node = nullptr;
        for (auto it = source_dir->children.begin(); it != source_dir->children.end(); it++)
        {
            if (!(*it)->is_dir && (*it)->name == file_name)

            {
                file_node = *it;
                source_dir->children.erase(it);
                break;
            }
        }
        if (file_node)
        {
            dest_dir->children.push_back(file_node);
            cout << "Moved file " << file_name << " to " << dest_dir->name << endl;
        }
        else
        {
            cout << "File " << file_name << " not found in source directory. \n";
        }
    }

    void pprint(const string &file_name, int priority)
    {
        print_priority_queue.push({priority, file_name});
        cout << "Added " << file_name << " to priority print queue with priority " << priority << "\n";

        cout << "Current Priority Print Queue:\n";
        auto queue_copy = print_priority_queue;
        while (!queue_copy.empty())
        {
            auto item = queue_copy.top();
            queue_copy.pop();
            cout << "Priority: " << item.first << ", File: " << item.second << "\n";
        }
    }

    void change_promt(const string &new_prompt)
    {
        root->name = new_prompt;
        cout << "Prompt changed to: " << new_prompt << "\n";
    }

    void print_file(const string &file_name)
    {
        print_queue.push(file_name);
        cout << "Added " << file_name << " to the print queue. \n";
        cout << "Current Print Queue: \n";
        auto queue_copy = print_queue;
        while (!queue_copy.empty())
        {
            cout << "File: " << queue_copy.front() << endl;
            queue_copy.pop();
        }
    }

    void show_priority_queue()
    {
        cout << "Priotity Print Queue: \n";
        auto queue_copy = print_priority_queue;
        while (!queue_copy.empty())
        {
            auto item = queue_copy.top();
            queue_copy.pop();
            cout << "Priority: " << item.first << " , File: " << item.second << endl;
        }
    }

    void show_queue()
    {
        cout << "Print Queue:\n";
        auto queue_copy = print_queue;
        while (!queue_copy.empty())
        {
            cout << "File: " << queue_copy.front() << "\n";
            queue_copy.pop();
        }
    }

    void rename_file(const string &old, const string &new_)
    {
        for (Node *child : current_dir->children)
        {
            if (!child->is_dir && child->name == old)
            {
                child->name = new_;
                cout << "Renamed File " << old << " To " << new_ << endl;
                return;
            }
        }
        cout << "File " << old << "not Found. \n";
    }

    void rmdir(Node *dir)
    {
        for (Node *child : dir->children)
        {
            if (child->is_dir)
                rmdir(child);
            delete child;
        }
        dir->children.clear();
    }

    void remove_dir(const string &dir_name)
    {
        for (auto it = current_dir->children.begin(); it != current_dir->children.end(); it++)
        {
            if ((*it)->is_dir && (*it)->name == dir_name)
            {
                rmdir(*it);
                delete *it;
                current_dir->children.erase(it);
                cout << "Directory " << dir_name << " removed. \n";
                return;
            }
        }
        cout << "Directory " << dir_name << " not found. \n";
    }

    void save_file(const string &file_name)
    {
        for (Node *child : current_dir->children)
        {
            if (!child->is_dir && child->name == file_name)
            {
                ofstream out_file(file_name);
                if (out_file)
                {
                    cout << "Enter content to save in " << file_name << " (end input with an empty line):\n";
                    string line, content;
                    while (getline(cin, line) && !line.empty())
                    {
                        content += line + "\n";
                    }
                    out_file << content;
                    out_file.close();
                    cout << "File " << file_name << " has been saved.\n";
                }
                else
                {
                    cout << "Error: Cannot save file " << file_name << "\n";
                }
                return;
            }
        }
        cout << "Error: File " << file_name << " not found in the current directory.\n";
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
            else if (cmd == "CREATE" || cmd == "create")
            {
                if (arg.empty())
                {
                    cout << "Usage: CREATE <filename> <file_Content> \n";
                }
                else
                {
                    size_t spacePos = arg.find(' ');
                    if (spacePos == string::npos)
                    {
                        cout << "Please provide both file name and content. \n";
                    }
                    else
                    {
                        string filename = arg.substr(0, spacePos);
                        string content = arg.substr(spacePos + 1);
                        create_file(filename, content);
                    }
                }
            }
            else if (cmd == "ATTRIB" || cmd == "attrib")
            {
                if (arg.empty())
                {
                    cout << "Usage: ATTRIB <name> \n";
                }
                else
                {
                    Node *node = find_node(arg);
                    attri_file(node);
                }
            }
            else if (cmd == "CONVERT" || cmd == "convert")
            {
                size_t space_pos = arg.find(' ');
                if (space_pos == string::npos)
                {
                    cout << "Usage:: CONVERT <from_ext> <to_ext> \n";
                }
                else
                {
                    string from_ext = arg.substr(0, space_pos);
                    string to_ext = arg.substr(space_pos + 1);
                    convert_extension(from_ext, to_ext);
                }
            }
            else if (cmd == "COPY" || cmd == "copy")
            {
                size_t space_pos = arg.find(' ');
                if (space_pos == string::npos)
                {
                    cout << "Usage:: CONVERT <from_ext> <to_ext> \n";
                }
                else
                {
                    string file_name = arg.substr(0, space_pos);
                    string target = arg.substr(space_pos + 1);
                    copy_file(file_name, target);
                }
            }
            else if (cmd == "DEL" || cmd == "del")
            {
                delete_file(arg);
            }
            else if (cmd == "FIND" || cmd == "find")
            {
                find_file(arg);
            }
            else if (cmd == "FINDF" || cmd == "findf")
            {
                size_t pos = arg.find(' ');
                string file_name = arg.substr(0, pos);
                string text = arg.substr(pos + 1);
                findf(file_name, text);
            }
            else if (cmd == "FINDSTR" || cmd == "findstr")
            {
                findstr(arg);
            }
            else if (cmd == "FORMAT" || cmd == "format")
            {
                format_dir(current_dir);
            }
            else if (cmd == "ver" || cmd == "VER")
            {
                version(arg);
            }
            else if (cmd == "LOADTREE" || cmd == "loadtree")
            {
                if (!arg.empty())
                    load_tree(arg);
                else
                    cout << "Usage: loadtree <filename>\n";
            }
            else if (cmd == "SAVE" || cmd == "save")
            {
                if (!arg.empty())
                    save_file(arg);
                else
                    cout << "Usage: save <filename>\n";
            }
            else if (cmd == "MOVE" || cmd == "move")
            {
                size_t space_pos = arg.find(' ');
                string file_name = arg.substr(0, space_pos);
                string dest_dir_name = arg.substr(space_pos + 1);

                if (!file_name.empty() && !dest_dir_name.empty())
                {
                    Node *dest_dir = find_node(dest_dir_name);
                    if (dest_dir)
                        move_File(file_name, current_dir, dest_dir);
                    else
                        cout << "Error: Destination directory not found.\n";
                }
                else
                    cout << "Usage: move <filename> <destination_directory>\n";
            }
            else if (cmd == "PPRINT" || cmd == "pprint")
            {
                size_t space_pos = arg.find(' ');
                string file_name = arg.substr(0, space_pos);
                int priority = stoi(arg.substr(space_pos + 1));

                if (!file_name.empty() && priority)
                    pprint(file_name, priority);
                else
                    cout << "Usage: pprint <filename> <priority>\n";
            }
            else if (cmd == "PRINT" || cmd == "print")
            {
                if (!arg.empty())
                    print_file(arg);
                else
                    cout << "Usage: print <filename>\n";
            }
            else if (cmd == "PQ" || cmd == "pq")
            {
                show_priority_queue();
            }
            else if (cmd == "QUEUE" || cmd == "queue")
            {
                show_queue();
            }
            else if (cmd == "RENAME" || cmd == "rename")
            {
                size_t space_pos = arg.find(' ');
                string old_name = arg.substr(0, space_pos);
                string new_name = arg.substr(space_pos + 1);

                if (!old_name.empty() && !new_name.empty())
                    rename_file(old_name, new_name);
                else
                    cout << "Usage: rename <old_name> <new_name>\n";
            }
            else if (cmd == "RMDIR" || cmd == "rmdir")
            {
                if (!arg.empty())
                    remove_dir(arg);
                else
                    cout << "Usage: rmdir <directory_name>\n";
            }
            else if (cmd == "PROMPT" || cmd == "prompt")
            {
                if (!arg.empty())
                    change_promt(arg);
                else
                    cout << "Usage: prompt <new_prompt>\n";
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
