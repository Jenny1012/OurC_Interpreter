# include <sstream>
# include <stdio.h>
# include <string.h>
# include <iostream>
# include <iterator>
# include <iomanip>
# include <vector>
# include <cstdlib>
# include <string>

using namespace std ;

enum TYPE { IDENT, ADD, SUB, MUL, DIV, ASSIGN, EQU, END, GT, LT, LE, GE, NE,
            NONE, FINISH, FLOAT, INT, CHAR, BOOL, REMAINDER, EXP, NUM,
            VOID, IF, ELSE, WHILE, DO, RETURN, AND, OR, PE, ME, TE, DE, RE, PP, MM,
            RS, LS, EQ, STRING, COMMA, QUES, COLON, EXCLA, AND_1, OR_1,
            S_LEFT_PAREN, S_RIGHT_PAREN, M_LEFT_PAREN, M_RIGHT_PAREN,
            L_LEFT_PAREN, L_RIGHT_PAREN, CONSTANT 
} ;
// GT (>)大於 ,LT (<)小於 ,LE (<=)小於等於 ,GE (>=)大於等於, NE (<>)不等於, FINISH (EOF)
// END(;) 
// PE(+=), ME(-=), TE(*=), DE(/=), RE(%=), PP(++), MM(--), RS(>>), LS(<<)
// REMAINDER(%), EXP(^), QUES(?), COLON(:), EXCLA(!), COLON(:)

struct TokenType {
  string varName ;  // 變數名稱 
  int type ;
  float value ;  // 值  
  bool floatnum ;
  int column ;
  bool first ; // 看這個字是不是這行的第一個token 
  bool sysFunc ;
  bool end ;
  int line ;
} ;

class Scanner {
  public :
  void ScanFile( string input, vector< vector <TokenType> > &fullList ) ;
  TokenType GetToken() ;  // 串成token
  TokenType PeekToken() ;
  char GetNextChar() ;  // get the next char
  char GetNextNWSChar( bool &endline, int &line ) ;  // get next non white space char
  string GetIDENT( char ch ) ;   // 切出完整的IDENT
  string GetNUM( char ch, bool &floatnum ) ;    // 切出完整的NUM
  string GetOperator( char ch ) ; // 切出完整的operator 
  string GetConstant( char ch ) ; // 切出".........." 
  void ReadToEnd( char ch ) ;    // 處理註解(讀完整行不存) 
  bool IsIDENT( char ch ) ;
  bool IsNUM( char ch ) ;
  bool IsOperator( char ch ) ;
  TYPE SaveType( string word ) ;
  void GetSkipLineToken() ; // 取換行後的第一個token 
  string TypeTest( int type ) ;
};

TokenType gErrorToken ;
TokenType gNextToken ;
TokenType gSpecificToken ;
vector<TokenType> gDefineFuncIDENTList ; // 存function內定義的IDENT 
vector<TokenType> gDefineList ;  // 存定義的IDENT 
vector< vector<TokenType> > gDefineFuncList ; 
// 第一個 function name, 之後都存宣告的ident 
vector<TokenType> gTempFunc ;  // 暫存function的東西(等function宣告全部正確才存進去function) 
bool gCompound = false ;
bool gCompound_small = false ;  // IF / WHILE
bool gChangeType = false ;  // 看看有沒有不同型別同ident出現 
bool gChangeType2 = false ; // 看看有沒有同名稱的function出現 
int gErrorLine = 0 ; 
int gLine = 0 ;
int gDefinitionLine = 0 ;  // 進去定義的行數 
int gColumn = -1 ;
int gBlank = -1 ;

class Interpreter {
  public :
  bool FindDefineFunc( TokenType token ) ;
  void ThrowFunc( TokenType token ) ;
  bool FindDefineFuncIDENT( TokenType token ) ;
  void PutDefineFuncIDENT( TokenType idtype, TokenType token ) ;
  void PreStart() ;
  void PreStart2() ;
  void SaveFunction() ;
  void CheckUndefinedIDENT( TokenType token, bool &correct ) ;
  bool FindDefineIDENT( TokenType token ) ; 
  void PutDefineIDENT( TokenType idtype, TokenType token ) ;
  void CheckSysSupportFunction( TokenType token, bool &exist ) ;
  void User_Input( bool &correct ) ;
  void Definition( bool &correct ) ;
  void Type_Specifier( bool &correct ) ;
  void Function_Definition_Or_Declarators( bool &correct, TokenType idtype, TokenType varName ) ;
  void Rest_Of_Declarators( bool &correct, TokenType idtype, TokenType varName ) ;
  void Function_Definition_Without_ID( bool &correct ) ;
  void Formal_Parameter_List( bool &correct ) ;
  void Compound_Statement( bool &correct ) ;
  void Declaration( bool &correct ) ;
  void Statement( bool &correct ) ;
  void Expression( bool &correct ) ;
  void Basic_Expression( bool &correct ) ;
  void Rest_Of_Identifier_Started_Basic_Exp( bool &correct ) ;
  void Rest_Of_PPMM_Identifier_Started_Basic_Exp( bool &correct ) ;
  void Sign( bool &correct ) ;
  void Actual_Parameter_List( bool &correct ) ;
  void Assignment_Operator( bool &correct ) ;
  void Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Logical_Or_Exp( bool &correct ) ;
  void Maybe_Logical_And_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Logical_And_Exp( bool &correct ) ;
  void Maybe_Bit_Or_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Bit_Or_Exp( bool &correct ) ;
  void Maybe_Bit_Ex_Or_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Bit_Ex_Or_Exp( bool &correct ) ;
  void Maybe_Bit_And_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Bit_And_Exp( bool &correct ) ;
  void Maybe_Equality_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Equality_Exp( bool &correct ) ;
  void Maybe_Relational_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Relational_Exp( bool &correct ) ;
  void Maybe_Shift_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Shift_Exp( bool &correct ) ;
  void Maybe_Additive_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Additive_Exp( bool &correct ) ;
  void Maybe_Mult_Exp( bool &correct ) ;
  void Rest_Of_Maybe_Mult_Exp( bool &correct ) ;
  void Unary_Exp( bool &correct ) ;
  void Signed_Unary_Exp( bool &correct ) ;
  void Unsigned_Unary_Exp( bool &correct ) ;
  void BubbleSort( vector<TokenType> &funcName ) ;
  void PrintFunction() ;
  int FindLine() ;
} ;

Scanner gSList ;
Interpreter gIList ;

string Scanner :: TypeTest( int type ) {
  // 方便檢查型別 
  if ( type == 0 ) {
    return "IDENT" ;
  } // if
  else if ( type == 1 ) {
    return "ADD" ;
  } // else if
  else if ( type == 2 ) {
    return "SUB" ;
  } // else if
  else if ( type == 3 ) {
    return "MUL";
  } // else if
  else if ( type == 4 ) {
    return "DIV" ;
  } // else if
  else if ( type == 5 ) {
    return "ASSIGN";
  } // else if
  else if ( type == 6 ) {
    return "EQU"; 
  } // else if
  else if ( type == 7 ) {
    return "END" ;
  } // else if
  else if ( type == 8 ) {
    return "GT" ;
  } // else if
  else if ( type == 9 ) {
    return "LT";
  } // else if
  else if ( type == 10 ) {
    return "LE";
  } // else if
  else if ( type == 11 ) {
    return "GE" ;
  } // else if
  else if ( type == 12 ) {
    return "NE";
  } // else if
  else if ( type == 13 ) {
    return "NONE";
  } // else if
  else if ( type == 14 ) {
    return "FINISH";
  } // else if
  else if ( type == 15 ) {
    return "FLOAT";
  } // else if
  else if ( type == 16 ) {
    return "INT";
  } // else if
  else if ( type == 17 ) {
    return "CHAR";
  } // else if
  else if ( type == 18 ) {
    return "BOOL";
  } // else if
  else if ( type == 19 ) {
    return "REMAINDER";
  } // else if
  else if ( type == 20 ) {
    return "EXP";
  } // else if
  else if ( type == 21 ) {
    return "NUM";
  } // else if
  else if ( type == 22 ) {
    return "VOID";
  } // else if 
  else if ( type == 23 ) {
    return "IF";
  } // else if
  else if ( type == 24 ) {
    return "ELSE";
  } // else if
  else if ( type == 25 ) {
    return "WHILE" ;
  } // else if
  else if ( type == 26 ) {
    return "DO";
  } // else if
  else if ( type == 27 ) {
    return "RETURN";
  } // else if
  else if ( type == 28 ) {
    return "AND";
  } // else if
  else if ( type == 29 ) {
    return "OR";
  } // else if
  else if ( type == 30 ) {
    return "PE";
  } // else if
  else if ( type == 31 ) {
    return "ME";
  } // else if
  else if ( type == 32 ) {
    return "TE";
  } // else if
  else if ( type == 33 ) {
    return "DE";
  } // else if
  else if ( type == 34 ) {
    return "RE";
  } // else if
  else if ( type == 35 ) {
    return "PP";
  } // else if
  else if ( type == 36 ) {
    return "MM";
  } // else if
  else if ( type == 37 ) {
    return "RS";
  } // else if
  else if ( type == 38 ) {
    return "LS";
  } // else if
  else if ( type == 39 ) {
    return "EQ";
  } // else if 
  else if ( type == 40 ) {
    return "STRING";
  } // else if
  else if ( type == 41 ) {
    return "COMMA";
  } // else if
  else if ( type == 42 ) {
    return "QUES";
  } // else if
  else if ( type == 43 ) {
    return "COLON";
  } // else if
  else if ( type == 44 ) {
    return "EXCLA";
  } // else if
  else if ( type == 45 ) {
    return "AND_1" ;
  } // else if
  else if ( type == 46 ) {
    return "OR_1";
  } // else if
  else if ( type == 47 ) {
    return "S_LEFT_PAREN";
  } // else if
  else if ( type == 48 ) {
    return "S_RIGHT_PAREN";
  } // else if
  else if ( type == 49 ) {
    return "M_LEFT_PAREN";
  } // else if
  else if ( type == 50 ) {
    return "M_RIGHT_PAREN";
  } // else if
  else if ( type == 51 ) {
    return "L_LEFT_PAREN";
  } // else if
  else if ( type == 52 ) {
    return "L_RIGHT_PAREN";
  } // else if
  else if ( type == 53 ) {
    return "CONSTANT";
  } // else if

  return "NONE" ;
} // Scanner::TypeTest()

