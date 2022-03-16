#include <iomanip>
#include <math.h>
#include <stack>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

using namespace std ;

typedef struct aT {
	string name ;
	int subroutine ;
	int type ;
	int pointer ;
} idt ;


typedef struct aL {
	int line ;
	string operators ;
	string operand1 ;
	string operand2 ;
	string result ;
	string code ;
	string errorMsg ;
} line ;

typedef struct aF {
	string label ;
	int t6_loc ;
} fref ;


class compiler {
	
public:

	compiler() {
	
			
		delimiters[";"] = 1 , 	delimiters["("] = 2 , 	delimiters[ ")" ] = 3;
		delimiters[ "="] = 4 , 	delimiters[ "+"] = 5 , 	delimiters[ "-"] = 6 ;
		delimiters[ "*"] = 7 , 	delimiters[ "/" ] = 8 , 	delimiters[ "^"] = 9 ;
		delimiters[ ":"] = 12 ,  delimiters[","] = 100 ;
		
	
		
		reserved["AND"] = 1 , reserved["BOOLEAN"] = 2 , reserved["CALL"] = 3 , reserved["DIMENSION"] = 4 , reserved["ELSE"] = 5 ;
		reserved["ENP"] = 6 , reserved["ENS"] = 7 , reserved["EQ"] = 8 , reserved["GE"] = 9 , reserved["GT"] = 10 ;
		reserved["GTO"] = 11 , reserved["IF" ] = 12 , reserved["INPUT"] = 13 , reserved[ "INTEGER"] = 14  , reserved["LABEL"] = 15 ;
		reserved["LE" ] = 16, reserved["LT"] = 17 , reserved["NE"] = 18 , reserved["OR"] = 19 , reserved["OUTPUT"] = 20 ;
		reserved["PROGRAM"] = 21 , reserved["REAL"] = 22 , reserved["SUBROUTINE"] = 23 , reserved["THEN"] = 24 , reserved["VARIABLE"] = 25 ;

		
		logicvalue["EQ"] = 2 ;
		logicvalue["GE"] = 2 ;
		logicvalue["GT"] = 2 ;
		logicvalue["LE"] = 2 ;
		logicvalue["LT"] = 2 ;
		logicvalue["NE"] = 2 ;
		logicvalue["OR"] = 1 ;
		logicvalue["AND"] = 1 ;
		
		integer = real =  vector< string > ( 100, "" ) ;
		identifier = vector < idt > ( 100 ) ;	
	
		optvalue["^"] = 3 ;
		optvalue["*"] = 2 ;
		optvalue["/"] = 2 ;
		optvalue["+"] = 1 ;
		optvalue["-"] = 1 ;	
		optvalue["="] = 0 ;
		optvalue["("] = 0 ;
		
		info.push_back( -1 ) ;
		

	}



// parameter 

	string file ;
	vector< string > temptable ; // table 0 
	map< string,int > delimiters ;  // table 1 
	map< string, int > reserved ; // table 2 
	vector< string > integer ; // table 3 
	vector< string > real ; // table 4 
	vector< idt > identifier ; // table 5 
	vector< pair<string, vector<int> > > dimension ;
	
	vector< int > info ; // table 7 
	map< string, int > optvalue ;
	map< string, int > logicvalue ;
	
	vector< vector< pair<int,int> > > lex_result ;  // result ex (1,3)
	vector< vector< string > > lex_input ;   //  ori text 
	vector< vector< string > > lex_output ;  // converted text 
	vector< line > inter ; 
	int pc ;

	vector< fref > ifforward ;
	
	
	bool setpc ;
	bool call ;
// func 
	
	
	string to_lower( string  & str ) {
		for ( int i = 0 ; i < str.size(); i++) {
			if (  str[i] >= 'A' and str[i] <= 'Z' )str[i] += 32 ;
		}
		
		return str ;
	}
	
	string to_upper( string & str ) { 
		for ( int i = 0 ; i < str.size(); i++ ) {
			if ( str[i] >= 'a' and str[i] <= 'z' )str[i] -= 32 ;
		}
		
		return str ;
	}
	
	bool is_int( string & str ) {
		for ( int i = 0 ; i < str.size(); i++ ) {
			if ( str[i] < '0' or str[i] > '9') return 0 ;
		}
		
		return 1 ;
	}
	
	bool is_hex( string & str ) {
		for ( int i = 0 ; i < str.size(); i++ ) {
			if ( ! isxdigit(str[i]) ) return 0 ;
			
		}//for
		
		to_upper(str) ;
		return 1 ;
	}

	void lexical_ana() {
		
		string fileName ;
	
		cout << "輸入檔名(不用)txt" ;
		
		cin >> fileName ;  
		file = fileName ;
		ifstream input_file( (fileName+ ".txt" ).c_str() ) ;
	 	
		if (input_file.is_open()){	
			lexical_analysis( input_file ) ;
			pc = 0 ;
		    input_file.close();
		}
		else cout <<   "打不開此檔案 "  << fileName << endl ;	
		
		
		
		for ( int i = 0 ; i < lex_result.size(); i++) {
			for ( int j = 0 ; j < lex_result[i].size(); j++) {
				cout << lex_input[i][j] << ' ' ;
			}
			
			cout << endl ;
			
			for ( int j = 0 ; j < lex_result[i].size(); j++) {
				cout << lex_result[i][j].first << ',' << lex_result[i][j].second << ' ' ;
			}
			cout << endl ;
		}
		
		for ( int i = 0 ; i < identifier.size(); i++ ) {
			cout << i << " " << identifier[i].name << ' ' << identifier[i].subroutine  << endl ;
		}
		
	
	}
	
