%{
//#include <stdio.h>
void count();
int num = 0;

// 删除的语法规则
// L?'(\\.|[^\\'\n])+'	{ count(); printf("%s\n", yytext); }
// L?\"(\\.|[^\\"\n])*\"	{ count(); printf("%s\n", yytext); }
// .			{ /* Add code to complain about unmatched characters */ }
%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P           ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS          ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%%

"auto"			{ count(); printf("AUTO %s\n", yytext.c_str()); }
"_Bool"			{ count(); printf("BOOL %s\n", yytext.c_str()); }
"break"			{ count(); printf("BREAK %s\n", yytext.c_str()); }
"case"			{ count(); printf("CASE %s\n", yytext.c_str()); }
"char"			{ count(); printf("CHAR %s\n", yytext.c_str()); }
"_Complex"		{ count(); printf("_COMPLEX %s\n", yytext.c_str()); }
"const"			{ count(); printf("CONST %s\n", yytext.c_str()); }
"continue"		{ count(); printf("CONTINUE %s\n", yytext.c_str()); }
"default"		{ count(); printf("DEFAULT %s\n", yytext.c_str()); }
"do"			{ count(); printf("DO %s\n", yytext.c_str()); }
"double"		{ count(); printf("DOUBLE %s\n", yytext.c_str()); }
"else"			{ count(); printf("ELSE %s\n", yytext.c_str()); }
"enum"			{ count(); printf("ENUM %s\n", yytext.c_str()); }
"extern"		{ count(); printf("EXTERN %s\n", yytext.c_str()); }
"float"			{ count(); printf("FLOAT %s\n", yytext.c_str()); }
"for"			{ count(); printf("FOR %s\n", yytext.c_str()); }
"goto"			{ count(); printf("GOTO %s\n", yytext.c_str()); }
"if"			{ count(); printf("IF %s\n", yytext.c_str()); }
"_Imaginary"		{ count(); printf("IMAGINARY %s\n", yytext.c_str()); }
"inline"		{ count(); printf("INLINE %s\n", yytext.c_str()); }
"int"			{ count(); printf("INT %s\n", yytext.c_str()); }
"long"			{ count(); printf("LONG %s\n", yytext.c_str()); }
"register"		{ count(); printf("REGISTER %s\n", yytext.c_str()); }
"restrict"		{ count(); printf("RESTRICT %s\n", yytext.c_str()); }
"return"		{ count(); printf("RETURN %s\n", yytext.c_str()); }
"short"			{ count(); printf("SHORT %s\n", yytext.c_str()); }
"signed"		{ count(); printf("SIGNED %s\n", yytext.c_str()); }
"sizeof"		{ count(); printf("SIZEOF %s\n", yytext.c_str()); }
"static"		{ count(); printf("STATIC %s\n", yytext.c_str()); }
"struct"		{ count(); printf("STRUCT %s\n", yytext.c_str()); }
"switch"		{ count(); printf("SWITCH %s\n", yytext.c_str()); }
"typedef"		{ count(); printf("TYPEDEF %s\n", yytext.c_str()); }
"union"			{ count(); printf("UNION %s\n", yytext.c_str()); }
"unsigned"		{ count(); printf("UNSIGNED %s\n", yytext.c_str()); }
"void"			{ count(); printf("VOID %s\n", yytext.c_str()); }
"volatile"		{ count(); printf("VOLATILE %s\n", yytext.c_str()); }
"while"			{ count(); printf("WHILE %s\n", yytext.c_str()); }

{L}({L}|{D})*		{ count(); printf("IDENTIFIER %s\n", yytext.c_str()); }

0[xX]{H}+{IS}?		{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
0[0-7]*{IS}?		{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
[1-9]{D}*{IS}?		{ count(); printf("CONSTANT %s\n", yytext.c_str()); }


{D}+{E}{FS}?		{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
{D}*"."{D}+{E}?{FS}?	{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
{D}+"."{D}*{E}?{FS}?	{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
0[xX]{H}+{P}{FS}?	{ count(); printf("CONSTANT %s\n", yytext.c_str()); }
0[xX]{H}*"."{H}+{P}?{FS}?     { count(); printf("CONSTANT %s\n", yytext.c_str()); }
0[xX]{H}+"."{H}*{P}?{FS}?     { count(); printf("CONSTANT %s\n", yytext.c_str()); }


"..."			{ count(); printf("ELLIPSIS %s\n", yytext.c_str()); }
">>="			{ count(); printf("RIGHT_ASSIGN %s\n", yytext.c_str()); }
"<<="			{ count(); printf("LEFT_ASSIGN %s\n", yytext.c_str()); }
"+="			{ count(); printf("ADD_ASSIGN %s\n", yytext.c_str()); }
"-="			{ count(); printf("SUB_ASSIGN %s\n", yytext.c_str()); }
"*="			{ count(); printf("MUL_ASSIGN %s\n", yytext.c_str()); }
"/="			{ count(); printf("DIV_ASSIGN %s\n", yytext.c_str()); }
"%="			{ count(); printf("MOD_ASSIGN %s\n", yytext.c_str()); }
"&="			{ count(); printf("AND_ASSIGN %s\n", yytext.c_str()); }
"^="			{ count(); printf("XOR_ASSIGN %s\n", yytext.c_str()); }
"|="			{ count(); printf("OR_ASSIGN %s\n", yytext.c_str()); }
">>"			{ count(); printf("RIGHT_OP %s\n", yytext.c_str()); }
"<<"			{ count(); printf("LEFT_OP %s\n", yytext.c_str()); }
"++"			{ count(); printf("INC_OP %s\n", yytext.c_str()); }
"--"			{ count(); printf("DEC_OP %s\n", yytext.c_str()); }
"->"			{ count(); printf("PTR_OP %s\n", yytext.c_str()); }
"&&"			{ count(); printf("AND_OP %s\n", yytext.c_str()); }
"||"			{ count(); printf("OR_OP %s\n", yytext.c_str()); }
"<="			{ count(); printf("LE_OP %s\n", yytext.c_str()); }
">="			{ count(); printf("GE_OP %s\n", yytext.c_str()); }
"=="			{ count(); printf("EQ_OP %s\n", yytext.c_str()); }
"!="			{ count(); printf("NE_OP %s\n", yytext.c_str()); }
";"			{ count(); printf("%s\n", yytext.c_str()); }
"{"		{ count(); printf("%s\n", yytext.c_str()); }
"}"		{ count(); printf("%s\n", yytext.c_str()); }
","			{ count(); printf("%s\n", yytext.c_str()); }
":"			{ count(); printf("%s\n", yytext.c_str()); }
"="			{ count(); printf("%s\n", yytext.c_str()); }
"("			{ count(); printf("%s\n", yytext.c_str()); }
")"			{ count(); printf("%s\n", yytext.c_str()); }
"["		{ count(); printf("%s\n", yytext.c_str()); }
"]"		{ count(); printf("%s\n", yytext.c_str()); }
"."			{ count(); printf("%s\n", yytext.c_str()); }
"&"			{ count(); printf("%s\n", yytext.c_str()); }
"!"			{ count(); printf("%s\n", yytext.c_str()); }
"~"			{ count(); printf("%s\n", yytext.c_str()); }
"-"			{ count(); printf("%s\n", yytext.c_str()); }
"+"			{ count(); printf("%s\n", yytext.c_str()); }
"*"			{ count(); printf("%s\n", yytext.c_str()); }
"/"			{ count(); printf("%s\n", yytext.c_str()); }
"%"			{ count(); printf("%s\n", yytext.c_str()); }
"<"			{ count(); printf("%s\n", yytext.c_str()); }
">"			{ count(); printf("%s\n", yytext.c_str()); }
"^"			{ count(); printf("%s\n", yytext.c_str()); }
"|"			{ count(); printf("%s\n", yytext.c_str()); }
"?"			{ count(); printf("%s\n", yytext.c_str()); }

[K\t\v\n\f]		{ count(); }
%%

void count(){
	++num;
}

