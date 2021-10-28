#include <stdio.h>
#include <stdlib.h>

struct FileResult
{
	bool Valid;
	char* Data;
	int Length;
};

struct Token
{
	int Type;
	char* Data;
	int Length;
	int LineNumber; 
};

struct Tokenizer
{
	char* At;
	int LineNumber;
};

struct TokenList
{
	Token* Data;
	size_t Count;
	size_t Size;
	size_t Incrementor;
};



static bool
IsWhitespaceOrEmpty(char c)
{
	bool result = ((c == ' ')  || (c == '\n') ||
				   (c == '\t') || (c == '\r'));
	return result;
}

static void
EatEmptySpace(Tokenizer* tk)
{
	while(tk->At && IsWhitespaceOrEmpty(tk->At[0]))
	{
		if(tk->At[0] == '\n')
			tk->LineNumber++;
		tk->At++;
	}
}

static FileResult
ReadFile(char* filename)
{
	FileResult result = {0};
	FILE* file = fopen(filename, "r");

	if(file)
	{
		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);
		rewind(file);
		
		result.Length = fileSize;
		result.Data = (char*)calloc(sizeof(char), result.Length + 1);

		if(result.Data)
		{
			fread(result.Data, sizeof(char), result.Length, file);
			result.Data[result.Length] = 0;
			result.Valid = true;
		}

		fclose(file);
	}

	return result;
}

TokenList*
CreateTokenList(size_t count)
{
	TokenList* result = (TokenList*)calloc(1, sizeof(TokenList));
	if(result)
	{
		result->Data = (Token*)calloc(count, sizeof(Token));
		if(result->Data)
		{
			result->Count = 0;
			result->Size = count;
			result->Incrementor = count;
		}
		else
		{
			free(result);
			result = 0;
		}
	}

	return result;
}

void
InsertToken(TokenList* list, Token tk)
{
	if(list->Count >= list->Size)
	{
		list->Size += list->Incrementor;
		list->Data = (Token*)realloc(list->Data, list->Size);
	}

	list->Data[list->Count++] = tk;
}


enum TokenType
{
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_AND,
	TOKEN_ASTERISK,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_SINGLE_QUOTE,
	TOKEN_DOUBLE_QUOTE,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_LANCKLE,
	TOKEN_RANCKEL,
	TOKEN_COMMA,
	TOKEN_DIVIDE,
	TOKEN_BAR,
	TOKEN_PERIOD,
	TOKEN_POUND,
	TOKEN_MINUS,
	TOKEN_PLUS,
	TOKEN_BANG,
	TOKEN_ASSIGN,
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_STRING,
	TOKEN_LESS_OR_EQUAL,
	TOKEN_GREATER_OR_EQUAL,
	TOKEN_IDENTIFIER,
	TOKEN_INTEGER,
	TOKEN_FLOAT,



	TOKEN_INVALID,
	TOKEN_EOF
};



static char
Peek(Tokenizer* tk)
{
	char c = 0;
	if(tk->At)
	{
		c = tk->At[1];
	}

	return c;
}

static bool
IsNumeric(char c)
{
	return ((c >= '0') && (c <= '9'));
}

static bool
IsAlpha(char c)
{
	return ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z'));
}

static bool
IsAlphaNumeric(char c)
{
	return IsNumeric(c) || IsAlpha(c);
}
	

