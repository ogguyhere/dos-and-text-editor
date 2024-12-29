#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <algorithm> // for transform
#include <sstream>
#include <limits.h>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include <unordered_map>
#include <set>

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
                addch(letter->ch);
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
        int lineIndex = 0;
        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                mvprintw(lineIndex++, 0, line->getContent().c_str());
            }
        }
    }
    int paraCount() const
    {
        return paragraphs.size();
    }
    Para *getPara(int index)
    {
        if (index < 0 || index >= paragraphs.size())
        {
            return nullptr; // Check if index is out of bounds
        }

        // Use an iterator to access the element at the given index
        auto it = paragraphs.begin();
        std::advance(it, index); // Advance iterator to the desired index
        return *it;              // Dereference iterator to get the element
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

    Line *getLine(int index)
    {
        int lineCount = 0;
        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                if (lineCount == index)
                {
                    return line; // Found the line
                }
                lineCount++;
            }
        }
        return nullptr; // If no line is found
    }

    ~Document()
    {
        for (auto para : paragraphs)
        {
            delete para;
        }
    }

    void convertToUpperCase()
    {
        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                for (auto letter : line->letters)
                {
                    letter->ch = toupper(letter->ch);
                }
            }
        }
    }

    void convertToLowerCase()
    {
        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                for (auto letter : line->letters)
                {
                    letter->ch = tolower(letter->ch);
                }
            }
        }
    }

    bool findWord(const string &word, bool caseSensitive)
    {
        string wordLower = word;
        if (!caseSensitive)
        {
            transform(wordLower.begin(), wordLower.end(), wordLower.begin(), ::tolower);
        }

        for (auto para : paragraphs)
        {
            for (auto line : para->lines)
            {
                string lineContent = line->getContent();
                string lineContentLower = lineContent;
                if (!caseSensitive)
                {
                    transform(lineContentLower.begin(), lineContentLower.end(), lineContentLower.begin(), ::tolower);
                }

                if (lineContentLower.find(wordLower) != string::npos)
                {
                    return true; // Word found
                }
            }
        }
        return false; // Word not found
    }
};

class TextEditor
{
public:
    Document *currentDocument;
    int cursorRow;
    int cursorCol;

    TextEditor() : currentDocument(new Document()), cursorRow(0), cursorCol(0) {}
    std::string getUserInput(const std::string &prompt)
    {
        char ch;
        std::string input;

        clear();
        mvprintw(0, 0, prompt.c_str());
        refresh();

        // Capture input until user presses Enter (key 10)
        while ((ch = getch()) != 10)
        { // Enter key
            if (ch == KEY_BACKSPACE || ch == 127)
            { // Backspace key
                if (!input.empty())
                {
                    input.pop_back(); // Remove last character
                    mvprintw(1, 0, prompt.c_str());
                    mvprintw(1, prompt.length(), input.c_str());
                }
            }
            else if (ch >= 32 && ch <= 126)
            { // Printable characters
                input.push_back(ch);
                mvprintw(1, 0, prompt.c_str());
                mvprintw(1, prompt.length(), input.c_str());
            }
            refresh();
        }

        return input; // Return the captured input
    }

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
        while ((ch = getch()) != KEY_F(1))
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
            case 127:
                if (cursorCol > 0)
                {
                    currentLine->removeCharAt(cursorCol - 1);
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
            case 20: // CTRL+T to save
                saveDocument();
                break;
            case 15: // CTRL+O
                openFilePrompt();
                break;
            case 21: // CTRL+U for ToUpper()
                convertWordToUpperCase();
                break;
            case 12: // CTRL+L for ToLower()
                convertWordToLowerCase();
                break;
            case 6: // CTRL+F for Find Word
                findWordPrompt();
                break;

            case 7: // CTRL+G for finding sentence
                findSentencePromt();
                break;
            case 8:                    // CTRL+H for finding substring
                findSubstringPrompt(); // TODO: implement
                break;
            case 9: // CTRL + I for Add prefix to word
                AddPrefixToWord();
                break;
            case 11: // CTRL + K for add postfix to word
                AddPostfixToWord();
                break;
            case 14: // CTRL + N for substring count
                substringCount();
                break;
            case 16: // CTRL + P for special char count
                specialCharCount();
                break;
            case 18: // CTRL + R for para and sentence count
                countSentencesAndParagraphs();
                break;
            case 22: // CTRL + V for word count
                avgWordLength();
                break;

            case 23: // CTRL+W for replacing first word
                replaceFirstWordPrompt();
                break;
            case 1: // CTRL+A for replacing all words
                replaceAllWordPrompt();
                break;
            case 4: // CTRL + D
                findLargestWordLength();
                break;
            case 3: // CTRL + C
                findSmallestWordLength();
                break;
            case 5: // CTRL + E
                findWordsFromInput();
                break;
            case 24: // CTRL + X
                mergefileprompt();
                break;
            case KEY_F(3):
                countParagraphs();
                break;
            case KEY_F(2):
                findLargestParagraphWordLength();
                break;
            case KEY_F(4):
                readFileWithPasswordProtection();
                break;
            case KEY_F(6):
                encodeFilePrompt();
                break;

            default:
                currentLine->insertCharAt(cursorCol, ch);
                cursorCol++;
                break;
            }

