#!/usr/bin/python

# usage:
#   python3 keyword.py "input=input.txt;k=2;mostfrequent=Y;uppercase=N;output=keyword.txt"
# input = STRING
# k = INT
# mostfrequent = Y or N
# uppercase = Y or N
# output = STRING

#
# COSC 6345 - Fall 2019
# Assignment 3 - keyword.py
# Written by: 
#   Andrew Louie 0841797
#
# TODO:
# - REMOVE MUTATION ON ARG_ VARIABLES

import sys
import os



#################
# DEBUG & SYS DEFAULTS
#################
# Debug variables
ENABLE_WRITE = 1

# Python MAX recursion depth with some buffer space for functions above it
#   THE MORE WORDS in our input, the more we will call the
#   high level function add_list, using a smaller recursion limit gives
#   this function room
PY_MAX_REC = 10000
sys.setrecursionlimit(PY_MAX_REC)

# DEBUG LEVEL
# 0 - No Debug Output
# 1 - Summary Only (Input, Output, Major steps in a program)
# 2 - Verbose (Minor steps in a program)
# 3 - Hyper Verbose, you dont want this on (Debugging)
# 4+ - Output spam for developing purposes only
DEBUG = 3

# DBGPRINTLIST FORMAT
# 0 for comma form
# 1 for list form
DEBUG_PRINTLIST_FORMAT = 0

# Stopword file, set this to '' for no file
STOP_WORD_FILE = 'stopwords.txt'


#################
# USER DEFINED VARIABLES
#################
# input file, string only
#ARGS_INPUT_FILE = ''
# k value, must be integer
#ARGS_K = -1
# mostFrequent, boolean true or false for parsed Y or N
#ARGS_MOSTFREQUENT = True
# uppercase, boolean true or false for parsed Y or N
#ARGS_UPPERCASE = True
# output file, string only
#ARGS_OUTPUT_FILE = ''


#################
# Utility Functions
#################
# for printing debug text
def DBGPRINT(level, string):
  if DEBUG >= level:  print(string)

def DBGPRINTLIST(level, list):
  index = 0
  output_str = 'DBGList[..]: '
  if DEBUG >= level:
    for item in list:
      if DEBUG_PRINTLIST_FORMAT == 0:
        if(index < len(list) - 1):
          output_str += '(' + str(item) + "), "
        else:
          output_str += '(' + str(item) + ')'
      elif DEBUG_PRINTLIST_FORMAT == 1:
        DBGPRINT(level, "\t\t- DBGList[" + str(index) + "]: " + str(item))
      index = index + 1
    if DEBUG_PRINTLIST_FORMAT == 0:
      DBGPRINT(level, output_str)

def argMostFrequent():
  if ARGS_MOSTFREQUENT == 'Y' or ARGS_MOSTFREQUENT == 'y':
    return True
  elif ARGS_MOSTFREQUENT == 'N' or ARGS_MOSTFREQUENT == 'n':
    return False

def argUppercase():
  if ARGS_UPPERCASE == 'Y' or ARGS_UPPERCASE == 'y':
    return True
  elif ARGS_UPPERCASE == 'N' or ARGS_UPPERCASE == 'n':
    return False

def checkInput():
  global ARGS_INPUT_FILE, ARGS_K, ARGS_MOSTFREQUENT, \
          ARGS_UPPERCASE, ARGS_OUTPUT_FILE
  if len(ARGS_INPUT_FILE) <= 0:
    DBGPRINT(0, "checkInput(): Invalid INPUT_FILE parameter")
    return False
  if len(ARGS_OUTPUT_FILE) <= 0:
    DBGPRINT(0, "checkInput(): Invalid OUTPUT_FILE parameter")
    return False
  if int(ARGS_K) < 0:
    DBGPRINT(0, "checkInput(): Invalid K parameter")
    return False
  else:
    ARGS_K = int(ARGS_K)
  # check and convert ARGS_MOSTFREQUENT to bool
  if ARGS_MOSTFREQUENT != 'Y' and ARGS_MOSTFREQUENT != 'y' and \
     ARGS_MOSTFREQUENT != 'N' and ARGS_MOSTFREQUENT != 'n':
    DBGPRINT(0, "checkInput(): Invalid MOSTFREQUENT parameter")
    return False
  # check and convert ARGS_UPPERCASE
  if ARGS_UPPERCASE != 'Y' and ARGS_UPPERCASE != 'y' and \
     ARGS_UPPERCASE != 'N' and ARGS_UPPERCASE != 'n':
    DBGPRINT(0, "checkInput(): Invalid UPPERCASE parameter")
    return False
  # no issues
  return True


