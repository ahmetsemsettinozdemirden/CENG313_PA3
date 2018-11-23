#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// type definitions
typedef struct hist_node HistNode;

// constants
const int BUILTIN_COMMANDS_SIZE = 5;
const char* BUILTIN_COMMANDS[BUILTIN_COMMANDS_SIZE] = { "cd", "dir", "history", "findloc", "bye" };

// global variables
char input[100];
int tokenizerCounter;
char* tokenizedInput[10];
int histCounter = 1;
HistNode* histList;

// functions
char** parseInput(char input[100]);
int isBuiltIn(char* parsedInput[]);
void execute();
void cd();
void dir();
void history();
void findloc();
void bye();
void addHistoryNode(char* command);
void checkIfExistAndExecutable(char* directory, char* command);

struct hist_node {
    int index;
    char* command;
    struct hist_node* next;
};

int main() {

    while(1) {
        // IDE fix
        setbuf(stdout, 0);
        printf("\nbestshellever>");

        // get line
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0;

        // immediate enter fix
        if(*input == '\0' || *input == '\n')
            continue;

        // add command to history
        addHistoryNode(input);

        // tokenize input
        for (int i = 0; i < 10; ++i) {
            tokenizedInput[i] = NULL;
        }
        // since tokenizer consume data in input array we need to copy save it in another array
        char copyInput[100];
        strcpy(copyInput, input);
        tokenizerCounter = 0;
        // string tokenization with " " empty space
        char* token = strtok(input, " ");
        while(token != NULL && tokenizerCounter < 10) {
            tokenizedInput[tokenizerCounter] = token;
            token = strtok(NULL, " ");
            tokenizerCounter++;
        }

        // false is represented by 0
        if(isBuiltIn(tokenizedInput) != 0) { // TODO: how to do pipe `|` operation
            execute(tokenizedInput);
        } else {
            // create new process
            pid_t pid = fork();

            // if child
            if(pid == 0) {

                // remove background process argument
                if(*tokenizedInput[tokenizerCounter - 1] == '&') {
                    tokenizedInput[tokenizerCounter - 1] = NULL;
                }

                // run
                execvp(tokenizedInput[0], tokenizedInput);
            } else if(*tokenizedInput[tokenizerCounter - 1] != '&'){
                // wait for the completion of child execution
                wait(NULL);
            } else {
                // print background process id
                printf("pid: %d", pid);
            }
        }
    }

}

// look for the built-in commands
int isBuiltIn(char* parsedInput[]) {
    char* command = parsedInput[0];
    for (int i = 0; i < BUILTIN_COMMANDS_SIZE; ++i)
        if (strcmp(command, BUILTIN_COMMANDS[i]) == 0)
            return 1;
    return 0;
}

// execute built-in command
void execute(char* parsedInput[]) {
    char* command = parsedInput[0];
    if (strcmp(command, "cd") == 0) {
        cd();
    } else if (strcmp(command, "dir") == 0){
        dir();
    } else if (strcmp(command, "history") == 0){
        history();
    } else if (strcmp(command, "findloc") == 0){
        findloc();
    } else if (strcmp(command, "bye") == 0){
        bye();
    } else {
        perror("command is not builtin.");
    }
}

// change directory
void cd() {

    // usage: cd <directory>
    char* directory = tokenizedInput[1];

    // get home directory if <directory> does not exist
    if(directory == NULL || strcmp(directory, "") == 0) {
        directory = getenv("HOME");
    }

    // change directory
    chdir(directory);

    // get current absolute path
    char cwd[100];
    getcwd(cwd, sizeof(cwd));

    // update PWD
    setenv("PWD", cwd, 1);
}

// print current directory
void dir() {
    char cwd[100];
    if(getcwd(cwd, sizeof(cwd))) {
        setbuf(stdout, 0);
        printf(cwd);
    } else {
        setbuf(stdout, 0);
        perror("'dir' command couldn't executed.");
    }
}

// print last 10 executed commands
void history() {
    HistNode *node = histList;

    // skip until last 10 command
    int skipItems = histCounter - 11;
    while(skipItems > 0 && node != NULL) {
        node = node->next;
        skipItems--;
    }

    // print items
    while (node != NULL) {
        setbuf(stdout, 0);
        printf("[%d] %s\n", node->index, node->command);
        node = node->next;
    }
}

// find location of the executable
void findloc() {
    char* path = getenv("PATH");
    char* command = tokenizedInput[1];
    if(command == NULL){
        printf("You should enter some command");
        return;
    }
    //printf("Path %s", path);

    char* copyPath[100];
    strcpy(copyPath, path);
    char* token = strtok(copyPath, ":"); //Can't call findloc twice properly if we don't do this.

    //Check current directory
    char cwd[100];
    char* directory = getcwd(cwd, sizeof(cwd));
    checkIfExistAndExecutable(directory,command);

    //check PATH
    while(token != NULL ) {

        directory = token;
        checkIfExistAndExecutable(directory,command);
        token = strtok(NULL, ":");
    }
}

void checkIfExistAndExecutable(char* directory, char* command){
    DIR *dir;
    struct dirent *ent= NULL;
    struct stat sb;
    if((dir = opendir(directory))!= NULL) {
        while((ent = readdir (dir)) != NULL) {
            if(strcmp(ent->d_name,command) == 0 ){
                strcat(directory,"/");
                strcat(directory,ent->d_name);

                if(stat(directory, &sb) == 0 && sb.st_mode & S_IXUSR)
                    printf("%s\n", directory );
                else{
                    printf("Found but not executable");
                }
            }
            else{
                //printf("cant open %s\n", dir);
            }

        }
        closedir(dir);
    }
}

// exit from command line
void bye() {
    setbuf(stdout, 0);
    printf("bye");
    exit(0);
}

HistNode* createHistNode(char* command) {
    HistNode* newNode = malloc(sizeof(HistNode));
    newNode->index = histCounter++;
    newNode->command = malloc(strlen(command) + 1); // create new memory space for command string
    strcpy(newNode->command, command); // copy the command string to the node
    newNode->next = NULL;
    return newNode;
}

// Insert a new task into task queue
void addHistoryNode(char* command) {
    if(histList == NULL) {
        histList = createHistNode(command); // create list if the list is null
    } else {
        HistNode* prevNode = histList;
        while (prevNode->next != NULL) {
            prevNode = prevNode->next; // skip to the last item
        }
        prevNode->next = createHistNode(command); // connect newly created node to list
    }
}