            clear();
            currentDocument->printDocument();
            move(cursorRow, cursorCol);
        }

        endwin();
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

        ifstream file(filename, ios::in | ios::out);
        if (!file.is_open())
        {
            clear();
            mvprintw(0, 0, "Failed to open file: %s\nPress any key to return to editor...", filename.c_str());
            getch();
            return;
        }

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
        cursorRow = para->lineCount() - 1;
        cursorCol = para->getLine(cursorRow)->length();
    }

    void findWordPrompt()
    {
        clear();
        mvprintw(0, 0, "Enter word to find: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        clear();
        mvprintw(0, 0, "Case-sensitive? (y/n): ");
        echo();
        char caseChoice = getch();
        noecho();

        bool caseSensitive = (caseChoice == 'y' || caseChoice == 'Y');
        if (currentDocument->findWord(word, caseSensitive))
        {
            mvprintw(1, 0, "Word found!");
        }
        else
        {
            mvprintw(1, 0, "Word not found.");
        }
        getch();
    }

    string getWordUnderCursor(Line *line, int cursorCol)
    {
        int start = cursorCol;
        int end = cursorCol;

        // Find the start of the word
        while (start > 0 && isalnum(line->getContent()[start - 1]))
        {
            --start;
        }

        // Find the end of the word
        while (end < line->getContent().length() && isalnum(line->getContent()[end]))
        {
            ++end;
        }

        return line->getContent().substr(start, end - start);
    }
    // Function to convert the word under the cursor to uppercase
    void convertWordToUpperCase()
    {
        Line *currentLine = currentDocument->getLine(cursorRow);
        string word = getWordUnderCursor(currentLine, cursorCol);

        // Convert the word to uppercase
        transform(word.begin(), word.end(), word.begin(), ::toupper);

        // Replace the word in the line
        int start = cursorCol - word.length();
        for (int i = 0; i < word.length(); ++i)
        {
            currentLine->removeCharAt(start + i);
            currentLine->insertCharAt(start + i, word[i]);
        }
    }
    // Function to convert the word under the cursor to lowercase
    void convertWordToLowerCase()
    {
        Line *currentLine = currentDocument->getLine(cursorRow);
        string word = getWordUnderCursor(currentLine, cursorCol);

        // Convert the word to lowercase
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        // Replace the word in the line
        int start = cursorCol - word.length();
        for (int i = 0; i < word.length(); ++i)
        {
            currentLine->removeCharAt(start + i);
            currentLine->insertCharAt(start + i, word[i]);
        }
    }
    // Find Sentence
    bool findSentence(const string &sentence)
    {
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                if (line->getContent().find(sentence) != string::npos)
                {
                    return true; // Sentence found
                }
            }
        }
        return false; // Sentence not found
    }

    void findSentencePromt()
    {
        clear();
        mvprintw(0, 0, "Find sentence: ");
        echo();
        char word[1000];
        getstr(word);
        noecho();
        if (findSentence(word))
        {
            mvprintw(1, 0, "Sentence found!");
        }
        else
        {
            mvprintw(1, 0, "Sentence not found.");
        }
        getch();
    }
    // Find Substring
    bool findSubstring(const string &substring)
    {
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                if (line->getContent().find(substring) != string::npos)
                {
                    return true; // Substring found
                }
            }
        }
        return false; // Substring not found
    }

    void findSubstringPrompt()
    {
        clear();
        mvprintw(0, 0, "Find substring: ");
        echo();
        char word[1000];
        getstr(word);
        noecho();
        if (findSubstring(word))
        {
            mvprintw(1, 0, "Substring found!");
        }
        else
        {
            mvprintw(1, 0, "Substring not found.");
        }
        getch();
    }
    // replace first word
    void replaceFirstWord(const string &oldWord, const string &newWord)
    {
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                size_t pos = line->getContent().find(oldWord);
                if (pos != string::npos)
                {
                    int count = line->length();
                    string temp = line->getContent();
                    temp.replace(pos, oldWord.length(), newWord);

                    for (int i = count - 1; i >= 0; --i)
                    {
                        line->removeCharAt(i);
                    }
                    int k = 0;
                    for (int i = 0; i < temp.size(); i++)
                    {
                        char c = temp[k++];
                        line->insertCharAt(i, c);
                    }
                    return; // Stop after replacing the first occurrence
                }
            }
        }
    }
    // replace first word prompt
    void replaceFirstWordPrompt()
    {
        clear();
        mvprintw(0, 0, "Enter word to replace: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        mvprintw(2, 0, "Enter new word: ");
        echo();
        char word1[256];
        getnstr(word1, 255);
        noecho();

        replaceFirstWord(word, word1);
        mvprintw(4, 0, "Word replace!");

        getch();
    }
    // replace all word
    void replaceAllWords(const string &oldWord, const string &newWord)
    {
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();
                size_t pos = 0;
                while ((pos = content.find(oldWord, pos)) != string::npos)
                {
                    content.replace(pos, oldWord.length(), newWord);
                    pos += newWord.length();
                }

                for (int k = line->length() - 1; k >= 0; --k)
                {
                    line->removeCharAt(k);
                }

                for (size_t k = 0; k < content.size(); ++k)
                {
                    line->insertCharAt(k, content[k]);
                }
            }
        }
    }
    // replace all word prompt
    void replaceAllWordPrompt()
    {
        clear();
        mvprintw(0, 0, "Enter word to replace: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        mvprintw(2, 0, "Enter new word: ");
        echo();
        char word1[256];
        getnstr(word1, 255);
        noecho();

        replaceAllWords(word, word1);
        mvprintw(4, 0, "Word replace!");

        getch();
    }
    // Add Prefix to Word
    void AddPrefixToWord()
    {
        clear();
        mvprintw(0, 0, "Enter word to add Prefix: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        mvprintw(2, 0, "Enter Prefix: ");
        echo();
        char word1[256];
        getnstr(word1, 255);
        noecho();

        for (int i = 0; i < currentDocument->paraCount(); i++)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                size_t pos = line->getContent().find(word);
                if (pos != string::npos)
                {
                    int count = line->length();
                    string temp = line->getContent();
                    string temp1 = word1;
                    string oldWord = word;
                    temp1.append(word);
                    temp.replace(pos, oldWord.length(), temp1);
                    for (int i = count - 1; i >= 0; --i)
                    {
                        line->removeCharAt(i);
                    }
                    int k = 0;
                    for (int i = 0; i < temp.size(); i++)
                    {
                        char c = temp[k++];
                        line->insertCharAt(i, c);
                    }
                }
            }
        }
        mvprintw(4, 0, "Prefix Added!");
        getch();
    }
    // Add Postfix to Word
    void AddPostfixToWord()
    {
        clear();
        mvprintw(0, 0, "Enter word to add Postfix: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        mvprintw(2, 0, "Enter Postfix: ");
        echo();
        char word1[256];
        getnstr(word1, 255);
        noecho();

        for (int i = 0; i < currentDocument->paraCount(); i++)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                size_t pos = line->getContent().find(word);
                if (pos != string::npos)
                {
                    int count = line->length();
                    string temp = line->getContent();
                    string temp1 = word;
                    string oldWord = word;
                    temp1.append(word1);
                    temp.replace(pos, oldWord.length(), temp1);
                    for (int i = count - 1; i >= 0; --i)
                    {
                        line->removeCharAt(i);
                    }
                    int k = 0;
                    for (int i = 0; i < temp.size(); i++)
                    {
                        char c = temp[k++];
                        line->insertCharAt(i, c);
                    }
                }
            }
        }
        mvprintw(4, 0, "Postfix Added!");
        getch();
    }
    // Word Lenght
    void avgWordLength()
    {
        int totalLength = 0, wordCount = 0;

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();

                stringstream ss(content);
                string word;

                while (ss >> word)
                {
                    totalLength += word.length();
                    wordCount++;
                }
            }
        }
        clear();
        string resultMessage = "Word Count is: " + std::to_string(wordCount);

        // Display the message
        mvprintw(4, 0, resultMessage.c_str());
        getch();
    }
    // Substring Count
    void substringCount()
    {
        clear();
        mvprintw(0, 0, "Enter substring to find count: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();
        int count = 0;
        string substring = word;

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();

                size_t pos = 0;
                while ((pos = content.find(substring, pos)) != string::npos)
                {
                    count++;
                    pos += substring.length();
                }
            }
        }
        clear();
        string resultMessage = "Substring Count is: " + std::to_string(count);

        // Display the message
        mvprintw(4, 0, resultMessage.c_str());

        getch();
    }
    // Special Character Count
    void specialCharCount()
    {
        int count = 0;

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();

                for (char ch : content)
                {
                    if (!isalnum(ch) && !isspace(ch))
                    {
                        count++;
                    }
                }
            }
        }
        string resultMessage = "Special Character Count is: " + std::to_string(count);

        // Display the message
        mvprintw(4, 0, resultMessage.c_str());

        getch();
    }
    // Sentence Count
    void countSentencesAndParagraphs()
    {
        int sentenceCount = 0, paragraphCount = currentDocument->paraCount();

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();

                for (char ch : content)
                {
                    if (ch == '.' || ch == '!' || ch == '?')
                    {
                        sentenceCount++;
                    }
                }
            }
        }

        // return {sentenceCount, paragraphCount};
        string resultMessage = "Sentence Count is:" + std::to_string(sentenceCount);

        // Display the message
        mvprintw(4, 0, resultMessage.c_str());

        getch();
    }

    void processDocument(int &minLength, int &maxLength)
    {
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                std::string content = line->getContent();

                std::stringstream ss(content);
                std::string word;

                while (ss >> word)
                {
                    int length = static_cast<int>(word.length());
                    minLength = std::min(minLength, length);
                    maxLength = std::max(maxLength, length);
                }
            }
        }
    }
    // Smallest word lenght
    void findSmallestWordLength()
    {
        int minLength = INT_MAX;
        int maxLength = 0;

        processDocument(minLength, maxLength);

        std::string resultMessage;
        if (minLength == INT_MAX)
        {
            resultMessage = "No words found in the document.";
        }
        else
        {
            resultMessage = "Smallest Word Length is: " + std::to_string(minLength);
        }

        mvprintw(4, 0, resultMessage.c_str());
        getch();
    }
    // Largest word lenght
    void findLargestWordLength()
    {
        int minLength = INT_MAX;
        int maxLength = 0;

        processDocument(minLength, maxLength);

        std::string resultMessage;
        if (maxLength == 0)
        {
            resultMessage = "No words found in the document.";
        }
        else
        {
            resultMessage = "Largest Word Length is: " + std::to_string(maxLength);
        }

        mvprintw(5, 0, resultMessage.c_str());
        getch(); // Ensure the message is displayed
    }

    void mergeIntoFirstFile(const string &file1, const string &file2)
    {
        ifstream infile2(file2);
        ofstream outfile1(file1, ios::app);

        if (infile2.is_open() && outfile1.is_open())
        {
            string line;
            while (getline(infile2, line))
            {
                outfile1 << line << endl;
            }
            infile2.close();
            outfile1.close();
            cout << "Files merged into: " << file1 << endl;
        }
        else
        {
            cerr << "Error opening files!" << endl;
        }
    }

    void mergeIntoNewFile(const string &file1, const string &file2, const string &newFile)
    {
        ifstream infile1(file1), infile2(file2);
        ofstream outfile(newFile);

        if (infile1.is_open() && infile2.is_open() && outfile.is_open())
        {
            string line;
            while (getline(infile1, line))
            {
                outfile << line << endl;
            }
            while (getline(infile2, line))
            {
                outfile << line << endl;
            }
            infile1.close();
            infile2.close();
            outfile.close();
            cout << "Files merged into: " << newFile << endl;
        }
        else
        {
            cerr << "Error opening files!" << endl;
        }
    }
    // merge file prompt
    void mergefileprompt()
    {
        string file1, file2, newFile;
        clear();
        mvprintw(0, 0, "Enter file path to open: ");
        echo();
        char filepath[256];
        getnstr(filepath, 255);
        noecho();
        file1 = filepath;

        mvprintw(1, 0, "Enter file path to open: ");
        echo();
        char filepath2[256];
        getnstr(filepath2, 255);
        noecho();
        file2 = filepath2;

        mvprintw(2, 0, "Enter file path to open: ");
        echo();
        char filepath3[256];
        getnstr(filepath3, 255);
        noecho();
        newFile = filepath3;

        mergeIntoNewFile(file1, file2, newFile);
    }
    // Word Game
    void findWordsFromInput()
    {
        clear();
        mvprintw(0, 0, "Enter word to replace: ");
        echo();
        char word[256];
        getnstr(word, 255);
        noecho();

        string inputWord = word;

        unordered_set<string> allWords;

        // Collect all words from the document
        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                string content = line->getContent();

                stringstream ss(content);
                string word;

                while (ss >> word)
                {
                    allWords.insert(word);
                }
            }
        }

        // Prepare the input word's character frequency
        multiset<char> inputChars(inputWord.begin(), inputWord.end());

        // Find smaller words
        unordered_set<string> smallerWords;
        for (const auto &word : allWords)
        {
            multiset<char> wordChars(word.begin(), word.end());
            bool canForm = true;

            for (char c : word)
            {
                if (inputChars.count(c) < wordChars.count(c))
                {
                    canForm = false;
                    break;
                }
            }

            if (canForm)
            {
                smallerWords.insert(word);
            }
        }

        mvprintw(4, 0, "Words that can be formed:");
        int row = 5;

        for (const auto &word : smallerWords)
        {
            mvprintw(row++, 0, word.c_str());
        }

        getch(); // Wait for user input before exiting
    }
    // Paragraph Count
    void countParagraphs()
    {
        int paragraphCount = 0;
        bool inParagraph = false;

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            bool paraEmpty = true;

            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                if (!line->getContent().empty()) // Checking if line is non-empty
                {
                    paraEmpty = false;
                    break;
                }
            }

            if (!paraEmpty)
            {
                paragraphCount++;
            }
        }
        clear();
        string resultMessage = "Paragraph Count is: " + std::to_string(paragraphCount);
        mvprintw(4, 0, resultMessage.c_str());
        getch();
    }
    // Find Largest Paragraph Word Lenght
    void findLargestParagraphWordLength()
    {
        int maxWordLength = 0;

        for (int i = 0; i < currentDocument->paraCount(); ++i)
        {
            Para *para = currentDocument->getPara(i);
            for (int j = 0; j < para->lineCount(); ++j)
            {
                Line *line = para->getLine(j);
                stringstream ss(line->getContent());
                string word;

                while (ss >> word)
                {
                    if (word.length() > maxWordLength)
                    {
                        maxWordLength = word.length();
                    }
                }
            }
        }
        clear();
        string resultMessage = "Largest Paragraph Word Lenght Count is: " + std::to_string(maxWordLength);
        mvprintw(4, 0, resultMessage.c_str());
        getch();
    }

    bool validatePassword(const string &inputPassword)
    {
        const string correctPassword = "password"; // Set your desired password here
        return inputPassword == correctPassword;
    }

    void readFileWithPasswordProtection()
    {
        clear();
        mvprintw(0, 0, "Enter file path to open: ");
        echo();
        char filepath[256];
        getnstr(filepath, 255);
        noecho();

        string filename = filepath;

        mvprintw(2, 0, "Enter password: ");
        echo();
        char pass[256];
        getnstr(pass, 255);
        noecho();
        string password = pass;
        if (!validatePassword(password))
        {
            std::cout << "Incorrect password!" << std::endl;
            return; // If password is incorrect, stop reading the file.
        }

        // If password is correct, read the file
        std::ifstream file(filename);
        openFile(filename);
    }

    string runLengthEncode(const string &input)
    {
        string encoded = "";
        int n = input.length();
        int count = 1;

        for (int i = 1; i < n; ++i)
        {
            if (input[i] == input[i - 1])
            {
                count++;
            }
            else
            {
                encoded += input[i - 1] + to_string(count); // Add char and count
                count = 1;                                  // Reset count for next character
            }
        }
        encoded += input[n - 1] + to_string(count); // Add the last character and its count
        return encoded;
    }

    // Function to encode the content of a file
    void encodeFileWithRLE(const string &filePath)
    {
        ifstream file(filePath);
        if (!file)
        {
            cerr << "File could not be opened!" << endl;
            return;
        }

        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        // Apply RLE encoding
        string encodedContent = runLengthEncode(content);

        // Save the encoded content back to the file
        ofstream outFile(filePath);
        outFile << encodedContent;
        outFile.close();

        cout << "File encoded using RLE and saved successfully." << endl;
    }

    void encodeFilePrompt()
    {
        clear();
        mvprintw(0, 0, "Enter the file path to encode with RLE: ");
        echo();
        char filepath[256];
        getnstr(filepath, 255);
        noecho();

        string filePath = filepath;

        // Call the function to encode the file
        encodeFileWithRLE(filePath);
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
