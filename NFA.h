#pragma once
#ifndef NFA_H
#define NFA_H
#include <iostream>
#include "Infix2Postfix.h"
using namespace std;

// ��ʾһ�� NFA ״̬
struct NFAState {
    int id;  // ״̬���
    unordered_map<char, vector<NFAState*>> transitions;  // �ߣ��ӵ�ǰ״̬����������ĳ�ַ�����Ŀ��״̬����
    vector<NFAState*> epsilonTransitions;  // epsilon-ת�ƣ��ձߣ�
    bool isAccept = false; // �Ƿ�����ֹ״̬
    int tokenID = -1;  // ��Ӧ�����������ȼ��ϸߵģ�
};

// ��ʾһ�� NFA Ƭ�Σ��������ڹ���ʱ���ӣ�
struct NFAFragment {
    NFAState* start;  // ��ʼ״̬
    NFAState* end; // ����״̬
};

// ���� NFA ��һ����ͳһ����״̬����
class NFA {
public:
    int startid; // NFA����ʼ��ţ��ϲ�nfaʹ�ã�������nfa�ڵ�ı��Ψһ
    vector<NFAState*> states;  // ��������״̬����ֹ�ڴ�й©

    // ����һ���ڵ�
    NFAState* createState() {
        NFAState* s = new NFAState{ startid };
        startid++;
        states.push_back(s);
        return s;
    }

    // ����һ������
    NFAFragment buildChar(char c) {
        NFAState* start = createState();
        NFAState* end = createState();
        start->transitions[c].push_back(end);
        return { start, end };
    }

    NFAState* getState(int id)
    {
        return states[id];
    }

    // ����ÿһ�������postfix����nfa, ������nfa��ͷ�ڵ�
    int createNfa(string& postfix, int ruleid);

    // ���ӵ�׮
    void merge2head(int start);

    // ������state����ӱ�
    void add_char_edge(int start, int end, char c);
    void add_epsilon_edge(int start, int end);

    int size() {
        return states.size();
    }

    NFA(int start_id = 0) :startid(start_id) {}
    ~NFA() {
        for (auto s : states)
            delete s;
    }

    static void createNcombineNfa(NFA& nfa, vector<Rule>& rules);
};
#endif // !NFA_H

