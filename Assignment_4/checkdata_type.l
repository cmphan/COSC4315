%{
  // usage format: clear && flex checkdatatype.l && g++ -std=c++11 lex.yy.c -o checkdatatype
  #include <vector>
  #include <iostream>
  #include <string>
  #include <map>
  #include <fstream>
  #include<iterator>
  using namespace std;

  /* Debug Levels
   * 0 = Critical only
   * 1 = Informational
   * 2 = Verbose
   * 3 = input parsing
   */
  #define DEBUG 5
  // values to variable offset
  #define VAL2VAR 4
  #define MAINPROGRAM -1

  // testing only
  //extern int yylex();

  //
  // Structures
  //
  // Debug levels
  enum dbg { CRITICAL, INFO, VERBOSE, ALL };
  enum lex { NO_VAL = 0, ERROR, WARNING, 
            SPACE, LF, INDENT, FILE_EOF, COMMENT, 
            FUNC_DEC, FUNC_DEC_END, FUNC_CALL,
            AND, OR, NOT,
            COND_IF, COND_ELSE, LOOP_FOR, LOOP_WHILE, RETURN, NO_RETURN, COLON,
            C_EQUALS, C_GREATER, C_LESSER, C_GREATER_EQUALS, C_LESSER_EQUALS, C_NOT_EQUALS,
            COMMA, ASSIGN, PAREN_OP, PAREN_CL, TRAILING_PAREN,
            OP_ADD, MATH_SUB, MATH_DIV, MATH_MUL, MATH_EXP, MATH_MOD,
            VAL_BOOL, VAL_STRING, VAL_INT, VAL_FLOAT,
            LABEL, OTHER, PRINT,
            LIST_INDEX, LIST_SLICE_ONE_ARG, LIST_SLICE_TWO_ARG, LIST_DEC, LIST_DEC_END, LIST_APPEND, LIST_APPEND_END, LIST_INSERT, LIST_INSERT_END, LIST_RANGE };
  static const char* lex2str[] = { "NO_VAL", "ERROR", "WARNING",
      "SPACE", "LF", "INDENT", "FILE_EOF", "COMMENT",
      "FUNC_DEC", "FUNC_DEC_END", "FUNC_CALL",
      "AND", "OR", "NOT",
      "COND_IF", "COND_ELSE", "LOOP_FOR", "LOOP_WHILE", "RETURN", "NO_RETURN", "COLON",
      "C_EQUALS", "C_GREATER", "C_LESSER", "C_GREATER_EQUALS", "C_LESSER_EQUALS", "C_NOT_EQUALS",
      "COMMA", "ASSIGN", "PAREN_OP", "PAREN_CL", "TRAILING_PAREN",
      "OP_ADD", "MATH_SUB", "MATH_DIV", "MATH_MUL", "MATH_EXP", "MATH_MOD",
      "BOOL", "STRING", "INT", "FLOAT",
      "LABEL", "OTHER", "PRINT",
      "LIST_INDEX", "LIST_SLICE_ONE_ARG", "LIST_SLICE_TWO_ARG", "LIST_DEC", "LIST_DEC_END", "LIST_APPEND", "LIST_APPEND_END", "LIST_INSERT", "LIST_INSERT_END", "LIST_RANGE" };

  // Tokens
  typedef struct {
    int type; // variable type (list will only take the first instance type)
    string text; // variable name
    int line;    // line number
  } token;

  // latest variable values
  typedef struct {
    int type; // type of the variable
    string text;  // value of the variable
  } simple_var;

  // variable construct, can be simple or list
  typedef struct
  {
    bool is_list;
    simple_var simple;  // simple variable
    vector<simple_var> list; // list variable
  } variable;

  // within a list flag
  bool in_list = false;
  // last label, used for input
  int input_last_label = 0;
  // input token list using FLEX
  vector<token> global_token_list;
  // function declaration flag
  bool func_decl = false;
  // list appending flag
  bool list_append = false;
  // list insert flag
  bool list_insert = false;
  // boolean to print a line or not
  vector<string> out_string;
  // integer for output line
  vector<string> err_string;
  vector<int> err_lines;

  // output file
  string out_file;
  ofstream out;

  // line number
  int py_line_num = 1;

  // handles token inputs
  void inputHandler(int type, string text)
  {
    token t;
    t.type = type;
    t.text = text;
    t.line = py_line_num;
    if(t.type == LF)
    {
      py_line_num++;
    }

    // if function definiton, remove "def "
    if(t.type == FUNC_DEC)
    {
      t.text = text.substr(4);
      func_decl = true;
    }
    // found a list append
    else if(t.type == LIST_APPEND)
    {
      list_append = true;
    }
    else if(t.type == LIST_INSERT)
    {
      list_insert = true;
    }

    // if function def occured before and we see "):"
    if(func_decl && t.type == FUNC_DEC_END)
    {
      func_decl = false;
    }
    // we probably encountered the end of a if/for/while statement
    else if(!func_decl && t.type == FUNC_DEC_END) 
    {
      t.type = PAREN_CL;
      t.text = ")";
      if(DEBUG >= 3) 
        cout << "Line " << t.line << ": (" << lex2str[t.type] << "): \"" << t.text << "\"\n";
      global_token_list.push_back(t);
      t.type = COLON;
      t.text = ":";
    }

    // check if we have a conditional statement and not a function call
    // i.e if( else(
    if(t.type == FUNC_CALL)
    {
      // not handling loops for this assignment
      if(t.text == "if(")
      {
        t.text = "if";
        t.type = COND_IF;
      }
      else if (t.text == "elif(")
      {
        t.text = "elif";
        t.type = COND_IF;
      }
      else if (t.text == "else(")
      {
        t.text = "else";
        t.type = COND_ELSE;
      }
      // remove last character
      if(t.type == COND_IF || t.type == COND_ELSE)
      {       
        if(DEBUG >= 3) 
          cout << "Line " << t.line << ": (" << lex2str[t.type] << "): \"" << t.text << "\"\n";
        global_token_list.push_back(t);
        t.type = PAREN_OP;
        t.text = "(";
      }
    }

    // check if any LABELs are a LIST
    if(t.type == LIST_DEC)
    {
      in_list = true;
      // last label is a LIST
      if(global_token_list[input_last_label].type == LABEL)
      {
        cout << "\tFound list label: (" << global_token_list[input_last_label].text << ")\n";
      }    
    }
    global_token_list.push_back(t);
    
    // temp save last label for list declarations
    if(t.type == LABEL)
    {
      input_last_label = global_token_list.size() - 1;
    }
    // end of list
    if(t.type == LIST_DEC_END)
      in_list = false;
    // Debug print
    if(DEBUG >= 3)
    {
      if(t.type != LF)
        cout << "Line " << t.line << ": (" << lex2str[t.type] << "): \"" << t.text << "\"\n";
      else
        cout << "Line " << t.line << ": (" << lex2str[t.type] << ")\n";
    }
  }
%}
%option noyywrap
%%
[ ]                           { inputHandler(SPACE, yytext); }
[\n]                          { inputHandler(LF, yytext); }
^[\t ]+                       { inputHandler(INDENT, yytext); }
#.*                           { inputHandler(COMMENT, yytext); }
<<EOF>>                       { inputHandler(FILE_EOF, yytext); return 0; }

