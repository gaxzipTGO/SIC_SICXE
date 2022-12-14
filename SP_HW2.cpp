#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <map>
#include <cstring>
#include <cmath>
#include <bitset>

using namespace std;

struct Table{
    vector<string> tableItems ;
} ;

struct Page_Token{
    vector<vector<string>> page_token_no ;
} ;

Page_Token g_instructionTable ;
Page_Token g_pseudoInsTable ;
Page_Token g_registerTable ;
Page_Token g_delimiterTable ;
Page_Token g_numberTable ;
Page_Token g_stringTable ;
Page_Token g_otherTable ;
string numbertable[100] ;
string stringtable[100] ;
string labeltable[100] ;


struct Instruction {
    int format ;
    string opcode ;
    int register_number = 0 ;
} ;

map<string, Instruction> g_InstructionMap ; // map from string to Instruction

void loadingMap( map<string, Instruction> &loadIngMap ) {
    loadIngMap["add"] = {3,"18"} ;
    loadIngMap["addf"] = {3,"58"} ;
    loadIngMap["addr"] = {2,"90",2} ;
    loadIngMap["and"] = {3,"40"} ;
    loadIngMap["clear"] = {2,"B4",1} ;
    loadIngMap["comp"] = {3,"28"} ;
    loadIngMap["compf"] = {3,"88"} ;
    loadIngMap["compr"] = {2,"A0",2} ;
    loadIngMap["div"] = {3,"24"} ;
    loadIngMap["divf"] = {3,"64"} ;
    loadIngMap["divr"] = {2,"9C",2} ;
    loadIngMap["fix"] = {1,"C4"} ;
    loadIngMap["float"] = {1,"C0"} ;
    loadIngMap["hio"] = {1,"F4"} ;
    loadIngMap["j"] = {3,"3C"} ;
    loadIngMap["jeq"] = {3,"30"} ;
    loadIngMap["jgt"] = {3,"34"} ;
    loadIngMap["jlt"] = {3,"38"} ;
    loadIngMap["jsub"] = {3,"48"} ;
    loadIngMap["lda"] = {3,"00"} ;
    loadIngMap["ldb"] = {3,"68"} ;
    loadIngMap["ldch"] = {3,"50"} ;
    loadIngMap["ldf"] = {3,"70"} ;
    loadIngMap["ldl"] = {3,"08"} ;
    loadIngMap["lds"] = {3,"6C"} ;
    loadIngMap["ldt"] = {3,"74"} ;
    loadIngMap["ldx"] = {3,"04"} ;
    loadIngMap["lps"] = {3,"D0"} ;
    loadIngMap["mul"] = {3,"20"} ;
    loadIngMap["mulf"] = {3,"60"} ;
    loadIngMap["mulr"] = {2,"98"} ;
    loadIngMap["norm"] = {1,"C8"} ;
    loadIngMap["or"] = {3,"44"} ;
    loadIngMap["rd"] = {3,"D8"} ;
    loadIngMap["rmo"] = {2,"AC"} ;
    loadIngMap["rsub"] = {3,"4C"} ;
    loadIngMap["shiftl"] = {2,"A4",2} ;
    loadIngMap["shiftr"] = {2,"A8",2} ;
    loadIngMap["sio"] = {1,"F0"} ;
    loadIngMap["ssk"] = {3,"EC"} ;
    loadIngMap["sta"] = {3,"0C"} ;
    loadIngMap["stb"] = {3,"78"} ;
    loadIngMap["stch"] = {3,"54"} ;
    loadIngMap["stf"] = {3,"80"} ;
    loadIngMap["sti"] = {3,"D4"} ;
    loadIngMap["stl"] = {3,"14"} ;
    loadIngMap["sts"] = {3,"7C"} ;
    loadIngMap["stsw"] = {3,"E8"} ;
    loadIngMap["stt"] = {3,"84"} ;
    loadIngMap["stx"] = {3,"10"} ;
    loadIngMap["sub"] = {3,"1C"} ;
    loadIngMap["subf"] = {3,"5C"} ;
    loadIngMap["subr"] = {2,"94",2} ;
    loadIngMap["svc"] = {2,"B0",1} ;
    loadIngMap["td"] = {3,"E0"} ;
    loadIngMap["tio"] = {1,"F8"} ;
    loadIngMap["tix"] = {3,"2C"} ;
    loadIngMap["tixr"] = {2,"B8",1} ;
    loadIngMap["wd"] = {3,"DC"} ;
}


string PlusHexToLog(string log, int addNum) {

	int i = stoi(log,nullptr,16) + addNum ;
	ostringstream ss;
	ss  << setfill('0') << setw(4) << hex << i;
	string result = ss.str();
	return result; 


}

char FourBitToHex( string binary ) {
    while ( binary.size() < 4 ) {
        binary = '0' + binary ;
    }
    int num = 0 ;
    int count = 3 ;
    char buffer[1] ;
    for ( char bit : binary ) {
        num = num + ( int(bit-'0') * pow(2,count)) ;
        count -- ;
    } 
    itoa(num, buffer , 16) ;
    return buffer[0] ;
}

string BinarytoHex(string log) {
    /*
    ?????????????????? ????????????????????? ????????????????????????
    */
    string hexString  = "" ;
    string temp = "" ;
    int first = log.size() % 4  ;
    for ( int i = 0 ; i < first ; i ++ ) {
        temp += log[i] ;
    }
    if (! temp.size() == 0) {
        hexString = hexString + FourBitToHex(temp) ;
        temp = "" ;
    }
    for ( int i = 0 ; i + first < log.size() ; i ++ ) {
        if ( ! i == 0 && i % 4 == 0 ) {
          hexString = hexString + FourBitToHex(temp) ; 
          temp = "" ;  
        }
        temp += log[i+first] ;
    }
    hexString = hexString + char(toupper( FourBitToHex(temp) )) ; 
    temp = "" ;  
    for ( int i = 0 ; i < hexString.size() ; i ++ ) {
        hexString[i] = char(toupper(hexString[i])) ;
    }
    return hexString ;
}

string hexToBinary(string hex_string, int lenth) {
    /*
    ?????????????????????????????? ????????????????????????binary
    */
    stringstream ss;
    ss << hex << hex_string;
    unsigned n;
    ss >> n;
    bitset<32> b(n);
    string str = b.to_string() ;
    str = &str[32-lenth] ;
    return str ;
}

int hexToDec( string hex ) {
    hex = "0x" + hex ;
    return stoi(hex,0,16) ;
}

bool CheakCanTranToNum( string hex ) {
    try
    {
        int dex = stoi(hex,nullptr,16) ;
        return true ;
    }
    catch(const std::exception& e)
    {
        return false ;
    }
    
}

class GetTokenMachine {
    protected: fstream fin ;
    protected: string token = "" ;
    protected: bool end = false ;
    protected: char nextChar = '\0' ; // used to cheak
    protected: char bufferDelimiter = '\0' ; // used to save delimiter

    public: GetTokenMachine(string file) {
        this->fin.open(file) ;
    }

    protected: bool IsDelimiter(char ch) {
        if ( ch == '\"' || ch == '\'' || ch == '.' || ch == ',' || ch == '*' ||
            ch == '!' || ch == '@' || ch == '#' || ch == '$' || ch == '%' ||
            ch == '^' || ch == '&' || ch == '(' || ch == ')' || ch == '[' ||
            ch == ']' || ch == '{' || ch == '}' || ch == '|' || ch == ';' ||
            ch == ':' || ch == ';' || ch == '/' || ch == '?' || ch == '<' || 
            ch == '=' || ch == '>' || ch == '+' || ch == '-' ) {
            return true ;
        }
        return false ;
    }