static Token
NextToken(Tokenizer* tk)
{
	Token result = {0};

	EatEmptySpace(tk);
	
	switch(tk->At[0])
	{
		case ('{'):
		{
			result.Type = TOKEN_LBRACE;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('}'):
		{
			result.Type = TOKEN_RBRACE;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('('):
		{
			result.Type = TOKEN_LPAREN;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case (')'):
		{
			result.Type = TOKEN_RPAREN;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('&'):
		{
			// TODO(afb) :: Maybe special
			result.Type = TOKEN_AND;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('*'):
		{
			// TODO(afb) :: Special
			result.Type = TOKEN_ASTERISK;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;
				
		case ('['):
		{
			result.Type = TOKEN_LBRACKET;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case (']'):
		{
			result.Type = TOKEN_RBRACKET;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;
		
		case ('\''):
		{
			// TODO(afb) :: Special
			// TODO(afb) :: Considere removing. It may not be used
			
			result.Type = TOKEN_SINGLE_QUOTE;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('"'):
		{
			// TODO(afb) :: Special
			char* start = ++tk->At;
			while(tk->At[0] != '"')
			{
				tk->At++;
			}

			result.Type = TOKEN_STRING;
			result.Data = start;
			result.Length = tk->At - start;
			result.LineNumber = tk->LineNumber;
			
		}break;

		case (':'):
		{
			result.Type = TOKEN_COLON;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case (';'):
		{
			result.Type = TOKEN_SEMICOLON;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;
		
		case ('<'):
		{
			if(Peek(tk) == '=')
			{
				result.Type = TOKEN_LESS_OR_EQUAL;
				result.Data = tk->At;
				result.Length = 2;
				result.LineNumber = tk->LineNumber;
				tk->At++;
			}
			else
			{
				result.Type = TOKEN_LANCKLE;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
		}break;

		case ('>'):
		{
			if(Peek(tk) == '=')
			{
				result.Type = TOKEN_GREATER_OR_EQUAL;
				result.Data = tk->At;
				result.Length = 2;
				result.LineNumber = tk->LineNumber;
				tk->At++;
			}
			else
			{
				result.Type = TOKEN_RANCKEL;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
		}break;

		case (','):
		{
			result.Type = TOKEN_COMMA;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('/'):
		{
			// TODO(afb) :: Special
			char c = Peek(tk);
			if(c == '/')
			{
				while(tk->At && (tk->At[0] != '\n' && tk->At[0] != '\r'))
				{
					tk->At++;
				}
				result = NextToken(tk);
			}
			else if(c == '*')
			{
				tk->At++;
				while(tk->At)
				{
				}
			}
			else
			{
				result.Type = TOKEN_DIVIDE;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
		}break;

		case ('|'):
		{
			result.Type = TOKEN_BAR;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;


		case ('.'):
		{
			result.Type = TOKEN_PERIOD;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		case ('#'):
		{
			result.Type = TOKEN_POUND;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;
		
		
		case ('-'):
		{
			result.Type = TOKEN_MINUS;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;

		}break;
		
		case ('+'):
		{
			result.Type = TOKEN_PLUS;
			result.Data = tk->At;
			result.Length = 1;
			result.LineNumber = tk->LineNumber;
		}break;

		
		case ('!'):
		{
			if(Peek(tk) == '=')
			{
				result.Type = TOKEN_NOT_EQUAL;
				result.Data = tk->At;
				result.Length = 2;
				result.LineNumber = tk->LineNumber;
				tk->At++;
			}
			else
			{
				result.Type = TOKEN_BANG;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
		}break;

		case ('='):
		{
			if(Peek(tk) == '=')
			{
				result.Type = TOKEN_EQUAL;
				result.Data = tk->At;
				result.Length = 2;
				result.LineNumber = tk->LineNumber;
				tk->At++;
			}
			else
			{				
 				result.Type = TOKEN_ASSIGN;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
		}break;

		default:
		{
			char c = tk->At[0];
			if(c == '\0')
			{
				result.Type = TOKEN_EOF;
				result.Data = "EOF";
				result.Length = 3;
				result.LineNumber = tk->LineNumber;
			}
			else if(IsNumeric(c))
			{
				char* start = tk->At;
				while(IsNumeric(tk->At[0]))
				{
					tk->At++;
				}
				
				result.Data = start;
				result.Length = tk->At - start;
				result.Type = TOKEN_INTEGER;
				result.LineNumber = tk->LineNumber;
			}
			else if(IsAlpha(c) || c == '_')
			{
				char* start = tk->At;
				while(IsAlphaNumeric(tk->At[0]) || tk->At[0] == '_')
				{
					tk->At++;
				}

				result.Type = TOKEN_IDENTIFIER;
				result.Data = start;
				result.Length = tk->At - start;
				result.LineNumber = tk->LineNumber;
			}
			else
			{
				result.Type = TOKEN_INVALID;
				result.Data = tk->At;
				result.Length = 1;
				result.LineNumber = tk->LineNumber;
			}
				
		}break;
	}

	return result;
}


static TokenList*
Tokenize(char* input, int length)
{
	TokenList* result = CreateTokenList(1024);

	if(result)
	{
		Tokenizer tokenizer = {0};
		tokenizer.At = input;

		Token token;
		do
		{
			EatEmptySpace(&tokenizer);
			token = NextToken(&tokenizer);
			tokenizer.At++;
			InsertToken(result, token);


		}while((token.Type != TOKEN_EOF));
	}

	return result;
}


int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Usage: Faaat <filename>\n");
		return 1;
	}

	char* filename = argv[1];
	FileResult file = ReadFile(filename);

	if(!file.Valid)
	{
		printf("Error reading file: %s\n", filename);
		printf("Usage: Faaat <filename>\n");
		return 1;
	}


	TokenList* tokens = Tokenize(file.Data, file.Length);
	if(tokens == 0)
	{
		printf("Failed to parse file: <%s>\n", filename);
		return 1;
	}
	
	for(int i = 0; i < tokens->Count; i++)
	{
		Token token = tokens->Data[i];
		printf("<%.*s> L%d\n", token.Length, token.Data, token.LineNumber);
	}
}