"if"                          { inputHandler(COND_IF, yytext); }
"elif"                        { inputHandler(COND_IF, yytext); }
"else"                        { inputHandler(COND_ELSE, yytext); }
"return "                     { inputHandler(RETURN, yytext); }
:                             { inputHandler(COLON, yytext); }

=                             { inputHandler(ASSIGN, yytext); }
,                             { inputHandler(COMMA, yytext); }
\(                            { inputHandler(PAREN_OP, yytext); }
\)                            { inputHandler(PAREN_CL, yytext); }

==                            { inputHandler(C_EQUALS, yytext); }
>                             { inputHandler(C_GREATER, yytext); }
\<                            { inputHandler(C_LESSER, yytext); }
>=                            { inputHandler(C_GREATER_EQUALS, yytext); }
\<=                           { inputHandler(C_LESSER_EQUALS, yytext); }
!=                            { inputHandler(C_NOT_EQUALS, yytext); }

\+                            { inputHandler(OP_ADD, yytext); }

[0-9]+                        { inputHandler(VAL_INT, yytext); }
\'(\\.|[^\'\\])*\'                    { inputHandler(VAL_STRING, yytext); }
\"(\\.|[^"\\])*\"                    { inputHandler(VAL_STRING, yytext); }

[_a-zA-Z][_a-zA-Z0-9]*\[[0-9]+\]                    { inputHandler(LIST_INDEX, yytext); }
[_a-zA-Z][_a-zA-Z0-9]*\[[0-9]+:[0-9]+\]             { inputHandler(LIST_SLICE_TWO_ARG, yytext); }
[_a-zA-Z][_a-zA-Z0-9]*\[[0-9]+:\]                   { inputHandler(LIST_SLICE_ONE_ARG, yytext); }
".append("                    { inputHandler(LIST_APPEND, yytext); }
".insert("                    { inputHandler(LIST_INSERT, yytext); }
\[                            { inputHandler(LIST_DEC, yytext); }
\]                            { inputHandler(LIST_DEC_END, yytext); }