    protected: bool GetFileToken( fstream &file,string & token ) {
        // if read delimite we need to save the token and return
        bool isEmpty = false ;
        if ( bufferDelimiter != '\0' ) {
            token = token + string(1,bufferDelimiter) ;
            bufferDelimiter = '\0' ;
            return isEmpty ;
        } // this means our buffer have a delimiter we need to use it first 
        file.get(nextChar) ;
        while ( nextChar != '\t' &&nextChar !=' ' && nextChar != '\n' && ! IsDelimiter(nextChar) && ! file.eof() ) {
            token = token + string(1,nextChar) ;
            file.get(nextChar) ;
        }
        if ( IsDelimiter(nextChar) ) {
            bufferDelimiter = nextChar ;
        } // if nextChar is delimiter, it will leave while and go to here

        if ( ! file.eof() ) {
            return isEmpty  ;
        }
        else return true ;

    }

    public: void Test() {
        char continueTime = 'y' ;
        while ( continueTime == 'y' && ! end ) {
            end = GetFileToken(fin,this->token) ;
            if ( token.length() > 0 ) {
                cout << token << endl ;
                token = "" ;             
            }
            //cout << "do yo countinue?" << endl ;
            //cin >> continueTime ; 
        } 
        cin >> token ;
    }

    public: bool GetNextToken(string &Out_token) {
        /*
        the function is we can get the token but we need to chiose Out_token != "" 
        */
        token = "" ;
        if ( ! end ) {
            do {
                end = GetFileToken(fin,token) ;
                if( end == true ) {
                    Out_token = token ; 
                    return false ;
                }         
            } while ( token.length() == 0 && nextChar != '\n') ;
            Out_token = token ;
            return true ;
        }
        return false ;         
    }

    public: char ReturnNextChar() {
        return nextChar ;
    }

    public: bool IsEnterChar() {
        if( nextChar == '\n' || end ) { 
            return true ;
        }
        else return false ;
    }
} ;

class SICXEGetTokenMachine : public GetTokenMachine {

    protected : bool nextReadWholeLine = false ;

    public: SICXEGetTokenMachine(string fileName) : GetTokenMachine(fileName){
      
    }

    private: void DelemiterChiose(char delimiter, string & token) {
        if (delimiter == '.') {
            nextReadWholeLine = true ;
        }
        /*
        else if ( delimiter == '+' || delimiter == '-' ) {
            string temp ;
            end = GetFileToken(fin,temp) ; 
            token = token + temp ;
        }
        */
        else if (delimiter == '\'') {
            string temp ;
            while ( temp != "\'" ) {
                temp = "" ;
               end = GetFileToken(fin,temp) ; 
               token = token + temp ;
            }
        }
    }

    public: bool GetNextToken(string &Out_token) {
        /*
        the function is we can get the token but we need to chiose Out_token != "" 
        */
        token = "" ;
        if ( ! end ) {
            do {
                if ( ! nextReadWholeLine ) {
                    end = GetFileToken(fin,token) ;
                    if ( IsDelimiter(token[0])  ) {
                        DelemiterChiose(token[0],token) ;
                    }
                    if( end == true ) {
                        Out_token = token ; 
                        return false ;
                    }
                }
                else {
                    nextReadWholeLine = false ;
                    char temp[100] ;
                    fin.getline(temp,sizeof(temp)) ;
                    token = temp ;
                    nextChar = '\n' ;
                }
            } while ( token.length() == 0 && nextChar != '\n') ;
            Out_token = token ;
            return true ;
        }
        return false ;         
    }

};

/*
now we have a tokenMachine we can use it like's : GetTokenMachine machine1(file_name) ;
and it have a function bool GetNextToken(string token) 
the token can read nextToken and delimiter if it is true means not eof and we can used it 
*/

struct Token{
    string programToken ;
    int tokenPage ;
    int tokenPosition ;
} ;

struct Program_Token{
    vector<vector<Token>> token ;
};

struct AssemblerRemember{
    
    int state = 0 ; // state 0 = not yet, state 1 = pass, state 2 = sysax have error
    bool finished = false ;
    int line = 0 ;
    string location = "" ;
    vector<Token> program ;
    string ObCode = "" ;
    int format = 0 ;


} ;

class Assembler{

    protected: string nowLocation = "0000" ;
    protected: vector<AssemblerRemember> temp_memory ;
    protected: string thisPsedoCode = "" ;
    protected: string base_register = "" ;

    public: void LoadingProgram( vector<Token> nowProgram ) {
        /*
        this is read vector<string> and add it program
        */
        AssemblerRemember temp ; 
        if ( temp_memory.size() == 0  ) {
            temp.line = 5 ;
        }
        else {
            temp.line = temp_memory[temp_memory.size()-1].line + 5 ;
        }
        temp.program = nowProgram ;
        temp_memory.push_back(temp) ;
    }

    public: void PrintLineMessage(int now ) {
        bool comment = false ; 
        cout << temp_memory[now].line << "\t" << temp_memory[now].location << "\t";
        for ( auto program : temp_memory[now].program ) {
            cout << program.programToken << '\t' ;           
        }
        cout << temp_memory[now].ObCode << endl ;
    }

    public: void PrintAllMessage() {
        bool comment = false ;
        for ( int now = 0 ; now < temp_memory.size() ; now ++ ) { 
            if ( temp_memory[now].state != 2 ) {
                cout << temp_memory[now].line << "\t" ;
                if ( temp_memory[now].location != "-99999" ) {
                    cout << temp_memory[now].location << "\t";
                }
                else {
                    cout << '\t' ;
                }
                bool tab = false ;
                int size = 0 ;
                for ( auto program : temp_memory[now].program ) {
                    if ( tab == true ) {
                        if ( program.programToken != "," && program.programToken != "-"  && program.programToken != "/" ) {
                            cout << '\t' ;
                            size ++ ;
                        }
                        tab = false ;
                    }
                    cout << program.programToken ;
                    if ( program.programToken != "+" && program.programToken != "#" && program.programToken != "@" && 
                         program.programToken != "C" && program.programToken != "X" && program.programToken != "," &&
                         program.programToken != "-" && program.programToken != "*" && program.programToken != "/" && program.programToken != "="  ) {
                        tab = true ;
                    }           
                }
                if ( temp_memory[now].finished ) {
                    if ( temp_memory[now].ObCode == "") {
                        cout << endl ;
                    }
                    else {
                        for ( ; size < 4 ; size ++ ) {
                            cout << '\t' ;
                        }
                        cout << BinarytoHex(temp_memory[now].ObCode) << endl ;
                    }
                }
                else {
                    cout << endl ;
                }
            }
        }
    }

    public: void WriteFile( string fileName ) {
        bool comment = false ;
        ofstream out ;
        out.open("out_"+fileName) ;
        for ( int now = 0 ; now < temp_memory.size() ; now ++ ) { 
            if ( temp_memory[now].state != 2 ) {
                out << temp_memory[now].line << '\t' ;
                if ( temp_memory[now].location != "-99999" ) {
                    out << temp_memory[now].location << '\t';
                }
                else {
                    out << '\t' ;
                }
                bool tab = false ;
                int size = 0 ;
                for ( auto program : temp_memory[now].program ) {
                    if ( tab == true ) {
                        if ( program.programToken != "," ) {
                            out << '\t' ;
                            size ++ ;
                        }
                        tab = false ;
                    }
                    out << program.programToken ;
                    if ( program.programToken != "+" && program.programToken != "#" && program.programToken != "@" && program.programToken != "C" && program.programToken != "X" && program.programToken != ","  ) {
                        tab = true ;
                    }           
                }
                if ( temp_memory[now].finished ) {
                    if ( temp_memory[now].ObCode == "") {
                        out << endl ;
                    }
                    else {
                        for ( ; size < 5 ; size ++ ) {
                            out << '\t' ;
                        }
                        out << BinarytoHex(temp_memory[now].ObCode) << endl ;
                    }
                }
                else {
                    out << endl ;
                }
            }
        }        
    }

