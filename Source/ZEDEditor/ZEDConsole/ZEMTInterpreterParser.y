%name ZEMTInterpreterParserParse

%token_prefix ZEMT_IT_
%extra_argument {ZEString* Output}

%left ADD SUB.
%left MUL DIV.

%token_type {ZEMTInterpreterToken*}

%include 
{
	#include "ZEMTInterpreterLexer.h"
	#include "ZEDS/ZEFormat.h"
	#include <assert.h>
}

%syntax_error 
{
    *Output += "Error: Syntax error.\n";
}

%parse_failure 
{
    *Output += "Error: Giving up.  Parser is hopelessly lost...\n";
}

%start_symbol program

program ::= statements.
{
	*Output += "WOW much program.\n";
}

statements ::= .
{
	*Output += "WOW empty statements.\n";
}

statements ::= statement.
{
	*Output += "WOW single statement statements.\n";
}

statements ::= statement statement.
{
	*Output += "WOW recursive statmenets.\n";
}

statement ::= expression SEMI_COLON.
{
	*Output += "WOW End of Statement.\n";
}

statement ::= IDENTIFIER OPERATOR_ASSIGNMENT expression END_OF_STATEMENT.
{
	*Output += "WOW assignement.\n";
}

expression(A) ::= expression(B) OPERATOR_REFERENCE IDENTIFIER(C).
{
	A = new ZEMTInterpreterToken();
	A->Text = ZEFormat::Format("{0}.{1}", B->Text, C->Text);
	*Output += ZEFormat::Format("WOW Reference. a:\"{0}\", b:\"{1}\".\n", B->Text, C->Text);
}

expression ::= INTEGER.
{
	*Output += "WOW integer.\n";
}

expression ::= IDENTIFIER.
{
	*Output += "WOW Identifier.\n";
}

expression ::= FLOAT.
{
	*Output += "WOW integer.\n";
}

expression ::= DOUBLE.
{
	*Output += "WOW integer.\n";
}


// Expression
/////////////////////////////////////////////////////////////////////////////////

/*expression ::= IDENTIFIER.

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