#################
# List Handling Functions
#################
def list_append(list, item):
  new_list = list + [item]
  return new_list

def list_remove(list, item):
  # no item
  if len(list) == 0:
    return list
  # match, last item
  elif list[0] == item and len(list) == 1:
    return []
  # no match, last item
  elif len(list) == 1:
    return [list[0]]
  # match
  elif list[0] == item:
    return list_remove(list[1:], item)
  # no match
  else:
    return [list[0]] + list_remove(list[1:], item)

#################
# Word Formatting Functions
#################
def is_lower(c):
  if c >= 'a' and c <= 'z':
    return True
  return False

def is_upper(c):
  if c >= 'A' and c <= 'Z':
    return True
  return False

def is_num(c):
  if c >= '0' and c <= '9':
    return True
  return False

def case_check(c):
  if argUppercase() == True and is_lower(c):
    return chr(ord(c) - 32)
  elif argUppercase() == False and is_upper(c):
    return chr(ord(c) + 32)
  return str(c)

def char_check(c):
  if c == '\'' or is_upper(c) or is_lower(c) or is_num(c):
    DBGPRINT(5, "char_check(): Found GOOD char [" + c + "]")
    return str(c)
  else:
    DBGPRINT(5, "char_check(): Found BAD char: [" + c + "]")
    return ''

# sets defined case
def case_word(word):
  if len(word) > 1:
    return case_check(word[0]) + case_word(word[1:])
  elif len(word) == 0:
    return ''
  else:
    return case_check(word[0])

# cleans words of punctuations that aren't apostrophes
def clean_word(word):
  # check if character is valid
  if len(word) > 1:
    return char_check(word[0]) + clean_word(word[1:])
  else:
    return char_check(word[0])

# checks if a word is a stop word, returns true if it is, false if it isnt
def is_stop_word(word, stop_list):
  # it is a stop word
  if word == stop_list[0]:
    return True
  # check next stop word
  if len(stop_list) > 1:
    return is_stop_word(word, stop_list[1:])
  # is not a stop word
  else:
    return False

def has_numbers(word):
  if len(word) > 1:
    if is_num(word[0]):
      DBGPRINT(5, "has_numbers(): Found NUM char (if) [" + word[0] + "]")
      return True
    else:
      DBGPRINT(5, "has_numbers(): Found CHAR char (1) [" + word[0] + "]")
      return has_numbers(word[1:])
  else:
    if is_num(word[0]):
      DBGPRINT(5, "has_numbers(): Found NUM char (else) [" + word[0] + "]")
      return True
    else:
      DBGPRINT(5, "has_numbers(): Found CHAR char (2) [" + word[0] + "]")
      return False

def is_valid_word(word, stop_list):
   if word == '':
     DBGPRINT(5, "is_valid_word(): null found, returning [" + word + "]")
     return ''
   elif has_numbers(word) == True:
     DBGPRINT(5, "is_valid_word(): Removing word with numbers [" + word + "]")
     return ''
   elif is_stop_word(word, stop_list):
     DBGPRINT(5, "is_valid_word(): Removing stop word [" + word + "]")
     return ''
   else:
     DBGPRINT(5, "is_valid_word(): Valid word [" + word + "]")
     return word

#################
# Input Functions
#################
# reads lines and generates a list
def read_input_lines(f):
  line = str(f.readline())
  if line:
    return [line] + read_input_lines(f)
  else:
    return ['']

# formats a word list by removing unncessary characters and changing the case
def format_word_list(word_list, stop_list):
  # set up words
  if len(word_list) > 0:
    DBGPRINT(5, "format_word_list() word_list[0]: " + str(word_list[0]))
    # clean word
    word_1 = clean_word(str(word_list[0]))
    # set casing on word
    DBGPRINT(5, "format_word_list() word_1: " + str(word_1))
    word_2 = case_word(word_1)
    # remove stop words
    DBGPRINT(5, "format_word_list() word_2: " + str(word_2))
    word_3 = is_valid_word(word_2, stop_list)
    DBGPRINT(5, "format_word_list() word_3: " + str(word_3))
  else:
    return ['']
  # process word list
  if len(word_list) > 1:
    return [word_3] + format_word_list(word_list[1:], stop_list)
  else:
    return [word_3]

