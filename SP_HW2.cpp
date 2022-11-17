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
    loadIngMap["mulf"] = {1,"60"} ;
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
    從前面開始抓 先看長度是多少 決定第一個怎麼取
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
    return hexString ;
}

string hexToBinary(string hex_string, int lenth) {
    /*
    根據讀入的字串及數字 轉換成特定長度的binary
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

} ;

class Assembler{

    protected: string nowLocation = "0000" ;
    protected: vector<AssemblerRemember> temp_memory ;
    protected: string thisPsedoCode = "" ;

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
                for ( auto program : temp_memory[now].program ) {
                    cout << program.programToken << '\t' ;           
                }
                if ( temp_memory[now].finished ) {
                    if ( temp_memory[now].ObCode == "") {
                        cout << endl ;
                    }
                    else {
                        cout << BinarytoHex(temp_memory[now].ObCode) << endl ;
                    }
                }
                else {
                    cout << endl ;
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
    
    2022/09/20 把token都處理完畢了 要開始弄可愛的opcode
    PASS_ONE  :
        演算法的部份:
            for 每一行 :
                if 文法 == 通過 :
                    如果 token_6 START Hex 這樣代表從Hex開始 並且下一個是1000 這個沒東西
                    先紀錄該行的loc 
                    if 有指令 == true :
                        把該行instruction code 記錄到前兩位
                        if 掃前面有label出現 :
                            把前面label的Loc輸入至後面4位
                            設定該行opcode完成
                        else :
                            等待pass2再處理
                    else :
                        if 是 BYTE C'string' :
                            把string用ascii轉換 套進opcode 
                        elif 是 WORD table_4 :
                            直接把hex轉成dec並放進opcode
                        elif 是 BYTE X' string ' :
                            opcode 轉成hex 不要有0000補充
                        設定該行opcode完成
                    loc + 3 
                else :
                    印出錯誤
                    將該行狀態改為2
                進入下一行

    PASS2 :
        for 每一行 :
            if 這行的opcode未完成 :
                for 下面的每一行 :
                    if program[0] 為table_6 :
                        將該行log輸入進目前opcode[2:6]
                        跳離迴圈
                    else :
                        換下一行
            else :
                不做事
            換下一行                    
    */

class SIC : public Assembler {

    protected: bool CheakIsPseudo( AssemblerRemember cheakedProgram ) {
        /*
        該行讀入後確定是否擁有pseudo code 如果是 回傳true 不是 回傳false
        */
        bool getTable = false ;
        for( int i = 0 ; i < cheakedProgram.program.size() && ! getTable ; i++ ) {
            if ( cheakedProgram.program[i].tokenPage == 1 ) {
                getTable = true ;
            }
        } // 先確認這裡面有沒有pseudo
        return getTable ;
    }



    protected: bool CheakPsedoCodeSysaxFinal( AssemblerRemember cheakedProgram, int position ) {
        int cheakNum = 0 ;
        if ( position != 6 ) {
            if ( cheakedProgram.program[cheakNum].tokenPage == 6 ) {
                cheakNum ++ ;
            } // { label } 有遇到就跳過
            cheakNum ++ ;
            switch ( position ) {
            case 0 :
                if ( cheakedProgram.program[cheakNum].tokenPage == 4 ) {
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
        經過這邊代表知道他是psedoCode 我們要先知道他是哪一種pseudoCode把他丟到相對應的文法分析裡面
        */
        string psedoCode = "" ;
        for ( auto token : cheakedProgram.program ) {
            if ( token.tokenPage == 1 ) {
                return CheakPsedoCodeSysaxFinal( cheakedProgram, token.tokenPosition ) ;       
            } // 代表他在token_table2 這個是psedoCode的table
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
        確認他的文法是否正確
        分成兩層 只要知道他們是哪種文法 code 這function會自己想辦法
        */
        if ( cheakedProgram.program[0].programToken[0] != '.' ) {
            if ( IsPseudo ) {
                return CheakPsedoCodeSysax( cheakedProgram ) ;
            } // 如果是pseudo 要用他自己的作法
            else {
                return CheakInstructCodeSysax(cheakedProgram) ;
            } // 如果是一般的 則是用正常的文法
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

    protected: void FindLog( AssemblerRemember &LineProgram ) {
        string label = "" ;
        for ( int i = 0 ; i < LineProgram.program.size() && label == "" ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                i ++ ; 
                if ( i < LineProgram.program.size() && LineProgram.program[i].tokenPage == 6 ) {
                    label = LineProgram.program[i].programToken ;
                }
            }
        } // 找這行的label
        string tempObcode = LineProgram.ObCode ;
        for ( int i = 0 ; i < temp_memory.size() && ( temp_memory[i].location != "" || temp_memory[i].state == 2 )  ; i ++ ) {
            if ( temp_memory[i].state != 2 ) {
                if ( temp_memory[i].program[0].tokenPage == 6 && temp_memory[i].program[0].programToken == label ) { // 找跟他一樣的label
                    LineProgram.ObCode = tempObcode + hexToBinary(temp_memory[i].location,15) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ; // 找的到就把他改好
                }
                else if ( LineProgram.program.size() == 1 ) {
                    LineProgram.ObCode = tempObcode + hexToBinary("0",15) ;
                    LineProgram.state = 1 ;
                    LineProgram.finished = true ;
                }
            }
        }
    }

    protected: virtual void Transaction_Adress( AssemblerRemember &LineProgram, int format ) {
        /*
        先確定是否有, X 的出現
        如果有的話該行的opCode 多一個1 沒有就0
        用line知道現在讀到哪一行
        由上往下讀到這條的前一行
        如果在該行的第一項有出現
        把他的loc拿來用 放到後面15個byte
        順便讓結束指令為1
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
        FindLog( LineProgram ) ;
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

    protected: virtual int Transaction_ObCode( AssemblerRemember &LineProgram ) {
        /*
        他會先根據instruct的opCode來找出前面8個byte
        接下來再根據後面的format來找出adress
        */
        bool get = false ;
        for ( int i = 0 ; i < LineProgram.program.size() && get == false ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                Instruction temp ;
                temp = g_InstructionMap[toLower(LineProgram.program[i].programToken)] ;
                LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) ;
                Transaction_Adress(LineProgram, temp.format) ;
                return temp.format ;
            } // 有找到instr
        }
        return 3 ;
    }

    protected: void Transaction_Psedo_ObCode( AssemblerRemember &LineProgram,string PseudoCode ) {
        if ( PseudoCode != "BYTE" && PseudoCode != "WORD" ) {
            LineProgram.state = 1 ;
            LineProgram.finished = true ;
            LineProgram.ObCode ="" ;
            if ( PseudoCode == "RESB" ) {
                int temp = stoi( LineProgram.program[LineProgram.program.size()-1].programToken ) ;
                SettingItSelfAndPlusNextLog( LineProgram, temp ) ; 
            }
            else if ( PseudoCode == "EQU" ) {
                int temp = stoi( LineProgram.program[LineProgram.program.size()-1].programToken ) ;
                LineProgram.location = PlusHexToLog( "0000",temp ) ;
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
                            i ++ ; // 這裡可以得到X後或C後的字串
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
                            i ++ ; // 這裡可以得到X後或C後的字串
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
                        }
                        else {
                            string opcode = "" ;
                            opcode = LineProgram.program[i].programToken ;
                            for ( ; opcode.size() < 6 ; ) {
                                opcode = '0'+ opcode ;
                            }
                            LineProgram.ObCode = hexToBinary(opcode,24) ;
                            LineProgram.state = 1 ;
                            LineProgram.finished = true ;
                        }
                        SettingItSelfAndPlusNextLog( LineProgram, 3 ) ;;
                    }
                }
            }
            SettingItSelfAndPlusNextLog( LineProgram, 3 )  ;
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
                    } // 如果startBegin == false 代表還沒出現START這個虛指令 需要先判斷這行是不是 如果出現過了就不用管它了
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
                    } // 此處要else 第一個是start他會獨自執行自己的工作 其他人不要管他
                } // 代表文法正確 可以繼續下一項
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
        } // for 逐行讀取
    }

    public: void PassTwo() {
        for( auto &LineProgram : temp_memory ) {
            if ( LineProgram.finished == false ) {
                bool IsPseudo = CheakIsPseudo(LineProgram) ;
                if (! IsPseudo ) {
                    if ( LineProgram.state != 2 ) {
                        FindLog( LineProgram ) ;
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
        檢查第二型態的對不對 先看第一個instruction 判斷他要有一個Register還是兩個 再判斷是否合乎文法
        */
        int count = 0 ;
        if ( program[count].tokenPage == 0 ) {
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
        char n = '1' ;
        char e = '0' ;
        char i = '1' ;
        char b = '0' ;
        char p = '0' ;
        char x = '0' ;
        if ( format == 4 ) {
            e = '1' ;
        }
        for ( auto program : LineProgram.program ) {
            if ( program.programToken == "X" ) {
                x = '1' ;
            }
            if ( program.programToken == "@" ) {
                i = '0' ;
            }
            if ( program.programToken == "#" ) {
                n = '0' ;
            }
            if ( program.programToken == "B" ) {
                b = '1' ;
            }
            if  ( program.programToken == "PC" ) {
                p = '1' ;
            }
        }
        flag = n + i + x + b + p + e ; 
        return flag ;
    }

    protected: void Transaction_Adress( AssemblerRemember &LineProgram, int format ) override {
       /* 11/8 */
    }

    protected: int Transaction_ObCode( AssemblerRemember &LineProgram ) override {
        /*
        他會先根據instruct的opCode來找出前面8個byte
        接下來再根據後面的format來找出adress
        */
        bool get = false ;
        for ( int i = 0 ; i < LineProgram.program.size() && get == false ; i ++ ) {
            if ( LineProgram.program[i].tokenPage == 0 ) {
                Instruction temp ;
                temp = g_InstructionMap[toLower(LineProgram.program[i].programToken)] ;
                stringstream s1 ;
                stringstream s2 ;
                switch ( temp.format ) {
                    case 1 :
                        LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) ;
                        LineProgram.state = 1 ;
                        LineProgram.finished = true ; // 找的到就把他改好
                        break ;
                    case 2 :
                        if ( temp.register_number ==2 ) {
                            s1 << LineProgram.program[1].tokenPosition ;
                            s2 << LineProgram.program[3].tokenPosition ;
                            LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) + hexToBinary(s1.str(),4) + hexToBinary(s2.str(),4);
                        }
                        else {
                            s1 << LineProgram.program[1].tokenPosition ;
                            LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) + hexToBinary(s1.str(),4) + hexToBinary("0",4);
                        }
                        LineProgram.state = 1 ;
                        LineProgram.finished = true ; // 找的到就把他改好    
                        break ;
                    case 3 :
                        if ( LineProgram.program[0].programToken == "+" ) {
                            LineProgram.ObCode[7] =  '\0' ;
                            LineProgram.ObCode[6] = '\0' ;
                            Transaction_Adress(LineProgram, 4) ;
                            return 4 ;
                        }
                        else {
                            LineProgram.ObCode = LineProgram.ObCode + hexToBinary(temp.opcode,8) ;
                            LineProgram.ObCode[7] =  '\0' ;
                            LineProgram.ObCode[6] = '\0' ;
                            Transaction_Adress(LineProgram, temp.format) ;
                        }
                        break ;
                    default :
                        break ;
                }
                return temp.format ;
            } // 有找到instr
        }
        return 3 ;
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
    尋找那該死的table 如果有找到就離開迴圈 並且回傳正值
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
    這是最後一關 先從最後一個table找token 如果找到就給他位置 如果沒找到就只能在這個token新建一個 因此要兩個call by reference
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
        這裡是如果再最後一個token沒找到東西才會進來 新增token用的
        */
        vector<string> line_token ;
        line_token.push_back( tempToken.programToken ) ;
        tempTable.page_token_no.push_back( line_token ) ;
        tempToken.tokenPosition = tempTable.page_token_no.size() -1 ;
    }
}

bool GetTableLine(int table_NO, Token &tempToken ) {
    /*
    根據輸入進來的NO 來看要到哪個table去尋找 如果在那個table裡面有找到 就將Token的page跟row call 進去
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
    cout << g_InstructionMap.size() << endl ;
    LoadingPageTokenInital("Table1.table",g_instructionTable ) ;
    cout << g_instructionTable.page_token_no.size() << endl ;
    LoadingPageTokenInital("Table2.table",g_pseudoInsTable ) ;
    cout << g_pseudoInsTable.page_token_no.size() << endl ;
    LoadingPageTokenInital("Table3.table",g_registerTable ) ;
    cout << g_registerTable.page_token_no.size() << endl ;
    LoadingPageTokenInital("Table4.table",g_delimiterTable ) ;
    cout << g_delimiterTable.page_token_no.size() << endl ;
}


int main()
{
    /*
    cout << hexToBinary("0",15) ;
    
    LoadingInital() ;
    string file_name = "" ;
    Program_Token program_token ;
    cin >> file_name ;
    do {
        int i = 0 ;        
        LoadingPageToken(file_name,program_token) ;
        SICXE assembler1 ;
        for(auto TokenLine : program_token.token ) {
            assembler1.LoadingProgram( TokenLine ) ;
            assembler1.PrintLineMessage( i ) ;
            i ++ ;
        }
        assembler1.PassOne() ;
        assembler1.PassTwo() ;
        assembler1.PrintAllMessage() ;
        cin >> file_name ;
    }while ( file_name != "0" ) ;
    
    */
}

/*
todo list
  11/8 : 
  *寫一個判斷 direct indirect imiediate 的function 
  *寫一個判斷 X 的function
  *寫一個判斷 p b null 的function
  *把他六個bit加上去


*/