	string ptopar( pair<int,int> p ) {
		return "(" + to_string(p.first) + "," + to_string(p.second) + ")" ;
	}
	
	
	string ptopar( int a, int b ) {
		return "(" + to_string(a) + "," + to_string(b) + ")" ;
	}
	
	pair<int,int> locate( int type, string str, bool head = false  ) {
		int idx = 0 ;
		if ( type == 0 ) {
			return make_pair(0, atoi( str.substr(1).c_str() ) ) ;
		}
		if ( type == 3 ) {
			while ( str!=integer[idx] ) 
				idx++ ;
				
			return make_pair(3,idx) ;
		}
		
		if ( type == 4 ) {
			while ( str!=real[idx] ) 
				idx++ ;
				
			return make_pair(4,idx) ;
		}
		
		if ( type == 5 ) {
			
			if ( ! head ) {
				while ( str!=identifier[idx].name or pc != identifier[idx].subroutine )  
					idx++ ;	
			}
			else 
				while ( str!=identifier[idx].name )  
					idx++ ;	

				
			return make_pair(5,idx) ;
		}
	}
	
	
	
	
	void syntax() {
		
		
		int linectr = 1 ;
		
		for ( int i = 0 ; i < lex_result.size(); i++) {
			bool label = false ;
			cout << lex_output[i][0] << endl ;
			line one ; 
			
			
			if ( lex_output[i][ lex_result[i].size()-1 ] != ";" ) {
				one.line = i+1 ;
				one.errorMsg = "最後的字元不是結束指令';'\nSyntax Error!!!" ;
				inter.push_back(one) ;
				break ;
			}//if
			
			int len = lex_result[i].size()-1 ;
	
			string cur = lex_output[i][0] ;
			
			if ( lex_result[i][0].first == 5 ) {
				if ( identifier[lex_result[i][0].second].type ==5 ) {  // is a label ;
					label = true ;
					cur = lex_output[i][1] ;
					int loc = identifier[lex_result[i][0].second].pointer = inter.size() + 1 ;
					
					
					// check is there's an if need a label loc
					
					string name = lex_output[i][0] ;
					for ( int j = 0 ; j < ifforward.size(); ) {
						if ( name == ifforward[j].label ) {
							
							inter[ifforward[j].t6_loc].result = ptopar( 6, loc ) ;
							inter[ifforward[j].t6_loc].code += name ;
							
							ifforward.erase( ifforward.begin()+j ) ;
						}
						else
							j++ ;
					}
				}	
			}
			
			if ( cur == "LABEL" ) {
				
				
				if ( lex_result[i][1].first == 5 ) {
				
				
					for ( int j = 1 ; j < len ; j++ ) {
						if ( lex_result[i][j].first == 5  ) {
							identifier[ lex_result[i][j].second].type = 5 ;
							one.line = linectr ;
							one.code = lex_output[i][j] ;
							one.operators = ptopar( lex_result[i][j] ) ;
							inter.push_back(one) ;
							linectr++ ;
	
						}
					}
					
					continue ;
				}
			
			}	
			else if ( cur == "SUBROUTINE") {
				pc = locate( 5,lex_output[i][1] ).second ;
				


				string type = lex_output[i][3] ;
				
				for ( int j = 4 ; j < len ; j++) {
					if ( lex_result[i][j].first == 5  ) {
						cout << lex_output[i][j] << endl ;
						one.line = linectr ;
						one.code = lex_output[i][j] ;
						one.operators = ptopar( lex_result[i][j] ) ;
						
						if ( type == "BOOLEAN" )
								identifier[lex_result[i][j].second].type = 2 ;
							
						if ( type == "INTEGER" )
								identifier[lex_result[i][j].second].type = 4 ;
							
						if ( type ==  "REAL" )
								identifier[lex_result[i][j].second].type = 6 ;
						
						inter.push_back(one) ;
						linectr++ ;
					}
				}
			}
			else if ( cur == "PROGRAM" ) {
				if ( cur == "PROGRAM"){ 
				
					pc = locate( 5,lex_output[i][1], true ).second ;
					
					identifier[pc].pointer = 1 ;
					
				} 

			}
			else if ( cur == "CALL" ) {
							
				one.line = linectr ;
				one.operand1 = ptopar( locate(5, lex_output[i][1], true )) ;
				one.result = ptopar( 7, info.size())  ;
				one.operators = ptopar( 2,3 ) ;
				
		
				vector<string> clist ;
				for ( int j = 2 ; j < len ; j++ ) {
					if ( lex_result[i][j].first != 1 ) {
						clist.push_back( lex_output[i][j]) ;
					}
				}
				

				
				info.push_back( clist.size() ) ;
				for ( int j = 0 ; j < clist.size(); j++ ) {
					pair<int,int>  p = locate( type(clist[j]), clist[j]) ;
					info.push_back( p.first ) ;
					info.push_back( p.second ) ;
				}
				
				one.code = "CALL " ;
				for ( int j = 1 ; j < len ; j++ ) {
					one.code += lex_output[i][j]  ;
				}
	
				inter.push_back( one ) ;
	 			linectr++ ;
				continue ;
			}
			else if ( cur == "DIMENSION") {
				int j = 1 ;

				string type = lex_output[i][j] ;
				
				for ( ; j < len ; j++) {
					if ( lex_result[i][j].first == 5  ) {
						cout << lex_output[i][j] << endl ;
						one.line = linectr ;
						one.code = lex_output[i][j] ;
						one.operators = ptopar( lex_result[i][j] ) ;
						inter.push_back(one) ;
						linectr++ ;
						
						if ( type == "BOOLEAN" )
								identifier[lex_result[i][j].second].type = 2 ;
							
						if ( type == "INTEGER" )
								identifier[lex_result[i][j].second].type = 4 ;
							
						if ( type ==  "REAL" )
								identifier[lex_result[i][j].second].type = 6 ;
					
						identifier[lex_result[i][j].second].pointer = info.size() ;
						
						string name = lex_output[i][j] ;
						j++ ;
						vector<int> d ; 
						 
						for ( ; lex_output[i][j] != ")" ; j++) {
							if ( is_int( lex_output[i][j] ) ) 
								d.push_back(  atoi( lex_output[i][j].c_str() )  ) ;
						}
						dimension.push_back( make_pair( name, d ) ) ;
						
						info.push_back( 4 ) ;
						info.push_back( d.size() ) ;
				
						for ( int k = 0 ; k < d.size(); k++)
							info.push_back( d[k] ) ;
						
					}
				}
			}
			else if ( cur == "VARIABLE" ) {
				int j = 1 ;
	
				
				string type = lex_output[i][j] ;
				
				for ( ; j < len ; j++) {
					if ( lex_result[i][j].first == 5  ) {
						cout << lex_output[i][j] << endl ;
						one.line = linectr ;
						one.code = lex_output[i][j] ;
						one.operators = ptopar( lex_result[i][j] ) ;
						inter.push_back(one) ;
						linectr++ ;
				
						if ( type == "BOOLEAN" )
							identifier[lex_result[i][j].second].type = 2 ;
							
						if ( type == "INTEGER" )
								identifier[lex_result[i][j].second].type = 4 ;
							
						if ( type ==  "REAL" )
								identifier[lex_result[i][j].second].type = 6 ;
						
						
					}
				}
			}	
			else if ( cur == "ENP" or cur == "ENS" ) {
				
				one.line = linectr ;
				if ( label ) {
					one.code = lex_output[i][0] ;
				}
				if ( cur == "ENP") {
					one.code += " ENP" ;
					one.operators = ptopar( 2,6) ;
				}
				else {
					one.code += " ENS" ;
					one.operators = ptopar( 2,7) ;
				}
				
				inter.push_back( one ) ;
				
				linectr++ ;
				continue ;
			}
			else if ( cur == "GTO" ) {
				one.line = linectr ;
				
				one.operators = ptopar( 2,11) ;
				
				one.result = ptopar( 6, identifier[lex_result[i][1].second].pointer) ;
				
				one.code = "GTO " + lex_output[i][1] ;
				
				inter.push_back( one ) ;
				linectr++ ;
				
			}
			
			int equal = -1 ;
			
			for ( int j = 0 ;  j < len ; j++) {
				if ( lex_output[i][j] == "=") {
					equal = j ;
					break ;
				}
			}
			
			if ( lex_output[i][0] == "IF" or lex_output[i][1] == "IF" ) { // IF X GT Y AND Q THEN X=X+1 ELSE X=X+2
				
				int j = 1 ;
				
				if ( lex_output[i][1] == "IF")
					j = 2 ;
					
				vector<string> list ;
				
				while ( lex_output[i][j] != "THEN" ) {
				
					list.push_back( lex_output[i][j] ) ;
					j++ ;
				}
				
				
				
				vector<string> rpnl ;
				rpnl.push_back( list[0] ) ;
				stack<string> optr ;
				
				
				// reverse poliosh notation
				for (  int j = 1 ; j < list.size(); j++ ) {
					
					if ( isreserved(list[j]) ) { // > < = or and 
						if ( optr.size() == 0 )
							optr.push( list[j] ) ;
						else {
							if ( logicvalue[list[j]] <= logicvalue[optr.top()] ) {
								rpnl.push_back( optr.top() ) ;
								optr.pop() ;
								optr.push( list[j] ) ;
							}
							else {
								optr.push( list[j] ) ;
							}
						}
					}
					else {  // identifier 
						rpnl.push_back( list[j] ) ;
					}
				}
				
				
				while ( optr.size() != 0 ) {
					rpnl.push_back( optr.top() ) ;
					optr.pop() ;
				}
				
				
				// reverse poliosh notation
			
				//  condition assingment 

					
				int idx = 0 ;
				while ( rpnl.size() != 1 ) {
				
					
					for ( int k = 0 ; k < rpnl.size(); k++)
						cout << rpnl[k] << ' ' ;
					cout << endl;
				
					while ( ! isreserved( rpnl[idx] ) )
						idx++ ;
					int nextidx = idx - 2 ;
					one = line() ;
					cout << rpnl[idx] << endl ;
					one.line = linectr ;
					one.operators = ptopar( 2, reserved[rpnl[idx]] ) ;
					one.operand1  = ptopar( locate( type( rpnl[nextidx]), rpnl[nextidx] )) ;		
					one.operand2  = ptopar( locate( type( rpnl[nextidx+1]), rpnl[nextidx+1] )) ;
					one.result    = ptopar( gen_temp() ) ;
					one.code = temptable.back() + "=" + rpnl[nextidx] + " " + rpnl[idx] + " " + rpnl[nextidx+1] ;
					rpnl[nextidx] = temptable[temptable.size()-1] ;
					for ( int k = nextidx+3 ; k < rpnl.size(); k++) {
						rpnl[k-2] = rpnl[k] ;
					}
		
					inter.push_back( one ) ;
					rpnl.pop_back() ;
					rpnl.pop_back() ;
					idx = nextidx ;
					linectr++ ; 
				}
			
				
				//  condition assingment   If T1 then go t2 else go t3 先create 之後再給operand 直 
				one = line() ;
				one.line = linectr ;
				one.operators = ptopar( 2,12 ) ;
				one.operand1 = ptopar( 0, temptable.size() ) ;
				one.operand2 = ptopar( 6, inter.size()+2 ) ;
				int gtoidx = inter.size()  ;
				one.code = "IF " + temptable.back() + " GO TO " +  to_string( gtoidx+2 ) + ", ELSE GO TO";
				
				
				inter.push_back( one ) ;
				linectr++ ;
				//
				
				// calculate s1 s2 
				vector<string> s1, s2 ;
				
				j++;
				
				while ( lex_output[i][j] != "ELSE" ) {
					s1.push_back( lex_output[i][j] ) ;
					j++ ;
				}
				
		
				int s1gtoidx = -1 ;
				if ( s1[0] != "GTO") {
				
					vector<string> rpn1 = rpnF( s1 ) ;
					assignment( rpn1, s1, linectr ) ;
					
					one = line() ;
					one.line = linectr ;
					one.operators = ptopar( 2,11 ) ; // gto
					one.code = "GTO " ;
					s1gtoidx = inter.size()  ;
					
					inter.push_back( one ) ;
					
 					linectr++ ;
				}
				else { // GTO
					one = line() ;
					one.line = linectr ;
					one.operators = ptopar(2,11) ;
					one.code = "GTO " ;
					if ( identifier[locate(5,s1[1]).second].pointer == 0 )
						ifforward.push_back( gen_ref(s1[1], inter.size() )  ) ;
					else 
						one.result = ptopar( 6, identifier[locate(5,s1[1]).second].pointer ) ;
					inter.push_back( one ) ;
					linectr++ ;
				}
				
				inter[gtoidx].result = ptopar( 6,inter.size()+1 ) ;
				inter[gtoidx].code += to_string( inter.size()+1 ) ;
				j++ ;
				while ( lex_output[i][j] != ";" ) {
					s2.push_back( lex_output[i][j] ) ;
					j++ ;
				}
				
				if ( s2[0] != "GTO" ) {
					vector<string> rpn2 = rpnF( s2 ) ;
					assignment( rpn2, s2, linectr ) ;
				} 
				else { // GTO
					one = line() ;
					one.line = linectr ;
					one.operators = ptopar(2,11) ;
					one.code = "GTO " ;
					if ( identifier[locate(5,s2[1]).second].pointer == 0 )
						ifforward.push_back( gen_ref(s2[1], inter.size() )  ) ;
					else 
						one.result = ptopar( 6, identifier[locate(5,s2[1]).second].pointer ) ;
					inter.push_back( one ) ;
					linectr++ ;
				}
				
				cout << s1gtoidx << endl ;
				if ( s1gtoidx != -1 ) {
					inter[s1gtoidx].result = ptopar( 6,inter.size()+1 ) ;
					inter[s1gtoidx].code += ptopar( 6,inter.size()+1 ) ;
				}
				
				equal = -1 ;
			}
			
			if ( equal != -1 ) {
				vector<string> list ;
				int j = 0 ;
				if ( lex_result[i][0].first == 5 ) {
					if ( identifier[lex_result[i][0].second].type == 5 ){
						identifier[lex_result[i][0].second].pointer = inter.size() +1  ;
						j++ ;
					}
					
				}
				for (  ;  j < len ; j++) {
			
					string str ;
					if ( lex_result[i][j].first == 5 and j+1 != len ) {
						str += lex_output[i][j] ;
						if ( lex_output[i][j+1] == "(" ) {
							
							j++ ;
							str += lex_output[i][j] ;
							for ( j = j+1 ; lex_output[i][j] != ")" ; j++) {
								str += lex_output[i][j] ;
							}
							str += lex_output[i][j] ;
						}
					}
					else str = lex_output[i][j] ;
					
	
					list.push_back( str ) ;
				}
				
	
				
				// reverse polish notation 
		
	
		
				vector<string> rpn = rpnF( list ) ;
				
	
				for ( int j = 0 ; j < rpn.size(); j++) {
					cout << rpn[j] <<' ' ;
				}
				cout << endl ;
						
				
				assignment( rpn, list,linectr ) ;
	
				


			 
	
			}
		}
	}
	
