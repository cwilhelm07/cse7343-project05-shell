#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 100
#define MAX_FILE_SIZE   10000
#define MAX_COMMANDS    3

void parseUserInput(char *);
void processFirstCmd(char **, int);
void processTypeCmd(char *);
void processCopyCmd(char *, char *);
void processDeleteCmd(char *);
void processProgramExecution(char *);
void showErrorMessage(char *);

int main()
{
   char *userInput;
   userInput = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
   
   printf("Here is my shell:\n");
   while ( 1 )
   {
      printf("$ ");
      fgets(userInput, MAX_LINE_LENGTH, stdin);
      parseUserInput(userInput);
   }
}

void parseUserInput(char *userInput)
{
   int i = 0;
   char *parsedData;
   parsedData = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
   char *cmd[MAX_COMMANDS];
   
   parsedData = strtok(userInput, " ");
   while ( parsedData != NULL && i < MAX_COMMANDS )
   {
      cmd[i] = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
      cmd[i] = parsedData;
      parsedData = strtok(NULL, " ");
      i++;
   }
   
   // Check for more than 2 arguments after a command
   if ( parsedData != NULL )
   {
      showErrorMessage("MAX ARGS");
      return;
   }
   
   processFirstCmd(cmd, i-1);
}

void processFirstCmd(char **cmd, int numArgs)
{
   if ( (strcmp(cmd[0], "exit") == 0 || strcmp(cmd[0], "exit\n") == 0) &&
         numArgs == 0 )
   {
      exit(0);
   }
   else if ( strcmp(cmd[0], "type") == 0 && numArgs == 1 )
   {
      processTypeCmd(cmd[1]);
   }
   else if ( strcmp(cmd[0], "copy") == 0 && numArgs == 2 )
   {
      processCopyCmd(cmd[1], cmd[2]);
   }
   else if ( strcmp(cmd[0], "delete") == 0 && numArgs == 1)
   {
      processDeleteCmd(cmd[1]);
   }
   else if ( numArgs == 0 )
   {
      // Execute file if it exist
      processProgramExecution(cmd[0]);
   }
   else
   {
      showErrorMessage("INVALID CMD");
   }
}

void processTypeCmd(char *secondCmd)
{
   int i;
   // Remove the newline character from the filename
   secondCmd[strlen(secondCmd)-1] = 0;
   // Open the file for reading only
   int fileDescriptor = open(secondCmd, O_RDONLY);
   struct stat fileInfo;
   // Get the file info including the file size
   fstat(fileDescriptor, &fileInfo);
   // Reserve space on the heap for the file contents
   char *fileContents = malloc(fileInfo.st_size + 1);

   if ( fileDescriptor < 0 )
   {
      showErrorMessage("FILE DNE");
      return;
   }
   
   // Read the file
   if ( read(fileDescriptor, fileContents, fileInfo.st_size) < 0 )
   {
      showErrorMessage("READ ERR");
      return;
   }
   
   // Output the file to the console
   for ( i = 0; i < fileInfo.st_size; i++ )
   {
      printf("%c", fileContents[i]);
   }
   
   printf("\n");
   
   // Close the file
   if ( close(fileDescriptor) < 0 )
   {
      showErrorMessage("CLOSE ERR");
      return;
   }
}

void processCopyCmd(char *secondCmd, char *thirdCmd)
{
   // Remove the newline character from the filename
   thirdCmd[strlen(thirdCmd)-1] = 0;
   // Open the file for reading only
   int readFileDescriptor = open(secondCmd, O_RDONLY);
   // Create or open the file for reading and writing
   int writeFileDescriptor = open(thirdCmd, O_CREAT | O_RDWR);
   struct stat fileInfo;
   // Get the file info including the file size
   fstat(readFileDescriptor, &fileInfo);
   // Reserve space on the heap for the file contents
   char *fileContents = malloc(fileInfo.st_size + 1);
   
   if ( readFileDescriptor < 0 )
   {
      showErrorMessage("FILE DNE");
      return;
   }
   
   // Read the file
   if ( read(readFileDescriptor, fileContents, fileInfo.st_size) < 0 )
   {
      showErrorMessage("READ ERR");
      return;
   }

   if ( writeFileDescriptor < 0 )
   {
      showErrorMessage("CREATE ERR");
      return;
   }
   
   // Write the file with the contents of the read file
   if ( write(writeFileDescriptor, fileContents, fileInfo.st_size) < 0 )
   {
      showErrorMessage("WRITE ERR");
      return;
   }
   
   // Close both files
   if ( close(readFileDescriptor) < 0 || close(writeFileDescriptor) < 0 )
   {
      showErrorMessage("CLOSE ERR");
      return;
   }
}

void processDeleteCmd(char *secondCmd)
{
   // Remove the newline character from the filename
   secondCmd[strlen(secondCmd)-1] = 0;
   // Remove the file
   if ( remove(secondCmd) < 0 )
   {
      showErrorMessage("DELETE ERR");
      return;
   }
}

void processProgramExecution(char *firstCmd)
{
   char *cmd[] = {(char *)0, '\0', '\0'};
   char *temp = malloc(MAX_LINE_LENGTH * sizeof(char));
   cmd[0] = malloc(MAX_LINE_LENGTH * sizeof(char));

   // Remove the newline character from the filename
   firstCmd[strlen(firstCmd)-1] = 0;
 
   strcpy(temp, "./");
   strcat(temp, firstCmd);
   cmd[0] = temp;
   
   if ( execvp(cmd[0], cmd) < 0 )
   {
      showErrorMessage("INVALID CMD");
      return;
   }
}

void showErrorMessage(char *msgType)
{
   if ( strcmp(msgType, "MAX ARGS") == 0 )
      printf("To many arguments entered\n");
   else if ( strcmp(msgType, "INVALID CMD") == 0 )
      printf("Please enter a valid command\n");
   else if ( strcmp(msgType, "FILE DNE") == 0 )
      printf("File does not exist\n");
   else if ( strcmp(msgType, "CREATE ERR") == 0 )
      printf("Unable to create file\n");
   else if ( strcmp(msgType, "READ ERR") == 0 )
      printf("Unable to read file\n");
   else if ( strcmp(msgType, "WRITE ERR") == 0 )
      printf("Unable to write file\n");
   else if ( strcmp(msgType, "CLOSE ERR") == 0 )
      printf("Unable to close file\n");
   else if ( strcmp(msgType, "DELETE ERR") == 0 )
      printf("Unable to remove file\n");
}