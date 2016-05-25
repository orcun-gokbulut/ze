%token_prefix ZEMT_IT_

%left ADD SUB.
%left MUL DIV.

%token_type 
{ 
	YYSTYPE 
}

%extra_argument 
{ 
	ParserState *state 
}

%include 
{
	#include <stdio.h>
}

%syntax_error 
{
    fprintf(stderr, "Syntax error\n");
}

%parse_failure 
{
    fprintf(stderr,"Giving up.  Parser is hopelessly lost...\n");
}

%start_symbol program

program ::= statements.
{
	prinf("Wow much program.");
}

statements ::= .
{
	printf("wow empty statements.");
}

statements ::= statement.
{
	printf("wow single statement statements.");
}

statements ::= statement statement.
{
	printf("wow recursive statmenets.");
}

statement ::= expression SEMI_COLON.
{
	printf("wow expressions");
}

statement ::= IDENTIFIER ASSIGNMENT expression SEMI_COLON.
{
	printf("wow assignement");
}

expression ::= INTEGER.
{
	printf("wow integer");
}

expression ::= FLOAT.
{
	printf("wow integer");
}


expression ::= DOUBLE.
{
	printf("wow integer");
}

/*
// Expression
/////////////////////////////////////////////////////////////////////////////////

expression ::= IDENTIFIER.

expression(A) ::= PARENTHESES_LEFT expression(B) PARENTHESES_RIGHT. 
{
    A.Value = B.Value;
}


// ARITHMETIC OPERATORS
/////////////////////////////////////////////////////////////////////////////////

expression(A) ::= expression(B) SUBTRACTION expression(C). 
{
    A.Value = B.Value - C.Value;
}

expression(A) ::= expression(B) ADDITION expression(C). 
{
    A.Value = B.Value + C.Value;
}

expression(A) ::= expression(B) DIVISION expression(C). 
{
    A.Value = B.Value / C.Value;
}

expression(A) ::= expression(B) MULTIPLICATION expression(C). 
{
    A.Value = B.Value * C.Value;
}

expression(A) ::= expression(B) MODULUS expression(C). 
{
    A.Value = B.Value % C.Value;
}

// LOGICAL OPERATORS
/////////////////////////////////////////////////////////////////////////////////

expression(A) ::= LOGICAL_NOT expression(B). 
{
    A.Value = !B.Value;
}

expression(A) ::= expression(B) LOGICAL_OR expression(C). 
{
    A.Value = B.Value || C.Value;
}

expression(A) ::= expression(B) LOGICAL_AND expression(C). 
{
    A.Value = B.Value && C.Value;
}

expression(A) ::= expression(B) LOGICAL_XOR expression(C). 
{
    A.Value = B.Value ^ C.Value;
}


// COMPARASION OPERATORS
/////////////////////////////////////////////////////////////////////////////////

expression(A) ::= expression(B) EQUAL expression(C). 
{
    A.Value = B.Value == C.Value;
}

expression(A) ::= expression(B) NOT_EQUAL expression(C). 
{
    A.Value = B.Value != C.Value;
}

expression(A) ::= expression(B) LESS expression(C). 
{
    A.Value = B.Value < C.Value;
}

expression(A) ::= expression(B) LESS_EQUAL expression(C). 
{
    A.Value = B.Value <= C.Value;
}

expression(A) ::= expression(B) GREATER expression(C). 
{
    A.Value = B.Value > C.Value;
}
expression(A) ::= expression(B) GREATER_EQUAL expression(C). 
{
    A.Value = B.Value >= C.Value;
}


// LITERALS
/////////////////////////////////////////////////////////////////////////////////

expression(A) ::= literal(B). 
{
    A.int_value = B.int_value;
}

literal(A) ::= INTEGER(B). 
{
    A.Value = B.Value;
}

literal(A) ::= FLOAT(B). 
{
    A.Value = B.Value;
}

literal(A) ::= STRING(B). 
{
    A.Value = B.Value;
}

literal(A) ::= BOOLEAN(B). 
{
    A.Value = B.Value;
}

literal(A) ::= TRUE.
{
	A.Value = true;
}

literal(A) ::= FALSE.
{
	A.Value = true;
}*/