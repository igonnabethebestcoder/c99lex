#include "Infix2Postfix.h"

// ���������ʽ֮��������ӷ�.������Ȼ�󡱣��γ���׺���ʽ
void Infix2Postfix::add_dot(string& s)
{
	// �������� |, (, ), *, +, ?
	// ����ת��֮�⣬�κβ�����
	string res = "";
	int n = s.size();
	for (int i = 0; i < n; i++)
	{
		res += s[i];
		//��ת���
		if (s[i] == '\\')
			continue;
		//�Ƿ�ת��� (, |
		if ((s[i] == '(' || s[i] == '|') && (i == 0 || s[i - 1] != '\\'))
			continue;
		//���һ���ַ�
		if (i == n - 1)
			continue;
		//��ǰ�ַ��ĺ�һ���ַ��ǲ�����,��a*
		if (s[i + 1] == '|' || s[i + 1] == ')' || s[i + 1] == '*' || s[i + 1] == '+' || s[i + 1] == '?')
			continue;
		res += '#';
	}
	s = move(res);
	cout << "inffix : " << s << endl;
}

unordered_map<char, int> precedence = {
    {'*', 3}, {'+', 3}, {'?', 3},
    {'#', 2}, // ���ӷ���ԭ��ӦΪ��ʽ��
    {'|', 1},
};

// �ж��Ƿ��ǲ�����
bool isOperator(char c) {
    return precedence.count(c);
}


void Infix2Postfix::infix2postfix(string& expr)
{
    string postfix;
    stack<char> opStack;
    bool escape = false;

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        if (escape) {
            postfix += '\\'; // ����ת���
            postfix += c;
            escape = false;
            continue;
        }

        if (c == '\\') {
            escape = true;
            continue;
        }

        if (isalnum(c) || (!isOperator(c) && c != '(' && c != ')')) {
            postfix += c;
        }
        else if (c == '(') {
            opStack.push(c);
        }
        else if (c == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                postfix += opStack.top();
                opStack.pop();
            }
            if (!opStack.empty()) 
                opStack.pop(); // ����'('
        }
        else if (isOperator(c)) {
            // ���������ȼ���ͬ���ȼ��Ĳ�����
            while (!opStack.empty() && opStack.top() != '(' && precedence[opStack.top()] >= precedence[c]) {
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.push(c);
        }
    }
    while (!opStack.empty()) {
        postfix += opStack.top();
        opStack.pop();
    }

    expr = move(postfix);
    cout << "postfix : " << expr << endl;
}

void Infix2Postfix::add_dot4rules(vector<Rule>& rules)
{
	for (auto& rule : rules)
	{
		add_dot(rule.pattern);
		infix2postfix(rule.pattern);
        cout << endl;
	}
}

void Infix2Postfix::prepare4nfa(vector<Rule>& rules)
{
	add_dot4rules(rules);
}
