#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Defined Colors using ANSI escape codes for Linux
#define BLACK "\033[30m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"
#define BROWN "\033[33m"
#define LIGHTGREY "\033[37m"
#define DARKGREY "\033[90m"
#define LIGHTBLUE "\033[94m"
#define LIGHTGREEN "\033[92m"
#define LIGHTCYAN "\033[96m"
#define LIGHTRED "\033[91m"
#define LIGHTMAGENTA "\033[95m"
#define YELLOW "\033[93m"
#define WHITE "\033[97m"
#define RESET "\033[0m"

void setConsoleColor(const string& color) {
    cout << color;
}

void gotoRowCol(int rpos, int cpos) {
    cout << "\033[" << rpos << ";" << cpos << "H";
}

// Line class
class Lines {
    friend class TextEditor;
    friend class Document;
    string Cs;
    int Size;
    static bool isChar(char c);
    static bool islower(char c);
    static bool isupper(char c); 
public:
    Lines();
    Lines(const Lines& A);
    void InsertCharAt_i(int i, char ch);
    void CharRemoverAt_i(int i);
    void ToUpper(int li, int ci);
    void ToLower(int li, int ci);
    void SubStrings(string Tofind);
    void AddPrefix(string ToFind, string Prefix);
    void AddPostfix(string toFind, string PostFix);
    bool Replacefirst(string toFind, string toReplace);
    void ReplaceAll(string toFind, string toReplace);
    void FindNextOnly();
    void FindPrevOnly();
    void FindNextAll();
    void FindPrevAll();
    void FindAndReplaceNextOnly();
    void FindAndReplacePrevOnly();
    void FindAndReplaceNextAll();
    void FindAndReplacePrevAll();
    void PrintLine();
    void Writer(ofstream& wtr);
    ~Lines();
};

// Document class
class Document {
    friend class TextEditor;
    friend class Lines;
    Lines* Ls;
    int NOLs;
    string DName;
    int dri = 0, dci = 0;
    void Deepcopy(Lines& NLs, const Lines& L);
public:
    Document(string fname);
    void Load(string Fname);
    void LoadEncodedFile(string Fname);
    void SimpleUpdater();
    void UpdaterEncodedFiles();
    void Enter_AddNewLine(int ri, int ci);
    void BackSpace_LineRemover(int Ln);
    void BackSpace_Concat(int Ln);
    void FindCaseSens(string Tofind);
    void FindInCaseSens(string Tofind);
    double AvgWordLen();
    int SmallestWordLen();
    int LargestWordLength();
    int WordCount();
    int SpecialCharCount();
    int SentencesCount();
    int ParagraphCount();
    void FindAllSubStrings(string toFind);
    void Prefix(string Tofind, string PreFix);
    void PostFix(string Tofind, string PostFix);
    void FindAndReplaceFirst(string tofind, string toreplace);
    void FindAndReplaceAll(string tofind, string toreplace);
    void FindReplaceNext(string find, string replace, int cr, int cc);
    void FindReplaceAllNext(string find, string replace, int cr, int cc);
    void FindReplacePrev(string find, string replace, int cr, int cc);
    void FindReplaceAllPrev(string find, string replace, int cr, int cc);
    void FindAndReplaceMenu(int cr, int cc) {
        cout << "Ctrl + N to find and replace only the next word." << endl;
        cout << "Ctrl + Q to find and replace All next words." << endl;
        cout << "Ctrl + P to find and replace only the previous 1 word." << endl;
        cout << "Ctrl + O to find and replace all of the previous words." << endl;
        // You can use any Shortcut keys.
    }
    void FindingSentence();
    void WordGame(int& cr, int& cc);
    void PrintDocument();
    void ExitDocument();
    void DeleteAllLines();
    ~Document();
};

// TextEditor class
class TextEditor {
    friend class Document;
    friend class Lines;
    int CRow = 0, CCol = 0, NODs, CDoci;
    Document* Doc;
public:
    TextEditor(string Fname);
    void Run();
    void PrintShortcuts();
    void CommandModeShortcutsPrinter();
    void CommandMode();
    void MergeDocuments(string Fname1, string Fname2);
    void OpenMultipleDocs();
};

int main() {
    // Sample usage of TextEditor
    TextEditor editor("sample.txt");
    editor.Run();
    return 0;
}
