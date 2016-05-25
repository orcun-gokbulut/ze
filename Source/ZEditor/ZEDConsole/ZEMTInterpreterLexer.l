#include "ZEMTInterpreterLexer.h"

#include "ZEMTInterpreterParser.y.h"

bool fill(int n) 
{
 
    // is eof?
    if (ifs->eof()) 
	{
        if ((m_limit-m_cursor) <= 0) 
		{
            return false;
        }
    }
 
    int restSize = m_limit-m_token;
    if (restSize+n >= m_buffer_size) {
        // extend buffer
        m_buffer_size *= 2;
        char* newBuffer = new char[m_buffer_size];
        for (int i=0; i<restSize; ++i) { // memcpy
            *(newBuffer+i) = *(m_token+i);
        }
        m_cursor = newBuffer + (m_cursor-m_token);
        m_token = newBuffer;
        m_limit = newBuffer + restSize;
 
        delete [] m_buffer;
        m_buffer = newBuffer;
    } else {
        // move remained data to head.
        for (int i=0; i<restSize; ++i) { //memmove( m_buffer, m_token, (restSize)*sizeof(char) );
            *(m_buffer+i) = *(m_token+i);
        }
        m_cursor = m_buffer + (m_cursor-m_token);
        m_token = m_buffer;
        m_limit = m_buffer+restSize;
    }
 
    // fill to buffer
    int read_size = m_buffer_size - restSize;
    ifs->read( m_limit, read_size );
    m_limit += ifs->gcount();
 
    return true;
}

bool ZEMTInterpreterLexer::Scan(ZEMTInterpreterToken& Token)
{
	std:
    TokenStart = Cursor;
 
	/*!re2c
		re2c:define:YYCTYPE = "char";
		re2c:define:YYCURSOR = Cursor;
		re2c:define:YYMARKER = Marker;
		re2c:define:YYLIMIT = Limit;
		re2c:define:YYFILL:naked = 1;
		re2c:define:YYFILL@len = #;
		re2c:define:YYFILL = "if (!fill(#)) { return 0; }";
		re2c:yyfill:enable = 1;
		re2c:indent:top = 2;
		re2c:indent:string="    ";

		IDENTIFIER = [a-zA-Z_][a-zA-Z0-9_]*;
		INTEGER = [1-9][0-9]*;
		HEXADECIMAL = '0x' [0-9a-fA-F] [0-9a-fA-F]*;
		FLOAT = [0-9] [0-9]* '.' [0-9] [0-9]* 'f';
		DOUBLE = [0-9][0-9]* '.' [0-9][0-9]*;
        COMMENT = "//" [^\n]* "\n";
		MULTI_LINE_COMMENT = "/*" ([^*] | ("*" [^/]))* "*""/";
        WHITE_SPACE = ([ \t\v\n\r] | COMMENT | MULTI_LINE_COMMENT)+;
		
		IDENTIFIER		
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_IDENTIFIER;
			return true;
		}
		
		INTEGER
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_INTEGER;
			Token.Value.SetInt64(ZEString(Token.Text).ToInt64());
			return true;
		}

		HEXADECIMAL
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_INTEGER;
			Token.Value.SetInt64(ZEString(Token.Text).ToInt64());
			return true;
		}

		FLOAT
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_FLOAT;
			Token.Value.SetFloat(ZEString(Token.Text).ToFloat());
			return true;
		}

		DOUBLE
		{
			InitializeToken(Token);
			Token.Type = ZEMT_IT_DOUBLE;
			Token.Value.SetFloat(ZEString(Token.Text).ToDouble());
			return true;
		}

		';'
		{
			InitializeToken(Token);
			Token.Type =ZEMT_IT_SEMI_COLON;
			return true;
		}

		[\n]
		{
			Line++;
			LineStart = TokenStart;
			goto std;
		}

		WHITE_SPACE
		{
			goto std;
		}

		[^]
		{
			printf("Unexpected character: '%c(%d)'\n", *TokenStart, *TokenStart);
			return false;
		}
	*/
}