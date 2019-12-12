# COSC 4315 - ASSIGNMENT 4 -Interpreter Program- Evaluating List Expressions using Flex and Bison 
Author: Cuong Phan

Create a program to evaluate simple list expressions. The input source code will be: Python. This “interpreter” program will be developed in C++.
The program will use regular expressionm, flex and bison to recognize identifiers, numbers and strings. 

## INPUT

The input is one python source code file. The programs will contain the following statements: variable assignment with arithmetic expressions, list [] operators, functional if form (used in lambda expressions)
List expressions can combine constants, simple variables (int, string) and list elements.Traditional if/else statements to control flow (The if condition will be one comparison (no and/or). Nested lists or lists mixing data types.
The main arithmetic operator will be the ’+’ operator. The list elements can be numbers or strings.
In the case of numbers + means addition, and for lists it means union.
## OUTPUT
The "interpreter" program written in C++ will dectect any errors or warnings in the python input file and will perform all list expressions operation and output the content of the lists 

##EXAMPLE
###Input: file "tc.py":

l1 = [1,2,3]

l2 = [4,5,6]

l3 = l1 + l2

print(l3)

###Output: file "checkdata_type.l" generates a "tc.out" file:

l1 = [1,2,3]

l2 = [4,5,6]

l3 = l1 + l2

print(l3)

[1,2,3,4,5,6]

## Deployment

Compile format:
flex checkdata_type.l && g++ -std=c++11 lex.yy.c -o checkdatatype

Usage format:
./checkdatatype file=tc1.py

Output File:
File "*.out" with the same name as the input file
