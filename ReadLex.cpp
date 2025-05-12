#include "ReadLex.h"


// ���ڼ���ȡ��lex�����Ƿ���ȷ
static void check(const vector<string>& user_declarations,
    const unordered_map<string, string>& elements,
    const vector<Rule>& rules,
    const vector<string>& subroutines)
{
    cout << "\n====== CHECKING LEX FILE PARSE RESULT ======" << endl;

    // ����û���������
    cout << "\n[User Declarations] (" << user_declarations.size() << " lines):" << endl;
    for (const auto& decl : user_declarations) {
        cout << decl << endl;
    }

    // ��鶨�岿��
    cout << "\n[Element Definitions] (" << elements.size() << " entries):" << endl;
    for (const auto& [key, value] : elements) {
        cout << key << " = " << value << endl;
    }

    // �����򲿷�
    cout << "\n[Rules] (" << rules.size() << " rules):" << endl;
    for (const auto& rule : rules) {
        cout << "Pattern: " << rule.pattern << ", Action: " << rule.actions << endl;
    }

    // ����ӳ��򲿷�
    cout << "\n[Subroutines] (" << subroutines.size() << " lines):" << endl;
    for (const auto& sub : subroutines) {
        cout << sub << endl;
    }

    cout << "====== CHECK COMPLETE ======\n" << endl;
}


ReadLex::ReadLex(string fname):filename(fname)
{
	file.open(filename);
    // ����ļ��Ƿ�ɹ���
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file ../bin/test.txt" << std::endl;
        exit(1);
    }
}

ReadLex::~ReadLex()
{
	file.close();
}

void ReadLex::read_lex_file(vector<string>& user_declarations, unordered_map<string, string>& elements, vector<Rule>& rules, vector<string>& subroutines)
{
    string line;//�ļ���ÿһ��
    int state = DEFINITIONS;


    // ���ж�ȡ�ļ�
    while (getline(file, line)) {
        switch (state) {
        case DEFINITIONS:
            if (line == "%{")
                continue;
            else if (line == "%}")
                state++;
            else
                user_declarations.push_back(move(line));
            break;

        case ELEMENTS:
            trim(line);
            if (line == "%%")
                state++;
            else if (line.empty())
                continue;
            else
            {
                string ele_name = "";
                string element = "";
                int i = 0;
                while (i < line.size()) {
                    if (line[i] == ' ' || line[i] == '\t')
                    {
                        ele_name = move(line.substr(0, i));
                        break;
                    }
                    i++;
                }
                while (line[i] == ' ' || line[i] == '\t') i++;//�����м�
                element = move(line.substr(i, line.size() - i));
                elements[ele_name] = element;
            }
            break;

        case RULES:
            trim(line);  // ȥ�������˵Ŀո���Ʊ��
            if (line == "%%")
                state++;
            else if (line.empty())
                continue;
            else
            {
                int i = 0;
                Rule rule;
                while (i < line.size()) {
                    if (line[i] == ' ' || line[i] == '\t')
                    {
                        rule.pattern = move(line.substr(0, i));
                        if (rule.pattern.size() > 1 && rule.pattern[1] == 'K')
                            rule.pattern[1] = ' ';
                        break;
                    }
                    i++;
                }
                while (line[i] == ' ' || line[i] == '\t') i++;//�����м�
                rule.actions = move(line.substr(i, line.size() - i));
                rules.push_back(rule);
            }
            break;

        case SUBROUTINES:
            subroutines.push_back(move(line));
            break;

        default:
            cout << "unknown state" << endl;
            exit(1);
        }
    }

    check(user_declarations, elements, rules, subroutines);
}

//ȥ��line���˵Ŀո��\t
string& ReadLex::trim(string& s)
{
    if (s.size() == 0)return s;
    s.erase(0, s.find_first_not_of('\t'));
    s.erase(0, s.find_first_not_of(' '));
    s.erase(s.find_last_not_of(' ') + 1);
    s.erase(s.find_last_not_of('\t') + 1);
    return s;
}