#include "NormaliseRE.h"

static void PrintNormalizedRE(const unordered_map<string, string>& mp, const vector<Rule>& vRules) {
	cout << "========= [Normalized Definitions] =========" << endl;
	for (const auto& def : mp) {
		cout << def.first << " => " << def.second << endl;
	}

	cout << endl << "========= [Normalized Rules] =========" << endl;
	int idx = 0;
	for (const auto& rule : vRules) {
		cout << "Rule " << idx++ << ":" << endl;
		cout << "Pattern : " << rule.pattern << endl;
		cout << "Action  : " << rule.actions << endl;
		cout << "----------------------------------------------------" << endl;
	}
}

//将花括号{}中的进行替换
void NormalizeRE::HandleBrace(string& s, unordered_map<string, string>& mp)
{
	string replace_item = "";
	bool brace = false;
	string res = "";
	for (int i = 0; i < s.size(); i++)
	{
		// 不是转义
		if (s[i] == '{' && ((i > 0 && s[i - 1] != '\\') || i == 0))
		{
			brace = true;
			continue;
		}
		// 会有/}在前面被去引号时转义出现
		else if (s[i] == '}' && (i > 0 && s[i - 1] != '\\'))
		{
			brace = false;
			// 替换replace_item
			auto it = mp.find(replace_item);
			if (it == mp.end())
			{
				cout << "ERR : replace_item dosen't exist" << endl
					<< "replace_item = " << replace_item << endl
					<< "cur_item = " << s << endl;
				exit(1);
			}
			res += mp[replace_item];
			replace_item.clear();
		}
		else if (!brace)// 不在花括号内
			res += s[i];
		else// 不是花括号，并在花括号内
			replace_item += s[i];
	}
	s = move(res);
}

//将s中的形式转义变成真正的转义字符,用于处理[ \t\v\n\f]规则
void NormalizeRE::HandleLexRC(string& s)
{
	if (s.find('\\') == string::npos)
		return;

	string ans = "";
	bool flag = false;
	for (auto& c : s) {
		if (flag) {
			switch (c) {
			case 'n': ans += '\n'; break;
			case 't': ans += '\t'; break;
			case 'v': ans += '\v'; break;
			case 'f': ans += '\f'; break;
			case '\\': ans += '\\'; break;
			default: ans += c; break;  // 保留未知转义符（如 \x）
			}
			flag = false;
		}
		else {
			if (c == '\\') {
				flag = true;
			}
			else {
				ans += c;
			}
		}
	}
	s = std::move(ans);
}

// 获取如[a-z]的字符集， flag表示是否有^
void NormalizeRE::getSet(unordered_set<char>& charSet, string& s, bool flag)
{
	unordered_set<char> newset;
	int i = 0;
	//将rec中的形式转义变成真正的转义字符
	HandleLexRC(s);
	for (i = 0; i < s.size(); i++)
	{
		char c = s[i];
		if (c == '-' && i > 0 && i < s.size() - 1)
		{
			char first = s[i - 1];
			char last = s[i + 1];
			for (int j = ALLSET.find(first); j <= ALLSET.find(last); j++)
				newset.insert(ALLSET[j]);
			i += 1;
		}
		else
			newset.insert(c);
	}

	//根据flag将字符加入newset
	if (flag)
	{
		//有^
		for (auto c : ALLSET) {
			//字符c不在newset中
			if (newset.find(c) == newset.end()) 
				charSet.insert(c);
		}
	}
	else
		charSet = move(newset);
}

// 处理中括号[]将中括号中的内容转换为(a|b|c)
void NormalizeRE::HandleBrackets(string& s)
{
	string replace_item = "";
	bool braket = false;
	string res = "";
	for (int i = 0; i < s.size(); i++)
	{
		//非转义 \[
		if (s[i] == '[' && (i == 0 || (i > 0 && s[i - 1] != '\\')))
		{
			braket = true;
			continue;
		}
		//非转义 \]
		else if (s[i] == ']' && (i > 0 && s[i - 1] != '\\'))
		{
			cout << "[replace_item] : " << replace_item << endl;
			braket = false;
			//进行替换
			unordered_set<char> lcharset;//局部charset
			if (replace_item[0] == '^')
			{
				string charStr = replace_item.substr(1, replace_item.size() - 1);
				getSet(lcharset, charStr, true);
			}
			else
				getSet(lcharset, replace_item, false);
			res += "(";
			for (auto& c : lcharset)
			{
				// 字符c是lex预留字，进行转义
				if (lex_RC.find(c) != lex_RC.end())
					res += '\\';
				res += c;
				res += '|';
			}
			res.pop_back();
			res += ')';
			replace_item.clear();
		}
		else if (braket)//在[]内
			replace_item += s[i];
		else
			res += s[i];
	}
	s = move(res);
}


// 除去“”并将“”中的特殊字符进行转义
void NormalizeRE::HandleQuote(string& s)
{
	bool quote = false;
	string res = "";
	for (int i = 0; i < s.length(); i++)
	{
		//当前字符是",并且i==0或是\"
		if (s[i] == '\"')
		{
			if (!quote)
			{
				quote = true;
				continue;
			}
			else 
			{
				quote = false;
				continue;
			}
		}
		// 在引号内并属于lexRE预留字符，使用\进行转义
		else if (quote && lex_RC.find(s[i]) != lex_RC.end())
			res += '\\';
		res += s[i];
	}
	//更新
	s = move(res);
}

//将 . 替换为[^ \n\v\f\t]。
void NormalizeRE::HandleDot(string& s)
{
	if (s.find('.') == string::npos)
		return;
	string ans = "";
	int n = s.size();
	for (int i = 0; i < n; i++) {
		if (s[i] == '.' && (i == 0 || (i != 0 && s[i - 1] != '\\'))) {
			ans += '(';
			unordered_set<char> charset;
			string replace_item = " \t\v\n\f"; //[^ \n\v\f\t]
			// 获取所有字符
			getSet(charset, replace_item, true);
			for (auto c : charset) {
				if (lex_RC.find(c) != lex_RC.end())
					ans += '\\';
				ans += c; 
				ans += '|';
			}
			ans.pop_back();
			ans += ')';
			continue;
		}
		ans += s[i];
	}
	s = move(ans);
}


void NormalizeRE::RENormalization(vector<Rule>& re, unordered_map<string, string>& mp)
{
	for (auto& m : mp) {
		HandleQuote(m.second);
		HandleBrace(m.second, mp);
		HandleBrackets(m.second); 
	}

	int i = 0;
	for (auto& r : re) {
		HandleQuote(r.pattern);
		HandleBrace(r.pattern, mp);
		HandleBrackets(r.pattern);
		HandleDot(r.pattern);
		//HandleLexRC(r.pattern, false);
	}

	PrintNormalizedRE(mp, re);
}