    protected: void SyntaxError() {
        cout << "this is SyntaxError" << endl ;
    }

    protected: void LexicalError() {
        cout << "this is LexicalError" << endl ;
    }


};

    /*
    
    2022/09/20 ???token?????????????????? ?????????????????????opcode
    PASS_ONE  :
        ??????????????????:
            for ????????? :
                if ?????? == ?????? :
                    ?????? token_6 START Hex ???????????????Hex?????? ??????????????????1000 ???????????????
                    ??????????????????loc 
                    if ????????? == true :
                        ?????????instruction code ??????????????????
                        if ????????????label?????? :
                            ?????????label???Loc???????????????4???
                            ????????????opcode??????
                        else :
                            ??????pass2?????????
                    else :
                        if ??? BYTE C'string' :
                            ???string???ascii?????? ??????opcode 
                        elif ??? WORD table_4 :
                            ?????????hex??????dec?????????opcode
                        elif ??? BYTE X' string ' :
                            opcode ??????hex ?????????0000??????
                        ????????????opcode??????
                    loc + 3 
                else :
                    ????????????
                    ?????????????????????2
                ???????????????

    PASS2 :
        for ????????? :
            if ?????????opcode????????? :
                for ?????????????????? :
                    if program[0] ???table_6 :
                        ?????????log???????????????opcode[2:6]
                        ????????????
                    else :
                        ????????????
            else :
                ?????????
            ????????????                    
    */

class SIC : public Assembler {

    protected: string cheakedPseudo = "" ;
    protected: bool CheakIsPseudo( AssemblerRemember cheakedProgram ) {
        /*
        ?????????????????????????????????pseudo code ????????? ??????true ?????? ??????false
        */
        bool getTable = false ;
        for( int i = 0 ; i < cheakedProgram.program.size() && ! getTable ; i++ ) {
            if ( cheakedProgram.program[i].tokenPage == 1 ) {
                getTable = true ;
                cheakedPseudo = cheakedProgram.program[i].programToken ;
            }
        } // ???????????????????????????pseudo
        return getTable ;
    }

    protected: bool CheakPsedoCodeSysaxFinal( AssemblerRemember cheakedProgram, int position ) {
        int cheakNum = 0 ;
        if ( position != 6 ) {
            if ( cheakedProgram.program[cheakNum].tokenPage == 6 ) {
                cheakNum ++ ;
            } // { label } ??????????????????
            cheakNum ++ ;
            switch ( position ) {
            case 0 :
                if ( cheakedProgram.program[cheakNum].tokenPage == 4 || CheakCanTranToNum( cheakedProgram.program[cheakNum].programToken ) == true ) {
                    thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                    return true ;
                }
                return false ;
            case 1 :
                thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                return true ;
                break ;
            case 2  :
                if (  (( cheakedProgram.program[cheakNum].programToken == "X" || cheakedProgram.program[cheakNum].programToken == "C" ) 
                && cheakedProgram.program[cheakNum+1].tokenPage == 5 ) || cheakedProgram.program[cheakNum].tokenPage == 4 ) {
                    thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                    return true ;
                }
                return false ;
            case 3 :
                if (  (( cheakedProgram.program[cheakNum].programToken == "X" || cheakedProgram.program[cheakNum].programToken == "C" ) 
                && cheakedProgram.program[cheakNum+1].tokenPage == 5 ) || cheakedProgram.program[cheakNum].tokenPage == 4 ) {
                    thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                    return true ;
                }
                return false ;
            case 4 :
                if ( cheakedProgram.program[cheakNum].tokenPage == 4 ) {
                    thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                    return true ;
                }
                return false ;
            case 5 :
                if ( cheakedProgram.program[cheakNum].tokenPage == 4 ) {
                    thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                    return true ;
                }
            case 7 :
               if ( cheakedProgram.program[cheakNum].tokenPage == 3 || cheakedProgram.program[cheakNum].tokenPage == 4 ) {
                thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                return true ;
               } 
            case 8 :
                thisPsedoCode = g_pseudoInsTable.page_token_no[position][0] ;
                return true ;           
            default:
                return false;
            }
        } // 
        else {
            if ( cheakedProgram.program[cheakNum].tokenPage == 6 ) {
                cheakNum = cheakNum + 2 ;
                int x = cheakedProgram.program[cheakNum].tokenPage ;
                if ( x == 6 || x == 4 || cheakedProgram.program[cheakNum].programToken == "*" ) {
                    thisPsedoCode = "EQU" ;
                    return true ;
                }
            } 
            return false ;
        } // EQU
        return true ;
    }

    protected: bool CheakPsedoCodeSysax( AssemblerRemember cheakedProgram ) {
        /*
        ??????????????????????????????psedoCode ?????????????????????????????????pseudoCode??????????????????????????????????????????
        */
        string psedoCode = "" ;
        for ( auto token : cheakedProgram.program ) {
            if ( token.tokenPage == 1 ) {
                return CheakPsedoCodeSysaxFinal( cheakedProgram, token.tokenPosition ) ;       
            } // ????????????token_table2 ?????????psedoCode???table
        }
        return false ;

    }

    protected: bool CheakDirect(AssemblerRemember cheakProgram, int cheakNum ) {
        if ( cheakProgram.program[cheakNum].tokenPage == 6 ) {
            return true ;
        }
        else return false ;
    }

    protected: bool CheakIndex(AssemblerRemember cheakProgram, int cheakNum ) {
        if ( cheakProgram.program[cheakNum].tokenPage == 6 ) {
            cheakNum ++ ;
            if ( cheakProgram.program.size() > cheakNum && cheakProgram.program[cheakNum].programToken == ","  ) {
                cheakNum ++ ;
                if ( cheakProgram.program[cheakNum].programToken == "X" ) {
                    return true ;
                }
            }
        }
        return false ;
    }

    protected: bool CheakM(AssemblerRemember cheakProgram, int cheakNum ) {
        if ( CheakIndex( cheakProgram, cheakNum ) || CheakDirect(cheakProgram, cheakNum ) ) {
            return true ;
        }
        else return false ; 
    }

    protected: bool CheakStandard( AssemblerRemember cheakedProgram ) {
        int cheakNum = 0 ;
        if ( cheakedProgram.program[cheakNum].tokenPage == 6 ) {
            cheakNum ++ ;
        }
        if ( cheakedProgram.program[cheakNum].tokenPage == 0 ) {
            if ( cheakedProgram.program[cheakNum].tokenPosition == 35 ) {
                if ( cheakNum == cheakedProgram.program.size()-1 ) {
                    return true ;
                }
                else {
                    return false ;
                }
            }
            cheakNum ++ ;
             if ( CheakM( cheakedProgram, cheakNum ) ) {
                return true ;
            }
        }
        return false ;
    }

    protected:virtual  bool CheakInstructCodeSysax( AssemblerRemember cheakedProgram ) {
        return CheakStandard( cheakedProgram ) ;

    }

    protected: bool ChaekTheSysax( bool IsPseudo, AssemblerRemember cheakedProgram ) {
        /*
        ??????????????????????????????
        ???????????? ????????????????????????????????? code ???function??????????????????
        */
        if ( cheakedProgram.program[0].programToken[0] != '.' ) {
            if ( IsPseudo ) {
                return CheakPsedoCodeSysax( cheakedProgram ) ;
            } // ?????????pseudo ????????????????????????
            else {
                return CheakInstructCodeSysax(cheakedProgram) ;
            } // ?????????????????? ????????????????????????
        }
        else {
            return true ;
        }
    }

    protected: bool CheakStart( AssemblerRemember cheakedProgram ) {
        if ( thisPsedoCode == "START" )
            return true ;
        else return false ;
    }

    protected: void SettingItSelfAndNextLog(AssemblerRemember &LineProgram ) {
        nowLocation = LineProgram.program[LineProgram.program.size()-1].programToken ; // 1000
        LineProgram.location = nowLocation ;
    }

