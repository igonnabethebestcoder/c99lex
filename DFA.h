#pragma once
#ifndef DFA_H
#define DFA_H
#include "NFA.h"
#include <set>
using namespace std;

#define DEBUG

struct DFAState {
    int id;
    unordered_set<int> nfaStates;  // ��DFA״̬������NFA״̬����
    unordered_map<char, int> transitions; // �ַ� -> DFAState.id
    bool isAccept = false;
    int tokenID = -1;  // ��Ӧ�����������ȼ��ϸߵģ�
};

class DFA
{
public:
    // unordered_map<unordered_set<int>, int>��ϣ����
    struct SetHash {
        size_t operator()(const unordered_set<int>& s) const {
            size_t hash = 0;
            for (int x : s) {
                hash ^= std::hash<int>()(x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

public:
    int startid; // Ψһ�����ʹ��
    int minDFAsid; //��С��dfa��ʼid
    NFA& nfa;
    vector<DFAState*> states;
    unordered_map<unordered_set<int>, int, SetHash> subset2id;  // key: NFA״̬����, value: DFA״̬���

#ifdef DEBUG
    vector<int> nfaidInUse;
#endif // DEBUG


    DFA(NFA& nfa, int start_id = 0) :startid(start_id), nfa(nfa) {}
    ~DFA()
    {
        for (auto& p : states)
            delete p;
        states.clear();
    }

    // ����һ���ڵ�, ��ֵ��
    DFAState* createState(unordered_set<int>&& closure) {
        DFAState* s = new DFAState{ startid };
        startid++;
        s->nfaStates = std::move(closure);
        // ����״̬��
        states.push_back(s);
        // ��ӳ��
        subset2id[s->nfaStates] = s->id;
        // �жϵ�ǰ״̬�Ƿ�ɱ�����
        // �������ù����ţ�**���������С�Ĺ���**������ᷢ������
        int minTokenId = INT_MAX;
        for (int id : s->nfaStates)
        {
            NFAState* ns = nfa.getState(id);
            if (ns->isAccept)
            {
                s->isAccept = true;
                minTokenId = min(minTokenId, ns->tokenID);
            }
        }
        s->tokenID = minTokenId == INT_MAX ? -1 : minTokenId;
        return s;
    }

    // epsilon�հ�����
    unordered_set<int> epsilonClosure(NFAState* s);
    void epsilonClosure(unordered_set<int>& states);//ֱ���޸ĵ�ǰstates

    // move(T,c),��״̬��ͨ��a����ת��
    unordered_set<int> move(const unordered_set<int>& states, char symbol);

    void buildDFA();

    void checksum(vector<unordered_set<int>>& newStates);

    void hopcroftMinDFA();

    void printDFAinfo()
    {
        cout << endl << endl << "----------------------------------------------" << endl;
        cout << "cur DFA size = " << states.size() << endl
            << "startid = " << startid << endl
            << "----------------------------------------------" << endl;
    }

    void clearOldStates()
    {
        for (auto& p : states)
            delete p;
        states.clear();
    }
};
#endif // !DFA_H



