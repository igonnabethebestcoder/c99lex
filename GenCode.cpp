#include "GenCode.h"
void GenCode::printDeclaration()
{
    // 1. Include 头文件
    out << "#include <iostream>\n";
    out << "#include <unordered_map>\n";
    out << "#include <string>\n";
    out << "#include <fstream>\n";
    out << "using namespace std;\n";

    // 2. 定义全局变量
    out << "string yytext;\n";
    out << "int yylval;\n\n";
    for (auto& s : user_declarations)
        out << s << "\n";
}

void GenCode::printSubroutines()
{
    for (auto& s : subroutines)
        out << s << "\n";
}

void GenCode::printActions()
{
    out << "void action(int tokenID){\n";
    out << "    switch (tokenID) {\n";
    for (size_t i = 0; i < rules.size(); ++i) {
        out << "    case " << i << ":\n";
        out << "        " << rules[i].actions << "\n";  // 假设 action 是 C++ 代码
        out << "        break;\n";
    }
    out << "    }\n";
    out << "}\n";
}

static string handleEscape(char c)
{
    switch (c)
    {
    case '\n':
        return "\\n";
    case '\t':
        return "\\t";
    case '\r':
        return "\\r";
    case '\\':
        return "\\\\";
    }
    return string(1,c);
}

void GenCode::printMinDFA()
{
    // 3. 定义 minDFA 数据
    out << "// DFA 转移表\n";
    out << "vector<unordered_map<char, int>> transitions = {\n";
    for (const auto& state : dfa.states) {
        out << "    {{";
        for (const auto& [ch, target] : state->transitions) {
            out << "'" << handleEscape(ch) << "', " << target << "}, {";
        }
        out << "}},\n";
    }
    out << "};\n\n";

    out << "// 接受状态及其 tokenID\n";
    out << "unordered_map<int, int> acceptStates = {\n";
    for (const auto& state : dfa.states) {
        if (state->isAccept) {
            out << "    {" << state->id << ", " << state->tokenID << "},\n";
        }
    }
    out << "};\n\n";

    out << "int startState = " << dfa.minDFAsid << ";\n\n";

    // 4. yylex() 函数
    out << "int yylex() {\n";
    out << "    static int currentState = startState;\n";
    out << "    static string lexeme = \"\";\n";
    out << "    char ch;\n";
    out << "    while (true) {\n";
    out << "        ch = cin.get();\n";
    out << "        if (cin.eof()) {\n";
    out << "            if (acceptStates.count(currentState)) {\n";
    out << "                int tokenID = acceptStates[currentState];\n";
    out << "                yytext = lexeme;\n";
    out << "                // 执行动作\n";
    out << "                action(tokenID);\n";
    out << "                lexeme = \"\";\n";
    out << "                currentState = startState;\n";
    out << "                return tokenID;\n";
    out << "            }\n";
    out << "            return 0;  // EOF\n";
    out << "        }\n";
    out << "        auto it = transitions[currentState].find(ch);\n";
    out << "        if (it != transitions[currentState].end()) {\n";
    out << "            currentState = it->second;\n";
    out << "            lexeme += ch;\n";
    out << "        } else {\n";
    out << "            if (acceptStates.count(currentState)) {\n";
    out << "                int tokenID = acceptStates[currentState];\n";
    out << "                yytext = lexeme;\n";
    out << "                // 执行动作\n";
    out << "                action(tokenID);\n";
    out << "                lexeme = \"\";\n";
    out << "                currentState = startState;\n";
    out << "                cin.putback(ch);\n";
    out << "                return tokenID;\n";
    out << "            } else {\n";
    out << "                cerr << \"Lexical error at char: \" << ch << std::endl;\n";
    out << "                return -1;\n";
    out << "            }\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";

}

void GenCode::printMainFuc()
{
    out << "int main()\n";
    out << "{\n"; 
    out << "    ifstream file(\"test.txt\");\n";
    out << "    // 备份原始 cin\n";
    out << "    streambuf* origCin = cin.rdbuf();\n";
    out << "    cin.rdbuf(file.rdbuf()); // 替换 cin 为 file\n";
    out << "    cout << \"Lexical Analysis Result : \\n\";\n";
    out << "    int token;\n";
    out << "    while ((token = yylex()) != 0) {}\n";
    out << "    cin.rdbuf(origCin);  // 恢复 cin\n";
    out << "    return 0;\n";
    out << "}\n";
}

void GenCode::genLexer()
{
	printDeclaration();
	printSubroutines();
	printActions();
	printMinDFA();
	printMainFuc();
}
