#include "Infix2Postfix.h"

// 在两个表达式之间添加连接符.（代表“然后”）形成中缀表达式
void Infix2Postfix::add_dot(string& s)
{
	// 操作符有 |, (, ), *, +, ?
	// 除了转义之外，任何操作符
	string res = "";
	int n = s.size();
	for (int i = 0; i < n; i++)
	{
		res += s[i];
		//是转义符
		if (s[i] == '\\')
			continue;
		//是非转义的 (, |
		if ((s[i] == '(' || s[i] == '|') && (i == 0 || s[i - 1] != '\\'))
			continue;
		//最后一个字符
		if (i == n - 1)
			continue;
		//当前字符的后一个字符是操作符,如a*
		if (s[i + 1] == '|' || s[i + 1] == ')' || s[i + 1] == '*' || s[i + 1] == '+' || s[i + 1] == '?')
			continue;
		res += '#';
	}
	s = move(res);
	cout << "inffix : " << s << endl;
}

unordered_map<char, int> precedence = {
    {'*', 3}, {'+', 3}, {'?', 3},
    {'#', 2}, // 连接符（原本应为隐式）
    {'|', 1},
};

// 判断是否是操作符
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
            postfix += '\\'; // 保留转义符
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
                opStack.pop(); // 弹出'('
        }
        else if (isOperator(c)) {
            // 弹出高优先级或同优先级的操作符
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
