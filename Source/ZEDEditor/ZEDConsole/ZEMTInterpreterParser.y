%name ZEMTInterpreterParser_

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

%parse_accept
{
	*Output += "OK\n";
}

%parse_failure 
{
    *Output += "FAILED\n";
}

%syntax_error 
{
    *Output += "  Syntax error.\n";
}

%start_symbol program

program ::= statements.
{
	*Output += "  Program.\n";
}

statements ::= statements statement.
{
	*Output += "  Multiple statement.\n";
}

statements ::= .

statement ::= END_OF_STATEMENT.
{
	*Output += "  Empty statements.\n";
}

statement ::= expression END_OF_STATEMENT.
{
	*Output += "  Statement.\n";
}

expression ::= expression OPERATOR_ASSIGNMENT VALUE END_OF_STATEMENT.
{
	*Output += "  Assignment.\n";
}

expression(A) ::= expression(B) OPERATOR_REFERENCE IDENTIFIER(C).
{
	A = new ZEMTInterpreterToken();
	A->Text = ZEFormat::Format("{0}.{1}", B->Text, C->Text);
	*Output += ZEFormat::Format("  Reference. Left:\"{0}\", Right:\"{1}\".\n", B->Text, C->Text);
}

expression ::= IDENTIFIER(Token).
{
	*Output += ZEFormat::Format("  Identifier: \"{0}\"\n", Token->Text);
}

expression ::= VALUE(Token).
{
	*Output += ZEFormat::Format("  Value: {0}.\n", Token->Value);
}