#include "ZECommandLexer.h"

int ZECommandLexer::Scan(YYSTYPE& yylval)
{
	std:
    m_token = m_cursor;
 
	/*!re2c
		re2c:define:YYCTYPE = "char";
		re2c:define:YYCURSOR = m_cursor;
		re2c:define:YYMARKER = m_marker;
		re2c:define:YYLIMIT = m_limit;
		re2c:define:YYFILL:naked = 1;
		re2c:define:YYFILL@len = #;
		re2c:define:YYFILL = "if (!fill(#)) { return 0; }";
		re2c:yyfill:enable = 1;
		re2c:indent:top = 2;
		re2c:indent:string="    ";

		INTEGER                = [1-9][0-9]*;
		WS                     = [ \r\n\t\f];
		ANY_CHARACTER          = [^];

		INTEGER {
			yylval.int_value = atoi(this->text().c_str());
			return TOKEN_INT;
		}
		"+" { return TOKEN_ADD; }
		"-" { return TOKEN_SUB; }
		"*" { return TOKEN_MUL; }
		"/" { return TOKEN_DIV; }
		WS {
			goto std;
		}
		ANY_CHARACTER {
			printf("unexpected character: '%c(%d)'\n", *m_token, *m_token);
			goto std;
		}
	*/
}