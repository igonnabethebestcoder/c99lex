# c99lex
workflow:
- read c99.l
- turn lex RE into regular RE
- turn regular RE into infix expr
- turn infix expr into postfix, help Thompson algo to build NFA
- consturct NFA using Thompson algo
- build DFA by NFA
- minimize DFA using Hopcroft algo
- gen lexer file by minDFA
- test

feature:
- easy to understand
- structure clear
- play some trick in parse c99.l
- smaller lexer.cpp only around 900 lines

drawback:
- you read the code, you'll find out
- i won't tell, haha
