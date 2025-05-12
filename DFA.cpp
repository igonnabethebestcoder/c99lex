#include "DFA.h"

unordered_set<int> DFA::epsilonClosure(NFAState* s)
{
    unordered_set<int> closure;
    queue<NFAState*> q;
    q.push(s);
    closure.insert(s->id);
    while (!q.empty())
    {
        NFAState* cur_state = q.front();
        q.pop();
        for (auto& next : cur_state->epsilonTransitions)
        {
            if (!closure.count(next->id))
            {
                closure.insert(next->id);
                q.push(next);
            }
        }
    }
    return closure;
}

void DFA::epsilonClosure(unordered_set<int>& states)
{
    queue<NFAState*> q;

    for (int id : states) 
        q.push(nfa.getState(id));


    while (!q.empty()) {
        NFAState* s = q.front(); q.pop();
        for (NFAState* next : s->epsilonTransitions) {
            if (!states.count(next->id)) {
                states.insert(next->id);
                q.push(next);
            }
        }
    }
}

unordered_set<int> DFA::move(const unordered_set<int>& states, char symbol)
{
    unordered_set<int> result;
    for (int id : states) {
        NFAState* s = nfa.getState(id);
        //unordered_map<char, vector<NFAState*>> transitions;
        auto it = s->transitions.find(symbol);
        if (it != s->transitions.end()) {
            for (auto& target : it->second) {
                result.insert(target->id);
            }
        }
    }
    return result;
}

void DFA::buildDFA()
{
    queue<DFAState*> q; 
    //��nfa��ʼ״̬�����һ���Ӽ�
    DFAState* initState = createState(epsilonClosure(nfa.states[0]));
    q.push(initState);
    while (!q.empty())
    {
        DFAState* cur_dfaState = q.front(); q.pop();
        //unordered_set<int> next_state;
        unordered_set<char> tran_char;// ��ת���ַ�
        //�ҵ����еĿ�ת���ַ�
        for (int nfaid : cur_dfaState->nfaStates)
        {
            NFAState* nfa_state = nfa.getState(nfaid);
            // ����ǰnfa_state�е��ַ����뼯����
            for (auto& [c, states] : nfa_state->transitions)
                tran_char.insert(c);
#ifdef DEBUG
            //nfaidInUse[nfaid]++;
#endif // DEBUG
        }

        for (char c : tran_char) {
            //�ƶ�������հ�
            auto moveSet = move(cur_dfaState->nfaStates, c);
            epsilonClosure(moveSet);

            if (moveSet.empty())
                continue;

            //��״̬
            int id = 0;
            if (subset2id.find(moveSet) == subset2id.end()) {
                DFAState* new_state = createState(std::move(moveSet));
                id = new_state->id;
                q.push(new_state);
            }
            else 
                id = subset2id[moveSet];

            //���ת��
            cur_dfaState->transitions[c] = id;
        }
    }

#ifdef DEBUG
    cout << "NFADEBUG----------------------------------NFADEBUG" << endl;
    nfaidInUse.resize(nfa.size(), 0);
    for (int i = 0; i < states.size(); i++)
    {
        for (int id : states[i]->nfaStates)
            nfaidInUse[id]++;
    }
    for (int i = 0; i < nfa.size(); i++)
    {
        if (nfaidInUse[i] == 0)
        {
            cout << "nfa-id : " << i << " not use" << endl
                << "accept : " << nfa.states[i]->isAccept << "; tokenid : " << nfa.states[i]->tokenID;
        }
    }
    cout << "NFADEBUG----------------------------------NFADEBUG" << endl;

    vector<int> totalUsedToken(95, 0);
    vector<int> finalUsedToken(95, 0);
    vector<set<int>> each_stoken(states.size());
    int acceptnum = 0;
    for (auto& s : states)
    {
        /*int minTokenId = INT_MAX;
        for (int id : s->nfaStates)
        {
            NFAState* ns = nfa.getState(id);
            if (ns->isAccept)
            {
                s->isAccept = true;
                minTokenId = min(minTokenId, ns->tokenID);
                totalUsedToken[ns->tokenID]++;
                each_stoken[s->id].insert(ns->tokenID);
            }
        }
        s->tokenID = minTokenId == INT_MAX ? -1 : minTokenId;*/
        if (s->isAccept)
        {
            finalUsedToken[s->tokenID]++;
            acceptnum++;
        }
    }
    cout << "total ac dfa states = " << acceptnum << endl;

    cout << "breakpoint" << endl;
#endif
}