# builds a raw word list with no edits, split by spaces/newlines
def build_raw_word_list(line, stop_list):
  raw_words = line[0].split()
  words = format_word_list(raw_words, stop_list)
  cleaned_words = list_remove(words, "")
  # no words
  if cleaned_words == None:
    if len(line) > 1:
      return build_raw_word_list(line[1:], stop_list)
    else:
      return ['']
  else:
    if len(line) > 1:
      # get next words and clean them
      next_words = build_raw_word_list(line[1:], stop_list)
      cleaned_next_words = list_remove(next_words, "")
      if len(cleaned_next_words) > 0:
        return cleaned_words + cleaned_next_words
      else:
        return cleaned_words
    else:
      return cleaned_words

# opens the input file for reading
def open_input_file(FILE_INPUT, stop_list):
  with open(FILE_INPUT, 'r') as f:
    input_lines = read_input_lines(f)
    words = build_raw_word_list(input_lines, stop_list)
    f.close()
    return words
  return ''

#################
# Output Functions
#################
# new output file
def new_output_file(FILE_OUTPUT):
  if ENABLE_WRITE == 1:
    # delete if it exists
    if FILE_OUTPUT is not None:
      if(os.path.exists(FILE_OUTPUT)):
        DBGPRINT(1, "\t\tDeleting old file " + str(FILE_OUTPUT))
        os.remove(FILE_OUTPUT)
    # now open it
    DBGPRINT(1, "\t\tCreating new file" + str(FILE_OUTPUT))
    f= open(FILE_OUTPUT,"w+")
    f.close()
  return

# write to output file
def write_output(string):
  if ENABLE_WRITE == 1:
    with open(ARGS_OUTPUT_FILE, "a+") as f:
      f.write(string)
    f.close()
  return

def write_2d(list_2d):
  if len(list_2d) >= 1:
    write_output(str(list_2d[0][0]) + " " + str(list_2d[0][1]) + "\n")
    write_2d(list_2d[1:])
  else:
    write_output(str(list_2d[0][0]) + " " + str(list_2d[0][1]) + "\n")
    return

#################
# Word List Functions
#################
# checks if an item is in the list
def in_list(list, item):
  # nothing in list
  if len(list) == 0:
    DBGPRINT(3, "\t" + item + " add to empty list")
    return False
  elif len(list) > 1:
    # not at this index
    if list[0] != item:
      DBGPRINT(5, "\t" + "in_list(): \"" + item + " != " + list[0] + "\" check next item")
      return in_list(list[1:], item)
    # found the item
    else:
      DBGPRINT(5, "\t" + item + " already in list")
      return True
  else:
    # not in the list
    if list[0] != item:
      DBGPRINT(5, "\t" + item + " add to list never found")
      return False
    # in list
    else:
      return True

# builds a list of unique words
# ADD COUNTING
def unique_words(word_list, unique_list):
  #DBGPRINT(3, "\tunique_words(): " + word_list[0])
  if len(word_list) > 1:
    # check next word
    if in_list(unique_list, word_list[0]) == True:
      return unique_words(word_list[1:], unique_list)
    # add word to the return
    else:
      return [word_list[0]] + unique_words(word_list[1:], unique_list + [word_list[0]])
  # final word
  else:
    # return list as is
    if in_list(unique_list, word_list[0]) == True:
      return [""]
    # return list with another unique word
    else:
      return [word_list[0]]

#def unique_words_lambda(word_list, unique_list)

# count word in word_list and return count
def count_words(word_list, word):
  if len(word_list) > 1:
    if word_list[0] == word:
      return 1 + count_words(word_list[1:], word)
    else:
      return count_words(word_list[1:], word)
  # final word
  else:
    if word_list[0] == word:
      return 1
    else:
      return 0

# builds a list of counts parallel to unique_words
def get_unique_counts(word_list, unique_list):
  if len(unique_list) > 1:
    return [[unique_list[0], count_words(word_list, unique_list[0])]] + get_unique_counts(word_list, unique_list[1:])
  # final word
  else:
    return [[unique_list[0], count_words(word_list, unique_list[0])]]

# returns values with the same K
def get_equals_K(list_2d, k):
  if len(list_2d) > 1:
    if list_2d[0][1] == k:
      return [list_2d[0]] + get_equals_K(list_2d[1:], k)
    else:
      return get_equals_K(list_2d[1:], k)
  # final word
  else:
    if list_2d[0][1] >= k:
      return [list_2d[0]]
    else:
      return []