"print("                      { inputHandler(PRINT, yytext); }
[_a-zA-Z][_a-zA-Z0-9]*        { inputHandler(LABEL, yytext); }

"def "[_a-zA-Z][_a-zA-Z0-9]*\(   { inputHandler(FUNC_DEC, yytext); }
\):                           { inputHandler(FUNC_DEC_END, yytext); }
[_a-zA-Z][_a-zA-Z0-9]*\(         { inputHandler(FUNC_CALL, yytext); }

%%
/* Temp storage for dead rules
[.*]+                         { inputHandler(OTHER, yytext); }
\"[^\"\\]*(\\.[^\"\\]*)*\"    { inputHandler(VAL_STRING, yytext); } "
[ ]                           { inputHandler(SPACE, yytext); }
*/

//////////////////////////////////////////////////////////////////////////////
// PRINT FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void printTokenList(vector<token> tokens, int tabs)
{
  if(DEBUG >= VERBOSE) {
    for(int i = 0; i < tokens.size(); i++)
    {
      for(int t = 0; t < tabs; t++)
        cout << "\t";
      cout << "Token " << tokens[i].text << " has a type of " << lex2str[tokens[i].type] << "\n";
    }
  }
}
// prints a list
void printVarList(vector<simple_var> list, int tabs)
{
  cout << "printVarList, list has a size of: " + to_string(list.size()) + "\n";
  if(DEBUG >= VERBOSE) {
    for(int i = 0; i < list.size(); i++)
    {
      for(int t = 0; t < tabs; t++)
        cout << "\t";
      cout << "List element[" << to_string(i) << "] = " << list[i].text << " has a type of " << lex2str[list[i].type] << "\n";
    }
  }
}

void printError()
{
  out << "#error\n";
}

//////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
// removes those pesky quotes from string
string cleanString(string str)
{
  // remove first character
  str.erase(0,1);
  // remove last character
  str.pop_back();
  return str;
}

// is a CONDITION
bool isCondition(token t)
{
  switch(t.type)
  {
    case C_EQUALS:
    case C_GREATER:
    case C_LESSER:
    case C_GREATER_EQUALS:
    case C_LESSER_EQUALS:
    case C_NOT_EQUALS:
      return true;
  };
  return false;
}

// is ERROR
bool isError(token t)
{
  if(t.type == ERROR)
    return true;
  return false;
}

// is a LABEL
bool isLabel(token t)
{
  if(t.type == LABEL)
    return true;
  return false;
}

// is an IF token
bool isIFToken(token t)
{
  if(t.type == LABEL)
    return true;
  else if(t.type == VAL_INT)
    return true;
  else if(t.type == VAL_STRING)
    return true;
  return false;
}

// is an IF operator
bool isIFOp(token t)
{
  switch(t.type)
  {
    case C_EQUALS:
    case C_GREATER:
    case C_LESSER:
    case C_GREATER_EQUALS:
    case C_LESSER_EQUALS:
    case C_NOT_EQUALS:
      return true;
  };
  return false;
}

// returns true for data mismatch
bool typeMismatch(variable var, int type, int line)
{
  bool res = false;
  if(var.list.size() > 0)
  {
    if(var.list[0].type != type)
    {
      cout << "typeMismatch(): Found a conflict on line " << line
      << " (" << lex2str[var.list[0].type] << " != " << lex2str[type] << ")\n";
      res = true;
    }
  }
  return res;
}

//////////////////////////////////////////////////////////////////////////////
// EVALUATING EXPRESSIONS/EQUATIONS
//////////////////////////////////////////////////////////////////////////////
// appends sub to the end of master
variable appendListToList( int start_index, int stop_index, variable sub, variable master)
{
  // add a 1 to include the final index for sub full length calls
  if(stop_index < sub.list.size())
    stop_index++;
  for(int i = start_index; i < stop_index; i++)
    master.list.push_back(sub.list[i]);
  return master;
}

// the string must contain a []
// returns a list name
string getLabelFromListIndex(string list_index_full)
{
  return list_index_full.substr(0, list_index_full.find('['));
}

// the string must contain a []
// returns the integer index
int getIndexFromListIndex(string list_index_full)
{
  string str_index = "";
  int i = 0;
  bool index_start = false;
  while(i < list_index_full.length())
  {
    if(list_index_full.at(i) == '[')
      index_start = true;
    else if(index_start && list_index_full.at(i) != ']')
      str_index = str_index + list_index_full.at(i);
    else
      index_start = false;
    i++;
  }
  if(str_index == "")
    return -1;
  else
    return stoi(str_index);
}

// gets the start index from a slice
int getStartIndexFromSlice(string list_index_full)
{
  string str_index = "";
  int i = 0;
  bool index_start = false;
  while(i < list_index_full.length())
  {
    if(list_index_full.at(i) == '[')
      index_start = true;
    else if(index_start && list_index_full.at(i) != ':')
      str_index = str_index + list_index_full.at(i);
    else
      index_start = false;
    i++;
  }
  if(str_index == "")
    return -1;
  else
    return stoi(str_index);
}

// gets the end index from a slice
int getEndIndexFromSlice(string list_index_full)
{
  string str_index = "";
  int i = 0;
  bool index_start = false;
  while(i < list_index_full.length())
  {
    if(list_index_full.at(i) == ':')
      index_start = true;
    else if(index_start && list_index_full.at(i) != ']')
      str_index = str_index + list_index_full.at(i);
    else
      index_start = false;
    i++;
  }
  if(str_index == "")
    return -1;
  else
    return stoi(str_index);
}

// gets the index value given a string
simple_var getIndexValue(string list_index, map<string, variable> var_map)
{
  string name = getLabelFromListIndex(list_index);
  int index = getIndexFromListIndex(list_index);
  cout << "getIndexValue: " << name << "[" << to_string(index) << "] = ";
  cout << var_map[name].list[index].text << "\n";
  return var_map[name].list[index];
}

// evaluates an if statement and returns true or false
// will only evaulate simple variables as SPECIFIED by TA
// HW4 - questions about if-else
bool evalIF(token tk1, token tk2, token op, map<string, variable> var_map)
{
  bool res = false;
  variable var1;
  variable var2;
  // check type if label then we need to derive the value
  if(tk1.type == LABEL)
  {
    var1 = var_map[tk1.text];
    cout << "evalIF(): using var_map for \""<< tk1.text << "\" (" << var1.simple.text << "," << lex2str[var1.simple.type] << ")\n";
  }
  else if(tk1.type == VAL_INT || tk1.type == VAL_STRING)
  {
    var1.is_list = false;
    var1.simple.type = tk1.type;
    var1.simple.text = tk1.text;
  }
  else
    cout << "evalIF(): Invalid data type for token: " << tk1.text << " with a type of " << lex2str[tk1.type] << "\n";
  if(tk2.type == LABEL)
  {
    var2 = var_map[tk2.text];
    cout << "evalIF(): using var_map \"" << tk2.text << "\" (" << var1.simple.text << "," << lex2str[var2.simple.type] << ")\n";
  }
  else if(tk2.type == VAL_INT || tk2.type == VAL_STRING)
  {
    var2.is_list = false;
    var2.simple.type = tk2.type;
    var2.simple.text = tk2.text;
  }
  // script is not clean return true
  else
  {
    cout << "evalIF(): Invalid data type for token: " << tk2.text << " with a type of " << lex2str[tk2.type] << "\n";
    return true;
  }
  cout << "evalIF(): Comparing: \n";
  cout << "\t" << tk1.text << " " << op.text << " " << tk2.text;
  cout << ",\t" << lex2str[tk1.type] << " " << lex2str[op.type] << " " << lex2str[tk2.type] << "\n";
  cout << "\t" << var1.simple.text << " " << op.text << " " << var2.simple.text;
  cout << ",\t" << lex2str[var1.simple.type] << " " << op.text << " " << lex2str[var2.simple.type] << "\n";

  // evaluate the IF statement
  if(var1.simple.type == VAL_INT && var2.simple.type == VAL_INT)
  {
    int v1 = stoi(var1.simple.text);
    int v2 = stoi(var2.simple.text);
    switch(op.type)
    {
      case C_EQUALS:
        if(v1 == v2)  res = true;
        break;
      case C_GREATER:
        if(v1 > v2)  res = true;
        break;
      case C_LESSER:
        if(v1 < v2)  res = true;
        break;
      case C_GREATER_EQUALS:
        if(v1 >= v2)  res = true;
        break;
      case C_LESSER_EQUALS:
        if(v1 <= v2)  res = true;
        break;
      case C_NOT_EQUALS:
        if(v1 != v2)  res = true;
        break;
    };
  }
  else if(var1.simple.type == VAL_STRING && var2.simple.type == VAL_STRING)
  {
    string v1 = cleanString(var1.simple.text);
    string v2 = cleanString(var2.simple.text);
    switch(op.type)
    {
      case C_EQUALS:
        if(v1 == v2)  res = true;
        break;
      case C_GREATER:
        if(v1 > v2)  res = true;
        break;
      case C_LESSER:
        if(v1 < v2)  res = true;
        break;
      case C_GREATER_EQUALS:
        if(v1 >= v2)  res = true;
        break;
      case C_LESSER_EQUALS:
        if(v1 <= v2)  res = true;
        break;
      case C_NOT_EQUALS:
        if(v1 != v2)  res = true;
        break;
    };

  }
  else
  {
    cout << "evalIF(): Mismatched or invalid datatypes: (" << var1.simple.text << ", " << lex2str[var1.simple.type] << ") and "
        << var2.simple.text << ", " << lex2str[var2.simple.type] << ")\n";
  }
  return res;
}

// evaluates an "equation" - anything with variables or types is evaluated here
// returns a token:
//   token.text - text of evaluated equation
//   token.type - int type of evaluated equation (NO_VAL default, ERROR for issues )
//   token.line - line number of evaluated equation
map<string, variable> evalLine(int state, int index_t, vector<token> expr, map<string, variable> var_map)
{
  string var_name = "";
  variable var;
  int var_index = 0;
  int i = 0;
  // left hand side and operator
  token LHS, OP;

  // loop through expression
  while(i < expr.size())
  {
    // STEP1: look for a label first
    if(state == NO_VAL && expr[i].type == LABEL)
    {
      var_name = expr[i].text;
      state = LABEL;
    }
    else if(state == NO_VAL && expr[i].type == LIST_INDEX)
    {
      var_name = getLabelFromListIndex(expr[i].text);
      state = LABEL;
    }
    // STEP2: look for an assignment after
    else if(state == LABEL && expr[i].type == ASSIGN)
    {
      state = ASSIGN;
    }
    // STEP2: append
    // STEP2: remove
    // STEP3: assignment stuff
    else if(state == ASSIGN)
    {
      //cout << "\t" << "in state ASSIGN: " << expr[i].text << " " << lex2str[expr[i].type] << "\n";
      if(expr[i].type == VAL_INT || expr[i].type == VAL_STRING)
      {
        var.simple.text = expr[i].text;
        var.simple.type = expr[i].type;
        var.is_list = false;
        cout << "\tVAL_INT_STRING simple var assign: (" << var.simple.text << "," << lex2str[var.simple.type] << ")\n";
      }
      else if(expr[i].type == LIST_DEC)
      {
        i++;
        while(i < expr.size() && (expr[i].type == VAL_INT || expr[i].type == VAL_STRING || expr[i].type == LABEL || expr[i].type == LIST_INDEX || expr[i].type == COMMA))
        {
          simple_var temp_var;
          switch(expr[i].type)
          {
            case COMMA:
              // do nothing with commas
              break;
            case VAL_INT:
            case VAL_STRING:
              // check for data mismatch
              if(typeMismatch(var, expr[i].type, expr[i].line))
              {
                printError();
                state = ERROR;
              }
              temp_var.type = expr[i].type;
              temp_var.text = expr[i].text;
              var.is_list = true;
              var.list.push_back(temp_var);
              cout << "\tVAL_INT_STRING concat: " << temp_var.text << "\n";
              break;
            case LABEL:              
              // check for data mismatch
              if(typeMismatch(var, var_map[expr[i].text].simple.type, expr[i].line))
              {
                printError();
                state = ERROR;
              }
              // simple variable (non-list) assume the map is correct
              temp_var.type = var_map[expr[i].text].simple.type;
              temp_var.text = var_map[expr[i].text].simple.text;
              var.list.push_back(temp_var);
              cout << "\tLABEL concat: " << expr[i].text + " with a value of \"" << temp_var.text << "\"\n";
              break;
            case LIST_INDEX:
              // item at index
              temp_var = getIndexValue(expr[i].text, var_map);
              var.list.push_back(temp_var);
               cout << "\tLIST_INDEX concat: " + temp_var.text << "\n";
              break;
            default:
              cout << "\tUnsupported or bad type: " << expr[i].text << ", type = " << lex2str[expr[i].type] << "\n";
          };
          i++;
        } // end while for list assignment
      }
      else if(expr[i].type == LABEL)
      {
        // check for data mismatch
        if(typeMismatch(var, var_map[expr[i].text].list[0].type, expr[i].line))
        {
          printError();
          state = ERROR;
        }
        cout << "\tLABEL concat: " << expr[i].text << "\n";
        printVarList(var_map[expr[i].text].list, 1);
        var = appendListToList(0, var_map[expr[i].text].list.size(), var_map[expr[i].text], var);
      }
      else if(expr[i].type == LIST_SLICE_ONE_ARG)
      {
        // check for data mismatch
        if(typeMismatch(var, var_map[expr[i].text].list[0].type, expr[i].line))
        {
          printError();
          state = ERROR;
        }
        string name = getLabelFromListIndex(expr[i].text);
        int start_index = getStartIndexFromSlice(expr[i].text);
        cout << "\tLIST_SLICE_ONE_ARG concat: " << name << "\n";
        cout << "\tStart/End Index: " << start_index << "\n";
        var = appendListToList(start_index, var_map[name].list.size(), var_map[name], var);
      }
      else if(expr[i].type == LIST_SLICE_TWO_ARG)
      {
        // check for data mismatch
        if(typeMismatch(var, var_map[expr[i].text].list[0].type, expr[i].line))
        {
          printError();
          state = ERROR;
        }
        string name = getLabelFromListIndex(expr[i].text);
        int start_index = getStartIndexFromSlice(expr[i].text);
        int end_index = getEndIndexFromSlice(expr[i].text);
        cout << "\tLIST_SLICE_TWO_ARG concat: " << name << "\n";
        cout << "\tStart/End Index: " << start_index << ", " << end_index << "\n";
        var = appendListToList(start_index, end_index, var_map[name], var);
      }
      else if(expr[i].type == COND_IF)
      {
        token tk1;
        token tk2;
        token op;
        int if_state = 0;
        bool if_res = true;
        // Get everything before the ELSE statement
        while(i < expr.size() && expr[i].type != COND_ELSE)
        {
          // first token
          if(if_state == 0 && isIFToken(expr[i]))
          {
            tk1 = expr[i];
            if_state = 1;
          }
          // operator
          else if(if_state == 1 && isIFOp(expr[i]))
          {
            op = expr[i];
            if_state = 2;
          }
          // second token
          else if(if_state == 2 && isIFToken(expr[i]))
          {
            tk2 = expr[i];
            if_state = 3;
            // process the if statement
            if_res = evalIF(tk1, tk2, op, var_map);
          }
          i++;
        }
        // if the if statement evaluated true, dont evaluate any further
        if(expr[i].type == COND_ELSE && if_res)
        {
          cout << "evalIF() returned true\n";
          state = COND_IF;
        }
        // clear the variable list just in case we're concatenating lists (i dont think this will be the case)
        else if(expr[i].type == COND_ELSE && !if_res)
        {
          cout << "evalIF() returned false\n";
          var.list.clear();
        }
      }
    } // end if(state == ASSIGN)
    i++;
  } // END EXPR while

  // value changed
  if(state == ASSIGN || state == COND_IF)
  {
    var_map[var_name] = var;
    // is a list
    if(var.is_list)
    {
      cout << "\tState: " << lex2str[state] << "\n";
      cout << "\tList variable (" + var_name + ") value changed with a size of " << to_string(var.list.size()) +"\n";
      printVarList(var.list, 1);
    }
    // is not a list
    else
    {
      cout << "\tSimple variable changed: " << var_name << " = (" 
            << var_map[var_name].simple.text << " of type " << lex2str[var_map[var_name].simple.type] << ")\n";
    }
  }
  // return the map - changed or unchanged
  return var_map;
}


//////////////////////////////////////////////////////////////////////////////
// BUILDING AN EQUATION/EXPRESSION
//////////////////////////////////////////////////////////////////////////////
// build an equation, returns when a LF, EOF, or FUNC_CALL are encountered
int buildLine(int line, int t_index, vector<token> t_list, vector<token>* expr, string* line_to_print)
{
  for(int i = t_index; i < t_list.size(); i++)
  {
    // push back things on the same line
    if(t_list[i].line == line)
    {
      if(t_list[i].type != LF)
        expr->push_back(t_list[i]);
      // print this line to file
       *line_to_print = *line_to_print + t_list[i].text;
    }
    // we're done
    else
      return expr->size();
  }
  return expr->size();
}


//////////////////////////////////////////////////////////////////////////////
// PRINT()
//////////////////////////////////////////////////////////////////////////////
void printLabelAndSlice(string list_name, map<string, variable> var_map, int index, int start_index, int end_index)
{
  int i=index;
  for (int j=start_index; j<end_index-1; j++)
  {
    cout<<var_map[list_name].list[j].text<<",";
    out<<var_map[list_name].list[j].text<<",";
  }
  cout<<var_map[list_name].list[end_index-1].text;
  out<<var_map[list_name].list[end_index-1].text;

}
void printHandler(vector<token> t_list, map<string, variable> var_map)
{
  
  int i = 0;
  bool is_print_at_index = false, is_concatenation = false, end_print_list = false;
  bool start_printing = false;
  // loop through token list
  while(i < t_list.size())
  {
    // check if there is a print statement
    //Check if it's a print index statement 
    if(t_list[i].type == PRINT)
    {
      start_printing = true;
      if(lex2str[t_list[i+1].type] == "LIST_INDEX")
      {
        is_print_at_index = true;
      }
      if (!is_concatenation && !is_print_at_index)
      {
        cout<<"[";
        out<<"[";
      }
    }
    // start printing
    if(start_printing && t_list[i].type != EOF)
    {
      // out << "Print looping through tokens: " << t_list[i].text << " of type "<< lex2str[t_list[i].type] << "\n";
      // cout << "Print looping through tokens: " << t_list[i].text << " of type "<< lex2str[t_list[i].type] << "\n";
      //If it's LABEL case then print the whole list
      //Print space if there is concatenation
      if (lex2str[t_list[i].type]=="LIST_INDEX")
      {
        is_print_at_index = true;
        string list_name = getLabelFromListIndex(t_list[i].text);
        int index = getIndexFromListIndex(t_list[i].text);
        //Check for out of range index 
        if(index>=var_map[list_name].list.size())
        {
          cout<<"Index out of range"<<std::endl;
          out<<"Index out of range"<<"\n";
          break;
        }
        cout<<var_map[list_name].list[index].text<<endl;
        out<<var_map[list_name].list[index].text<<"\n";
      }
      else if(lex2str[t_list[i].type]=="LABEL")
      {
        string list_name = t_list[i].text ;
        printLabelAndSlice(list_name,var_map,i,0,var_map[list_name].list.size());
      }
      //In case of print at index
      //Print using slice arg
      else if (lex2str[t_list[i].type]=="LIST_SLICE_ONE_ARG")
      {
        int index = getStartIndexFromSlice(t_list[i].text);
        string list_name = getLabelFromListIndex(t_list[i].text);
        if(index>var_map[list_name].list.size())
        {
          cout<<"Index out of range"<<std::endl;
          out<<"Index out of range"<<"\n";
          break;
        }
        printLabelAndSlice(list_name,var_map,i,index,var_map[list_name].list.size());
      }
      //In case list slices with 2 args
      else if (lex2str[t_list[i].type]=="LIST_SLICE_TWO_ARG")
      {
        string list_name = getLabelFromListIndex(t_list[i].text);
        int start_index = getStartIndexFromSlice(t_list[i].text);
        int end_index = getEndIndexFromSlice(t_list[i].text);
        if(end_index>var_map[list_name].list.size())
        {
          cout<<"Index out of range"<<std::endl;
          out<<"Index out of range"<<"\n";
          break;
        }
        printLabelAndSlice(list_name,var_map,i,start_index,end_index);
      }
      //If detect concatenation
      if (lex2str[t_list[i].type]=="OP_ADD")
      {
        cout<<",";
        out<<",";
      }
      //Mark the end of printing list
      else if (lex2str[t_list[i].type]=="PAREN_CL" && !is_print_at_index)
      {
        cout<<"]"<<endl;
        out<<"]"<<"\n";
      }
    }
    //If it's one list index -> parse to get index value and print only the value at that index
    i++;
  }
}

//////////////////////////////////////////////////////////////////////////////
// PARSER FOR MAIN PROGRAM
//////////////////////////////////////////////////////////////////////////////
// parse main body line by line
void pyCodeParser(vector<token> t_list)
{
  map<string, variable> var_map;
  int i = 0;
  int on_line = 0;
  // read all tokens and build expressions for each line
  while(i < t_list.size())
  {
    // evaluate this line
    if(on_line != t_list[i].line)
    {
      vector<token> line_expr;
      string line_to_print;
      int expr_size = buildLine(t_list[i].line, i, t_list, &line_expr, &line_to_print);
      on_line = t_list[i].line;
      // evaluate the expression
      if(expr_size > 0)
      {
        cout << "Expression received:\n";
        printTokenList(line_expr, 1);
        var_map = evalLine(NO_VAL, 0, line_expr, var_map);

        // call print on every iteration cause why not
        out << line_to_print;
        printHandler(line_expr, var_map);
      }
      else
      {
        cout << "No expression on this line\n";
      }
    }
    i++;
  }
}


//////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  vector<token> t_list;
  string in_file;

  cout << "You entered: " << argc << " arguments\n";
  if(argc != 2)
  {
    cout << "ERR: No program arguments (0), please restart in the format: ./checkdatatype file=program1.py\n";
    return 1;
  }

  // set up input/output files
  in_file = argv[1];
  size_t pos = in_file.find("file=");
  if(pos != 0)
  {
    cout << "ERR: Invalid program arguments (1), please restart in the format: checkdatatype file=program1.py\n";
    return 1;
  }
  in_file.erase(pos, 5);
  out_file = in_file;
  cout << "File " << in_file << " specified as input file\n";
  pos = out_file.find(".py");
  if(pos <= 0)
  {
    cout << "ERR: Invalid program arguments (2), please restart in the format: checkdatatype file=program1.py\n";
    return 1;
  }
  out_file.erase(pos, 3);
  out_file += ".out";
  cout << "File " << out_file << " set as output file\n";
  remove(out_file.c_str());

  // read input file
  FILE *py_file = fopen(in_file.c_str(), "r");
  if(!py_file) {
    cout << "ERR: Error opening file, maybe it doesnt exist?\n";
    return 1;
  }

  cout << "Input file Opened\n";
  yyin = py_file;

  // lex through the input
  while(yylex());
  t_list = global_token_list;
  // evaluate the call stack for all VAR types on
  // the LHS of the assign (where the RHS is constant)

  // evaluate the call stack for all VAR types on the
  // LHS of the assign (where the RHS is not a constant)
  out.open(out_file.c_str());
  pyCodeParser(t_list);
  out.close();
}