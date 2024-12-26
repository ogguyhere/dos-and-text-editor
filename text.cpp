#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <sys/stat.h> // For checking file permissions
#include <fcntl.h>    // For open flags (O_RDWR)
#include <unistd.h>   // For access() function
#include <ncurses.h>
using namespace std;

struct Letter
{
    char ch;
    Letter *next;

    Letter(char c) : ch(c), next(nullptr) {}
};

class Line
{
public:
    list<Letter *> letters;

    void insertCharAt(int pos, char c)
    {
        auto it = letters.begin();
        advance(it, pos);
        letters.insert(it, new Letter(c));
    }

    void removeCharAt(int pos)
    {
        auto it = letters.begin();
        advance(it, pos);
        delete *it;
        letters.erase(it);
    }

    void printLine()
    {
        for (auto letter : letters)
        {
            if (letter != nullptr)
            {
                addch(letter->ch); // Use ncurses addch
            }
        }
    }

    string getContent()
    {
        string content;
        for (auto letter : letters)
        {
            content += letter->ch;
        }
        return content;
    }

    int length()
    {
        return letters.size();
    }

    ~Line()
    {
        for (auto letter : letters)
        {
            delete letter;
        }
    }
};

class Para
{
public:
    list<Line *> lines;

    void addLine(Line *line)
    {
        lines.push_back(line);
    }

    ~Para()
    {
        for (auto line : lines)
        {
            delete line;
        }
    }

    int lineCount()
    {
        return lines.size();
    }

    Line *getLine(int index)
    {
        auto it = lines.begin();
        advance(it, index);
        return *it;
    }
};

class Document
{
public:
    list<Para *> paragraphs;

    void addParagraph(Para *para)
    {
        paragraphs.push_back(para);
    }

    void printDocument()
    {
        // Print each paragraph
        int lineIndex = 0;
        for (auto para : paragraphs)
        {
            // Print each line in the paragraph
            for (auto line : para->lines)
            {
                // Print the content of the line using ncurses mvprintw
                mvprintw(lineIndex++, 0, line->getContent().c_str());
            }
        }
    }

    void saveToFile(const string &filename)
    {
        ofstream file(filename);
        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                for (auto letter : line->letters)
                {
                    file << letter->ch;
                }
                file << '\n';
            }
            file << '\n';
        }
        file.close();
    }

    ~Document()
    {
        for (auto para : paragraphs)
        {
            delete para;
        }
    }
};

class TextEditor
{
public:
    Document *currentDocument;
    int cursorRow;
    int cursorCol;

    TextEditor() : currentDocument(new Document()), cursorRow(0), cursorCol(0) {}

    void run()
    {
        initscr();
        keypad(stdscr, TRUE);
        noecho();

        Line *line = new Line();
        Para *para = new Para();
        para->addLine(line);
        currentDocument->addParagraph(para);

        int ch;
        while ((ch = getch()) != KEY_F(1)) // KEY_F(1) for F1 key to exit the editor
        {
            int numLines = para->lineCount();
            Line *currentLine = para->getLine(cursorRow);

            switch (ch)
            {
            case KEY_UP:
                if (cursorRow > 0)
                {
                    cursorRow--;
                    if (cursorCol > para->getLine(cursorRow)->length())
                    {
                        cursorCol = para->getLine(cursorRow)->length();
                    }
                }
                break;
            case KEY_DOWN:
                if (cursorRow < numLines - 1)
                {
                    cursorRow++;
                    if (cursorCol > para->getLine(cursorRow)->length())
                    {
                        cursorCol = para->getLine(cursorRow)->length();
                    }
                }
                break;
            case KEY_LEFT:
                if (cursorCol > 0)
                {
                    cursorCol--;
                }
                else if (cursorRow > 0)
                {
                    cursorRow--;
                    cursorCol = para->getLine(cursorRow)->length();
                }
                break;
            case KEY_RIGHT:
                if (cursorCol < currentLine->length())
                {
                    cursorCol++;
                }
                else if (cursorRow < numLines - 1)
                {
                    cursorRow++;
                    cursorCol = 0;
                }
                break;
            case 10: // Enter key
                line = new Line();
                para->addLine(line);
                cursorRow++;
                cursorCol = 0;
                break;
            case KEY_BACKSPACE: // Backspace key
            case 127:           // Backspace can also be represented by 127
                if (cursorCol > 0)
                {
                    currentLine->removeCharAt(cursorCol - 1); // Remove character before the cursor
                    cursorCol--;
                }
                else if (cursorRow > 0)
                {
                    Line *prevLine = para->getLine(cursorRow - 1);
                    int prevLength = prevLine->length();
                    for (auto letter : currentLine->letters)
                    {
                        prevLine->insertCharAt(prevLength++, letter->ch);
                    }
                    auto it = para->lines.begin();
                    advance(it, cursorRow);
                    para->lines.erase(it);

                    delete currentLine;
                    cursorRow--;
                    cursorCol = prevLength;
                }
                break;
            case 20: // CTRL+T to save (ASCII 20 for CTRL+T)
                saveDocument();
                break;
            case 15: // CTRL+O (ASCII 15)
                openFilePrompt();
                break;

            default:
                currentLine->insertCharAt(cursorCol, ch);
                cursorCol++;
                break;
            }

            clear();
            currentDocument->printDocument(); // Re-render the document after any action
            move(cursorRow, cursorCol);       // Move the cursor to the current position
        }

        endwin(); // End ncurses mode
    }

    void saveDocument()
    {
        char filename[100];
        clear();
        printw("Enter file path to save the document: ");
        echo();
        getstr(filename);
        noecho();
        currentDocument->saveToFile(filename);

        // Confirm save operation
        clear();
        printw("Document saved to %s\nPress any key to continue...", filename);
        getch();
    }

    void openFilePrompt()
    {
        clear();
        mvprintw(0, 0, "Enter file path to open: ");
        echo();
        char filepath[256];
        getnstr(filepath, 255);
        noecho();
        openFile(filepath);
    }

    void openFile(const string &filename)
    {
        // Check if the file exists and has read and write permissions
        struct stat fileStat;
        if (stat(filename.c_str(), &fileStat) != 0)
        {
            clear();
            mvprintw(0, 0, "Failed to access file: %s\nPress any key to return to editor...", filename.c_str());
            getch();
            return;
        }

        if (!(fileStat.st_mode & S_IRUSR) || !(fileStat.st_mode & S_IWUSR))
        {
            clear();
            mvprintw(0, 0, "File %s does not have read/write permissions.\nPress any key to return to editor...", filename.c_str());
            getch();
            return;
        }

        // Try opening the file in both read and write mode
        ifstream file(filename, ios::in | ios::out); // open for reading and writing
        if (!file.is_open())
        {
            clear();
            mvprintw(0, 0, "Failed to open file: %s\nPress any key to return to editor...", filename.c_str());
            getch();
            return;
        }

        // Clear the current document
        delete currentDocument;
        currentDocument = new Document();

        Para *para = new Para();
        currentDocument->addParagraph(para);

        string lineText;
        while (getline(file, lineText))
        {
            Line *line = new Line();
            for (char ch : lineText)
            {
                line->insertCharAt(line->length(), ch);
            }
            para->addLine(line);
        }

        file.close();

        // Set cursor to the end of the last line
        cursorRow = para->lineCount() - 1;              // Move to the last line
        cursorCol = para->getLine(cursorRow)->length(); // Move to the end of that line
    }

    ~TextEditor()
    {
        delete currentDocument;
    }
};

int main()
{
    TextEditor editor;
    editor.run();
    return 0;
}