    protected: void SettingItSelfAndPlusNextLog(AssemblerRemember &LineProgram, int byte) {
        LineProgram.location = nowLocation ;
        nowLocation = PlusHexToLog(nowLocation, byte) ; // 1000
    }

    public: void TestCheakIsPseudo() {
        /*
        this is test function doesn't it is to test CheakIsPseudo can or can not detect the line is pseudo
        */
        for ( auto LineProgram : temp_memory ) {
            if ( ChaekTheSysax( CheakIsPseudo(LineProgram), LineProgram ) ) {
                for ( auto program : LineProgram.program ) {
                    cout << program.programToken << '\t' ;
                }
                cout << endl ;
            }
            else {
                SyntaxError() ;
            }
        }
    }

    protected: void FindDex( AssemblerRemember &LineProgram,int bit ) {
        string dex_num = "" ;
        bool dex_is = false ;
        for ( int i = 0 ; i < LineProgram.program.size() && dex_num == "" ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                i ++ ; 
                if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 4 ) {
                    dex_num = LineProgram.program[i].programToken ;
                    dex_is = true ;
                }
                else if ( LineProgram.program[i].programToken == "#" || LineProgram.program[i].programToken == "@" ) {
                    i ++ ;
                    if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 4 ) {
                        dex_num = LineProgram.program[i].programToken ;
                        dex_is = true ;
                    }
                }
            }
        } // ????????????dex_num
        if ( dex_is == true ) {
            string tempObcode = LineProgram.ObCode ;
            stringstream s ;
            string hex_num ;
            s << hex << stoi(dex_num,nullptr,10) ;
            s >> hex_num ;
            LineProgram.ObCode = tempObcode + hexToBinary(hex_num,bit) ;   
            LineProgram.state = 1 ;
            LineProgram.finished = true ; 
        }    
    }

    protected: void FindLog( AssemblerRemember &LineProgram,int bit ) {

        string label = "" ;
        bool imm = false ;
        bool redict = false ;
        int token_page = 0 ;
        for ( int i = 0 ; i < LineProgram.program.size() && label == "" ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                i ++ ; 
            }
            if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 6 || LineProgram.program[i].tokenPage == 4 ) {
                if ( i != 0 ) {
                    label = LineProgram.program[i].programToken ;
                    token_page = LineProgram.program[i].tokenPage ;
                }
            }
            if ( LineProgram.program[i].tokenPage == 3 && ( LineProgram.program[i].programToken == "#" || LineProgram.program[i].programToken == "@" ) ) {
                if ( LineProgram.program[i].programToken == "#" ) {
                    imm = true ;
                }
                else {
                    redict = true ;
                }
            }
        } // ????????????label
        string tempObcode = LineProgram.ObCode ;
        for ( int i = 0 ; i < temp_memory.size() && ( temp_memory[i].location != "" || temp_memory[i].state == 2 || temp_memory[i].program[0].programToken == "BASE" )  ; i ++ ) {
            if ( temp_memory[i].state != 2 ) {
                if ( token_page != 4 ) {
                    if ( temp_memory[i].program[0].tokenPage == 6 && temp_memory[i].program[0].programToken == label ) { // ??????????????????label
                        LineProgram.ObCode = tempObcode + hexToBinary(temp_memory[i].location,bit) ;
                        LineProgram.state = 1 ;
                        LineProgram.finished = true ; // ????????????????????????
                    }
                    else if ( LineProgram.program.size() == 1 ) {
                        for ( int i = 8 ; i < tempObcode.size() ; i ++  ) {
                            tempObcode[i] = '0' ;
                        }
                        LineProgram.ObCode = tempObcode + hexToBinary("0",bit) ;
                        LineProgram.state = 1 ;
                        LineProgram.finished = true ;
                    }
                }

            }
        } 
        if ( LineProgram.state != 1 && LineProgram.finished != true ) {
            if ( imm == true ) {
                tempObcode[10] = '0' ;
                if ( token_page == 4 ) {
                    LineProgram.ObCode = tempObcode + hexToBinary(label,bit) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // ????????????????????????
                }
            }
            else {
                tempObcode[10] = '0' ;
                if ( token_page == 4 ) {
                    LineProgram.ObCode = tempObcode + hexToBinary(label,bit) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // ????????????????????????
                }               
            }
        }

    }

    protected: virtual void Transaction_Adress( AssemblerRemember &LineProgram, int format ) {
        /*
        ??????????????????, X ?????????
        ????????????????????????opCode ?????????1 ?????????0
        ???line???????????????????????????
        ????????????????????????????????????
        ????????????????????????????????????
        ?????????loc????????? ????????????15???byte
        ????????????????????????1
        */
       int size = LineProgram.program.size() ;
        if ( size >= 4 ) {
            if ( LineProgram.program[size-1].programToken == "X" && LineProgram.program[size-2].programToken == "," && LineProgram.program[size-3].tokenPage == 6 ) {
                LineProgram.ObCode = LineProgram.ObCode + "1" ;
            }
            else {
                LineProgram.ObCode = LineProgram.ObCode + "0" ;
            }
        }
        else {
            LineProgram.ObCode = LineProgram.ObCode + "0" ;
        }
        FindLog( LineProgram,15 ) ;
    }

    protected: string toLower(string str) {
        for ( char &i : str ) {
            if ( i >=65 && i <=90 ) {
                i = i + 32 ;
            }
        }
        return str ;
    }

    protected: string toUpperer(string str) {
        for ( char &i : str ) {
            if ( i >= 97 && i <= 122 ) {
                i = i - 32 ;
            }
        }
        return str ;
    }

    protected: void FindLogBase( AssemblerRemember &LineProgram ) {

        string label = "" ;
        for ( int i = 0 ; i < LineProgram.program.size() && label == "" ; i ++ ) {
            if ( LineProgram.program[i].programToken == "BASE" ) {
                i ++ ; 
                if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 6 ) {
                    label = LineProgram.program[i].programToken ;
                }
            }
        } // ????????????label
        string tempObcode = LineProgram.ObCode ;
        for ( int i = 0 ; i < temp_memory.size() && ( temp_memory[i].location != "" || temp_memory[i].state == 2 || temp_memory[i].program[0].programToken == "BASE" )  ; i ++ ) {
            if ( temp_memory[i].state != 2  ) {
                if ( temp_memory[i].program[0].tokenPage == 6 && temp_memory[i].program[0].programToken == label ) { // ??????????????????label
                    LineProgram.ObCode = "" ;
                    base_register = temp_memory[i].location ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // ????????????????????????
                    cheakedPseudo = "" ;
                }

            }
        }

    }

    protected: virtual int Transaction_ObCode( AssemblerRemember &LineProgram ) {
        /*
        ???????????????instruct???opCode???????????????8???byte
        ???????????????????????????format?????????adress
        */
        bool get = false ;
        for ( int i = 0 ; i < LineProgram.program.size() && get == false ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                Instruction temp ;
                temp = g_InstructionMap[toLower(LineProgram.program[i].programToken)] ;
                LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) ;
                Transaction_Adress(LineProgram, temp.format) ;
                return temp.format ;
            } // ?????????instr
        }
        return 3 ;
    }

    protected: bool FindThisLocation( string label,string & location ) {
        for ( auto LineProgram : temp_memory ) {
            if ( LineProgram.program[0].programToken == label ) {
                if ( LineProgram.location != "" ) {
                    location = LineProgram.location ;
                    return true ;
                }
            }
        }
        return false ;
    }

    protected: void Transaction_Psedo_ObCode( AssemblerRemember &LineProgram,string PseudoCode ) {
        if ( PseudoCode != "BASE" ) {
            if ( PseudoCode != "BYTE" && PseudoCode != "WORD" ) {
                LineProgram.state = 1 ;
                LineProgram.finished = true ;
                LineProgram.ObCode ="" ;
                if ( PseudoCode == "RESB" ) {
                    int temp = stoi( LineProgram.program[LineProgram.program.size()-1].programToken ) ;
                    SettingItSelfAndPlusNextLog( LineProgram, temp ) ; 
                }
                else if ( PseudoCode == "RESW" ) {
                    int temp = stoi( LineProgram.program[LineProgram.program.size()-1].programToken )*3 ;
                    SettingItSelfAndPlusNextLog( LineProgram, temp ) ; 
                }
                else if ( PseudoCode == "EQU" ) {
                    if ( LineProgram.program[LineProgram.program.size()-1].tokenPage == 4 ) {
                        int temp = stoi( LineProgram.program[LineProgram.program.size()-1].programToken ) ;
                        LineProgram.location = PlusHexToLog( "0000",temp ) ;
                    }
                    else if ( LineProgram.program[LineProgram.program.size()-1].programToken == "*" ) {
                        LineProgram.location = nowLocation ;
                    }

                    else if ( LineProgram.program[LineProgram.program.size()-1].tokenPage == 6 ) {
                        LineProgram.state = 0 ;
                        LineProgram.finished = false ;
                        LineProgram.ObCode ="" ;
                        string location = "" ;
                        string tempToken = LineProgram.program[LineProgram.program.size()-2].programToken ;
                        if ( tempToken == "+" || tempToken == "-" || tempToken == "*" || tempToken == "/" ) {
                            if ( LineProgram.program[LineProgram.program.size()-3].tokenPage == 6 ) {
                                string location1 = "" ;
                                string location2 = "" ;
                                if ( FindThisLocation(LineProgram.program[LineProgram.program.size()-1].programToken,location2) && 
                                     FindThisLocation(LineProgram.program[LineProgram.program.size()-3].programToken,location1) ) {
                                    int location_num1 = stoi( location1,nullptr,16 ) ;
                                    int location_num2 = stoi( location2,nullptr,16 ) ;
                                    int result = 0 ;
                                    stringstream s ;
                                    if ( tempToken == "+" ) {
                                        result = location_num1 + location_num2 ;
                                    }
                                    else if ( tempToken  == "-" ) {
                                        result = location_num1 - location_num2 ;
                                    }
                                    else if ( tempToken == "*" ) {
                                        result = location_num1 * location_num2 ;
                                    }
                                    else if ( tempToken == "/" ) {
                                        result = location_num1 / location_num2 ;
                                    }
                                    s << hex << result ;
                                    s >> LineProgram.location ;
                                }
                            }
                        }
                        else {

                            if ( FindThisLocation(LineProgram.program[LineProgram.program.size()-1].programToken,location) ) {
                                LineProgram.location = location ;
                                LineProgram.state = 1 ;
                                LineProgram.finished = true ;
                                LineProgram.ObCode ="" ;
                            }
                        }
                    }
                }
                else {
                    SettingItSelfAndPlusNextLog( LineProgram, 3 ) ; 
                }
            }
            else {
                if ( PseudoCode == "BYTE" || PseudoCode == "WORD" ) {
                    for( int i = 0 ; i < LineProgram.program.size() ; i ++ ) {
                        if ( LineProgram.program[i].programToken == "BYTE" || LineProgram.program[i].programToken == "WORD" ) {
                            i ++ ;
                            if ( LineProgram.program[i].programToken == "X" ) {
                                i ++ ; // ??????????????????X??????C????????????
                                string opcode = "" ;
                                for ( int j = 0 ; j < LineProgram.program[i].programToken.size() ; j ++ ) {
                                    if ( LineProgram.program[i].programToken[j] != '\'' ) {
                                        opcode = opcode + LineProgram.program[i].programToken[j] ;
                                    }
                                }
                                int size = (opcode.size()+1)/2 ;
                                LineProgram.ObCode = hexToBinary(opcode,size*8) ;
                                LineProgram.state = 1 ;
                                LineProgram.finished = true ;
                                SettingItSelfAndPlusNextLog( LineProgram, size ) ; 
                            }
                            else if ( LineProgram.program[i].programToken == "C" ) {
                                i ++ ; // ??????????????????X??????C????????????
                                string opcode = "" ;
                                stringstream ss ;
                                for ( int j = 0 ; j < LineProgram.program[i].programToken.size() ; j ++ ) {
                                    if ( LineProgram.program[i].programToken[j] != '\'' ) {
                                        ss << hex << (int)LineProgram.program[i].programToken[j] ;
                                    }
                                }
                                opcode = ss.str() ;
                                for ( ; opcode.size() < 6 ; ) {
                                    opcode = '0'+ opcode ;
                                }

                                LineProgram.ObCode = hexToBinary(opcode,24) ;
                                LineProgram.state = 1 ;
                                LineProgram.finished = true ;
                                SettingItSelfAndPlusNextLog( LineProgram, 3 )  ;
                            }
                            else {
                                string opcode = "" ;
                                if ( LineProgram.program[i].tokenPage == 4 ) {
                                    int dec_num = stoi( LineProgram.program[i].programToken,nullptr,10 ) ;
                                    stringstream s ;
                                    s << hex << dec_num ;
                                    s >> opcode ;
                                }
                                else {
                                    opcode = LineProgram.program[i].programToken ;
                                }
                                for ( ; opcode.size() < 6 ; ) {
                                    opcode = '0'+ opcode ;
                                }
                                LineProgram.ObCode = hexToBinary(opcode,24) ;
                                LineProgram.state = 1 ;
                                LineProgram.finished = true ;
                                SettingItSelfAndPlusNextLog( LineProgram, 3 )  ;
                            }
                        }
                    }
                }
            }
        }
        else {
            FindLogBase( LineProgram ) ;
        }
    }

    public: void CheakWhole() {
        int count = 0 ;
        for ( auto &LineProgram : temp_memory ) {
            bool IsPseudo = CheakIsPseudo(LineProgram) ;
            cout << "Line " << count << ": " << ChaekTheSysax(IsPseudo,LineProgram) << endl ;
            count ++ ;
        }
    }

    public: void PassOne() {
        bool startBegin  = false ;
        for( auto &LineProgram : temp_memory  ) {
            if ( LineProgram.program[0].programToken != "." ) {
                bool IsPseudo = CheakIsPseudo(LineProgram) ;
                if ( ChaekTheSysax( IsPseudo, LineProgram ) ) {
                    if ( startBegin != true ) {
                        if ( IsPseudo ) {
                            startBegin = CheakStart( LineProgram ) ;
                            if ( startBegin ) {
                                int temp = hexToDec( LineProgram.program[LineProgram.program.size()-1].programToken ) ;
                                nowLocation = PlusHexToLog( nowLocation, temp ) ;
                                LineProgram.location = nowLocation ;
                                LineProgram.finished = true ;
                                LineProgram.state = 1 ;
                            }     
                        }               
                    } // ??????startBegin == false ??????????????????START??????????????? ?????????????????????????????? ????????????????????????????????????
                    else {
                        if ( ! IsPseudo ) {
                            int format = 0 ;
                            format = Transaction_ObCode( LineProgram ) ;
                            SettingItSelfAndPlusNextLog( LineProgram, format ) ;
                        }
                        else {
                            int format = 0 ;
                            Transaction_Psedo_ObCode( LineProgram,thisPsedoCode ) ;
                        }
                    } // ?????????else ????????????start????????????????????????????????? ?????????????????????
                } // ?????????????????? ?????????????????????
                else {
                    LineProgram.state = 2 ;
                    for( Token program : LineProgram.program ) {
                        cout << program.programToken << '\t' ;
                    }
                    SyntaxError() ;
                }
            } // if ( LineProgram.program[0].programToken != "." )
            else {
                LineProgram.state = 1 ;
                LineProgram.finished = true ;
                LineProgram.location="-99999" ;
            }
        } // for ????????????
    }

    public:virtual void PassTwo() {
        for( auto &LineProgram : temp_memory ) {
            if ( LineProgram.finished == false ) {
                bool IsPseudo = CheakIsPseudo(LineProgram) ;
                if (! IsPseudo ) {
                    if ( LineProgram.state != 2 ) {
                        FindLog( LineProgram,15 ) ;
                    }                    
                }
                
            }
        }
    }

} ;