	void array_cal( string array ,int & linectr ) {
		int idx = 0  ;
	
		string name = array.substr(0, array.find("(")) ;
		while ( name != dimension[idx].first )  
			idx++ ;
			
		int size = dimension[idx].second.size() ;
		
		line one ;
		
		cout << dimension[idx].first << endl ;
		if ( size == 1 ) {  // T1 = A(K) 8	((1,4)	,(5,65)	,(5,75)	,(0,1)	)	T1 = A(K)
		
			one.line = linectr ;  
			one.operators = ptopar( 1,4 ) ;
			one.operand1 = ptopar( locate( 5, array.substr(0, array.find("(") ) ) ) ;
			one.operand2 = ptopar( locate( 5, array.substr( array.find("(")+1,  array.find(")")-array.find("(")-1 ) ) ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + array ;
			
			inter.push_back( one ) ;
			
			linectr ++ ;
		}
		else if ( size == 2) {
			int idx = array.find("(") ;
			string f = array.substr(0, idx ) ;
			string s = array.substr( idx+1, array.find(",")-idx-1 ) ;
			string t = array.substr( array.find(",")+1, array.find(")") - array.find(",") -1 ) ;
			
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( locate(5,t) ) ;
			one.operand2 = ptopar( 3,49 ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + t + "-1" ;
			linectr++ ;
			inter.push_back( one ) ;
			
			
			one.line = linectr ;
			one.operators = ptopar( 1,7 ) ;
			one.operand1 = ptopar( 0, temptable.size() ) ;
			one.operand2 = ptopar( 3, hashing( to_string( dimension[idx].second[0] ) ) ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " +  temptable[temptable.size()-2] + "*" + to_string( dimension[idx].second[0] ) ;
			linectr++ ;
			inter.push_back( one ) ;
		
			
			
			one.line = linectr ;
			one.operators = ptopar( 1,5 ) ;
			one.operand1 = ptopar( locate( 5,s ) ) ;
			one.operand2 = ptopar( 0, temptable.size() ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " +  s  + "+" + temptable[temptable.size()-2] ;
			linectr++ ;
			inter.push_back( one ) ;
			
			
			one.line = linectr ;
			one.operators = ptopar( 1,4 ) ;
			one.operand1 = ptopar( locate( 5,f ) ) ;
			one.operand2 = ptopar( 0, temptable.size() ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " +  f  + "(" + temptable[temptable.size()-2] + ")" ;
			linectr++ ;
			inter.push_back( one ) ;
				
		}
		
	
		else if ( size == 3 ) {
			int idx = array.find("(") ;  
			string f = array.substr(0, idx ) ;  
			string s = array.substr( idx+1, array.find(",")-idx-1 ) ;
			string t = array.substr( array.find(",")+1, array.find_last_of(",") - array.find(",") -1 ) ;		
			string four = array.substr( array.find_last_of(",")+1 , array.find(")")-array.find_last_of(",") -1 ) ;
			
			line one ;
			
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( locate(5,s) ) ;
			one.operand2 = ptopar( 3,49 ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + s + "-1" ;
			linectr++ ;
			inter.push_back( one ) ;
			
			one.line = linectr ;  
			one.operators = ptopar( 1,7 ) ;
			one.operand1 = ptopar( 0, temptable.size() ) ;
			one.operand2 = ptopar( 3, hashing( to_string( dimension[idx].second[1]) ) ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + temptable[temptable.size()-2] + "*" + to_string( dimension[idx].second[1] ) ;
			linectr++ ;
			inter.push_back( one ) ;
			
			
			one.line = linectr ;  
			one.operators = ptopar( 1,7 ) ;
			one.operand1 = ptopar( 0, temptable.size() ) ;
			one.operand2 = ptopar( 3, hashing( to_string( dimension[idx].second[0]) ) ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + temptable[temptable.size()-2] + "*" + to_string( dimension[idx].second[0] ) ;
			linectr++ ;
			inter.push_back( one ) ;
			
			int ft = temptable.size() ;
			
			
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( locate(5,t) ) ;
			one.operand2 = ptopar( 3,49 ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + t + "-1" ;
			linectr++ ;
			inter.push_back( one ) ;
	
			
			
			one.line = linectr ;  
			one.operators = ptopar( 1,7 ) ;
			one.operand1 = ptopar( 0, temptable.size() ) ;
			one.operand2 = ptopar( 3, hashing( to_string( dimension[idx].second[0]) ) ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + temptable[temptable.size()-2] + "*" + to_string( dimension[idx].second[0] ) ;
			linectr++ ;
			inter.push_back( one ) ;
			
			int st = temptable.size() ;
			
			
					
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( locate(5,four) ) ;
			one.operand2 = ptopar( 3,49 ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + four + "-1" ;
			linectr++ ;
			inter.push_back( one ) ;
			
			int tt = temptable.size() ;
			
			
							
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( 0, ft ) ;
			one.operand2 = ptopar( 0, st ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + temptable[ft-1] + "+" +  temptable[st-1] ;
			linectr++ ;
			inter.push_back( one ) ;
			
			
									
			one.line = linectr ;  
			one.operators = ptopar( 1,6 ) ;
			one.operand1 = ptopar( 0, temptable.size() ) ;
			one.operand2 = ptopar( 0, tt ) ;
			one.result = ptopar( gen_temp() ) ;
			one.code = temptable[temptable.size()-1] + " = " + temptable[temptable.size()-2] + "+" +  temptable[tt-1] ;
			linectr++ ;
			inter.push_back( one ) ;
		}//else if
		
	
	
	}
	
	int hashing( string str ) {
		int ans = 0 ;
		
		for ( int i = 0 ; i < str.size(); i++ )
			ans += str[i] ;
			
		return ans%100 ;
	}
	
	pair<int,int> gen_temp() {
		pair<int,int> p ;
		temptable.push_back( "T"+ to_string(temptable.size()+1) ) ;
		
		return make_pair( 0, temptable.size() ) ;
	}
	
	int type ( string str ) {
		
		if ( str.size() >= 2 ) {
			if ( str[0] == 'T' and isdigit(str[1]) ) {
				return 0 ;
			}
			else if ( isdigit(str[0]) ) {
				if ( str.find(".") != -1 )
					return 4 ;
				else 
					return 3 ;
			}
			else return 5 ;
		}
	
		if (  isdigit(str[0])  ) {
			if ( str.find(".") != -1 )
				return 4 ;
			else 
				return 3 ;
		}
		else return 5 ;
	
		
	}

	void write_file() {
		string nfile = file+"_compiler.txt" ;
		ofstream out ( ( nfile ).c_str() ) ;
		
	
	
		for ( int i = 0 ; i < inter.size(); i++ ) {
			
			if ( inter[i].errorMsg.size() == 0 ) {
				out << setw(8) << i+1 << "(" << setw(8) << inter[i].operators+"," << setw(8) << inter[i].operand1+"," ;
				out << setw(8) << inter[i].operand2+"," << setw(8) << inter[i].result+")"<<  '\t' << inter[i].code << '\n' ;
					
			}
			else {
				out << "line " +  to_string( inter[i].line ) << " : " << inter[i].errorMsg << '\n' ;
			}
	
			
			
		}
		
		out.close() ;
		
		cout << file << " has been created." << endl ;
	}

	string to_string( int num ) {
		stringstream ss ;
		ss << num ;
		return ss.str() ;
	}
	
	
	idt newidt( string name , int sub =0, int type=0,int ptr = 0 ) {
		idt newi = idt() ;
		newi.name = name ;
		newi.subroutine = sub ;
		newi.type = type ;
		newi.pointer = ptr ;
		
		return newi ;
	}
	
	int hashing( int type,string word)  {
	
		int idx = 0 ;
		for ( int i = 0 ; i < word.size(); i++)
			idx+= word[i] ;
		idx%=100 ;
		
		
		if ( type == 3 ) {
			
			if ( integer[idx] != word ) {
			
				while ( integer[idx] != "") {
					idx ++ ;
					if ( idx == 100 ) idx = 0 ; 
				}
				
				integer[idx] = word ;
			}
			
			
			
		}
		else if ( type == 4 ) {
			if ( real[idx] != word ) {
				while ( real[idx] != "") {
					idx ++ ;
					if ( idx == 100 ) idx = 0 ; 
				}
				
				real[idx] = word ;
			}
		}
		else { // type == 5 
			cout << identifier[idx].name << ' ' << word << ' ' << identifier[idx].subroutine << ' ' << pc << endl ;
			if ( call ) {
	
				if ( identifier[idx].name != word ) {
					while ( identifier[idx].name != "") {
						idx ++ ;
						if ( idx == 100 ) idx = 0 ; 
					}
					
					identifier[idx] = newidt( word, pc ) ;
				}
				
				call = false ;
				
		
			}
			else {
				
				
				if ( identifier[idx].name != word ) {
					while ( identifier[idx].name != "") {
						idx ++ ;
						if ( idx == 100 ) idx = 0 ; 
					}
					
					identifier[idx] = newidt( word, pc ) ;
				}
				else {
					if ( identifier[idx].subroutine != pc ) {
						
						while ( identifier[idx].name != "" ) {
							idx ++ ;
						
							if ( idx == 100 ) idx = 0 ; 
							if ( identifier[idx].subroutine == pc and identifier[idx].name == word  )
								break ;
						}
						
						identifier[idx] = newidt( word, pc ) ;
					}
				}
				
			}
		}
			

		
		return idx ;
	}
	
	pair<int,int> locate( string & buf ) {
		
		pair<int,int> ans ;
		
		if ( buf == "PROGRAM" or buf == "SUBROUTINE") {
			setpc = true ;
		}
		
		
		if ( buf == "CALL" ) {
			call = true ;
		}

		// check is delimiter   t1
		
		if ( isdelimiter(buf) ) 
			ans = make_pair( 1, delimiters[buf]) ;		
		// check is reserved word   t2 		
		else if ( isreserved( to_upper(buf) ) )
			ans = make_pair( 2, reserved[buf]) ;			
		// check is integer t3 		
		else if ( is_int(buf) ) {
			ans = make_pair( 3, hashing(3,buf) ) ;
		}			
		// check is real t4 
		else if ( isdigit(buf[0])  and buf.find(".") != -1 )
			ans = make_pair( 4, hashing(4,buf) ) ;
	
		else 
			ans = make_pair( 5, hashing(5,buf) ) ;
		
		if ( setpc and ans.first == 5 ) {
			pc = ans.second ;
			setpc = false ;
		}
		
		return ans ; 
	}

	void get_token ( string  word, vector< pair<int,int> > & out, vector<string> & sL, vector<string> & cL ) {
		string buf = "" ;
		
		for ( int i = 0 ; i < word.size(); i++) {
			if ( word[i] == ' ' or word[i] == '\t' or word[i] == '\n') {  // white space   1 or 0 token 
				if ( buf.size() != 0 ) {
					sL.push_back( buf ) ;
					pair<int,int> p = locate( buf ) ;
					out.push_back( make_pair(p.first,p.second) )  ;
					cL.push_back( buf ) ;
					buf = "" ;
				}
			
			}
			else {   // not a white space 
				if ( isdelimiter( string(1,word[i] ) )  ) { // 
				
					if ( buf.size() != 0 ) {
						sL.push_back( buf ) ;
						pair<int,int> p = locate( buf ) ;
						out.push_back( make_pair(p.first,p.second) )  ;
						cL.push_back( buf ) ;
						buf = "" ;
					}
						
				
					if ( word[i] == ';' ) {
						out.push_back( make_pair(1,1) )  ;
						sL.push_back( word.substr(i) ) ;
						cL.push_back( word.substr(i) ) ;
						return ;
					}
					else {
						buf += word[i] ;

						out.push_back( make_pair(1,delimiters[buf] ) )  ;
						sL.push_back( buf ) ;
						cL.push_back( buf ) ;
						buf = "" ;
						
					} 
				}
				else {
					buf += word[i] ;
				}
			}
		}
		
		
				
		if ( buf.size() != 0 ) {
			sL.push_back( buf ) ;
			pair<int,int> p = locate( buf ) ;
			out.push_back( make_pair(p.first,p.second) )  ;
			cL.push_back( buf ) ;
		}
	}

	void lexical_analysis( ifstream & input_file) {

		
		string line ;

		while ( getline (input_file,line) ){
			

			if ( line.size() != 0 ) {
	 			vector< pair<int,int> > p ;
				vector< string > str ;
				vector< string > constr ;
				
				get_token( line, p , str, constr ) ;

				
				lex_result.push_back( p ) ;
				lex_input.push_back( str ) ;
				lex_output.push_back( constr ) ;
			}
	    }
	    
	}
	



	
	bool isdelimiter( string str) {
		
		if(delimiters.find(str)==delimiters.end())
			return false ;
		return true ;
	
	}
	
	
	bool isreserved ( string str) {
			
		if(reserved.find(str)==reserved.end())
			return false ;
		return true ;
	}


	vector<string> rpnF( vector<string> list ) {
		vector<string> rpn ;
		rpn.push_back(list[0]) ;
	
		stack<string> optr ;
				
				
		for ( int j = 2 ; j < list.size() ; j++) {
		
	
			if ( list[j] == ")" ) { //  is )
				

				
				while ( optr.top() != "(" ) {  
					rpn.push_back( optr.top() ) ;
					optr.pop() ;
				}
				optr.pop() ;

			}
			else if ( isdelimiter( list[j] ) ) {  // is + - * / ^  ( 
				if ( list[j] != "(") {
					if ( optr.empty() ) {
						optr.push( list[j]) ;
					}
					else {
						if ( optvalue[list[j]] >= optvalue[ optr.top() ] ) {
							
							optr.push( list[j]) ;
						}
						else { // 
							while ( optvalue[list[j]] < optvalue[ optr.top() ] ) {
								rpn.push_back( optr.top() ) ;
								optr.pop() ;
							}
							optr.push( list[j] ) ;
						}
					}
				}
				else {
					optr.push( "(" ) ;
				}

			}
			else { // is a operand 
				rpn.push_back(  list[j]  ) ;
			}
		}
		


		while ( !optr.empty() ) {

			rpn.push_back( optr.top()) ;
			optr.pop() ;
		}
		
		rpn.push_back("=") ;
		
		
		return rpn ;
	}
	
	void assignment( vector<string> & rpn, vector<string> list , int & linectr ) {
		line one ;
		if ( rpn.size() > 3 ) {
				
			int idx = 0 ;
			while ( rpn.size() != 1 ) {
			
				
				for ( int k = 0 ; k < rpn.size(); k++)
					cout << rpn[k] << ' ' ;
				cout << endl;
			
				while ( ! isdelimiter( rpn[idx] ) )
					idx++ ;
				int nextidx = idx - 2 ;
				one = line() ;
				cout << rpn[idx] << endl ;
				one.line = linectr ;
				one.operators = ptopar( 1, delimiters[rpn[idx]] ) ;
					
				if ( rpn[idx] != "=" ) {
					
				
					if ( rpn[nextidx].find( "(") != -1) {
						array_cal( rpn[nextidx], linectr ) ;
						rpn[nextidx] = temptable[temptable.size()-1] ;
					}
					
					one.operand1  = ptopar( locate( type( rpn[nextidx]), rpn[nextidx] )) ;
					
					
					if ( rpn[nextidx+1].find( "(") != -1) {
						array_cal( rpn[nextidx+1], linectr ) ;
						rpn[nextidx+1] = temptable[temptable.size()-1] ;
					}
					one.operand2  = ptopar( locate( type( rpn[nextidx+1]), rpn[nextidx+1] )) ;
					one.result    = ptopar( gen_temp() ) ;
					
					if ( one.operators == "=" ) {
						one.code = temptable.back() + " = " + rpn[nextidx] ;
					}
					else 
						one.code = temptable.back() + " = " + rpn[nextidx] + rpn[nextidx+2] + rpn[nextidx+1] ;
				}
				else { // unknown solution temp solu
				
					if ( rpn[nextidx].find("(") == -1  ) {// B = a 
						one.result = ptopar( locate(5,rpn[nextidx]) )  ;
						if ( rpn[nextidx+1].find( "(") != -1) {
							array_cal( rpn[nextidx+1], linectr ) ;
							rpn[nextidx+1] = temptable[temptable.size()-1] ;
						}
						
						one.operand1  = ptopar( locate( type( rpn[nextidx+1]), rpn[nextidx+1] )) ;
						
						
					}
					else {   // A(K) = a
													
						if ( rpn[nextidx+1].find( "(") != -1) {
							array_cal( rpn[nextidx+1], linectr ) ;
							rpn[nextidx+1] = temptable[temptable.size()-1] ;
						}
						
						one.operand1  = ptopar( locate( type( rpn[nextidx+1]), rpn[nextidx+1] )) ;
						
						string f = rpn[nextidx].substr(0, rpn[nextidx].find("(")) ;
						string s = rpn[nextidx].substr( rpn[nextidx].find("(")+1, rpn[nextidx].find(")") -rpn[nextidx].find("(") -1 ) ;
						
						cout << f << s << endl;
						one.result =   ptopar( locate( 5, s ) ) ;
						one.operand2 = ptopar( locate( 5, f ) ) ;
						
						
					}
					
					one.code = rpn[nextidx] + " = " + rpn[nextidx+1] ;
						
	
				}
				
				rpn[nextidx] = temptable[temptable.size()-1] ;
				for ( int k = nextidx+3 ; k < rpn.size(); k++) {
					rpn[k-2] = rpn[k] ;
				}
	
					
				
				inter.push_back( one ) ;
				rpn.pop_back() ;
				rpn.pop_back() ;
				cout << rpn.size() << endl ;
				idx = nextidx ;
				linectr++ ; 
			}
			
	
		}
		else {
			if ( rpn.size() < 3 ) {
				one.line = linectr ;
				one.errorMsg = "assign error';'\nSyntax Error!!!" ;
				inter.push_back(one) ;
				
				linectr++ ;
				return ;
				
			}//if
			else {
				one.line = linectr ;
				if ( rpn[0].find( "(") != -1 ) {  // 	A(K)=I;
		
					one.operators = ptopar( 1,4 ) ;
					one.operand1 =  ptopar( locate( 5,list[2]) ) ;
					one.operand2 =  ptopar( locate( 5,rpn[0].substr(0,rpn[0].find("(")))) ; // A(K) -> A 
					one.result =    ptopar( locate( 5,rpn[0].substr( rpn[0].find("(")+1, rpn[0].find(")")-rpn[0].find("(")-1 ))) ;
					
					one.code = list[0] + " " + list[1] + " " + list[2] ;
			
				} //if
				else {   //  A = 2 or A = (K) 
					if ( list[2].find( "(") == -1 ) {  // a = 2 , a= b 
						one.operators = ptopar( 1,4 ) ;
						one.operand1 =  ptopar( locate( type(list[2]),list[2]) ) ;
						one.result =    ptopar( locate( 5, list[0])) ;
						one.code = list[0] + " " + list[1] + " " + list[2] ;
					}//if
					else {  // B = A(k)  ->  8	((1,4)	,(5,65)	,(5,75)	,(0,1)	)	T1 = A(K)
						array_cal( list[2], linectr ) ;
						one.operators = ptopar( 1,4 ) ;
						one.operand1 =  ptopar( make_pair( 0, temptable.size() ) ) ;
						one.result =    ptopar( locate( 5, list[0])) ;
						one.code = list[0] + " " + list[1] + " " + temptable[temptable.size()-1] ;
					}//else
				}//else
				inter.push_back(one) ;
				linectr++ ;
			}//else
		}//else
	} // void assignment( vector<string> & rpn, vector<string> list , int & linectr )
	
	fref gen_ref( string name, int loc ) {
		fref newf = fref() ;
		newf.label = name ;
		newf.t6_loc = loc ;
		return newf ;
	} // fref gen_ref( string name, int loc )
};

int main  ()  {
	compiler f ;
	f.lexical_ana() ;
	f.syntax() ;
	f.write_file() ;
	return 0;
} // main()