TYPE Scanner :: SaveType( string word ) {
  // 存型別 
  TokenType temp ;
  if ( word == "+" ) {
    return ADD ;
  } // if
  else if ( word == "-" ) {
    return SUB ;
  } // else if
  else if ( word == "*" ) {
    return MUL ;
  } // else if
  else if ( word == "/" ) {
    return DIV ;
  } // else if
  else if ( word == "%" ) {
    return REMAINDER ;
  } // else if
  else if ( word == "^" ) {
    return EXP ;
  } // else if
  else if ( word == "=" ) {
    return ASSIGN ;
  } // else if
  else if ( word == "!=" ) {
    return NE ;
  } // else if 
  else if ( word == ">" ) {
    return GT ;
  } // else if
  else if ( word == "<" ) {
    return LT ;
  } // else if
  else if ( word == ">=" ) {
    return GE ;
  } // else if
  else if ( word == "<=" ) {
    return LE ;
  } // else if
  else if ( word == ";" ) {
    return END ;
  } // else if
  else if ( word == "(" ) {
    return S_LEFT_PAREN ;
  } // else if
  else if ( word == ")" ) {
    return S_RIGHT_PAREN ;
  } // else if
  else if ( word == "[" ) {
    return M_LEFT_PAREN ;
  } // else if
  else if ( word == "]" ) {
    return M_RIGHT_PAREN ;
  } // else if
  else if ( word == "{" ) {
    return L_LEFT_PAREN ;
  } // else if
  else if ( word == "}" ) {
    return L_RIGHT_PAREN ;
  } // else if
  else if ( word == "==" ) {
    return EQU ;
  } // else if
  else if ( word == "&" ) {
    return AND_1 ;
  } // else if
  else if ( word == "|" ) {
    return OR_1 ;
  } // else if
  else if ( word == "&&" ) {
    return AND ;
  } // else if
  else if ( word == "||" ) {
    return OR ;
  } // else if
  else if ( word == "+=" ) {
    return PE ;
  } // else if
  else if ( word == "-=" ) {
    return ME ;
  } // else if
  else if ( word == "*=" ) {
    return TE ;
  } // else if
  else if ( word == "/=" ) {
    return DE ;
  } // else if
  else if ( word == "%=" ) {
    return RE ;
  } // else if
  else if ( word == "++" ) {
    return PP ;
  } // else if
  else if ( word == "--" ) {
    return MM ;
  } // else if
  else if ( word == ">>" ) {
    return RS ;
  } // else if
  else if ( word == "<<" ) {
    return LS ;
  } // else if
  else if ( word == "int" ) {
    return INT ;
  } // else if
  else if ( word == "float" ) {
    return FLOAT ;
  } // else if
  else if ( word == "char" ) {
    return CHAR ;
  } // else if
  else if ( word == "bool" ) {
    return BOOL ;
  } // else if
  else if ( word == "string" ) {
    return STRING ;
  } // else if
  else if ( word == "void" ) {
    return VOID ;
  } // else if
  else if ( word == "if" ) {
    return IF ;
  } // else if
  else if ( word == "else" ) {
    return ELSE ;
  } // else if
  else if ( word == "while" ) {
    return WHILE ;
  } // else if
  else if ( word == "do" ) {
    return DO ;
  } // else if
  else if ( word == "return" ) {
    return RETURN ;
  } // else if
  else if ( word == "true" || word == "false" ) {
    return CONSTANT ;
  } // else if
  else if ( word == "," ) {
    return COMMA ;
  } // else if
  else if ( word == "?" ) {
    return QUES ;
  } // else if
  else if ( word == ":" ) {
    return COLON ;
  } // else if
  else if ( word == "!" ) {
    return EXCLA ;
  } // else if
  else if ( ( word[0] >= 'a' && word[0] <= 'z' ) || ( word[0] >= 'A' && word[0] <= 'Z' ) ) {
    return IDENT ;
  } // else if	

  return NONE ;
} // Scanner::SaveType()

void Scanner :: GetSkipLineToken() {
  // 取換行後的第一個token 
  TokenType token ;
  while ( !gNextToken.first ) {
    token = GetToken() ;
  } // while 
} // Scanner::GetSkipLineToken()

TokenType Scanner :: GetToken() {   // 回傳一個token
  char ch, peekch ;
  TokenType currentToken = gNextToken ;
  string temp ;
  bool floatnum = false ;
  bool endline = false ;
  int line = 0 ; 
  int firstColumn ; // 第一個出現的column 

  ch = GetNextNWSChar( endline, line ) ;
  peekch = cin.peek() ;

  if ( ch == '/' && peekch == '/' ) {
    while ( ch == '/' && peekch == '/' ) {
      ReadToEnd( ch ) ;
      ch = GetNextNWSChar( endline, line ) ;
      peekch = cin.peek() ;
    } // while
  } // if
  else if ( ch == EOF ) {
    gNextToken.type = FINISH ;
  } // else if
  else if ( peekch == EOF ) {
    gNextToken.type = FINISH ;
  } // else if

  if ( IsIDENT( ch ) ) {  // 判斷IDENT(然後讀完整個token) 
    gNextToken.varName = GetIDENT( ch ) ;
    gNextToken.column = gColumn ;
    gNextToken.type = SaveType( gNextToken.varName ) ;
    gNextToken.line = gLine ;
  } // if
  else if ( IsNUM( ch ) || ch == '.' ) {   // 判斷NUM(然後讀完整個token)
    gNextToken.column = gColumn ;
    gNextToken.varName = GetNUM( ch, floatnum ) ;
    gNextToken.type = CONSTANT ;
    gNextToken.floatnum = floatnum ;
    gNextToken.line = gLine ;
  } // else if
  else if ( ch == '"' || ch == '\'' ) {  // 是CONSTANT 
    gNextToken.column = gColumn ;
    gNextToken.varName = GetConstant( ch ) ;
    gNextToken.type = CONSTANT ;
    gNextToken.line = gLine ;
  } // else if
  else if ( IsOperator( ch ) ) {
    gNextToken.column = gColumn ;
    gNextToken.varName = GetOperator( ch ) ;
    gNextToken.type = SaveType( gNextToken.varName ) ;
    gNextToken.line = gLine ;
  } // else if
  else {
    temp.push_back( ch ) ;
    ch = GetNextChar() ;
    // 改成 if 
    while ( ch != '\n' && ch != '\t' && ch != ' ' ) {
      temp.push_back( ch ) ;
      ch = GetNextChar() ;
    } // while 

    gNextToken.varName = temp ;
    gNextToken.type = SaveType( gNextToken.varName ) ; // return NONE
  } // else

  if ( endline ) {  // 看看是不是換行後的第一個token 
    gNextToken.first = true ;
    if ( line >= gBlank ) {
      gBlank = line ;
    } // if
  } // if
  else {
    gNextToken.first = false ;
  } // else

  return currentToken ;
} // Scanner::GetToken()

TokenType Scanner :: PeekToken() {  // 偷看下一個nextToken(global) 
  return gNextToken ;
} // Scanner::PeekToken()

void Scanner :: ReadToEnd( char ch ) {   // 讀到這行底(跳過)   註解、error 
  char peekch = cin.peek() ;
  while ( peekch != '\n' ) {
    ch = GetNextChar() ;
    peekch = cin.peek() ;
  } // while

  // gLine++ ;
  gColumn = -1 ;
} // Scanner::ReadToEnd()

bool Scanner :: IsIDENT( char ch ) {
  if ( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) ) {
    return true ;
  } // if

  return false ;
} // Scanner::IsIDENT()

bool Scanner :: IsNUM( char ch ) {
  if ( ch >= '0' && ch <= '9' ) {
    return true ;
  } // if

  return false ;
} // Scanner::IsNUM()

bool Scanner :: IsOperator( char ch ) {
  if ( ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '>' || ch == '<' || ch == '(' || 
       ch == ')' || ch == ';' || ch == '=' || ch == '&' || ch == '|' || ch == '%' || ch == ':' ||
       ch == '?' || ch == '!' || ch == ',' || ch == '^' || ch == '[' || ch == ']' || ch == '{' ||
       ch == '}'  ) {
    return true ;
  } // if

  return false ;
} // Scanner::IsOperator()

string Scanner :: GetIDENT( char ch ) {  // IDENT是從英文字母開頭，後面殼以接數字&底線 
  string str ;
  char peekch = cin.peek() ;

  while ( IsIDENT( peekch ) || IsNUM( peekch ) || peekch == '_' ) {
    str.push_back( ch ) ;
    ch = GetNextChar() ;
    peekch = cin.peek() ;
  } // while

  str.push_back( ch ) ;
  return str ;
} // Scanner::GetIDENT()

string Scanner :: GetNUM( char ch, bool &floatnum ) {  // ex. 6.3、 .32 
  string str ;
  char chNext;
  bool decimalPoint = false ;   // 找到小數點沒 
  bool end = false ;

  if ( ch == '.' ) {   // 開頭是小數點 
    floatnum = true ;
    str.push_back( ch ) ;
    chNext = cin.peek() ;

    while ( IsNUM( chNext ) ) {
      ch = GetNextChar() ;
      str.push_back( ch ) ;
      chNext = cin.peek() ;
    } // while
  } // if
  else if ( IsNUM( ch ) ) {
    str.push_back( ch ) ;
    chNext = cin.peek() ;

    while ( IsNUM( chNext ) || ( chNext == '.' && decimalPoint == false ) ) {
      if ( chNext == '.' ) {
        ch = GetNextChar() ;
        str.push_back( ch ) ;
        decimalPoint = true ;
        chNext = cin.peek() ;
        floatnum = true ;
      } // if
      else {
        ch = GetNextChar() ;
        str.push_back( ch ) ;
        chNext = cin.peek() ;
      } // else
    } // while
  } // else if

  return str ;
} // Scanner::GetNUM()