class SICXE : public SIC {





    protected: int CheakFormat( AssemblerRemember cheakedProgram ) {
        for ( auto token : cheakedProgram.program ) {
            if ( token.tokenPage == 0 ) {
                return g_InstructionMap[toLower(token.programToken)].format ;
            }
        }
        return -99999 ;
    }

    protected: bool CheakFormat_One( vector<Token> program ) {
        for ( int i = 0 ; i < program.size()  ; i ++ ) {
            if (  program[i].tokenPage == 0 && i + 1 == program.size() ) {
                return true ;
            }
            else {
                if ( program[i+1].programToken == "." ) {
                    return true ;
                }
                return false ;
            }
        }
        return false ;
    }

    protected: bool CheakFormat_Two( vector<Token> program ) {
        /*
        ?????????????????????????????? ???????????????instruction ?????????????????????Register???????????? ???????????????????????????
        */
        int count = 0 ;
        string instruction = "" ;
        if ( program[count].tokenPage == 6 ) {
            count ++ ;
        }
        if ( program[count].tokenPage == 0 ) {
            instruction = program[count].programToken ;
            switch ( g_InstructionMap[toLower(program[count].programToken)].register_number ) {
                case 1 :  
                    if ( program.size() >= 2 ) {
                        count ++ ;
                        if ( program[count].tokenPage == 2 ) {
                            if ( count == program.size()-1 ) {
                                return true ;
                            }
                            else {
                                if ( program[count+1].programToken == "." ) {
                                    return true ;
                                }
                                else {
                                    return false ;
                                }
                            }
                        }
                        else if ( instruction == "SVC" ) {
                            if ( program[count].tokenPage == 4 ) {
                                if ( count == program.size()-1 ) {
                                    return true ;
                                }
                                else {
                                    if ( program[count+1].programToken == "." ) {
                                        return true ;
                                    }
                                    else {
                                        return false ;
                                    }
                                }
                            }                            
                        }
                    }
                    break ;
                case 2 :
                    if ( program.size() >=4 ) {
                        count ++ ;
                        if ( program[count].tokenPage == 2 ) {
                            count ++ ;
                            if ( program[count].programToken == "," ) {
                                count ++ ;
                                if ( program[count].tokenPage == 2 ) {
                                    if ( count == program.size()-1 ) {
                                        return true ;
                                    }
                                    else {
                                        if ( program[count+1].programToken == "." ) {
                                            return true ;
                                        }
                                        else {
                                            return false ;
                                        }
                                    }
                                }
                                else if ( instruction == "SHIFTL" || instruction == "SHIFTR" ) {
                                    if ( program[count].tokenPage == 4 ) {
                                        if ( count == program.size()-1 ) {
                                            return true ;
                                        }
                                        else {
                                            if ( program[count+1].programToken == "." ) {
                                                return true ;
                                            }
                                            else {
                                                return false ;
                                            }
                                        }                                    
                                    }
                                }
                            }
                        }
                    }
            }

        }
        return false ;
    }

