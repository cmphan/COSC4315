/*Name: CUONG PHAN
HomeWork02 - COSC 4315 - Fall_2019
PSID: 1851430 */
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<algorithm>
#include<iterator>
#include<fstream>
using namespace std;
string RemoveSpaceFromString(string str);
bool IsNumber(string str);
string checkVariableType(const string right_hand_side);
string checkDataTypeConflict(const string variable_type, const string right_hand_side_type);
bool checkErrorWithMultiplication(const vector<string> Variable_Names, map<string,string> VarAndTypes, map<int,string> &LinesAndMessages, int line_number, bool &is_Error_Warning);
void checkErrorWithAddition(const vector<string> Variable_Names, const string var_name, map<string,string> &VarAndTypes, map<int,string> &LinesAndMessages, int line_number, bool& is_Error_Warning);
bool hasAdditionOperand(const string str);
bool hasMultiplicationOperand(const string str);
vector<string>VectorOfVariables(string str);
bool IsLiteralString(string str);
bool hasChar(const string str);
bool IsAList(const string str);
bool IsListOperation(const string str, map<string,string> &VarAndTypes,map<int,string>&LineAndMessages, int line_number, bool& is_Error_Warning);
bool isInsertListOperation(const string str);
bool isAppendListOperation(const string str);
void hanleConditionalStatement(string str,map<string,string> &VarAndTypes,map<int,string>&LineAndMessages, int line_number, bool &is_Error_Warning);
string RemoveIfKeyWord(string str);
string RemoveElifKeyWord(string str);
string RemoveSemiColon(const string str);
vector<string> ExtractionFunctionVariables(string str);
string ExtractFunctionName(const string str);
bool checkForIndentation(const string str);
bool checkStatementErrorAndWarning(const string statement, map<string,string>& VarAndTypes, map<int,string>&LineAndMessages,int line_number, map<string,int>FunctionNamesAndSize);
bool SkipNextStatement(const string statement);
int HandleFunctionDef(const vector<string> Function_statement,map<string,string>& VarAndTypes, map<string,string>&VarAndTypes_Function, map<int,string>&LineAndMessages,int line_number, bool &is_Error_Warning, map<string,int> &FunctionNamesAndSize);
string RemoveReturnKeyWord(string return_statement);
bool isFuctionCall(const string statement);
void HandleFunctionCall(string Function_call_statement,map<string,string>& VarAndTypes, map<int,string>&LineAndMessages,int line_number, bool &is_Error_Warning, map<string, int> FunctionNamesAndSize);
void WriteToOutPutFile(vector<string> pythonStatements, string file_name,map<int,string> LineAndMessages);
int main(int argc, char**argv)
{
    string original_string, input, input_file_name;
    //Parse through command line to get the input Python file name 
    //*************************************************************
    string commandline_arguments;
    int number_arguments = 0;
    for (int i=0; i<argc; i++)
    {
        commandline_arguments.append(argv[i]);
        number_arguments++;
    }
    if(commandline_arguments.length()==0 || number_arguments>2)
    {   
        cerr<<"Error: Wrong Call Program Syntax"<<endl;
        cerr<<"*************************"<<endl;
        cerr<<"Example Call Syntax for python file name \"program1.py\":"<<endl;
        cerr<<"     ./checkdatatype file=program1.py"<<endl;
        cerr<<"*************************"<<endl;
        return 0;
    }
	
    input_file_name = commandline_arguments.substr(commandline_arguments.find("=")+1,commandline_arguments.length());
    vector<string> pythonStatements;
    map<string,string> VarAndTypes;
    map<string,string>VarAndTypes_Function;
    map<string,int>FunctionNamesAndSize;
    map<int,string>LineAndMessages;
    vector<string> Variables_Names;
    vector<string> Function_statements;
    //Read Python file in a string => append <EOS> end of statement at the end of each Python statement
    //***************************************************************************************************
    ifstream Python_File(input_file_name);
    if (Python_File.is_open())
    {
        while(getline(Python_File,input))
        {
            original_string.append(input);
            if(cin.good())
            {
                original_string.append("<EOS>");
            }
        }
        Python_File.close();
        //Check if file is empty
        if(original_string.length()==0)
        {
            cerr<<"Error: input file is empty"<<endl;
            return 0;
        }
    }
    //If file does not exist or error with file
    else {
        cerr<<"Error: operning file. File does not exist"<<endl;
        return 0;
    }
    //Remove <EOS> symbol from the string and put all Python statements into a vector of string
    //******************************************************************************************
    size_t pos = 0;
    while ((pos = original_string.find("<EOS>"))!=string::npos)
    {
        pythonStatements.push_back(original_string.substr(0,pos));
        original_string.erase(0, pos + 5);
    }
    pythonStatements.push_back(original_string);
    string variable_name;
    string right_hand_side;
    bool is_Error_Warning;
    int i=0;
    //Go through the whole Python statement string vector -> classify statements and check for errors/warnings
    //******************************************************************************************
    while(i<pythonStatements.size())
    {
        //If a function defition => Send to function handle function 
        if (pythonStatements.at(i).find("def")!=string::npos &&!SkipNextStatement(pythonStatements.at(i)))
        {
            i = HandleFunctionDef(pythonStatements,VarAndTypes,VarAndTypes_Function,LineAndMessages,i,is_Error_Warning,FunctionNamesAndSize);
            i++;
        }
        if(i>=pythonStatements.size())
        {
            break;
        }
        //Check other statement in global scope (outside of function defition)
        if(checkStatementErrorAndWarning(pythonStatements.at(i),VarAndTypes,LineAndMessages,i,FunctionNamesAndSize)){};
        i++;
    }
    //Write to the output file
    WriteToOutPutFile(pythonStatements,input_file_name,LineAndMessages);
    return 0;
}
//Write Out Put to inputfilename .out
//******************************************************************************************
void WriteToOutPutFile(vector<string> pythonStatements, string file_name, map<int,string>LineAndMessages)
{
    string output;
    ofstream output_file;
    int track_insert =0;
    map<int, string>::iterator itr2; 
    vector<string>::iterator vector_pos;
    //Insert error/warning in the original python input file if any. 
    if(!LineAndMessages.empty())
    {
        for (itr2 = LineAndMessages.begin(); itr2 != LineAndMessages.end(); ++itr2)
        { 
            vector_pos = pythonStatements.begin() + itr2->first;
            pythonStatements.insert(vector_pos+track_insert,itr2->second);
            track_insert++;
        }  
    } 
    for (int i=0; i<pythonStatements.size(); i++)
    {
        if(i<pythonStatements.size()-1)
            if(checkForIndentation(pythonStatements.at(i+1))&&pythonStatements.at(i).find("#")!=string::npos)
               output.append("    ");
         output.append(pythonStatements.at(i));
         output.append("\n");
    }
    string out_put_file_name = file_name.erase(file_name.find("."),file_name.length()) + ".out";
    output_file.open(out_put_file_name);
    output_file <<output;
    output_file.close();
}
//Hanle Function Def and Function Scope
//******************************************************************************************
int HandleFunctionDef(const vector<string> Function_statement,map<string,string>& VarAndTypes, map<string,string> &VarAndTypes_Function, map<int,string>&LineAndMessages,int line_number, bool &is_Error_Warning, map<string,int> &FunctionNamesAndSize)
{
    map<string,string>::iterator it;
    is_Error_Warning = false;
    vector<string> Function_Variables_Declaration;
    vector<string> Return_Variable_Names;
    int next_instruction = line_number +1;
    string function_def_line = RemoveSpaceFromString(Function_statement.at(line_number));
    Function_Variables_Declaration = ExtractionFunctionVariables(function_def_line);
    FunctionNamesAndSize.insert(pair<string,int>(ExtractFunctionName(function_def_line),Function_Variables_Declaration.size()));
    VarAndTypes.insert(pair<string,string>(ExtractFunctionName(function_def_line),"FUNCTION"));
    for (int j=0; j<Function_Variables_Declaration.size(); j++)
    {
        VarAndTypes_Function.insert(pair<string,string>(Function_Variables_Declaration.at(j),"FUNCTION_DECLARED_VARIABLE"));
    }
    //Searching for a return statement
    while(Function_statement.at(next_instruction).find("return")==string::npos)
    {
        if(checkStatementErrorAndWarning(Function_statement.at(next_instruction),VarAndTypes_Function,LineAndMessages,next_instruction,FunctionNamesAndSize))
        {
            is_Error_Warning = true;
        }
        next_instruction++;
    }
    //Hanlde return statement => Determine function type
    string return_expression = RemoveReturnKeyWord(RemoveSpaceFromString(Function_statement.at(next_instruction)));
    /****Check return expression and determine function type in map ****/
    //if has * operand => FUNCTION_INT type only (cannot take list or string as parameter)
    //if return expression is minial => classify function type
    if(!hasAdditionOperand(return_expression)&&!hasMultiplicationOperand(return_expression)&&!IsNumber(return_expression)&&!IsLiteralString(return_expression))
    {
        VarAndTypes[ExtractFunctionName(function_def_line)]="FUNCTION_"+VarAndTypes_Function[return_expression];
    }
    Return_Variable_Names = VectorOfVariables(return_expression);
    if(Return_Variable_Names.size()==1)
    {
        if(VarAndTypes.count(Return_Variable_Names.at(0))!=0)
        {
            VarAndTypes[ExtractFunctionName(function_def_line)] = "FUNCTION_" + VarAndTypes_Function[return_expression];
        }
        else {
            VarAndTypes[ExtractFunctionName(function_def_line)] = "FUNCTION_" + checkVariableType(return_expression);
        }
    }
    if(hasMultiplicationOperand(return_expression))
    {
        VarAndTypes[ExtractFunctionName(function_def_line)]="FUNCTION_INT";
    }
    else if(hasAdditionOperand(return_expression))
    {
        //If there is a conflict in variable return type => error
        checkErrorWithAddition(Return_Variable_Names,"NO_VARIABLE_ON_LEFT",VarAndTypes_Function,LineAndMessages,next_instruction, is_Error_Warning);
        if (is_Error_Warning) 
        {
            return next_instruction;
        }
        /*THERE ARE NOT CONLIFCT IN TYPE AT RETURN STATEMENT => CLASSIFY FUNCTION TYPE */
        //If any of them are list => FUNCTION_LIST_TYPE
        if(VarAndTypes[Return_Variable_Names.at(0)]=="STRING")
        {
            VarAndTypes[ExtractFunctionName(function_def_line)]="FUNCTION_STRING";
            return next_instruction;
        }
        //if function return nothing
        else if (Return_Variable_Names.size()==0)
        {
            return next_instruction;
        }
        //if function return one thing => variable type determine function type
        else if (Return_Variable_Names.size()==1)
        {
            VarAndTypes[ExtractFunctionName(function_def_line)]="FUNCTION_" + checkVariableType(Return_Variable_Names.at(0));
        }
        //If any of them are string => FUNCTION_STRING_TYPE
        else {
            if(Return_Variable_Names.size()==2)
            {
                if(VarAndTypes_Function[Return_Variable_Names.at(0)]!=VarAndTypes_Function[Return_Variable_Names.at(1)])
                {
                LineAndMessages.insert(pair<int,string>(next_instruction,"#warning"));
                VarAndTypes[ExtractFunctionName(function_def_line)] ="FUNCTION_LIST";
                is_Error_Warning = true;
                return next_instruction;
                }
                VarAndTypes[ExtractFunctionName(function_def_line)]="FUNCTION_LIST";
                return next_instruction+1;
            }
        }
    }
 
  // Erase map when function go out of scope
        VarAndTypes_Function.empty();
         map<string, string>::iterator itr; 
    return next_instruction;
}
//Go thorugh the Python Vector Statement to identify variables, function def => put in a map VarAndTypes
//Check for warnings and errors => insert line number corresponding messages(#error/#warning) into LineAndMessages map
//******************************************************************************************
bool checkStatementErrorAndWarning(const string statement, map<string,string>& VarAndTypes, map<int,string>&LineAndMessages,int line_number, map<string,int>FunctionNamesAndSize)
{
    bool isError_Warning = false;
    size_t pos_equal = string::npos;
    size_t pos_underscore = string::npos;
    string variable_name, right_hand_side;
    vector<string> Variables_Names;
    if(SkipNextStatement(statement))
    {
        return isError_Warning;
    }
    //if it's an if or elif statement => send to function handle if / elif statement 
    else if (statement.find("if")!=string::npos || statement.find("elif")!=string::npos)
    {
        hanleConditionalStatement(RemoveSpaceFromString(statement),VarAndTypes,LineAndMessages,line_number, isError_Warning);
        return isError_Warning;
    }
    else if(isFuctionCall(RemoveSpaceFromString(statement)))
    {
        //Assignment statement of function call =>ADD to the map of new variable
        if((pos_equal=statement.find("="))!=string::npos)
        {
            string var_name = RemoveSpaceFromString(statement).substr(0,pos_equal-1);
            string funct_name = RemoveSpaceFromString(statement).substr(RemoveSpaceFromString(statement).find("=")+1,RemoveSpaceFromString(statement).find("(")-RemoveSpaceFromString(statement).find("=")-1);
            string right_hand_side = RemoveSpaceFromString(statement).substr(pos_equal, RemoveSpaceFromString(statement).length());
            if(VarAndTypes.count(funct_name)==0)
            {
                LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                isError_Warning = true;
                return isError_Warning;
            }
            else if(!hasMultiplicationOperand(right_hand_side)&&!hasAdditionOperand(right_hand_side))
                VarAndTypes.insert(pair<string,string>(var_name,VarAndTypes[funct_name]));
            else 
            {
                Variables_Names = VectorOfVariables(right_hand_side);
                for (int k=0; k<Variables_Names.size();k++)
                {
                    if(Variables_Names.at(k).find(funct_name)!=string::npos)
                    {
                        Variables_Names.erase(Variables_Names.begin()+k);
                    }
                }
                string funct_type = VarAndTypes[funct_name].substr(VarAndTypes[funct_name].find("_"),VarAndTypes[funct_name].length());
                if (hasAdditionOperand(right_hand_side))
                {
                    for (int l=0; l<Variables_Names.size(); l++)
                    {
                        if(!(checkVariableType(Variables_Names.at(l))==funct_type))
                        {
                            LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                            isError_Warning = true;
                        }
                    }
                }
                else if(hasMultiplicationOperand(right_hand_side))
                {
                     for (int l=0; l<Variables_Names.size(); l++)
                    {
                        if(checkVariableType(Variables_Names.at(l))=="STRING")
                        {
                            LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                            isError_Warning = true;
                        }
                    }
                }
            }
        }
        else
            HandleFunctionCall(RemoveSpaceFromString(statement),VarAndTypes,LineAndMessages,line_number,isError_Warning,FunctionNamesAndSize);
        return isError_Warning;
    }

    /*=========================
    IF ITS AN ASSIGNMENT STATEMENT 
    =============================*/
    else if((pos_equal=RemoveSpaceFromString(statement).find("=")) != string::npos)
    {
        /****EXTRACT VARIABLE ON LEFT and RIGHT SIDE of ASSIGNMENT STATEMENT*****/
        //In case there are addtional operand on the left -side => NEED TO HANDLE MORE
        variable_name = RemoveSpaceFromString(statement).substr(0,pos_equal);
        right_hand_side = RemoveSpaceFromString(statement).substr(pos_equal+1, statement.length()-1);
        //Check it's if a list declartion statement list = []
        if (IsAList(right_hand_side))
        {
            VarAndTypes.insert(pair<string,string>(variable_name,"LIST"));
            return isError_Warning;
        }
        //Check if the right hand side is minimal (no more operand)
        else if(!hasAdditionOperand(right_hand_side)&&!hasMultiplicationOperand(right_hand_side))
        {
            string variable_type = checkVariableType(right_hand_side);
            //If variable is not in map before, map variable and its type to the map
            if (VarAndTypes.count(variable_name)==0)
            {
                VarAndTypes.insert(pair<string,string>(variable_name,variable_type));
                return isError_Warning;
            }
             //Check if the variable is in the map. This a reassignment statement so check for conflict
             //if there is conflict it's a warning
            //Check any data type conflicts
            else {
                if(VarAndTypes.count(variable_name)!=0 &&VarAndTypes[variable_name]=="FUNCTION_DECLARED_VARIABLE")
                {
                    return isError_Warning;
                }
                //function to check status of statement
                else if(VarAndTypes.count(right_hand_side)==0)
                {
                    if (checkDataTypeConflict(VarAndTypes[variable_name],variable_type) == "NO")
                    {
                        LineAndMessages.insert(pair<int,string>(line_number,"#warning"));
                        isError_Warning= true;
                        //If a warning still update new variable type
                        VarAndTypes[variable_name] = variable_type;
                        //Check either error and warnings
                        //Error when there is a type conflict in operation
                        //Warning when the type is changed 
                        return isError_Warning;
                    }
                }
                else if(VarAndTypes[right_hand_side]!=VarAndTypes[variable_name])
                {
                    LineAndMessages.insert(pair<int,string>(line_number,"#warning"));
                    isError_Warning= true;
                    return isError_Warning;
                }
            }
        }
         //If there are operands on the right side
        else 
        {
            //If has Operand, break down more to get each variable
            //Handle cases with * operand 
            Variables_Names = VectorOfVariables(right_hand_side);
            //If there is an * operand check for type on both side of the operand, 
            if(hasMultiplicationOperand(right_hand_side))
            {
               bool checkError = checkErrorWithMultiplication(Variables_Names,VarAndTypes,LineAndMessages,line_number, isError_Warning);
               if (!checkError) //no error create a new variable 
               {
                    VarAndTypes.insert(pair<string,string>(variable_name,"INT"));
               }
               return isError_Warning;
            }
            //Only has Addition Operand
            else {
                checkErrorWithAddition(Variables_Names,variable_name,VarAndTypes,LineAndMessages,line_number, isError_Warning);
                return isError_Warning;
            }
        }
    }
    /*=========================
    IF ITS NOT AN ASSIGNMENT STATEMENT 

    =============================*/
    else 
    {
        string no_assignment_statement = RemoveSpaceFromString(statement);
        //Check if it's a list operation. If it is => send to function handle list operation error/ warning => exit
        if(IsListOperation(no_assignment_statement, VarAndTypes,LineAndMessages,line_number, isError_Warning))
        {
            return isError_Warning;
        }
        //If are no more operand on the right side => Check its type => it will determine data type of the left side
        else if(!hasAdditionOperand(no_assignment_statement)&&!hasMultiplicationOperand(no_assignment_statement))
        {
            //check if it's declared in map => no=> error
            if(VarAndTypes.count(no_assignment_statement)==0 && no_assignment_statement.find("def")==string::npos) 
                {
                    LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                    isError_Warning = true;
                }
            return isError_Warning;
        }
        //in case there are operand
        else 
        {
            //Break down to get variables
            Variables_Names = VectorOfVariables(no_assignment_statement);
            //Check if each of them are of the same type
            if(hasMultiplicationOperand(right_hand_side))
            {
                bool checkError = checkErrorWithMultiplication(Variables_Names,VarAndTypes,LineAndMessages,line_number,isError_Warning);
                if (!checkError) //no error create a new variable 
                {
                    VarAndTypes.insert(pair<string,string>(variable_name,"INT"));
                    return isError_Warning;
                }
            }
            //Only has Addition Operand
            else 
            {
                checkErrorWithAddition(Variables_Names,"NO_VARIABLE_ON_LEFT",VarAndTypes,LineAndMessages,line_number,isError_Warning);
                return isError_Warning;
            }
        }
    }         
    return isError_Warning;
}
bool SkipNextStatement(const string statement)
{
    bool skipStatement = false;
     /*if it's a comment statement (start with #)
     if it's an empty line 
     if it's an import library statement (Start with import)
     if it's an else statement
      => skip to the next line */
     if (statement.find("#")!=string::npos || statement.length() == 0 || statement.find("import")!=string::npos || statement.find("else")!=string::npos||statement.find("return")!=string::npos)
        {
            skipStatement = true;
        }
    return skipStatement;
}
string RemoveSpaceFromString(string str)
{
    str.erase(remove(str.begin(), str.end(),' '),str.end());
    return str;
}
string RemoveReturnKeyWord(string return_statement)
{
    size_t return_keyword_position = string::npos;
    if (return_statement.find("return")!=string::npos)
    {
        return_statement.erase(0,6);
    }
    return return_statement;
}
bool hasAdditionOperand(const string str)
{
    bool hasAddition= false;
    size_t pos = 0;
    if (( pos = str.find("+")) != string::npos)
        hasAddition = true;
    return hasAddition;
}
bool hasMultiplicationOperand(const string str)
{
    bool hasMultiplication= false;
    size_t pos = 0;
    if (( pos = str.find("*")) != string::npos)
        hasMultiplication= true;
    return hasMultiplication;
}
//Check if a string is a number
bool IsNumber(const string str)
{
    bool isNumber = false;
    bool hasNumber = any_of(str.begin(), str.end(), ::isdigit);
    //Has Number and do not have any other character => it's a number
    if (hasNumber&&!hasChar(str))
    {
        isNumber = true;
    }
    return isNumber;
}
bool IsLiteralString(const string str)
{
    size_t pos = 0;
    bool isString = false;
    if (( pos = str.find("\"")) != string::npos)
        isString= true;
    return isString;
}
bool hasChar(string str)
{
    bool isChar = false;
    for (int k=0; k<str.length(); k++)
    {
        if ((str[k]>='a' && str[k]<='z') || (str[k]>= 'A'&& str[k]<= 'Z'))
        {
            isChar = true;
        } 
    }
    return isChar;
}
bool IsAList(const string str)
{
    size_t pos = 0;
    bool isList = false;
    if (( pos = str.find("[")) != string::npos)
        isList= true;
    return isList;
}
string checkVariableType(const string right_hand_side)
{
    //Check if it's a number 
    if (IsLiteralString(right_hand_side)) return "STRING";
    else if (IsNumber(right_hand_side)) return "INT";
    else return "STRING";
}
string checkDataTypeConflict(const string variable_type, const string right_hand_side_type)
{
    if (variable_type==right_hand_side_type) return "OK";
    else return "NO";
}
//Take a right hand side of a statement and extract variable into a vector base on operands.
vector<string>VectorOfVariables(string str)
{
    vector<string> variablesName_1;
    vector<string> variablesName;
   size_t pos = string::npos;
   while ((pos=str.find("*")) != string::npos)
   {
      variablesName_1.push_back(str.substr(0,pos));
      str.erase(0,pos+1);
   }
   if (str.length()>=1)
      variablesName_1.push_back(str);
    if(variablesName.size()==1) return variablesName;
   for (int j=0; j<variablesName_1.size(); j++)
   {
      string n = variablesName_1.at(j);
      while ((pos=n.find("+")) != string::npos)
      {
            variablesName.push_back(n.substr(0,pos));
            n.erase(0,pos+1);
      }
      if (n.length()>=1)
      variablesName.push_back(n);
   }
    return variablesName;
}
//In case there are * operand ->check for warning/errors
bool checkErrorWithMultiplication(const vector<string> Variable_Names, map<string,string> VarAndTypes, map<int,string> &LinesAndMessages, int line_number, bool& is_Error_Warning)
{
    bool isError = false;
    //Go through each variable check if it's declared, if not => error
    for (int k=0; k<Variable_Names.size(); k++)
    {
        //Check if it's a string literal or number => it's a variable 
        if (!IsLiteralString(Variable_Names.at(k))&&!IsNumber(Variable_Names.at(k)))
        {
            //Not declared yet => error 
            if (VarAndTypes.count(Variable_Names.at(k))==0)
            {
                isError =true;
                LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
                is_Error_Warning = true;
                break;
            }
            //If already declared => check type => string => error
            else if (VarAndTypes[Variable_Names.at(k)]=="STRING") 
            {
                isError =true;
                LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
                is_Error_Warning = true;
                break;
            }
            //If it's declared =>check type => list => error
            else if (VarAndTypes[Variable_Names.at(k)]=="LIST" || VarAndTypes[Variable_Names.at(k)]=="LIST_INT"||VarAndTypes[Variable_Names.at(k)]=="LIST_STRING")
            {
                isError =true;
                LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
                is_Error_Warning = true;
                break;
            }
        }
        //Any of right hand side are literal string => error
        else if (IsLiteralString(Variable_Names.at(k)))
        {
            isError =true;
            LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
            is_Error_Warning = true;
        }
    }
    return isError;
}
//In case there are + operand ->check for warning/errors
void checkErrorWithAddition(const vector<string> Variable_Names, const string var_name, map<string,string> &VarAndTypes, map<int,string> &LinesAndMessages, int line_number, bool& is_Error_Warning)
{
    string String_type;
    string Int_type;
    string list_type, list_int, list_string;
    //Go through each variable check if it's declared, if not => error
    for (int k=0; k<Variable_Names.size(); k++)
    {
        if (!IsNumber(Variable_Names.at(k))&&!IsLiteralString(Variable_Names.at(k))) //variable
        {
            if (VarAndTypes.count(Variable_Names.at(k))==0) //check if variable is declared
            {
                LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
                is_Error_Warning = true;
                break;
            }
            //If declared in map => check any data type conflict
            else if (VarAndTypes[Variable_Names.at(k)]=="STRING")
            {
                String_type +="S";
            }
            else if (VarAndTypes[Variable_Names.at(k)]=="INT")
            {
                Int_type += "I";
            }
            else if (VarAndTypes[Variable_Names.at(k)]=="LIST")
            {
                list_type += "L";
            }
            else if (VarAndTypes[Variable_Names.at(k)]=="LIST_INT")
            {
                list_int += "I";
            }
            else if (VarAndTypes[Variable_Names.at(k)]=="LIST_STRING")
            {
                list_string += "S";
            }
        }
        //If it's a string Literal
        else if (IsLiteralString(Variable_Names.at(k)))
        {
            String_type += "S";
        }
        //if it's a number
        else if (IsNumber(Variable_Names.at(k)))
        {
            Int_type += "I";
        }
    }
    // There are more than one type of data in addition => error
    //There are string and int in addition => error
    if (String_type.length()>0 && Int_type.length()>0) 
    {
         LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
         is_Error_Warning = true;
    }
    //There are string and list in addition => error
    else if ((String_type.length()>0 && list_type.length()>0) || (String_type.length()>0 && list_int.length()>0) || (String_type.length()>0 && list_string.length()>0)) 
    {
        LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
        is_Error_Warning = true;
    }
    //There are int and list in addition => error
    else if ((Int_type.length()>0 && list_type.length()>0) || (Int_type.length()>0 && list_int.length()>0) || (Int_type.length()>0 && list_string.length()>0)) 
    {
        LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
        is_Error_Warning = true;
    }
    //Addition is valid so create a new STRING variable in map
    else if (String_type.length()== Variable_Names.size() && Int_type.length()==0 && var_name != "NO_VARIABLE_ON_LEFT")
    {
        if(VarAndTypes.count(var_name)==0)
        {
            VarAndTypes.insert(pair<string,string>(var_name, "STRING"));
        }
        //Check if the type is now changed => warning
        else if (VarAndTypes[var_name]!="STRING")
        {
            LinesAndMessages.insert(pair<int,string>(line_number,"#warning"));
            is_Error_Warning = true;
            //Update the type in map
            VarAndTypes[var_name] = "STRING";
        }
    }
    //Addition is valid so create a new INT variable in map
    else if (String_type.length()==0 && Int_type.length() == Variable_Names.size() && var_name != "NO_VARIABLE_ON_LEFT")
    {
         if(VarAndTypes.count(var_name)==0)
        {
            VarAndTypes.insert(pair<string,string>(var_name, "INT"));
        }
        //Check if the type is now changed => warning
        else if (VarAndTypes[var_name]!="INT")
        {
            LinesAndMessages.insert(pair<int,string>(line_number,"#warning"));
            is_Error_Warning = true;
            //Update the type in map
            VarAndTypes[var_name] = "INT";
        }
    }
    //In case all addition involved only list => different type of list => warning =>update type
    else if (String_type.length()==0 && Int_type.length() ==0)
    {
        if(list_int.length()>0 && list_string.length()>0)
        {
            LinesAndMessages.insert(pair<int,string>(line_number,"#warning"));
            is_Error_Warning = true;
        }
    }
}
//List operation
bool IsListOperation(const string str, map<string,string> &VarAndTypes, map<int,string> &LinesAndMessages, int line_number, bool& is_Error_Warning)
{
    bool hasListOperation = false;
    //Extract list name;
    size_t pos_dot = string::npos;
    string list_name, listElement;
    if ((pos_dot=str.find("."))!=string::npos)
    {
        list_name = str.substr(0,pos_dot);
    }

    //Check if there is append operation
    size_t pos_close_parentheses = string::npos;
    size_t pos_open_parentheses = string::npos;
    size_t pos_comma = string::npos;
    if(isAppendListOperation(str))
    {
            //Check if list is declared => error
        if(VarAndTypes.count(list_name)==0)
        {
            LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
            hasListOperation =true;
        }
        if ((pos_close_parentheses = str.find(")"))!=string::npos)
        {
            hasListOperation = true;
        }
        if ((pos_open_parentheses = str.find("("))!=string::npos)
        {
            listElement = str.substr(pos_open_parentheses+1,pos_close_parentheses-pos_open_parentheses-1);
        }
    }
    else if (isInsertListOperation(str))
    {
         if ((pos_close_parentheses = str.find(")"))!=string::npos)
        {
            hasListOperation = true;
        }
        if ((pos_comma = str.find(","))!=string::npos)
        {
            listElement = str.substr(pos_comma+1,pos_close_parentheses-pos_comma-1);
        }
    }
    //If list is not declared in map => error 
    if (hasListOperation)
    {
    if (VarAndTypes.count(list_name)==0) 
    {
        LinesAndMessages.insert(pair<int,string>(line_number,"#error"));
        is_Error_Warning = true;
    }
    //if it's in the map => TYPE: LIST => determined it's type based on append element
    else if (VarAndTypes[list_name]=="LIST")
    {
        VarAndTypes[list_name] = "LIST_" + checkVariableType(listElement);
    }
    //List is in map but already got a type => conflict => warning => update list type
    else if (VarAndTypes[list_name] == "LIST_INT" && checkVariableType(listElement)=="STRING")
    {
       LinesAndMessages.insert(pair<int,string>(line_number,"#warning"));
        is_Error_Warning = true;
       //update new list type
       VarAndTypes[list_name] = "LIST_STRING"; 
    }
    else if (VarAndTypes[list_name] == "LIST_STRING" && checkVariableType(listElement)=="INT")
    {
       LinesAndMessages.insert(pair<int,string>(line_number,"#warning"));
        is_Error_Warning = true;
       //update new list type
       VarAndTypes[list_name] = "LIST_INT"; 
    }
    }
    return hasListOperation;
}
bool isInsertListOperation(const string str)
{
    size_t pos_insert = string::npos;
    bool isInsert = false;
    if ((pos_insert = str.find(".insert"))!=string::npos)
    {
        isInsert = true;
    }
    return isInsert;
}
bool isAppendListOperation(const string str)
{
    size_t pos_append = string::npos;
    bool isAppend = false;
    if ((pos_append = str.find(".append"))!=string::npos)
    {
        isAppend = true;
    }
    return isAppend;
}
//Hanle condition If /Elif statements
void hanleConditionalStatement(string str, map<string,string> &VarAndTypes,map<int,string>&LineAndMessages, int line_number, bool &is_Error_Warning)
{
    //If it's an if statement 
    vector<string> variables_names;
    size_t pos_equal_equal = string::npos;
    if (str.find("elif")!=string::npos)
    str = RemoveElifKeyWord(RemoveSemiColon(str));
    else 
    str = RemoveIfKeyWord(RemoveSemiColon(str));
    //Extract variable name
    if ((pos_equal_equal=str.find("=="))!=string::npos || (pos_equal_equal=str.find("!="))!=string::npos || (pos_equal_equal=str.find("<="))!=string::npos ||(pos_equal_equal=str.find(">="))!=string::npos)
    {
        variables_names.push_back(str.substr(0,pos_equal_equal));
        variables_names.push_back(str.substr(pos_equal_equal+2,str.length()));
    }
    else 
    {
        if ((pos_equal_equal=str.find(">"))!=string::npos || (pos_equal_equal=str.find("<"))!=string::npos)
        {
            variables_names.push_back(str.substr(0,pos_equal_equal));
            variables_names.push_back(str.substr(pos_equal_equal+1,str.length()));
        }
    }
    //Check if they declared in map , if not => error
    for (int k=0; k<variables_names.size(); k++)
    {
         if (VarAndTypes.count(variables_names.at(k))==0)
         {
             LineAndMessages.insert(pair<int,string>(line_number,"#error"));
             is_Error_Warning= true;
         }
         //if it's declared => check type conflict. =>warning
         // In case of if statement there is only 2 variables on either side of the if statement
         else if(VarAndTypes[variables_names.at(0)]!=VarAndTypes[variables_names.at(1)])
        {
            LineAndMessages.insert(pair<int,string>(line_number,"#warning"));
            is_Error_Warning = true;
        }
    }
}
string RemoveIfKeyWord(string str)
{
    size_t pos_if = string::npos;
    if((pos_if=str.find("if"))!=string::npos)
    {
        str=str.substr(2,str.length());
    }
    return str;
}
string RemoveElifKeyWord(string str)
{
    size_t pos_elif = string::npos;
    if((pos_elif=str.find("elif"))!=string::npos)
    {
        str=str.substr(4,str.length());
    }
    return str;
}
string RemoveSemiColon(const string str)
{
    string removedSemiColon;
    for (int j=0; j<str.length();j++)
    {
        if(str[j]!=':')
        {
            removedSemiColon += str[j];
        }
    }
    return removedSemiColon;
}
bool checkForIndentation(const string str)
{
    size_t pos_indentation = string::npos;
    bool hasIndentation = false;
    for (int j=0; j<str.length(); j++)
    {
        if ((str.find("    "))!=string::npos)
            hasIndentation = true;
    }
    return hasIndentation;
}
string ExtractFunctionName(const string str)
{
    string functionName;
    size_t pos_def = string::npos;
    size_t pos_open_parentheses = string::npos;
    if ((pos_def=str.find("def"))!=string::npos && (pos_open_parentheses=str.find("("))!=string::npos)
    {   
  
        functionName = str.substr(pos_def+3,pos_open_parentheses-3);
    }
    return functionName;
}
vector<string> ExtractionFunctionVariables(string str)
{
    size_t pos_comma = string::npos;
    size_t pos_closed_parenthesis = string::npos;
    vector<string>Variables;
    string functionName = ExtractFunctionName(str);
    if(str.find("def")!=string::npos)
    {
    str.erase(0,3+ExtractFunctionName(str).length()+1);
    while((pos_comma=str.find(","))!=string::npos)
        {
            Variables.push_back(str.substr(0,pos_comma));
            str.erase(0,pos_comma+1);
        }
    }
    pos_closed_parenthesis = str.find(")");
    Variables.push_back(str.substr(0,pos_closed_parenthesis));
    return Variables;
}
bool isFuctionCall(const string statement)
{
    bool is_Function_call = false;
    if(statement.find("(")!=string::npos&&statement.find(")")!=string::npos && statement.find("def")==string::npos &&!isAppendListOperation(statement)&&!isInsertListOperation(statement))
    {
        is_Function_call = true;
    }
    return is_Function_call;
}
void HandleFunctionCall(string Function_call_statement,map<string,string>& VarAndTypes, map<int,string>&LineAndMessages,int line_number, bool &is_Error_Warning, map<string,int> FunctionNamesAndSize)
{
    vector<string> Function_call_variables;
    //Extract function name from function call
    string function_call_name, string_type, list_type, int_type;
    size_t pos_open_parenthesis = string::npos;
    size_t pos_comma = string::npos;
    size_t pos_closed_parenthesis = string::npos;
    Function_call_statement = RemoveSpaceFromString(Function_call_statement);
    pos_open_parenthesis = Function_call_statement.find("(");
    function_call_name = Function_call_statement.substr(0,pos_open_parenthesis);
    Function_call_statement.erase(0,pos_open_parenthesis+1);
    //if call a non-declared function => error
    if(VarAndTypes.count(function_call_name)==0)
    {
        LineAndMessages.insert(pair<int,string>(line_number,"#error"));
        is_Error_Warning = true;
        return;
    }
    else 
    {
        //Extract Function call variables
        while((pos_comma=Function_call_statement.find(","))!=string::npos)
        {
            Function_call_variables.push_back(Function_call_statement.substr(0,pos_comma));
            Function_call_statement.erase(0,pos_comma+1);
        }
        pos_closed_parenthesis = Function_call_statement.find(")");
        Function_call_variables.push_back(Function_call_statement.substr(0,pos_closed_parenthesis));
    }
    //Check number of parameters => error
    if(FunctionNamesAndSize[function_call_name]!=Function_call_variables.size())
    {
        LineAndMessages.insert(pair<int,string>(line_number,"#error"));
        is_Error_Warning = true;
        return;
    }
    else {
        if(VarAndTypes[function_call_name]=="FUNCTION_INT")
        {
        //If function is FUNCT_INT and any of the parameter is string => error
            for(int n=0; n<Function_call_variables.size(); n++)
            {
                if(checkVariableType(Function_call_variables.at(n))!="INT")
                {
                    LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                    is_Error_Warning = true;
                return;
                }
            }
        }
        //Function of STRING
        else if(VarAndTypes[function_call_name]=="FUNCTION_STRING")
        {
        //If function is FUNCT_INT and any of the parameter is string => error
            for(int n=0; n<Function_call_variables.size(); n++)
            {
                if(checkVariableType(Function_call_variables.at(n))!="STRING")
                {
                    LineAndMessages.insert(pair<int,string>(line_number,"#error"));
                    is_Error_Warning = true;
                    return;
                }
            }
        }
        //Function of LIST => check if every single element are of the same type
        else {
            for(int n=0; n<Function_call_variables.size(); n++)
            {
                if(checkVariableType(Function_call_variables.at(n))=="STRING")
                {
                    string_type += "S";
                }
                else if(checkVariableType(Function_call_variables.at(n))=="INT")
                {
                    int_type += "I";
                }
            }
            //If there is a conlict -> warning
            if(string_type.length()>0&&int_type.length()>0)
            {
                LineAndMessages.insert(pair<int,string>(line_number,"#warning"));
                is_Error_Warning = true;
            }
        }
    }
}