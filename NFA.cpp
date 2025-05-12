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

// ����nfa��ʼ�ڵ�
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
			// epsilon��Ȼ����
			auto right = st.top(); st.pop();
			auto left = st.top(); st.pop();
			add_epsilon_edge(left.end->id, right.start->id);
			st.push({ left.start, right.end });
		}
		else if (c == '|') {
			// �� �ֲ��ϲ�
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
			// ��Ŀ����
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// ��start->��start
			add_epsilon_edge(start->id, end->id); // ��start->��end������
			add_epsilon_edge(frag.end->id, end->id); // ��end->��end
			add_epsilon_edge(frag.end->id, frag.start->id);// ��end��ָ
			st.push({ start, end });
		}
		// ͬ��ʵ�� '+' '?' ��
		else if (c == '+')
		{
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// ��start->��start
			add_epsilon_edge(frag.end->id, end->id); // ��end->��end
			add_epsilon_edge(frag.end->id, frag.start->id);// ��end��ָ
			st.push({ start, end });
		}
		else if (c == '?')
		{
			auto frag = st.top(); st.pop();
			NFAState* start = createState();
			NFAState* end = createState();
			add_epsilon_edge(start->id, frag.start->id);// ��start->��start
			add_epsilon_edge(start->id, end->id); // ��start->��end������
			add_epsilon_edge(frag.end->id, end->id); // ��end->��end
			st.push({ start, end });
		}
		else
			st.push(buildChar(c));
	}

	// ��top.end����Ϊ�ɱ�����
	st.top().end->isAccept = true;
	st.top().end->tokenID = ruleid;

	return st.top().start->id;  // �������� NFA ��ͷ�ڵ�
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

// ���ݽ�����rulesĬ���Ǻ�׺���ʽ
void NFA::createNcombineNfa(NFA& nfa, vector<Rule>& rules)
{
	if (nfa.size() != 0)
	{
		cout << "nfa is not clean" << endl;
		exit(1);
	}

	nfa.createState();//����׮�ڵ�����������nfa

	for (int i = 0; i < rules.size(); i++)
	{
		cout << "cur startid = " << nfa.startid << ";  cur pattern is :" << rules[i].pattern << endl;
		int son = nfa.createNfa(rules[i].pattern, i);
		nfa.merge2head(son);
	}

	// ׮�ڵ���ַ�Ǩ��Ӧ���ǿգ�ȫ��epsilonǨ��
	assert(nfa.states[0]->transitions.size() == 0);
}