    protected: bool CheakIMM( vector<Token> cheakedProgram, int cheakNum ) {
        if ( cheakedProgram.size() >= 3 ) {
            for ( int i = cheakNum ; i < cheakedProgram.size() ; i ++ ) {
                if ( cheakedProgram[i].programToken == "#" ) {
                    if ( cheakedProgram[i+1].tokenPage == 6 ) {
                        return true ;
                    }
                    else if ( cheakedProgram[i+1].tokenPage == 4 ) {
                        return true ;
                    }
                    else {
                        return false ;
                    }
                }
            }
        }
        return false ;
    }


    protected: bool CheakDirect( vector<Token> cheakedProgram, int cheakNum ) {
        if ( cheakedProgram.size() >= 2 ) {
            for ( int i = cheakNum ; i < cheakedProgram.size() ; i ++ ) {
                if ( cheakedProgram[i].tokenPage == 6 ) {
                    return true ;
                }
                else if ( cheakedProgram[i].tokenPage == 4 ) {
                    return true ;
                }
                else {
                    return false ;
                }
            }
        }
        return false ;
    }

    protected: bool CheakIndirect( vector<Token> cheakedProgram, int cheakNum ) {
        if ( cheakedProgram.size() >= 3 ) {
            for ( int i = cheakNum ; i < cheakedProgram.size() ; i ++ ) {
                if ( cheakedProgram[i].programToken == "@" ) {
                    if ( cheakedProgram[i+1].tokenPage == 6 ) {

                        return true ;
                    }
                    else {
                        return false ;
                    }
                }
            }
        }
        return false ;
    }

    protected: bool CheakIndex( vector<Token> cheakedProgram, int cheakNum ) {
        if ( cheakedProgram.size() >= 4 ) {
            for ( int i = cheakNum ; i < cheakedProgram.size() ; i ++ ) {
                if ( cheakedProgram[i].tokenPage == 6 ) {
                    if ( cheakedProgram[i+1].programToken == "," ) {
                        if ( cheakedProgram[i+2].programToken == "X" ) {
                            return true ;
                        }
                    }
                }
            }
        }
        return false ;
    }   


    protected: bool CheakLiteral( vector<Token> cheakedProgram, int cheakNum ) {
        if ( cheakedProgram.size() >= 3 ) {
            for ( int i = cheakNum ; i < cheakedProgram.size() ; i ++ ) {
                if ( cheakedProgram[i].programToken == "X" || cheakedProgram[i].programToken == "C" ) {
                    if ( cheakedProgram[i+1].tokenPage == 5 ) {
                        return true ;
                    }
                }
            }
        }
        return false ;
    }


    protected: bool CheakM( vector<Token> cheakedProgram, int cheakNum ) {
        if ( CheakIMM(cheakedProgram,cheakNum) ) {
            return true ;
        }
        else if ( CheakDirect(cheakedProgram,cheakNum) ) {
            return true ;
        }
        else if ( CheakIndirect(cheakedProgram,cheakNum) ) {
            return true ;
        }
        else if ( CheakIndex(cheakedProgram,cheakNum) ) {
            return true ;
        }
        else if ( CheakLiteral(cheakedProgram,cheakNum) ) {
            return true ;
        }
        return false ;
    }

    protected: bool CheakFormat_Three( vector<Token> cheakedProgram , int & format ) {
        if ( ( cheakedProgram[0].programToken == "RSUB" && cheakedProgram.size() == 1 ) || ( cheakedProgram.size() > 1 && cheakedProgram[1].programToken == ".") ) {
            return true ;
        }
        else {
            for ( int i = 0 ; i < cheakedProgram.size() ; i ++ ) {
                if ( i == 0 && cheakedProgram[i].programToken == "+" ) {
                    ;
                }
                else {
                    if ( cheakedProgram[i].tokenPage == 0 ) {
                        i ++ ;
                        return CheakM( cheakedProgram, i ) ;                
                    }
                }
            }
            return false ;
        }
    }

    protected: bool CheakInstructCodeSysax( AssemblerRemember cheakedProgram ) override {
        int format = 0 ;
        format = CheakFormat( cheakedProgram ) ;
        switch ( format ) {
        case 1 :
            return CheakFormat_One( cheakedProgram.program ) ;
            break ;
        
        case 2:
            return CheakFormat_Two( cheakedProgram.program ) ;
            break ;
        
        case 3:
            return CheakFormat_Three( cheakedProgram.program,format ) ;
            break ;
        default:
            break;
        } // switch
        return false ;
    }

