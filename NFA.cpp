#include "NFA.h"
#include <cassert>

char convertEscapeChar(char c) {
	switch (c) {
	case 'n': return '\n';
	case 't': return '\t';
	case 'r': return '\r';
	case '\\': return '\\';
	default: return c;
	}
}

// 返回nfa起始节点
int NFA::createNfa(string& postfix, int ruleid)
{
	stack<NFAFragment> st;
	bool escape = false;
	for (int i = 0; i < postfix.size(); i++) {
		char c = postfix[i];
		if (escape)
		{
			escape = false;
			st.push(buildChar(convertEscapeChar(c)));
			continue;
		}

		if (c == '\\')
			escape = true;
		else if (c == '#') {
			// epsilon自然连接
			auto right = st.top(); st.pop();
			auto left = st.top(); st.pop();
			add_epsilon_edge(left.end->id, right.start->id);
			st.push({ left.start, right.end });
		}
		else if (c == '|') {
			// 或， 分叉后合并
			auto b = st.top(); st.pop();
			auto a = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, a.start->id);
			add_epsilon_edge(start->id, b.start->id);
			add_epsilon_edge(a.end->id, end->id);
			add_epsilon_edge(b.end->id, end->id);
			st.push({ start, end });
		}
		else if (c == '*') {
			// 单目运算
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// 新start->旧start
			add_epsilon_edge(start->id, end->id); // 新start->新end，跳过
			add_epsilon_edge(frag.end->id, end->id); // 旧end->新end
			add_epsilon_edge(frag.end->id, frag.start->id);// 新end回指
			st.push({ start, end });
		}
		// 同理实现 '+' '?' 等
		else if (c == '+')
		{
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// 新start->旧start
			add_epsilon_edge(frag.end->id, end->id); // 旧end->新end
			add_epsilon_edge(frag.end->id, frag.start->id);// 新end回指
			st.push({ start, end });
		}
		else if (c == '?')
		{
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// 新start->旧start
			add_epsilon_edge(start->id, end->id); // 新start->新end，跳过
			add_epsilon_edge(frag.end->id, end->id); // 旧end->新end
			st.push({ start, end });
		}
		else
			st.push(buildChar(c));
	}

	// 将top.end设置为可被接受
	st.top().end->isAccept = true;
	st.top().end->tokenID = ruleid;

	return st.top().start->id;  // 返回完整 NFA 的头节点
}

void NFA::merge2head(int end)
{
	if (size() == 0)
	{
		cout << "Nfa merge ERR : nfa is empty" << endl;
		exit(1);
	}

	add_epsilon_edge(0, end);
}

void NFA::add_char_edge(int start, int end, char c)
{
	if (end >= startid || start >= startid)
	{
		cout << "unknown NFA state" << endl;
		exit(1);
	}

	NFAState* A = getState(start);
	NFAState* B = getState(end);
	A->transitions[c].push_back(B);
}

void NFA::add_epsilon_edge(int start, int end)
{
	if (end >= startid || start >= startid)
	{
		cout << "unknown NFA state" << endl;
		exit(1);
	}

	NFAState* A = getState(start);
	NFAState* B = getState(end);
	A->epsilonTransitions.push_back(B);
}

// 传递进来的rules默认是后缀表达式
void NFA::createNcombineNfa(NFA& nfa, vector<Rule>& rules)
{
	if (nfa.size() != 0)
	{
		cout << "nfa is not clean" << endl;
		exit(1);
	}

	nfa.createState();//创建桩节点连接所有子nfa

	for (int i = 0; i < rules.size(); i++)
	{
		cout << "cur startid = " << nfa.startid << ";  cur pattern is :" << rules[i].pattern << endl;
		int son = nfa.createNfa(rules[i].pattern, i);
		nfa.merge2head(son);
	}

	// 桩节点的字符迁移应该是空，全是epsilon迁移
	assert(nfa.states[0]->transitions.size() == 0);
}
