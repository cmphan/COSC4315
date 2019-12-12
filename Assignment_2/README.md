# COSC4315 - Assignment 2- CHECKING DATA TYPES

Author: Cuong Phan

This ”compiler” program to detect data type conflicts in source code. 
The input source code will be: Python. This “interpreter” program will be developed in C++. 
The output will be a 2nd program inserting comments with warning/error/undefined messages.

## INPUT 

The input is one source code file. The programs will contain the following statements: assigment with arithmetic expressions, function calls, if/else statements, function definitions with up to 3 arguments. 
The main arithmetic operator will be the ’+’ and ’*’ operator. The variables can be numbers, strings or lists. In the case of numbers + means addition, for strings it means concatenation and for lists union.

### OUTPUT

Input will be given a syntactically correct python program as your input file (program1.py).
The output should be in input filename.out (ex: program1.out) which will print “#warning” or “#error” before the line. 
For correct statements, nothing is printed. 

```
Give examples
```

### Deployment

Unix Command to compile my program (need to specify c++ 11)

*******************

g++ -std=c++11 checkdatatype.cpp -o checkdatatype

********************

Unix Command to run my program for "tc1.py" input file:

*******************

./checkdatatype file=tc1.py

********************

#####################
Description : This program was written in c++ to detect data types warnings/ errors in input file (Python source code)

My approach:

1. Parse command line to get file name=> read input file in a string=>put all statement into a vector<string>pythonStatments

2. Create 4 map: Use map here to ensure all identifiers and function names are unique and the access time is constant time O(1)

	map<string,string> VarAndTypes (store all variables and function names)
  
	map<string,string> VarAndTypes_Function(store local variables, erase content when detecting return statement of function defition)
  
	map<string,int>FunctionNamesAndSize (store function declared names and its parameters size to make sure there is not errors in function call parameters later)
  
	map<int,string>LineAndMessages (store line detect errors/warnings and messages: #error/ #warning)
  
3. If detects a function declaration statement, if else conditional statement => send to function Handle those cases

4. If detects a statement like (import, #comment, blank line) => skip to the next statement

5. If detects "=" slipt the left and right hand-side of "=" side and continue to slipt if detect any "*" or "+" operands

6. In case of "*" operands if any of them involved are "STRING" or "LIST" type => errors

6. In case of "+" make sure all of them are of the same type

6. Function Type are classified based on value at return statement. 

7. If detects a function call => check if the parameters passing in are of the same type as Function Type in map => errors

8. Insert back to vector<string>pythonStatements (if any #errors/#warning) with position of line and messages based on values inside map LineAndMessages

9. Output the vector<string>pythonStatements to output .out file