    protected: string CheakFlag(AssemblerRemember &LineProgram, int format) {
        string flag = "" ;
        string n = "1" ;
        string e = "0" ;
        string i = "1" ;
        string b = "0" ;
        string p = "1" ;
        string x = "0" ;
        if ( format == 4 ) {
            e = "1" ;
            p = "0" ;
        }
        for ( auto program : LineProgram.program ) {
            if ( program.programToken == "X" ) {
                x = "1" ;
            }
            if ( program.programToken == "@" ) {
                i = "0" ;
            }
            if ( program.programToken == "#" ) {
                n = "0" ;
            }
            if ( program.programToken == "B" ) {
                b = "1" ;
            }
            if  ( program.programToken == "PC" ) {
                p = "1" ;
            }
        }
        flag = n + i + x + b + p + e ;

        return flag ;
    }


    public: string CountDisp( string nowlocationString, string dislocationString ) {
        int nowlocation = hexToDec( nowlocationString ) ;
        int distination = hexToDec( dislocationString  ) ;
        int final =  distination - nowlocation ;
        if ( final < -2048 || final > 2048 ) {
            if (base_register != "") {
                int baselocaltion = hexToDec(base_register) ;
                final = distination - baselocaltion ;
                if ( final < 0 || final > 4096 ) {
                    return "error" ;
                }
            }
        }
        stringstream s ;
        s << hex << final ;
        string Disp ;
        s >> Disp ;
        Disp = hexToBinary(Disp,12) ;
        return Disp ;
    }

    protected: void FindDisp( AssemblerRemember &LineProgram, int bit, bool passTwo ) {
        string label = "" ;
        string PC = "" ;
        int label_page = 0 ;
        bool imm = false ;
        bool redict = false ;
        int token_page = 0 ;
        for ( int i = 0 ; i < LineProgram.program.size() && label == "" ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                i ++ ; 
            }
            if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 6 || LineProgram.program[i].tokenPage == 4 ) {
                if ( i != 0 ) {
                    label = LineProgram.program[i].programToken ;
                    token_page = LineProgram.program[i].tokenPage ;
                }
                if ( passTwo == false ) {
                    PC = PlusHexToLog(nowLocation,3) ;
                }
                else {
                    PC = PlusHexToLog(LineProgram.location,3) ;
                }
            }
            if ( LineProgram.program[i].tokenPage == 3 && ( LineProgram.program[i].programToken == "#" || LineProgram.program[i].programToken == "@" ) ) {
                if ( LineProgram.program[i].programToken == "#" ) {
                    imm = true ;
                }
                else {
                    redict = true ;
                }
            }
        } // ????????????label
        string tempObcode = LineProgram.ObCode ;
        for ( int i = 0 ; i < temp_memory.size() && ( temp_memory[i].location != "" || temp_memory[i].state == 2 || temp_memory[i].program[0].programToken == "BASE" )  ; i ++ ) {
            if ( temp_memory[i].state != 2 ) {
                if ( token_page != 4 ) {
                    if ( temp_memory[i].program[0].tokenPage == 6 && temp_memory[i].program[0].programToken == label ) { // ??????????????????label
                        int nowlocation = hexToDec( PC ) ;
                        int distination = hexToDec( temp_memory[i].location ) ;
                            int final =  distination - nowlocation ;
                            if (final > -2048 && final < 2048 ) {
                                LineProgram.ObCode = tempObcode + CountDisp(PC,temp_memory[i].location) ;
                                LineProgram.state = 1 ;
                                LineProgram.finished = true ; // ????????????????????????
                            }
                            else {
                                if ( base_register != "" ) {
                                    tempObcode[9] = '1' ;
                                    tempObcode[10] = '0' ;
                                    LineProgram.ObCode = tempObcode + CountDisp(base_register,temp_memory[i].location) ;
                                    LineProgram.state = 1 ;
                                    LineProgram.finished = true ; // ????????????????????????                                
                                }
                            }

                    }
                    else if ( LineProgram.program.size() == 1 ) {
                        for ( int i = 8 ; i < tempObcode.size() ; i ++  ) {
                            tempObcode[i] = '0' ;
                        }
                        LineProgram.ObCode = tempObcode + hexToBinary("0",bit) ;
                        LineProgram.state = 1 ;
                        LineProgram.finished = true ;
                    }
                }

            }
        } 
        if ( LineProgram.state != 1 && LineProgram.finished != true ) {
            if ( imm == true || redict == true  ) {
                tempObcode[10] = '0' ;
                if ( token_page == 4 ) {
                    LineProgram.ObCode = tempObcode + hexToBinary(label,bit) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // ????????????????????????
                }
            }
            else {
                tempObcode[10] = '0' ;
                if ( token_page == 4 ) {
                    LineProgram.ObCode = tempObcode + hexToBinary(label,bit) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // ????????????????????????
                }               
            }
        }       
        
    }


    protected: void Transaction_Adress( AssemblerRemember &LineProgram, int format ) override {
       LineProgram.ObCode = LineProgram.ObCode + CheakFlag( LineProgram,format ) ;
       if ( format == 3 ) {
        FindDisp(LineProgram,12,false) ;
       }
       else if ( format == 4 ) {
        FindLog(LineProgram,20) ;
       }
    }

    protected: int Transaction_ObCode( AssemblerRemember &LineProgram ) override {
        /*
        ???????????????instruct???opCode???????????????8???byte
        ???????????????????????????format?????????adress
        */
        bool get = false ;
        for ( int i = 0 ; i < LineProgram.program.size() && get == false ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                Instruction temp ;
                temp = g_InstructionMap[toLower(LineProgram.program[i].programToken)] ;
                stringstream s1 ;
                stringstream s2 ;
                int first_register = 1 ;
                int second_register = 3 ;
                switch ( temp.format ) {
                    case 1 :
                        LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) ;
                        LineProgram.state = 1 ;
                        LineProgram.format = 1 ;
                        LineProgram.finished = true ; // ????????????????????????
                        break ;
                    case 2 :
                        if ( LineProgram.program[0].tokenPage == 6  ) {
                            first_register ++ ;
                            second_register ++ ;
                        }
                        if ( temp.register_number ==2 ) {
                            s1 << LineProgram.program[first_register].tokenPosition ;
                            s2 << LineProgram.program[second_register].tokenPosition ;
                            LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) + hexToBinary(s1.str(),4) + hexToBinary(s2.str(),4);
                        }
                        else {
                            s1 << LineProgram.program[first_register].tokenPosition ;
                            LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) + hexToBinary(s1.str(),4) + hexToBinary("0",4);
                        }
                        LineProgram.state = 1 ;
                        LineProgram.format = 2 ;
                        LineProgram.finished = true ; // ????????????????????????    
                        break ;
                    case 3 :
                        LineProgram.ObCode = hexToBinary(temp.opcode,8) ;
                        LineProgram.ObCode.erase(6,2) ;
                        if ( LineProgram.program[0].programToken == "+" || ( LineProgram.program[0].tokenPage == 6 && LineProgram.program[1].programToken == "+" ) ) {                           
                            LineProgram.format = 4 ;
                            Transaction_Adress(LineProgram, 4) ;
                            return 4 ;
                        }
                        else {
                            LineProgram.format = 3 ;
                            Transaction_Adress(LineProgram, temp.format) ;
                            return 3 ;
                        }
                        break ;
                    default :
                        break ;
                }
                return temp.format ;
            } // ?????????instr
        }
        return 3 ;
    }

    public: void PassTwo() override {
        for( auto &LineProgram : temp_memory ) {
            if ( LineProgram.finished == false ) {
                bool IsPseudo = CheakIsPseudo(LineProgram) ;
                if (! IsPseudo ) {
                    if ( LineProgram.state != 2 ) {
                        if ( LineProgram.format == 3 ) {
                            FindDisp( LineProgram,12,true ) ;
                        }
                        else if ( LineProgram.format == 4 ) {
                            FindLog( LineProgram,20 ) ;
                        }
                    }                    
                }
                if ( cheakedPseudo == "BASE" ) {
                    FindLogBase(LineProgram) ;
                }
            }
            cheakedPseudo = "" ;
        }
    }


} ;