void DFA::checksum(vector<unordered_set<int>>& newStates)
{
    int sum = 0;
    for (auto& uset : newStates)
        sum += uset.size();

    if (sum != states.size())
    {
        cout << "ERR in hopcroftMinDFA" << endl;
        exit(1);
    }
}


// ��С��dfa
void DFA::hopcroftMinDFA()
{
    vector<unordered_set<int>> newStates; //��״̬���ϱ�
    // ��ʼ�����ɽ��ܺͲ��ɽ���״̬����
    // ��ʼ��ʱ�� tokenID �ָ����״̬
    unordered_map<int, unordered_set<int>> acceptGroups;
    unordered_set<int> nonaccept;
    for (DFAState* s : states) {
        if (s->isAccept)
            acceptGroups[s->tokenID].insert(s->id);
        else
            nonaccept.insert(s->id);
    }
    for (auto& [tokenID, group] : acceptGroups) {
        if (!group.empty()) {
            newStates.push_back(std::move(group));
        }
    }
    if (!nonaccept.empty())
        newStates.push_back(std::move(nonaccept));

    bool change = true;
    int round = 0;
    cout << "round : " << round++ << " -- newStates.size = " << newStates.size() << endl;
    checksum(newStates);
    while (change)
    {
        change = false;
        vector<unordered_set<int>> changeStates;
        // ����ÿһ�����飬����ת���и�
        int gnum = newStates.size(); // ǰһ������ĸ���
        for (auto& group : newStates) {
            unordered_map<string, unordered_set<int>> splitter;
            // ͨ��signature�жϷ����Ƿ���ͬ
            // ͬһ�����ת��Ӧ����ͬ
            for (int sid : group) {
                string signature;
                DFAState* s = states[sid];
                /* for (char c = 0; c <= 127; ++c)
                 * ����ִ�У�[-128,127] 127ʱ���
                 */
                for (int c = 0; c <= 127; ++c) {
                    auto it = s->transitions.find(c);
                    if (it != s->transitions.end()) {
                        int target = it->second;
                        //cout << "for char : " << c << " goto id:" << target << endl;
                        for (int i = 0; i < gnum; ++i) {
                            if (newStates[i].count(target)) {
                                signature += to_string(i) + ",";
                                break;
                            }
                        }
                    }
                    else {
                        signature += "#,"; // no transition
                    }
                }
                splitter[signature].insert(sid);
            }

            if (splitter.size() == 1) {
                // ���������ת����ͬ
                changeStates.push_back(group);
                for (auto& [sig, _] : splitter)
                    cout << "signature " << " : " << sig << endl;
            }
            else {
                // ���ڲ�ͬת��
                change = true;
                int k = 0;
                for (auto& [_, subset] : splitter)
                {
                    changeStates.push_back(subset);
                    cout << "signature " << k++ << " : " << _ << endl;
                }
            }
        }
        newStates = changeStates;
        cout << "round : " << round++ << " -- newStates.size = " << newStates.size() << endl;
        checksum(newStates);
    }

    // ��ʼ����minDFA
    vector<DFAState*> minStates(newStates.size());
    unordered_map<int, int> old2NewStateID; // ԭdfaState�����ĸ���dfa״̬����

    // ���¾�state����state��ӳ��
    for (int i = 0; i < newStates.size(); i++)
    {
        for (int id : newStates[i])
            old2NewStateID[id] = i;
    }

    // ��¼minDFAstartid
    minDFAsid = old2NewStateID[0];

    // �����µ�dfaStates
    for (int i = 0; i < newStates.size(); ++i) {
        DFAState* s = new DFAState;
        s->id = i;

        // ���������������һ���ǽ���״̬�������ǽ���״̬
        for (int oldID : newStates[i]) {
            DFAState* oldState = states[oldID];
            if (oldState->isAccept) {
                s->isAccept = true;
                s->tokenID = oldState->tokenID; // ����Ծ����Ƿ�����һ�� oldState �� tokenID ������ȵ�
                break;
            }
        }

        minStates[i] = s;
    }

    // �ؽ�ת�ƹ�ϵ
    for (int i = 0; i < newStates.size(); ++i) {
        int representativeID = *newStates[i].begin(); // ����һ����״̬
        DFAState* oldState = states[representativeID];
        for (auto& [c, targetOldID] : oldState->transitions) {
            int targetNewID = old2NewStateID[targetOldID];
            minStates[i]->transitions[c] = targetNewID;
        }
    }

    // ����ɽڵ�
    clearOldStates();
    states = std::move(minStates);

    cout << "----------------------------" << endl
        << "minimize DFA done" << endl
        << "total node number : " << states.size() << endl
        << "----------------------------" << endl;
}