string Scanner :: GetOperator( char ch ) {
  string temp ;
  char peekch = cin.peek() ;
  if ( ( ch == '>' && peekch == '=' ) || ( ch == '<' && peekch == '=' ) ||
       ( ch == '=' && peekch == '=' ) || ( ch == '!' && peekch == '=' ) ||
       ( ch == '&' && peekch == '&' ) || ( ch == '|' && peekch == '|' ) ||
       ( ch == '+' && peekch == '=' ) || ( ch == '-' && peekch == '=' ) ||
       ( ch == '*' && peekch == '=' ) || ( ch == '/' && peekch == '=' ) ||
       ( ch == '%' && peekch == '=' ) || ( ch == '+' && peekch == '+' ) ||
       ( ch == '-' && peekch == '-' ) || ( ch == '>' && peekch == '>' ) ||
       ( ch == '<' && peekch == '<' ) ) {
    temp.push_back( ch ) ;
    ch = GetNextChar() ;
    temp.push_back( ch ) ;
  } // if
  else {
    temp.push_back( ch ) ;
  } // else

  return temp ;
} // Scanner::GetOperator()

string Scanner :: GetConstant( char ch ) {
  string temp ;
  char peekch = cin.peek() ;
  if ( ch == '\"' ) {
    while ( peekch != '\"' ) {
      temp.push_back( ch ) ;
      ch = GetNextChar() ;
      peekch = cin.peek() ;
    } // while
  } // if
  else if ( ch == '\'' ) {
    while ( peekch != '\'' ) {
      temp.push_back( ch ) ;
      ch = GetNextChar() ;
      peekch = cin.peek() ;
    } // while
  } // else if

  temp.push_back( ch ) ;
  peekch = cin.peek() ;
  ch = GetNextChar() ;
  temp.push_back( ch ) ;
  return temp ;
} // Scanner::GetConstant()

char Scanner :: GetNextChar() {   // 拿下一個char -> 聽說要記錄line column number 
  char ch = getchar() ;
  if ( ch != EOF ) {
    gColumn++ ;
  } // if

  return ch ;
} // Scanner::GetNextChar()

char Scanner :: GetNextNWSChar( bool &endline, int &line ) {
  // 會回傳前一行的行數 & 出現字的column位置 
  // 如果沒有換到行 preLine && firstColumn 都是 -1 
  endline = false ;  // 看看有沒有換行 
  int origin = gLine ;
  char ch = GetNextChar() ;  // 會get到ReadToEnd的換行 
  if ( ch != EOF && ch != '\0' ) {
    while ( ch == ' ' || ch == '\t' || ch == '\n' ) {
      if ( ch == '\n' ) {
        gLine++ ;
        gColumn = -1 ;
        endline = true ;
      } // if

      ch = GetNextChar();
    } // while
  } // if

  line = gLine - origin - 1 ;
  return ch ;
} // Scanner::GetNextNWSChar()

bool Interpreter :: FindDefineFunc( TokenType token ) {
  // 檢查function本身有沒有被定義 
  for ( int i = 0 ; i < gDefineFuncList.size() ; i++ ) {
    if ( token.varName == gDefineFuncList[i][1].varName ) {
      return true ;
    } // if
  } // for

  return false ;
} // Interpreter::FindDefineFunc()

bool Interpreter :: FindDefineFuncIDENT( TokenType token ) {
  // 檢查function有被定義過的IDENT 
  for ( int i = 0 ; i < gDefineFuncIDENTList.size() ; i++ ) {
    if ( token.varName == gDefineFuncIDENTList[i].varName ) {
      return true ;
    } // if
  } // for

  return false ;
} // Interpreter::FindDefineFuncIDENT()

void Interpreter :: PutDefineFuncIDENT( TokenType idtype, TokenType token ) {
  // 把在跑function的定義存進來 
  if ( !FindDefineFuncIDENT( token ) ) {
    token.type = idtype.type ;
    gDefineFuncIDENTList.push_back( token ) ;
  } // if
  else {
    gChangeType = true ;
    for ( int i = 0 ; i < gDefineFuncIDENTList.size() ; i++ ) {
      if ( token.varName == gDefineFuncIDENTList[i].varName ) {
        if ( gDefineFuncIDENTList[i].type != NONE ) {
          gDefineFuncIDENTList[i].type = idtype.type ;
        } // if
      } // if
    } // for
  } // else
} // Interpreter::PutDefineFuncIDENT()

bool Interpreter :: FindDefineIDENT( TokenType token ) {
  // 檢查全域定義 
  for ( int i = 0 ; i < gDefineList.size() ; i++ ) {
    if ( token.varName == gDefineList[i].varName ) {
      return true ;
    } // if
  } // for

  return false ;
} // Interpreter::FindDefineIDENT()

void Interpreter :: PutDefineIDENT( TokenType idtype, TokenType token ) { 
  // idtype --> 型別, token --> 變數名稱 
  if ( !FindDefineIDENT( token ) ) {  // 沒被定義過
    token.type = idtype.type ;
    gDefineList.push_back( token ) ; 
  } // if
  else {   // 改型別 
    gChangeType = true ;
    for ( int i = 0 ; i < gDefineList.size() ; i++ ) {
      if ( token.varName == gDefineList[i].varName ) {
        gDefineList[i].type = idtype.type ;
      } // if
    } // for
  } // else 
} // Interpreter::PutDefineIDENT()

void Interpreter :: CheckSysSupportFunction( TokenType token, bool &exist ) {
  // ListAllVariables() ; ListAllFunctions() ; ListVariable( char name[] ) ;
  // ListFunction( char name[] ) ; Done() ;
  if ( token.varName == "ListAllVariables" || token.varName == "ListAllFunctions" || 
       token.varName == "ListVariable" || token.varName == "ListFunction" ||
       token.varName == "Done" ) {
    exist = true ;
    return ;
  } // if

  exist = false ;
  return ;
} // Interpreter::CheckSysSupportFunction()

void Interpreter :: SaveFunction() {
  // 把function內所有的東西存入vector
  vector< vector<TokenType> > temp ;
  vector<TokenType> v ;
  int loc = 0 ;
  TokenType idtype = gTempFunc[0] ; // return type
  TokenType funcName = gTempFunc[1] ;  // funcName
  PutDefineIDENT( idtype, funcName ) ; // 把function name塞進去ident list
  gChangeType = false ;

  for ( int i = 0 ; i < gDefineFuncList.size() ; i++ ) {
    if ( funcName.varName == gDefineFuncList[i][1].varName ) {
      loc = i ;
      gChangeType2 = true ;
    } // if
  } // for

  if ( gChangeType2 ) {
    gDefineFuncList.erase( gDefineFuncList.begin() + loc ) ;
    gDefineFuncList.push_back( gTempFunc ) ;
    cout << "> New definition of " << funcName.varName << "() entered ..." << endl ;
    gChangeType2 = false ;
    gCompound = false ;
    gTempFunc.clear() ;
  } // if
  else {
    gDefineFuncList.push_back( gTempFunc ) ;
    gTempFunc.clear() ;
    gCompound = false ;
    cout << "> Definition of " << funcName.varName << "() entered ..." << endl ;
  } // elsese ;
} // Interpreter::SaveFunction()

void Interpreter  :: ThrowFunc( TokenType token ) {
  if ( token.varName == "Done" ) {
    throw "Done" ;
  } // if
  else if ( token.varName == "ListAllVariables" ) {
    throw "ListAllVariables" ;
  } // else if
  else if ( token.varName == "ListAllFunctions" ) {
    throw "ListAllFunctions" ;
  } // else if
  else if ( token.varName == "ListVariable" ) {
    throw "ListVariable" ;
  } // else if
  else if ( token.varName == "ListFunction" ) {
    throw "ListFunction" ;
  } // else if
} // Interpreter::ThrowFunc()