bool IsNumber( string token ) {
    for( char ch : token ) {
        if ( ch < '0' || ch > '9' ) {
            return false ;
        }
    }
    return true ;
}

bool IsString( string token ) {
    if ( !( token[0] == '\'' && token[token.size()-1] == '\'' ) ) {
        return false ;
    }
    else {
        return true ;
    }    
}

bool SearchTable( Page_Token tempTable,  Token &tempToken ) {
    /*
    ??????????????????table ?????????????????????????????? ??????????????????
    */
    bool GetRow = false ;
    for ( int i = 0 ; i < tempTable.page_token_no.size() && GetRow != true ; i ++ ) {
        if ( strcasecmp(tempTable.page_token_no[i][0].c_str(), tempToken.programToken.c_str()) == 0 )  {            
            tempToken.tokenPosition = i ;
            GetRow = true ;
        }
    }
    return GetRow ;
}

void SerchANDCreatTable( Page_Token &tempTable,  Token &tempToken ) {
    /*
    ?????????????????? ??????????????????table???token ??????????????????????????? ?????????????????????????????????token???????????? ???????????????call by reference
    */
    bool GetRow = false ;
    for ( int i = 0 ; i < tempTable.page_token_no.size() && GetRow != true  ; i ++ ) {
        if ( tempTable.page_token_no[i][0] == tempToken.programToken )  {            
            tempToken.tokenPosition = i ;
            GetRow = true ;
        }
    }
    if ( GetRow == false ) {
        /*
        ??????????????????????????????token??????????????????????????? ??????token??????
        */
        vector<string> line_token ;
        line_token.push_back( tempToken.programToken ) ;
        tempTable.page_token_no.push_back( line_token ) ;
        tempToken.tokenPosition = tempTable.page_token_no.size() -1 ;
    }
}

bool GetTableLine(int table_NO, Token &tempToken ) {
    /*
    ?????????????????????NO ??????????????????table????????? ???????????????table??????????????? ??????Token???page???row call ??????
    */
    bool getTable = false ;   
    switch ( table_NO ) {
    case 0 :
        getTable = SearchTable(g_instructionTable , tempToken) ;
        tempToken.tokenPage = table_NO ;
        break ;
    case 1 :
        getTable = SearchTable(g_pseudoInsTable , tempToken) ;
        tempToken.tokenPage = table_NO ;
        break ;
    case 2 :
        getTable = SearchTable(g_registerTable , tempToken) ;
        tempToken.tokenPage = table_NO ;
        break ;
    case 3 :
        getTable = SearchTable(g_delimiterTable , tempToken) ;
        tempToken.tokenPage = table_NO ;
        break ; 
    case 4 :
        if ( IsNumber(tempToken.programToken) ) {
            SerchANDCreatTable(g_numberTable , tempToken) ;
            tempToken.tokenPage = table_NO ;
        }
        break ;
    case 5 :
        if ( IsString(tempToken.programToken) ) {
            SerchANDCreatTable(g_stringTable , tempToken) ;
            tempToken.tokenPage = table_NO ;
        }
        break ;    
    case 6 :
        if ( !IsNumber(tempToken.programToken) && ! IsString(tempToken.programToken) ) {
            SerchANDCreatTable(g_otherTable , tempToken) ;
            tempToken.tokenPage = table_NO ;
        }
        break ;
    default :
    break ;
    }
    return getTable ;
}

Token GetToken( string token ) {
    /*
    use string to know what is this token table and rows, if not creat a new table to this string 
    */
    Token tempToken ;
    bool GetPage = false ;
    tempToken.programToken = token ;

    for ( int i = 0 ; i < 7 && GetPage != true ; i ++ ) {
        GetPage = GetTableLine(i, tempToken ) ;
    }
    return tempToken ; 
}

void LoadingPageTokenInital( string file_name, Page_Token &page_token) {
    page_token.page_token_no.clear() ;
    string token = "" ;
    vector<string> line_token ;
    bool end = true ;
    GetTokenMachine machine1(file_name) ; // the machine is a class and we can used to get the token
    do {
        end = machine1.GetNextToken(token) ; // end is a bool means the page is EOF if end is false
        if ( token != "" ) {
            // here we can put some choise to get token like's string
            line_token.push_back(token) ;

        } // the compare is very important!!! that can cheak the token does read something
        if ( machine1.IsEnterChar() ) {
            if ( line_token.size() > 0 ) {
                page_token.page_token_no.push_back(line_token) ;
                line_token.clear() ;
            }
        } // this is means the line of page is readed            
    } while ( end ) ;   // this while is we can loading all token of page 
}

void LoadingPageToken( string file_name, Program_Token &page_token) {
    page_token.token.clear() ;
    string token = "" ;
    vector<Token> line_token ;
    bool end = true ;
    SICXEGetTokenMachine machine1(file_name) ; // the machine is a class and we can used to get the token
    do {
        end = machine1.GetNextToken(token) ; // end is a bool means the page is EOF if end is false
        if ( token != "" ) {
            // here we can put some choise to get token like's string
            Token tempToken ;
            tempToken = GetToken(token) ;
            line_token.push_back(tempToken) ;

        } // the compare is very important!!! that can cheak the token does read something
        if ( machine1.IsEnterChar() ) {
            if ( line_token.size() > 0 ) {
                page_token.token.push_back(line_token) ;
                line_token.clear() ;
            }
        } // this is means the line of page is readed            
    } while ( end ) ;   // this while is we can loading all token of page 
}

void LoadingInital() {
    loadingMap(g_InstructionMap) ;
    LoadingPageTokenInital("Table1.table",g_instructionTable ) ;
    LoadingPageTokenInital("Table2.table",g_pseudoInsTable ) ;
    LoadingPageTokenInital("Table3.table",g_registerTable ) ;
    LoadingPageTokenInital("Table4.table",g_delimiterTable ) ;
}


int main()
{

    LoadingInital() ;
    string file_name = "" ;
    string chiose = "" ;
    Program_Token program_token ;
    cout << "chiose SIC[1] or SICXE[2], other is exit" << endl ;
    cin >> chiose ;
    if ( chiose == "1" ) {
        cout << "input fileName :" ;
        cin >> file_name ;
        do {
            int i = 0 ;        
            LoadingPageToken(file_name,program_token) ;
            SIC assembler1 ;
            for(auto TokenLine : program_token.token ) {
                assembler1.LoadingProgram( TokenLine ) ;
            }
            assembler1.PassOne() ;
            assembler1.PassTwo() ;
            assembler1.PrintAllMessage() ;
            assembler1.WriteFile(file_name) ;
            cout << "input fileName :" ;
            cin >> file_name ;
        }while ( file_name != "0" ) ;
    }
    else if ( chiose == "2" ) {
        cout << "input fileName :" ;
        cin >> file_name ;
        do {
            int i = 0 ;        
            LoadingPageToken(file_name,program_token) ;
            SICXE assembler1 ;
            for(auto TokenLine : program_token.token ) {
                assembler1.LoadingProgram( TokenLine ) ;
            }
            assembler1.PassOne() ;
            assembler1.PassTwo() ;
            assembler1.PrintAllMessage() ;
            assembler1.WriteFile(file_name) ;
            cout << "input fileName :" ;
            cin >> file_name ;
        }while ( file_name != "0" ) ;
    }
    
    
}
/*
????????????????????????????????????
?????????????????????
*/