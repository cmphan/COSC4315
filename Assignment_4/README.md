# COSC 4315 - ASSIGNMENT 4 - Evaluating List Expressions
Author: Cuong Phan

Create a program to evaluate simple list expressions. The input source code will be: Python. This “interpreter” program will be developed in C++.
The program will use regular expressionm, flex and bison to recognize identifiers, numbers and strings. 

## INPUT

The input is one python source code file. The programs will contain the following statements: variable assignment with arithmetic expressions, list [] operators, functional if form (used in lambda expressions), function calls?,
List expressions can combine constants, simple variables (int, string) and list elements.
optional: Traditional if/else statements to control flow (The if condition will be one comparison (no and/or). Parentheses are optional, but unnecesary since the result will be the same. Function definitions with up to 3 arguments. Nested lists or lists mixing data types.
The main arithmetic operator will be the ’+’ operator. The list elements can be numbers or strings.
In the case of numbers + means addition, and for lists it means union. Notice ’*’ is not available for strings or lists.

## Deployment

Compile format:
flex checkdatatype.l && g++ -std=c++11 lex.yy.c -o checkdatatype

Usage format:
./checkdatatype file=tc1.py

Output File:
File "*.out" with the same name as the input file