void Interpreter :: User_Input( bool &correct ) {
  // 1 ( definition | statement ) { definition | statement }
  bool definition_correct, statement_correct, sysFunc ;
  string sysFuncName ;
  TokenType token ;
  definition_correct = false ;
  statement_correct = false ;
  gChangeType = false ;

  gDefinitionLine = gLine ;
  Definition( definition_correct ) ;
  if ( !definition_correct ) {
    gDefinitionLine = gLine ;
    Statement( statement_correct ) ;
  } // if

  if ( definition_correct && gCompound ) {
    SaveFunction() ;
  } // if
  else if ( statement_correct ) {
    cout << "> Statement executed ..." << endl ;
  } // else if
  else if ( !definition_correct && !statement_correct ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // else if

  while ( definition_correct || statement_correct ) {
    gCompound = false ;
    gCompound_small = false ;
    definition_correct = false ;
    statement_correct = false ;
    gTempFunc.clear() ;

    gDefinitionLine = gLine ;
    Definition( definition_correct ) ;
    if ( !definition_correct ) {
      gDefinitionLine = gLine ;
      Statement( statement_correct ) ;
    } // if

    if ( definition_correct && gCompound ) {
      SaveFunction() ;
    } // if
    else if ( statement_correct ) {
      cout << "> Statement executed ..." << endl ;
    } // else if
    else if ( !definition_correct && !statement_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // else if
  } // while

  correct = true ;
} // Interpreter::User_Input()

void Interpreter :: Definition( bool &correct ) {
  // 2 VOID Identifier function_definition_without_ID
  // | type_specifier Identifier function_definition_or_declaratiors
  // 先塞進去，看看是不是function_definition_without_ID再清 
  bool function_definition_without_ID_correct, type_specifier_correct ;
  bool function_definition_or_declarators_correct ;
  TokenType token, nextToken, ident ;
  TokenType varName ;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != VOID ) {
    Type_Specifier( type_specifier_correct ) ;
    if ( !type_specifier_correct ) {
      correct = false ;
      return ;
    } // if
  } // if

  if ( nextToken.type == VOID ) {
    token = gSList.GetToken() ;  // VOID
    gTempFunc.push_back( token ) ;

    if ( gNextToken.type != IDENT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
    // definition check
    varName = gSList.GetToken() ; // IDENT
    gTempFunc.push_back( varName ) ; 
    Function_Definition_Without_ID( function_definition_without_ID_correct ) ;
    if ( !function_definition_without_ID_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      gTempFunc.clear() ;
      throw "unexpected" ;
    } // if
  } // if
  else if ( type_specifier_correct ) {
    if ( gNextToken.type != IDENT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      gTempFunc.clear() ;
      throw "unexpected" ;
    } // if
    // definition check
    token = gSList.GetToken() ; // IDENT
    gTempFunc.push_back( token ) ;
    Function_Definition_Or_Declarators( function_definition_or_declarators_correct, nextToken, token ) ;
    // nextToken --> 型別, token --> 變數名稱 
    if ( !function_definition_or_declarators_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // else if

  correct = true ; 
} // Interpreter::Definition()

void Interpreter :: Type_Specifier( bool &correct ) {
  // 3 INT | CHAR | FLOAT | STRING | BOOL
  if ( gNextToken.type != INT && gNextToken.type != CHAR && gNextToken.type != FLOAT &&
       gNextToken.type != STRING && gNextToken.type != BOOL ) {
    correct = false ;
    return ;
  } // if

  TokenType token = gSList.GetToken() ;
  gTempFunc.push_back( token ) ;
  correct = true ;
  return ;
} // Interpreter::Type_Specifier()

void Interpreter :: Function_Definition_Or_Declarators( bool &correct, TokenType idtype, 
                                                        TokenType varName ) {
  // 4 function_definition_without_ID | rest_of_declarators
  // function宣告 | 變數宣告 
  bool function_definition_without_ID_correct, rest_of_declarators_correct ;
  Function_Definition_Without_ID( function_definition_without_ID_correct ) ;
  if ( !function_definition_without_ID_correct ) {
    gTempFunc.clear() ; // 不是function(前面會多存type_specifier IDENT)  
    gCompound = false ;  // 不是function 
    Rest_Of_Declarators( rest_of_declarators_correct, idtype, varName ) ;
  } // if

  correct = true ;
} // Interpreter::Function_Definition_Or_Declarators()

void Interpreter :: Rest_Of_Declarators( bool &correct, TokenType idtype, TokenType varName ) {
  // 5 [ '[' CONSTANT ']' ]
  // { ',' Identifier [ '[' CONSTANT ']' ] } ';'
  TokenType token ;
  vector<TokenType> tokens ;  // 暫存宣告過的IDENT (多重宣告)
  varName.type = idtype.type ; // 存型別 
  if ( gNextToken.type != M_LEFT_PAREN && gNextToken.type != COMMA && gNextToken.type != END ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  // if ( compound || compound_small ) tempFunc.push_back( varName ) ;
  if ( gNextToken.type == M_LEFT_PAREN ) {
    token = gSList.GetToken() ;  // M_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != CONSTANT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ; // CONSTANT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != M_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ; // M_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ; 
  } // if 

  tokens.push_back( varName ) ; // 存第一個token(確定是對的)
  if ( gCompound || gCompound_small ) PutDefineFuncIDENT( idtype, varName ) ;

  if ( gNextToken.type == COMMA ) {
     // { ',' Identifier [ '[' CONSTANT ']' ] } ';'
    while ( gNextToken.type == COMMA ) {
      token = gSList.GetToken() ;  // COMMA
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;  
      if ( gNextToken.type != IDENT ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      varName = gSList.GetToken() ;  // IDENT
      varName.type = idtype.type ;
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type == M_LEFT_PAREN ) {
        token = gSList.GetToken() ;  // M_LEFT_PAREN
        if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
        if ( gNextToken.type != CONSTANT ) {
          gErrorLine = gLine ;
          gErrorToken = gNextToken ;
          throw "unexpected" ;
        } // if

        token = gSList.GetToken() ; // CONSTANT
        if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ; 
        if ( gNextToken.type != M_RIGHT_PAREN ) {
          gErrorLine = gLine ;
          gErrorToken = gNextToken ;
          throw "unexpected" ;
        } // if

        token = gSList.GetToken() ; // M_RIGHT_PAREN
        if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ; 
      } // if

      tokens.push_back( varName ) ; // 一起宣告的IDENT先存到tokens裡面 
      if ( gCompound || gCompound_small ) PutDefineFuncIDENT( idtype, varName ) ;  
    } // while
  } // if

  if ( gNextToken.type != END ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if
  else {
    token = gSList.GetToken() ;
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  } // else

  correct = true ;
  if ( !gCompound ) {
    for ( int i = 0 ; i < tokens.size() ; i++ ) {
      PutDefineIDENT( idtype, tokens[i] ) ;
      if ( gChangeType ) {
        cout << "> New definition of " ;
      } // if
      else {
        cout << "> Definition of " ;
      } // else

      PutDefineIDENT( idtype, tokens[i] ) ;
      if ( i < tokens.size() - 1 ) {
        cout << tokens[i].varName << ", " ;
      } // if
      else {
        cout << tokens[i].varName ;
      } // else

      cout << " entered ..." << endl ;
      gChangeType = false ;
    } // for

    tokens.clear() ;
  } // if
} // Interpreter::Rest_Of_Declarators()

void Interpreter :: Function_Definition_Without_ID( bool &correct ) {
  // 6 '(' [ VOID | formal_parameter_list ] ')' compound_statement
  // 一定是function 
  bool formal_parameter_list_correct, compound_statement_correct ;
  TokenType token, nextToken ;
  if ( gNextToken.type != S_LEFT_PAREN ) {  // 讓它回definition在throw 
    correct = false ;
    return ;
  } // if

  gCompound = true ;  // 確定是function 
  token = gSList.GetToken() ; // S_LEFT_PAREN 
  gTempFunc.push_back( token ) ;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != VOID ) {
    Formal_Parameter_List( formal_parameter_list_correct ) ;  // 沒也沒差 
  } // if
  else if ( nextToken.type == VOID ) {
    token = gSList.GetToken() ;  // VOID
    gTempFunc.push_back( token ) ;
  } // else if

  if ( gNextToken.type != S_RIGHT_PAREN ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  token = gSList.GetToken() ; // S_RIGHT_PAREN
  gTempFunc.push_back( token ) ;
  Compound_Statement( compound_statement_correct ) ; 
  if ( !compound_statement_correct ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  correct = true ;
} // Interpreter::Function_Definition_Without_ID()

void Interpreter :: Formal_Parameter_List( bool &correct ) {
  // 7 type_specifier [ '&' ] Identifier [ '[' Constant ']' ]
  // { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
  // 直接存進gDefineFuncIDENTList 
  bool type_specifier_correct ;
  TokenType token, ident, idtype ;
  idtype = gSList.PeekToken() ;
  Type_Specifier( type_specifier_correct ) ;
  if ( !type_specifier_correct ) {
    correct = false ;  // 不能throw，因為function_definition_without_ID可能是空的 
    return ;
  } // if

  if ( gNextToken.type == AND_1 ) {
    token = gSList.GetToken() ;
    if ( gCompound ) gTempFunc.push_back( token ) ;
  } // if

  if ( gNextToken.type != IDENT ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  ident = gSList.GetToken() ;  // IDENT
  if ( gCompound ) gTempFunc.push_back( ident ) ;

  if ( gNextToken.type != M_LEFT_PAREN && gNextToken.type != COMMA ) {
    if ( gCompound ) PutDefineFuncIDENT( idtype, ident ) ;
    correct = true ;
    return ;
  } // if
  else if ( gNextToken.type == M_LEFT_PAREN ) {
    token = gSList.GetToken() ; // M_LEFT_PAREN
    if ( gCompound ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != CONSTANT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ; // CONSTANT
    if ( gCompound ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != M_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ; // M_RIGHT_PAREN  
    if ( gCompound ) gTempFunc.push_back( token ) ;
    if ( gCompound ) PutDefineFuncIDENT( idtype, ident ) ;
  } // else if

  // { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
  while ( gNextToken.type == COMMA ) {
    token = gSList.GetToken() ;  // COMMA
    if ( gCompound ) gTempFunc.push_back( token ) ;
    Type_Specifier( type_specifier_correct ) ;
    if ( !type_specifier_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type == AND_1 ) {
      token = gSList.GetToken() ;
      if ( gCompound ) gTempFunc.push_back( token ) ;
    } // if

    if ( gNextToken.type != IDENT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    ident = gSList.GetToken() ; // IDENT
    if ( gCompound ) gTempFunc.push_back( ident ) ;
    if ( gNextToken.type == M_LEFT_PAREN ) {
      token = gSList.GetToken() ; // M_LEFT_PAREN
      if ( gCompound ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type != CONSTANT ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ; // CONSTANT
      if ( gCompound ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type != M_RIGHT_PAREN ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ; // M_RIGHT_PAREN
      if ( gCompound ) gTempFunc.push_back( token ) ;
    } // if

    if ( gCompound ) PutDefineFuncIDENT( idtype, ident ) ;
  } // while

  correct = true ;
} // Interpreter::Formal_Parameter_List()

void Interpreter :: Compound_Statement( bool &correct ) {
  // 8 '{' { declaration | statement } '}'
  // sysFunc -> 判斷是否為五大function(會進statement) 
  string sysFuncName ;
  bool declaration_correct, statement_correct, sysFunc ;
  if ( gNextToken.type != L_LEFT_PAREN ) { 
    correct = false ;
    return ;
  } // if

  TokenType token = gSList.GetToken() ;  // L_LEFT_PAREN
  if ( gCompound ) gTempFunc.push_back( token ) ;

  Declaration( declaration_correct ) ;
  if ( !declaration_correct ) {
    Statement( statement_correct ) ;
  } // if

  while ( declaration_correct || statement_correct ) {
    Declaration( declaration_correct ) ;
    if ( !declaration_correct ) {
      Statement( statement_correct ) ;
    } // if
  } // while

  if ( gNextToken.type != L_RIGHT_PAREN ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if 

  token = gSList.GetToken() ;  // L_RIGHT_PAREN
  if ( gCompound ) gTempFunc.push_back( token ) ;
  correct = true ;
} // Interpreter::Compound_Statement()

void Interpreter :: Declaration( bool &correct ) {
  // 9 type_specifier Identifier rest_of_declarators
  // 一定是function內容 --> true 
  bool rest_of_declarators_correct, type_specifier_correct ;
  TokenType token, idtype ;

  idtype = gSList.PeekToken() ;
  Type_Specifier( type_specifier_correct ) ;
  if ( !type_specifier_correct ) {  // 回compound_statement再判斷 
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != IDENT ) { 
    gErrorToken = gNextToken ; 
    gErrorLine = gLine ;
    gTempFunc.clear() ;
    throw "unexpected" ;
  } // if

  token = gSList.GetToken() ; // IDENT(進Rest_Of_Declarators存tempFunc) 
  if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  Rest_Of_Declarators( rest_of_declarators_correct, idtype, token ) ; 
  if ( !rest_of_declarators_correct ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    gTempFunc.clear() ;
    throw "unexpected" ;
  } // if
  else {
    if ( gCompound ) {  // 如果rest_of_declarators是對的才塞進去 
      PutDefineFuncIDENT( idtype, token ) ;
    } // if
  } // else

  correct = true ;
} // Interpreter::Declaration()

void Interpreter :: Statement( bool &correct ) {
  // 10 ';'
  // | expression ';'
  // | RETURN [ expression ] ';'
  // | compound_statement
  // | IF '(' expression ')' statement [ ELSE statement ]
  // | WHILE  '(' expression ')' statement
  // | DO statement WHILE '(' expression ')' ';'
  TokenType token, nextToken ;
  bool expression_correct, compound_statement_correct, statement_correct ;

  nextToken = gSList.PeekToken() ; 
  if ( nextToken.type != END && nextToken.type != RETURN && nextToken.type != IF &&
       nextToken.type != WHILE && nextToken.type != DO ) {
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      Compound_Statement( compound_statement_correct ) ;
      if ( !compound_statement_correct ) {  // statement可以是空的 ( user_input & compound_statement )
        correct = false ;
        return ;
      } // if
    } // if
  } // if

  if ( nextToken.type == END ) {
    token = gSList.GetToken() ;  // END
    if ( gCompound ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // if
  else if ( expression_correct ) {
    // expression ';'
    if ( gNextToken.type != END ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // END
    if ( gCompound ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == RETURN ) {
    // RETURN [ expression ] ';'
    token = gSList.GetToken() ;  // RETURN
    if ( gCompound ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( gNextToken.type != END ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ; // END 
    if ( gCompound ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
  else if ( compound_statement_correct ) {
    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == IF || nextToken.type == WHILE ) {
    // IF '(' expression ')' statement [ ELSE statement ]
    // WHILE '(' expression ')' statement
    gCompound_small = true ;
    token = gSList.GetToken() ;  // IF / WHILE
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != S_LEFT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if 

    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( gNextToken.type != S_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;   // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Statement( statement_correct ) ;
    if ( !statement_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( nextToken.type == IF && gNextToken.type == ELSE ) {
      token = gSList.GetToken() ;  // ELSE
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      Statement( statement_correct ) ;
      if ( !statement_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if
    } // if

    gCompound_small = false ;
    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == DO ) {
    // DO statement WHILE '(' expression ')' ';'
    gCompound_small = true ;
    token = gSList.GetToken() ;  // DO
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Statement( statement_correct ) ;
    if ( !statement_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != WHILE ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // WHILE
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != S_LEFT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( gNextToken.type != S_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != END ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // END
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    gCompound_small = false ;
    correct = true ;
    return ;
  } // else if
} // Interpreter::Statement()

void Interpreter :: Expression( bool &correct ) {
  // 11 basic_expression { ',' basic_expression }
  bool basic_expression_correct;
  Basic_Expression( basic_expression_correct ) ;
  if ( !basic_expression_correct ) {
    correct = false ;  // 因為在statement還要判斷( expression; )，所以不能直接throw 
    return ;
  } // if

  while ( gNextToken.type == COMMA ) {
    TokenType token = gSList.GetToken() ; // COMMA
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Basic_Expression( basic_expression_correct ) ;
    if ( !basic_expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Expression()

void Interpreter :: Basic_Expression( bool &correct ) {
  // 12 Identifier rest_of_Identifier_started_basic_exp 
  //    | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
  //    | sign { sign } signed_unary_exp romce_and_romloe
  //    ( Contant | '(' expression ')' ) romce_and_romloe
  bool sign_correct, sysFunc ;
  TokenType token, func, nextToken;
  bool rest_of_PPMM_Identifier_started_basic_exp_correct = false ; 
  bool signed_unary_exp_correct, romce_and_romloe_correct ;
  bool rest_of_Identifier_started_basic_exp_correct = false ;
  bool expression_correct;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != IDENT && nextToken.type != PP && nextToken.type != MM &&
       nextToken.type != CONSTANT && nextToken.type != S_LEFT_PAREN ) {
    Sign( sign_correct ) ;
    if ( !sign_correct ) {
      Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
      if ( !romce_and_romloe_correct ) {
        correct = false ;  // 因為在statement還要判斷( expression; )，所以不能直接throw 
        return ;
      } // if
    } // if
  } // if

  if ( nextToken.type == PP || nextToken.type == MM ) {
    // ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
    token = gSList.GetToken() ;  // PP | MM
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != IDENT ) {
      gErrorToken = gNextToken ;
      gErrorLine = gLine ;
      throw "unexpected" ;
    }  // if

    if ( gCompound ) {
      if ( !FindDefineFuncIDENT( nextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = nextToken ;
        throw "undefined" ;
      } // if
    } // if
    else {
      if ( !FindDefineIDENT( nextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = nextToken ;
        throw "undefined" ;
      } // if
    } // else

    token = gSList.GetToken() ;  // IDENT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_PPMM_Identifier_Started_Basic_Exp( rest_of_PPMM_Identifier_started_basic_exp_correct ) ;
    TokenType empty ;
    empty.type = NONE ;
    if ( gCompound ) PutDefineFuncIDENT( empty, token ) ;
    correct = true ;
    return ;
  } // if
  else if ( nextToken.type == IDENT ) {
    // Identifier rest_of_Identifier_started_basic_exp 
    if ( gCompound ) {
      if ( !FindDefineFuncIDENT( nextToken ) && !FindDefineFunc( nextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = nextToken ;
        throw "undefined" ;
      } // if
    } // if
    else {
      if ( !FindDefineIDENT( nextToken ) && !FindDefineFunc( nextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = nextToken ;
        throw "undefined" ;
      } // if
    } // else

    func = gSList.GetToken() ;  // function name
    gSpecificToken = func ;  // 存function的參數名稱  
    if ( gCompound || gCompound_small ) gTempFunc.push_back( func ) ;
    CheckSysSupportFunction( func, sysFunc ) ; // 檢查系統提供功能 
    Rest_Of_Identifier_Started_Basic_Exp( rest_of_Identifier_started_basic_exp_correct ) ;
    if ( !rest_of_Identifier_started_basic_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( FindDefineIDENT( func ) && rest_of_Identifier_started_basic_exp_correct && sysFunc ) {
      ThrowFunc( func ) ;
    } // if
 
    correct = true ;
    return ;
  } // else if
  else if ( sign_correct ) {
    // sign { sign } signed_unary_exp romce_and_romloe
    Sign( sign_correct ) ;
    while ( sign_correct ) {
      Sign( sign_correct ) ;
    } // while

    Signed_Unary_Exp( signed_unary_exp_correct ) ;  // 直接throw 
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == CONSTANT ) {
    // ( Contant ) romce_and_romloe
    token = gSList.GetToken() ;  // CONSTANT
    // 截指定function name 
    string function = token.varName.substr( 1, token.varName.size() - 2 ) ;
    gSpecificToken.varName = function ;

    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == S_LEFT_PAREN ) {
    // ( '(' expression ')' ) romce_and_romloe
    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != S_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
} // Interpreter::Basic_Expression()

void Interpreter :: Rest_Of_Identifier_Started_Basic_Exp( bool &correct ) {
  // 13 [ '[' expression ']' ] ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )  ok
  //  |  '(' [ actual_parameter_list ] ')' romce_and_romloe
  TokenType token, nextToken ;
  string sysFuncName ;
  bool romce_and_romloe_correct = false ;
  bool expression_correct = false ;
  bool basic_expression_correct = false ;
  bool actual_parameter_list_correct = false ;
  bool assignment_operator_correct = false ;
  bool sysFunc = false ;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != M_LEFT_PAREN && nextToken.type != S_LEFT_PAREN &&
       nextToken.type != PP && nextToken.type != MM ) {
    Assignment_Operator( assignment_operator_correct ) ;
    if ( !assignment_operator_correct ) {
      Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
      if ( !romce_and_romloe_correct ) {
        correct = false ;
        return ;
      } // if
    } // if
  } // if

  // [ '[' expression ']' ] ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
  if ( nextToken.type == M_LEFT_PAREN ) {
    token = gSList.GetToken() ;  // M_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != M_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // M_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;

    Assignment_Operator( assignment_operator_correct ) ;
    if ( !assignment_operator_correct ) {
      if ( gNextToken.type != PP && gNextToken.type != MM ) {
        Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
        // 可以有也可以沒有 
      } // if
    } // if

    if ( gNextToken.type == PP || gNextToken.type == MM ) {
      token = gSList.GetToken() ;  // PP | MM
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    } // if

    if ( romce_and_romloe_correct ) {
      correct = true ;
      return ;
    } // if
    else if ( assignment_operator_correct ) {
      Basic_Expression( basic_expression_correct ) ;
      if ( !basic_expression_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      correct = true ;
      return ;
    } // else if
  } // if
  else if ( nextToken.type == S_LEFT_PAREN ) {
    // '(' [ actual_parameter_list ] ')' romce_and_romloe
    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != S_RIGHT_PAREN ) {
      Actual_Parameter_List( actual_parameter_list_correct ) ;  // 有沒有都行 
      if ( gNextToken.type != S_RIGHT_PAREN ) {
        gErrorToken = gNextToken ;
        gErrorLine = gLine ;
        throw "unexpected" ;
      } // if
    } // if

    token = gSList.GetToken() ;  // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
  else if ( assignment_operator_correct ) {
    // assignment_operator basic_expression
    Basic_Expression( basic_expression_correct ) ;
    if ( !basic_expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == PP || nextToken.type == MM ) {
    // [ PP | MM ] romce_and_romloe
    token = gSList.GetToken() ;  // PP | MM
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    correct = true ;
    return ;
  } // else if
  else if ( romce_and_romloe_correct ) {
    correct = true ;
    return ;
  } // else if
} // Interpreter::Rest_Of_Identifier_Started_Basic_Exp()

void Interpreter :: Rest_Of_PPMM_Identifier_Started_Basic_Exp( bool &correct ) {
  // 14 [ '[' expression ']' ] romce_and_romloe ok
  TokenType token, nextToken ;
  string sysFuncName ;
  bool romce_and_romloe_correct, expression_correct, sysFunc ;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != M_LEFT_PAREN ) {
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      correct = false ;
      return ;
    } // if
  } // if

  if ( nextToken.type == M_LEFT_PAREN ) {
    token = gSList.GetToken() ;  // M_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( gNextToken.type != M_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // M_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( romce_and_romloe_correct ) ;
    if ( !romce_and_romloe_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // if
  else if ( romce_and_romloe_correct ) {
    correct = true ;
    return ;
  } // else if
} // Interpreter::Rest_Of_PPMM_Identifier_Started_Basic_Exp()

void Interpreter :: Sign( bool &correct ) {
  // 15 sign : '+' | '-' | '!'   ok
  if ( gNextToken.type != ADD && gNextToken.type != SUB && gNextToken.type != EXCLA ) {
    correct = false ;
    return ;
  } // if

  TokenType token = gSList.GetToken() ;
  if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  correct = true ;
  return ;
} // Interpreter::Sign()

void Interpreter :: Actual_Parameter_List( bool &correct ) {
  // 16 basic_expression { ',' basic_expression }  ok
  TokenType token ;
  string sysFuncName ;
  bool basic_expression_correct, sysFunc ;
  Basic_Expression( basic_expression_correct ) ;
  if ( !basic_expression_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != COMMA ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == COMMA ) {
    token = gSList.GetToken() ;
    Basic_Expression( basic_expression_correct ) ;
    if ( !basic_expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
  return ;
} // Interpreter::Actual_Parameter_List()

void Interpreter :: Assignment_Operator( bool &correct ) {
  // 17 '=' | TE | DE | RE | PE | ME  ok
  if ( gNextToken.type != ASSIGN && gNextToken.type != TE && gNextToken.type != DE && 
       gNextToken.type != RE && gNextToken.type != PE && gNextToken.type != ME ) {
    correct = false ;
    return ;
  } // if

  TokenType token = gSList.GetToken() ;
  if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  correct = true ;
} // Interpreter::Assignment_Operator()

void Interpreter :: Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp( bool &correct ) {
  // 即 romce_and_romloe 
  // 18 rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]   ok
  TokenType token ;
  string sysFuncName ;
  bool rest_of_maybe_logical_or_exp_correct, basic_expression_correct, sysFunc ;
  
  Rest_Of_Maybe_Logical_Or_Exp( rest_of_maybe_logical_or_exp_correct ) ;
  if ( !rest_of_maybe_logical_or_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != QUES ) {
    correct = true ;
    return ;
  } // if

  token = gSList.GetToken() ;  // QUES
  if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  Basic_Expression( basic_expression_correct ) ;
  if ( !basic_expression_correct ) {
    gErrorLine = gLine ;  
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  if ( gNextToken.type != COLON ) {
    gErrorLine = gLine ; 
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  token = gSList.GetToken() ;  // COLON
  if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
  Basic_Expression( basic_expression_correct ) ;
  if ( !basic_expression_correct ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Conditional_Exp_And_Rest_Of_Maybe_Logical_Or_Exp()

void Interpreter :: Rest_Of_Maybe_Logical_Or_Exp( bool &correct ) {
  // 19 rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }  ok
  TokenType token ;
  bool rest_of_maybe_logical_and_exp_correct, maybe_logical_and_exp_correct ;
  Rest_Of_Maybe_Logical_And_Exp( rest_of_maybe_logical_and_exp_correct ) ;
  if ( !rest_of_maybe_logical_and_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != OR ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == OR ) {
    token = gSList.GetToken() ;  // OR
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Logical_And_Exp( maybe_logical_and_exp_correct ) ;
    if ( !maybe_logical_and_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Logical_Or_Exp()

void Interpreter :: Maybe_Logical_And_Exp( bool &correct ) {
  // 20 maybe_bit_OR_exp { AND maybe_bit_OR_exp }  ok
  TokenType token ;
  bool maybe_bit_or_exp_correct ;
  Maybe_Bit_Or_Exp( maybe_bit_or_exp_correct ) ;
  if ( !maybe_bit_or_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != AND ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == AND ) {
    token = gSList.GetToken() ;  // AND
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_Or_Exp( maybe_bit_or_exp_correct ) ;
    if ( !maybe_bit_or_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Maybe_Logical_And_Exp()

void Interpreter :: Rest_Of_Maybe_Logical_And_Exp( bool &correct ) {
  // 21 rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }  ok 
  TokenType token ;
  bool rest_of_maybe_bit_or_exp_correct, maybe_bit_or_exp_correct ;
  Rest_Of_Maybe_Bit_Or_Exp( rest_of_maybe_bit_or_exp_correct ) ;
  if ( !rest_of_maybe_bit_or_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != AND ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == AND ) {
    token = gSList.GetToken() ;  // AND
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_Or_Exp( maybe_bit_or_exp_correct ) ;
    if ( !maybe_bit_or_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Logical_And_Exp()

void Interpreter :: Maybe_Bit_Or_Exp( bool &correct ) {
  // 22 maybe_bit_ex_OR_exp { '|' maybe_bit_ex_or_exp }  ok
  TokenType token ;
  bool maybe_bit_ex_or_exp_correct ;
  Maybe_Bit_Ex_Or_Exp( maybe_bit_ex_or_exp_correct ) ;
  if ( !maybe_bit_ex_or_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != OR_1 ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == OR_1 ) {
    token = gSList.GetToken() ;  // OR_1
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_Ex_Or_Exp( maybe_bit_ex_or_exp_correct );
    if ( !maybe_bit_ex_or_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Maybe_Bit_Or_Exp()

void Interpreter :: Rest_Of_Maybe_Bit_Or_Exp( bool &correct ) {
  // 23 rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_or_exp }  ok
  TokenType token ;
  bool rest_of_maybe_bit_ex_or_exp_correct, maybe_bit_ex_or_exp_correct ;
  Rest_Of_Maybe_Bit_Ex_Or_Exp( rest_of_maybe_bit_ex_or_exp_correct ) ;
  if ( !rest_of_maybe_bit_ex_or_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != OR_1 ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == OR_1 ) {
    token = gSList.GetToken() ;  // OR_1
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_Ex_Or_Exp( maybe_bit_ex_or_exp_correct );
    if ( !maybe_bit_ex_or_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Bit_Or_Exp()

void Interpreter :: Maybe_Bit_Ex_Or_Exp( bool &correct ) {
  // 24 maybe_bit_AND_exp { '^' maybe_bit_AND_exp }  ok 
  TokenType token ;
  bool maybe_bit_and_exp_correct ;
  Maybe_Bit_And_Exp( maybe_bit_and_exp_correct ) ;
  if ( !maybe_bit_and_exp_correct ) {
    correct = false ;
    return ;
  }  // if

  if ( gNextToken.type != EXP ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == EXP ) {
    token = gSList.GetToken() ;  // EXP
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_And_Exp( maybe_bit_and_exp_correct ) ;
    if ( !maybe_bit_and_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Maybe_Bit_Ex_Or_Exp()

void Interpreter :: Rest_Of_Maybe_Bit_Ex_Or_Exp( bool &correct ) {
  // 25 rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }  ok 
  TokenType token ;
  bool rest_of_maybe_bit_and_exp_correct, maybe_bit_and_exp_correct ;
  Rest_Of_Maybe_Bit_And_Exp( rest_of_maybe_bit_and_exp_correct ) ;
  if ( !rest_of_maybe_bit_and_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != EXP ) {
    correct = true ;
    return ;
  } // if

  while ( gNextToken.type == EXP ) {
    token = gSList.GetToken() ;  // EXP
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Bit_And_Exp( maybe_bit_and_exp_correct ) ;
    if ( !maybe_bit_and_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Bit_Ex_Or_Exp()

void Interpreter :: Maybe_Bit_And_Exp( bool &correct ) {
  // 26 maybe_equality_exp { '&' maybe_equality_exp }  ok 
  TokenType token ;
  bool maybe_equality_exp_correct ;
  Maybe_Equality_Exp( maybe_equality_exp_correct ) ;
  if ( !maybe_equality_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != AND_1 ) {
    correct = true ; 
    return ;
  } // if
  
  while ( gNextToken.type == AND_1 ) {
    token = gSList.GetToken() ;  // AND_1
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Equality_Exp( maybe_equality_exp_correct );
    if ( !maybe_equality_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Maybe_Bit_And_Exp()

void Interpreter :: Rest_Of_Maybe_Bit_And_Exp( bool &correct ) {
  // 27 rest_of_maybe_equality_exp { '&' maybe_equality_exp }  ok 
  TokenType token ;
  bool rest_of_maybe_equality_exp_correct, maybe_equality_exp_correct ;
  Rest_Of_Maybe_Equality_Exp( rest_of_maybe_equality_exp_correct ) ;
  if ( !rest_of_maybe_equality_exp_correct ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type != AND_1 ) {
    correct = true ; 
    return ;
  } // if
  
  while ( gNextToken.type == AND_1 ) {
    token = gSList.GetToken() ;  // AND_1
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Equality_Exp( maybe_equality_exp_correct );
    if ( !maybe_equality_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpection" ;
    } // if
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Bit_And_Exp()

void Interpreter :: Maybe_Equality_Exp( bool &correct ) {
  // 28 maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }  ok 
  TokenType token, nextToken ;
  bool maybe_relational_exp_correct ;
  Maybe_Relational_Exp( maybe_relational_exp_correct ) ;
  if ( !maybe_relational_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != NE && nextToken.type != EQU ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == NE || nextToken.type == EQU ) {
    token = gSList.GetToken() ;  // NE | EQU
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Relational_Exp( maybe_relational_exp_correct ) ;
    if ( !maybe_relational_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  }  // while

  correct = true ;
} // Interpreter::Maybe_Equality_Exp()

void Interpreter :: Rest_Of_Maybe_Equality_Exp( bool &correct ) {
  // 29 rest_of_maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }  ok
  TokenType token, nextToken ;
  bool rest_of_maybe_relational_exp_correct, maybe_relational_exp_correct ;
  Rest_Of_Maybe_Relational_Exp( rest_of_maybe_relational_exp_correct ) ;
  if ( !rest_of_maybe_relational_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != NE && nextToken.type != EQU ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == NE || nextToken.type == EQU ) {
    token = gSList.GetToken() ;  // NE | EQU
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Relational_Exp( maybe_relational_exp_correct ) ;
    if ( !maybe_relational_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  }  // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Equality_Exp()

void Interpreter :: Maybe_Relational_Exp( bool &correct ) {
  // 30 maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }  ok 

  TokenType token, nextToken ;
  bool maybe_shift_exp_correct ;
  Maybe_Shift_Exp( maybe_shift_exp_correct ) ;
  if ( !maybe_shift_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != LT && nextToken.type != GT && nextToken.type != LE && nextToken.type != GE ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == LT || nextToken.type == GT || nextToken.type == LE || nextToken.type == GE ) {
    token = gSList.GetToken() ; // LT | GT | LE | GE
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Shift_Exp( maybe_shift_exp_correct ) ;
    if ( !maybe_shift_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  }  // while

  correct = true ;
} // Interpreter::Maybe_Relational_Exp()

void Interpreter :: Rest_Of_Maybe_Relational_Exp( bool &correct ) {
  // 31 rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp } ok
  TokenType token, nextToken ;
  bool rest_of_maybe_shift_exp_correct, maybe_shift_exp_correct ;
  Rest_Of_Maybe_Shift_Exp( rest_of_maybe_shift_exp_correct ) ;
  if ( !rest_of_maybe_shift_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != LT && nextToken.type != GT && nextToken.type != LE && nextToken.type != GE ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == LT || nextToken.type == GT || nextToken.type == LE || nextToken.type == GE ) {
    token = gSList.GetToken() ;  // LT | GT | LE | GE
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Shift_Exp( maybe_shift_exp_correct ) ;
    if ( !maybe_shift_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  }  // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Relational_Exp()

void Interpreter :: Maybe_Shift_Exp( bool &correct ) {
  // 32 maybe_additive_exp { ( LS | RS ) maybe_additive_exp }  ok
  TokenType token, nextToken ;
  bool maybe_additive_exp_correct ;
  Maybe_Additive_Exp( maybe_additive_exp_correct ) ;
  if ( !maybe_additive_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != LS && nextToken.type != RS ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == LS || nextToken.type == RS ) {
    token = gSList.GetToken() ;  // LS | RS
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Additive_Exp( maybe_additive_exp_correct ) ;
    if ( !maybe_additive_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ; 
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  } // while

  correct = true ;
} // Interpreter::Maybe_Shift_Exp()

void Interpreter :: Rest_Of_Maybe_Shift_Exp( bool &correct ) {
  // 33 rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }  ok 
  TokenType token, nextToken ;
  bool rest_of_maybe_additive_exp_correct, maybe_additive_exp_correct ;

  Rest_Of_Maybe_Additive_Exp( rest_of_maybe_additive_exp_correct ) ;
  if ( !rest_of_maybe_additive_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != LS && nextToken.type != RS ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == LS || nextToken.type == RS ) {
    token = gSList.GetToken() ;  // LS | RS
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Additive_Exp( maybe_additive_exp_correct ) ; 
    if ( !maybe_additive_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Shift_Exp()

void Interpreter :: Maybe_Additive_Exp( bool &correct ) {
  // 34 maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp } ok
  TokenType token, nextToken ;
  bool maybe_mult_exp_correct ;
  Maybe_Mult_Exp( maybe_mult_exp_correct ) ;
  if ( !maybe_mult_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != ADD && nextToken.type != SUB ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == ADD || nextToken.type == SUB ) {
    token = gSList.GetToken() ;  // ADD | SUB
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Mult_Exp( maybe_mult_exp_correct ) ;
    if ( !maybe_mult_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  } // while

  correct = true ;
} // Interpreter::Maybe_Additive_Exp()

void Interpreter :: Rest_Of_Maybe_Additive_Exp( bool &correct ) {
  // 35 rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp } ok
  TokenType token, nextToken ;
  bool rest_of_maybe_mult_exp_correct, maybe_mult_exp_correct ;
  Rest_Of_Maybe_Mult_Exp( rest_of_maybe_mult_exp_correct ) ;
  if ( !rest_of_maybe_mult_exp_correct ) {
    correct = false ;
    return ;
  } // if

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != ADD && nextToken.type != SUB ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == ADD || nextToken.type == SUB ) {
    token = gSList.GetToken() ;  // ADD | SUB
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Maybe_Mult_Exp( maybe_mult_exp_correct ) ;
    if ( !maybe_mult_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Additive_Exp()

void Interpreter :: Maybe_Mult_Exp( bool &correct ) {
  // 36 unary_exp rest_of_maybe_mult_exp  ok 
  bool unary_exp_correct, rest_of_maybe_mult_exp_correct ;
  bool error ;
  Unary_Exp( unary_exp_correct ) ;
  if ( !unary_exp_correct ) {
    correct = false ;
    return ;
  } // if

  Rest_Of_Maybe_Mult_Exp( rest_of_maybe_mult_exp_correct ) ;
  if ( !rest_of_maybe_mult_exp_correct ) {
    gErrorLine = gLine ; 
    gErrorToken = gNextToken ; 
    throw "unexpected" ;
  } // if

  correct = true ;
} // Interpreter::Maybe_Mult_Exp()

void Interpreter :: Rest_Of_Maybe_Mult_Exp( bool &correct ) {
  // 37 { ( '*' | '/' | '%' ) unary_exp }  ok 
  // 啥都沒有 could be empty 
  TokenType token, nextToken ;
  bool unary_exp_correct, error ;
  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != MUL && nextToken.type != DIV && nextToken.type != REMAINDER ) {
    correct = true ;
    return ;
  } // if

  while ( nextToken.type == MUL || nextToken.type == DIV || nextToken.type == REMAINDER ) {
    token = gSList.GetToken() ;  // MUL | DIV | REMAINDER
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Unary_Exp( unary_exp_correct ) ;
    if ( !unary_exp_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    nextToken = gSList.PeekToken() ;
  } // while

  correct = true ;
} // Interpreter::Rest_Of_Maybe_Mult_Exp()

void Interpreter :: Unary_Exp( bool &correct ) {
  // 38 sign { sign } signed_unary_exp | unsigned_unary_exp |   
  //    ( PP | MM ) Identifier [ '[' expression ']' ]
  bool signed_unary_exp_correct, expression_correct, unsigned_unary_exp_correct ;
  bool sign_correct, sysFunc ;
  TokenType token, nextToken ;
  string sysFuncName ;

  nextToken = gSList.PeekToken() ;
  if ( nextToken.type != PP && nextToken.type != MM ) {
    Sign( sign_correct ) ;
    if ( !sign_correct ) {
      Unsigned_Unary_Exp( unsigned_unary_exp_correct ) ;
      if ( !unsigned_unary_exp_correct ) {
        correct = false ;
        return ;
      } // if
    } // if
  } // if
  
  if ( sign_correct ) {
    // sign { sign } signed_unary_exp
    Sign( sign_correct ) ;
    while ( sign_correct ) {
      Sign( sign_correct ) ;
    } // while

    Signed_Unary_Exp( signed_unary_exp_correct ) ;
    if ( !signed_unary_exp_correct ) {
      correct = false ;
      return ;
    } // if

    correct = true ;
    return ;
  } // if
  else if ( unsigned_unary_exp_correct ) {
    // unsigned_unary_exp
    correct = true ;
    return ;
  } // else if
  else if ( nextToken.type == PP || nextToken.type == MM ) {
    // ( PP | MM ) Identifier [ '[' expression ']' ]
    token = gSList.GetToken() ;  // PP | MM 
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != IDENT ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( !FindDefineIDENT( gNextToken ) || ( !FindDefineFuncIDENT( gNextToken ) && gCompound ) ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "undefined" ;
    } // if

    token = gSList.GetToken() ;  // IDENT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != M_LEFT_PAREN ) {
      correct = true ;
      return ;
    } // if

    token = gSList.GetToken() ;  // M_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != M_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // M_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
} // Interpreter::Unary_Exp()

void Interpreter :: Signed_Unary_Exp( bool &correct ) {
  // 39 Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']' ]  
  //  |  Constant 
  //  |  '(' expression ')'
  bool actual_parameter_list_correct, expression_correct, sysFunc ;
  TokenType token, nextToken ;
  string sysFuncName ;

  if ( gNextToken.type != IDENT && gNextToken.type != CONSTANT && gNextToken.type != S_LEFT_PAREN ) {
    gErrorLine = gLine ;
    gErrorToken = gNextToken ;
    throw "unexpected" ;
  } // if

  if ( gNextToken.type == IDENT ) {
    if ( !FindDefineIDENT( gNextToken ) || ( !FindDefineFuncIDENT( gNextToken ) && gCompound ) ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "undefined" ;
    } // if 

    token = gSList.GetToken() ;  // IDENT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != S_LEFT_PAREN && gNextToken.type != M_LEFT_PAREN ) {
      correct = true ;
      return ;
    } // if
    else if ( gNextToken.type == S_LEFT_PAREN ) {
      // Identifier [ '(' [ actual_parameter_list ] ')' ]
      token = gSList.GetToken() ;  // S_LEFT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type == S_RIGHT_PAREN ) {
        correct = true ;
        return ;
      } // if

      Actual_Parameter_List( actual_parameter_list_correct ) ;
      if ( !actual_parameter_list_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      if ( gNextToken.type != S_RIGHT_PAREN ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ;  // S_RIGHT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      correct = true ;
      return ;
    } // else if
    else if ( gNextToken.type == M_LEFT_PAREN ) {
      // Identifier [ '[' expression ']' ]
      token = gSList.GetToken() ;  // M_LEFT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      Expression( expression_correct ) ;
      if ( !expression_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      if ( gNextToken.type != M_RIGHT_PAREN ) {
        gErrorToken = gNextToken ;
        gErrorLine = gLine ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ; // M_RIGHT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      correct = true ;
      return ;
    } // else if
  } // if
  else if ( gNextToken.type == CONSTANT ) {
    token = gSList.GetToken() ;  // CONSTANT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
  else if ( gNextToken.type == S_LEFT_PAREN ) {
    // '(' expression ')'
    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != S_RIGHT_PAREN ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
} // Interpreter::Signed_Unary_Exp()

void Interpreter :: Unsigned_Unary_Exp( bool &correct ) {
  // 40 Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']' ] [ ( PP | MM ) ] 
  //  |  Constant 
  //  |  '(' expression ')'
  bool actual_parameter_list_correct, expression_correct, sysFunc ;
  TokenType token ;
  string sysFuncName ;
  
  if ( gNextToken.type != IDENT && gNextToken.type != CONSTANT && gNextToken.type != S_LEFT_PAREN ) {
    correct = false ;
    return ;
  } // if

  if ( gNextToken.type == IDENT ) {
    // Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']' ] [ ( PP | MM ) ] 
    if ( gCompound ) {
      if ( !FindDefineFuncIDENT( gNextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "undefined" ;
      } // if
    } // if
    else {
      if ( !FindDefineIDENT( gNextToken ) ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "undefined" ;
      } // if
    } // else

    token = gSList.GetToken() ;  // IDENT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    if ( gNextToken.type != S_LEFT_PAREN && gNextToken.type != M_LEFT_PAREN && 
         gNextToken.type != PP && gNextToken.type != MM ) {
      correct = true ;
      return ;
    } // if
    else if ( gNextToken.type == S_LEFT_PAREN ) {
      // Identifier [ '(' [ actual_parameter_list ] ')' ]
      token = gSList.GetToken() ;  // S_LEFT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type == S_RIGHT_PAREN ) {
        // 沒有actual_parameter_list 
        correct = true ;
        return ;
      } // if

      Actual_Parameter_List( actual_parameter_list_correct ) ;
      if ( !actual_parameter_list_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      if ( gNextToken.type != S_RIGHT_PAREN ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ;  // S_RIGHT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      correct = true ;
      return ;
    } // else if
    else if ( gNextToken.type == M_LEFT_PAREN ) {
      // Identifier [ '[' expression ']' ] [ ( PP | MM ) ]
      token = gSList.GetToken() ;  // M_LEFT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      Expression( expression_correct ) ;
      if ( !expression_correct ) {
        gErrorLine = gLine ;
        gErrorToken = gNextToken ;
        throw "unexpected" ;
      } // if

      if ( gNextToken.type != M_RIGHT_PAREN ) {
        gErrorToken = gNextToken ;
        gErrorLine = gLine ;
        throw "unexpected" ;
      } // if

      token = gSList.GetToken() ;  // M_RIGHT_PAREN
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      if ( gNextToken.type != PP && gNextToken.type != MM ) {
        correct = true ;
        return ;
      } // if

      token = gSList.GetToken() ;  // PP | MM
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      correct = true ;
      return ;
    } // else if
    else if ( gNextToken.type == PP || gNextToken.type == MM ) {
      token = gSList.GetToken() ;  // PP | MM
      if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
      correct = true ;
      return ;
    } // else if
  } // if
  else if ( gNextToken.type == CONSTANT ) {
    token = gSList.GetToken() ;  // CONSTANT
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
  else if ( gNextToken.type == S_LEFT_PAREN ) {
    // '(' expression ')'
    token = gSList.GetToken() ;  // S_LEFT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    Expression( expression_correct ) ;
    if ( !expression_correct ) {
      gErrorLine = gLine ;
      gErrorToken = gNextToken ;
      throw "unexpected" ;
    } // if

    if ( gNextToken.type != S_RIGHT_PAREN ) {
      gErrorToken = gNextToken ;
      gErrorLine = gLine ;
      throw "unexpected" ;
    } // if

    token = gSList.GetToken() ;  // S_RIGHT_PAREN
    if ( gCompound || gCompound_small ) gTempFunc.push_back( token ) ;
    correct = true ;
    return ;
  } // else if
} // Interpreter::Unsigned_Unary_Exp()

void Interpreter :: PreStart() {
  // 存保留字 
  TokenType token ;
  token.varName = "cin" ;
  gDefineList.push_back( token ) ;
  token.varName = "cout" ;
  gDefineList.push_back( token ) ;
  token.varName = "Done" ;
  gDefineList.push_back( token ) ;
  token.varName = "ListAllVariables" ; 
  gDefineList.push_back( token ) ;
  token.varName = "ListAllFunctions" ;  // 印全部存過的function name 
  gDefineList.push_back( token ) ;
  token.varName = "ListVariable" ;
  gDefineList.push_back( token ) ;
  token.varName = "ListFunction" ;  // 印指定function (全部) 
  gDefineList.push_back( token ) ;
} // Interpreter::PreStart()

void Interpreter :: PreStart2() {
  // 存保留字 
  TokenType token ;
  token.varName = "cin" ;
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "cout" ;
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "Done" ;
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "ListAllVariables" ; 
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "ListAllFunctions" ;  // 印全部存過的function name 
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "ListVariable" ;
  gDefineFuncIDENTList.push_back( token ) ;
  token.varName = "ListFunction" ;  // 印指定function (全部) 
  gDefineFuncIDENTList.push_back( token ) ;
} // Interpreter::PreStart2()

void Interpreter :: BubbleSort( vector<TokenType> &funcName ) {
  TokenType token ;
  for ( int i = 0 ; i < gDefineFuncList.size() ; i++ ) {
    token = gDefineFuncList[i][1] ;
    funcName.push_back( token ) ;
  } // for 

  for ( int i = 0 ; i < funcName.size() - 1 ; i++ ) {
    if ( funcName[i].varName > funcName[i+1].varName ) {
      token = funcName[i] ;
      funcName[i] = funcName[i+1] ;
      funcName[i+1] = token ;
    } // if
  } // for
} // Interpreter::BubbleSort()

void Interpreter :: PrintFunction() {
  int count = 0 ;
  for ( int i = 0 ; i < gDefineFuncList.size() ; i++ ) {
    if ( gSpecificToken.varName == gDefineFuncList[i][1].varName ) {
      for ( int j = 0 ; j < gDefineFuncList[i].size() ; j++ ) {
        if ( gDefineFuncList[i][j].varName == "{" ||
             gDefineFuncList[i][j].varName == "}" ||
             gDefineFuncList[i][j].varName == ";" ) {
          if ( gDefineFuncList[i][j].varName == "{" ) {
            count ++ ;
          } // if

          cout << gDefineFuncList[i][j].varName << endl ;
          if ( j+1 < gDefineFuncList[i].size() ) {
            if ( gDefineFuncList[i][j+1].varName == "}" ) {
              count --  ;
            } // if 
          } // if  

          for ( int k = 0 ; k < count ; k++ ) {
            cout << "  " ;
          } // for 
        } // if
        else {
          cout << gDefineFuncList[i][j].varName ;
          if ( j+1 < gDefineFuncList[i].size() ) {   
            if ( gDefineFuncList[i][j+1].varName != "[" && gDefineFuncList[i][j+1].varName != "(" &&
                 gDefineFuncList[i][j+1].varName != "++" &&
                 gDefineFuncList[i][j+1].varName != "--" ) {
              cout << " " ;
            } // if 
  
            if ( gDefineFuncList[i][j].varName == "if" || gDefineFuncList[i][j].varName == "while" ) {
              cout << " " ; 
            } // if   
          } // if             
        } // else      
      } // for  
    } // if
  } // for
} // Interpreter::PrintFunction()

int Interpreter :: FindLine() {
  if ( gBlank >= 0 ) {
    return gLine - gDefinitionLine + gBlank + 1 ;
  } // if
  else {
    return gLine - gDefinitionLine + 1 ;
  } // else
} // Interpreter::FindLine()

int main() {
  string input ;
  bool correct = false ;
  bool floatnum = false ;
  float value ;
  bool boolValue, boolAns, end ;
  end = false ;
  boolAns = false ;
  boolValue = false ;
  TokenType token ;
  vector<TokenType> funcName ;
  int realLine = 0 ;
  gIList.PreStart() ;
  gIList.PreStart2() ;
  cin >> input ;
  cout << ">> Our-C running..." << endl ;
  token = gSList.GetToken() ;
  while ( !end ) {
    try {
      gIList.User_Input( boolAns ) ;
    } // try 
    catch ( char const* incident ) {
      if ( strcmp( incident, "unrecognized" ) == 0 ) {
        realLine = gIList.FindLine() ;
        cout << "> Line " << realLine << " : unrecognized token with first char : '" 
             << gErrorToken.varName << "'" << endl ; 
        if ( gCompound ) {
          gTempFunc.clear() ;
          gDefineFuncIDENTList.clear() ;
          gIList.PreStart2() ;
          gBlank = 0 ;
        } // if
      } // if
      else if ( strcmp( incident, "unexpected" ) == 0 ) {
        realLine = gIList.FindLine() ;
        cout << "> Line " << realLine << " : unexpected token : '" << gErrorToken.varName << "'" << endl ;
        if ( gCompound ) {
          gTempFunc.clear() ;
          gDefineFuncIDENTList.clear() ;
          gIList.PreStart2() ;
          gBlank = 0 ;
        } // if
      } // else if
      else if ( strcmp( incident, "undefined" ) == 0 ) {
        // realLine = gLine - gDefinitionLine + 1 ;
        realLine = gIList.FindLine() ;
        cout << "> Line : " << realLine << " : undefined identifier : '" << gErrorToken.varName << "'" 
             << endl ;
        if ( gCompound ) {
          gTempFunc.clear() ;
          gDefineFuncIDENTList.clear() ;
          gIList.PreStart2() ;
          gBlank = 0 ;
        } // if
      } // else if
      else if ( strcmp( incident, "Done" ) == 0 ) {
        end = true ;
      } // else if
      else if ( strcmp( incident, "ListAllVariables" ) == 0 ) {
        cout << "> " ;
        for ( int i = 0 ; i < gDefineList.size() ; i++ ) {
          cout << gDefineList[i].varName << endl ;
        } // for

        cout << "Statement executed ..." << endl ;
      } // else if
      else if ( strcmp( incident, "ListAllFunctions" ) == 0 ) {
        gIList.BubbleSort( funcName ) ;
        cout << "> " ;
        for ( int i = 0 ; i < funcName.size() ; i++ ) {
          for ( int j = 0 ; j < gDefineFuncList.size() ; j++ ) {
            if ( funcName[i].varName == gDefineFuncList[j][1].varName ) {
              cout << gDefineFuncList[j][1].varName << "()" << endl ;
            } // if
          } // for
        } // for

        cout << "Statement executed ..." << endl ;
        funcName.clear() ;
      } // else if
      else if ( strcmp( incident, "ListVariable" ) == 0 ) {
        cout << "> " << gSpecificToken.type << " " << gSpecificToken.varName << endl;
        cout << "Statement executed ..." << endl ;
      } // else if 
      else if ( strcmp( incident, "ListFunction" ) == 0 ) {
        cout << "> " ;
        gIList.PrintFunction() ;
        cout << "Statement executed ..." << endl ;
      } // else if
    } // catch

    if ( !end ) {
      if ( gLine == gErrorLine ) {
        TokenType temp = gSList.GetToken() ;
      } // if

      gSList.GetSkipLineToken() ;
      gTempFunc.clear() ;
    } // if
  } // while

  gDefineList.clear() ;
  cout << "> " << "Our-C exited..." << endl ;
} // main()

