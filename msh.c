/*
Name: Rikki Vanrey Martin
ID: 1001844256
*/

// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports four arguments

#define MAX_HISTORY_SUPPORTED 15 // Maximum history Mav shell supports


// Function is used to copy the command used to the history array
// It returns the number back to the counter if its under 15, but if its over then it resets to 0
int storeHistory(char history[MAX_HISTORY_SUPPORTED][100], int counter, char *command)
{
  if(counter < MAX_HISTORY_SUPPORTED)
  {
    strcpy(history[counter],command);
    return counter;
  }
  else
  {
    strcpy(history[0],command);
    return 0;
  }
}
  

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  
  // Created 2D char array to store commands that are going to be compared with the input.
  char commands[5][10] = {"quit","exit","cd","history","showpids"}; 
  
  // Counter for history, initialized at 0. Every time a command is used the counter will go up.
  int history_counter = 0; 
  
  // Created 2D char array to store the history of commands that have been used in the past.
  char command_history[MAX_HISTORY_SUPPORTED][100]; 
  
  // Created array to store the pid of the processes.
  pid_t pidhistory[10];
  
  // Counter for showpid, initialized at 0. Everytime a process is found, the counter will go up.
  int pid_counter = 0;
  

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    
    // Runs if loop if first token is not empty. If it's empty it just loops back until it isn't empty.
    if(token[0] != NULL)
    {

      // Shell program exits if "quit" or "exit" is ran
      if(strcmp(token[0],commands[0])== 0 || strcmp(token[0],commands[1]) == 0) 
      {
        // Exits the program with code 0
        exit(0);
      }
      
      // Shell program changes directory if "cd" is ran     
      else if(strcmp(token[0],commands[2]) == 0) 
      {
        chdir(token[1]); // Changes directory to the 2nd argument in the input
        history_counter = storeHistory(command_history,history_counter,token[0]);
        history_counter++;
      }
      
      // Shell program shows the history of commands used when "history" is ran
      else if(strcmp(token[0],commands[3]) == 0) 
      {
        history_counter = storeHistory(command_history,history_counter,token[0]);
        history_counter++;
        
        // For loop to iterate through the command history array and print its contents
        for(int i = 0; i < history_counter; i++)
        {
          printf("%d: %s\n",i+1,command_history[i]);
        }
        
      }
      
      // Shell programs shows the history of PIDs when the "showpids" command is ran
      else if(strcmp(token[0],commands[4]) == 0) 
      {
        history_counter = storeHistory(command_history,history_counter,token[0]);
        history_counter++;
        
        // For loop to iterate through the pid history array and print its contents
        for(int j = 0; j < pid_counter; j++)
        {
          printf("%d: %d\n",j+1,pidhistory[j]);
        }
      }
      
      // If the string compare doesn't match up and equal 0 to any of the string in the commands array,
      // this loop runs. It checks to see if runs a command that isn't supported and adds it to history.
      else 
      {
        history_counter = storeHistory(command_history,history_counter,token[0]);
        history_counter++;
        pid_t pid_child = fork();
        
        // Stores pid into the array and adds one to the counter.
        // Used for showpid command to be able to for loop print it
        pidhistory[pid_counter] = pid_child;
        pid_counter++;
        if(pid_child == 0)
        {
          if(execvp(token[0],token) == -1)
          {
            printf("%s: Command not found.\n\n",token[0]);
            printf("msh> ");
            return 0;
          }
          else
          {
            execvp(token[0],token);
          }
        }
      }
    }
        
    free( head_ptr );

  }
  return 0;
  // e2520ca2-76f3-11ec-90d6-0242ac120003
}