# runs get_equals_K for a list of K[]
def get_all_K(list_2d, k_list):
  if len(k_list) > 1:
    return get_equals_K(list_2d, k_list[0]) + get_all_K(list_2d, k_list[1:])
  else:
    return get_equals_K(list_2d, k_list[0])

####################################################################
#-MIN/MAX FREQUENCY 
    
###################################################################
#Recursively through [index[[1] of 2D list to return max frequency
def get_max_frequency(list_2d, index=0, max_frequency=0):
    if(index>=len(list_2d)):
        return max_frequency
    else:
        if(list_2d[index][1]>max_frequency):
            max_frequency = list_2d[index][1]
        index = index +1
        return get_max_frequency(list_2d,index,max_frequency)
        
#Recursively through [index[[1] of 2D list to return min frequency
def get_min_frequency(list_2d, index=0, min_frequency=1001):
    if(index>=len(list_2d)):
        return min_frequency
    else:
        if(list_2d[index][1]<min_frequency):
            min_frequency = list_2d[index][1]
        index = index +1
        return get_min_frequency(list_2d,index,min_frequency)
#Looking for words whose frequency are equal to either max/min frequency
#Return a list of words matching k values in sorted order
def get_list_output_result(list_2d, list_output_result, index, k, frequency):
    if (k<=0):
        return list_output_result
    if (index ==len(list_2d)):
        index =0
        k = k-1
        if argMostFrequent()==True:
            frequency = frequency -1
        else:
            frequency = frequency + 1        
    else:
        if (list_2d[index][1]==frequency):
            list_output_result = list_append(list_output_result, list_2d[index])
        index = index + 1
    return get_list_output_result(list_2d, list_output_result, index, k, frequency)

#In case of a ties frequency -> sort the list alphabetically using merge sort
#Recursive merge the list 
def recursively_iterate_list(i,j,k,Left, Right, list_2d):
    if (i<len(Left)) and (j<len(Right)):
        if Left[i]<Right[j]:
            list_2d[k] = Left[i]
            i+=1
        else:
            list_2d[k] = Right[j]
            j+=1
        k+=1
        return recursively_iterate_list(i,j,k,Left,Right,list_2d)
    return list_2d,k,i,j
#Merge left/right on the remaining L/R side of the list
def recursively_list_append(i,k, List, list_2d):
    if (i<len(List)):
        list_2d[k] = List[i]
        i+=1
        return recursively_list_append(i,k+1,List, list_2d)
    else:
        return list_2d
#Recursive MergeSort
def mergeSort_recursion(list_2d):
    if len(list_2d)>1:
        m = len(list_2d)//2
        Left = list_2d[:m]
        Right = list_2d[m:]
        mergeSort_recursion(Left)
        mergeSort_recursion(Right)
        i = j = k = 0
        list_2d,k,i,j = recursively_iterate_list(i,j,k,Left, Right, list_2d)
        recursively_list_append(i,k,Left,list_2d)
        recursively_list_append(j,k,Right,list_2d)
####################################################################
#------------------------------------------------------------------#
# START MAIN
#------------------------------------------------------------------#
####################################################################

#################
# Print Script Defaults
#################
DBGPRINT(1, "================================")
DBGPRINT(1, "SCRIPT DEBUGGING DEFAULTS\n")
DBGPRINT(1, "\t- INFO: DBGPRINT level: " + str(DEBUG))
# Writing
if ENABLE_WRITE == 1:
  DBGPRINT(1, "\t- INFO: Output file writing ENABLED")
else:
  DBGPRINT(1, "\t- WARNING: Output file writing DISABLED")
# Recursion depths
DBGPRINT(1, "\t- INFO: Max Recursion depth set: " + str(PY_MAX_REC))
DBGPRINT(1, "================================")


#################
# Read user input
#################
DBGPRINT(1, "================================")
DBGPRINT(1, "PARSING USER INPUT\n")
## load user arguments ##
try:
    #argv[0] = this python's file name
    INPUT_ARGS = str(sys.argv[1])
except IndexError:
  DBGPRINT(0, '\tUSER INPUT ERROR: Missing cmd line argument')
  sys.exit(0)

