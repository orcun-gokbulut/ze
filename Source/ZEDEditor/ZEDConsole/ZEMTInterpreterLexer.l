#include "ZEMTInterpreterLexer.h"

#include "ZEError.h"
#include "ZEDS/ZEFormat.h"
#include "ZEMTInterpreterParser.y.h"

bool ZEMTInterpreterLexer::Scan(ZEMTInterpreterToken& Token, ZEString& Output)
{
	std:
	TokenMarker = Cursor;
	if (Cursor == Limit)
		return false;

	/*!re2c
		re2c:define:YYCTYPE = "char";
		re2c:define:YYCURSOR = Cursor;
		re2c:define:YYLIMIT = Limit;
		re2c:define:YYMARKER = Marker;
		re2c:define:YYCTXMARKER = CtxMarker;
		re2c:yyfill:enable = 0;
		re2c:indent:top = 2;
		re2c:indent:string="    ";

		IDENTIFIER = [a-zA-Z_][a-zA-Z0-9_]*;
		INTEGER = [1-9][0-9]*;
		HEXADECIMAL = '0x' [0-9a-fA-F] [0-9a-fA-F]*;
		FLOAT = [0-9][0-9]* '.' [0-9][0-9]* 'f';
		DOUBLE = [0-9][0-9]* '.' [0-9][0-9]*;
        COMMENT = "//" [^\n]* "\n";
		MULTI_LINE_COMMENT = "/*" ([^*] | ("*" [^/]))* "*""/";
        WHITE_SPACE = ([ \t\v\r] | COMMENT | MULTI_LINE_COMMENT)+;
		
		'true'
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetBoolean(true);
			return true;
		}
		
		'false'
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetBoolean(false);
			return true;
		}
		
		INTEGER
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetInt64(ZEString(Token.Text).ToInt64());
			return true;
		}

		HEXADECIMAL
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetInt64(ZEString(Token.Text).ToInt64());
			return true;
		}

		FLOAT
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetFloat(ZEString(Token.Text).ToFloat());
			return true;
		}

		DOUBLE
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_VALUE;
			Token.Value.SetFloat(ZEString(Token.Text).ToDouble());
			return true;
		}
		
		IDENTIFIER		
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_IDENTIFIER;
			return true;
		}

		'='
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_OPERATOR_ASSIGNMENT;
			return true;
		}

		'.'
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_OPERATOR_REFERENCE;
			return true;
		}

		';'
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_END_OF_STATEMENT;
			return true;
		}

		[\n]
		{
			Line++;
			LineStart = Marker;
			goto std;
		}

		WHITE_SPACE
		{
			goto std;
		}

		[^]
		{
			Output += ZEFormat::Format("Unexpected character: {0}'{0:c}'. (Line: {1}, Column: {2})\n", *TokenMarker, Line, Cursor - LineStart);
			return false;
		}
	*/

	return false;
}