# parse the arguments
# "input=input.txt;k=2;output=topwords.txt"
INPUT_ARGS = INPUT_ARGS.split(';')
if len(INPUT_ARGS) == 5:
  # parse input
  ARGS_INPUT_FILE = str(INPUT_ARGS[0][INPUT_ARGS[0].find('=')+1:])
  ARGS_K = str(INPUT_ARGS[1][INPUT_ARGS[1].find('=')+1:])
  ARGS_MOSTFREQUENT = str(INPUT_ARGS[2][INPUT_ARGS[2].find('=')+1:])
  ARGS_UPPERCASE = str(INPUT_ARGS[3][INPUT_ARGS[3].find('=')+1:])
  ARGS_OUTPUT_FILE = str(INPUT_ARGS[4][INPUT_ARGS[4].find('=')+1:])
  # check input
  if checkInput():
    DBGPRINT(1, "\t- INPUT FILE: " + str(ARGS_INPUT_FILE))
    DBGPRINT(1, "\t- K VALUE: " + str(ARGS_K))
    DBGPRINT(1, "\t- MOSTFREQUENT: " + str(ARGS_MOSTFREQUENT))
    DBGPRINT(1, "\t- UPPERCASE: " + str(ARGS_UPPERCASE))
    DBGPRINT(1, "\t- OUTPUT FILE: " + str(ARGS_OUTPUT_FILE))
    DBGPRINT(1, "\nUser input parsed successfully")
  else:
    DBGPRINT(0, "\nPlease retry with a valid arguments:")
    DBGPRINT(0, "ex: python3 keyword.py \"input=input.txt;k=2;mostfrequent=Y;uppercase=N;output=keyword.txt\"")
    sys.exit(0)
else:
  DBGPRINT(0, '\nUSER_INPUT_ERROR: Improper argument syntax or not enough arguments ' + str(len(INPUT_ARGS)))
  sys.exit(0)

DBGPRINT(1, "================================\n")



#################
# Processing
#################
DBGPRINT(1, "================================")
DBGPRINT(1, "PROCESSING CORPUS\n")

# generate new output file
DBGPRINT(1, "\t- Output file setup")
new_output_file(ARGS_OUTPUT_FILE)

# read stop word file
if STOP_WORD_FILE:
  DBGPRINT(1, "\n\t- Opening stopword file")
  stop_words = open_input_file(STOP_WORD_FILE, [''])
  DBGPRINT(1, "\tStop Words Read: " + str(len(stop_words)))
  #DBGPRINTLIST(3, stop_words)

# Read input file
DBGPRINT(1, "\n\t- Opening input file")
input_words = open_input_file(ARGS_INPUT_FILE, stop_words)
DBGPRINT(1, "\tInput Words Read after removing stop words: " + str(len(input_words)))
#DBGPRINTLIST(3, input_words)

# Get uniques
DBGPRINT(1, "\n\t- Unique words")
unique_1 = unique_words(input_words, [])
unique = list_remove(unique_1, "")
#DBGPRINTLIST(3, unique)
DBGPRINT(1, "\tUnique Words in Input Words: " + str(len(unique)))

# Get count
DBGPRINT(1, "\n\t- Unique word counts")
unique_2d_list = get_unique_counts(input_words, unique)
DBGPRINT(1, "\tUnique Words & Counts in Input Words: " + str(len(unique_2d_list)))
#DBGPRINTLIST(3, unique_2d_list)

# TODO
# Find K most or least frequent numbers
# use ARGS_K here and argMostFrequent() to determine which K values
# to check
#Sort the list alphabetically
mergeSort_recursion(unique_2d_list)
max_frequency = get_max_frequency(unique_2d_list)
min_frequency = get_min_frequency(unique_2d_list)
#if most frequency, assign frequency to max_frequency
if argMostFrequent() == True:
    frequency = max_frequency
else:
    frequency = min_frequency
#Initialize an empty list
list_output_result = []
list_output_result = get_list_output_result(unique_2d_list,list_output_result, 0, ARGS_K,frequency)
#===================================#
#list_output_result is the final sorted list result
write_2d(list_output_result)
k_list = [4, 8]
# Get equals or over ARGS_K
DBGPRINT(1, "\n\t- Find words equal to K - unordered ")
unique_k_unordered = get_all_K(unique_2d_list, k_list)
DBGPRINT(1, "\tWords specified by input parameters" + str(ARGS_K) + " -> " + str(len(unique_k_unordered)))
DBGPRINTLIST(3, unique_k_unordered)
# TODO
# sort the unordered K list

#Output to file
DBGPRINT(1, "\n\t- Writing to output file")
if len(unique_k_unordered) > 0:
  write_2d(unique_k_unordered)
else:
  write_output("NO WORDS FOUND")
DBGPRINT(1, "\t- Write to output file complete")

DBGPRINT(1, "\nSCRIPT ENDING")
DBGPRINT(1, "================================")
