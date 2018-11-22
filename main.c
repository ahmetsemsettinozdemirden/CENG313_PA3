#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

// type definitions
#define a 5

typedef struct hist_node HistNode;

// constants
const int BUILTIN_COMMANDS_SIZE = 5;
const char* BUILTIN_COMMANDS[a] = { "cd", "dir", "history", "findloc", "bye" }; //TODO: check a

// global variables
char input[100];
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
        setbuf(stdout, 0);
        printf("\nbestshellever>");

        // get line
        fgets(input, 100, stdin);

        if(*input == '\n'){ //Check if user entered newline
                continue;
        }

        //printf("%s",input);
        input[strcspn(input, "\n")] = 0;

        // add command to history
        addHistoryNode(input);

        // tokenize
        char copyInput[100];
        strcpy(copyInput, input);
        int i = 0;
        char* token = strtok(input, " ");
        while(token != NULL && i < 10) {
            tokenizedInput[i] = token;
            token = strtok(NULL, " ");
            i++;
        }

        // false is represented by 0
        // TODO: builtin as a background task
        if(isBuiltIn(tokenizedInput) != 0) { // TODO: how to do pipe `|` operation
            execute(tokenizedInput);
        } else {
//            fork();
//            if(child) {
//
//            } else if (not a background process) {
//                wait for the child
//            }
        }

//        exit(0);
    }

}

int isBuiltIn(char* parsedInput[]) {
    // TODO: pipe `|` operation
    char* command = parsedInput[0];
    for (int i = 0; i < BUILTIN_COMMANDS_SIZE; ++i)
        if (strcmp(command, BUILTIN_COMMANDS[i]) == 0)
            return 1;
    return 0;
}

void execute(char* parsedInput[]) {
    // TODO: pipe `|` operation
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

void history() {
    HistNode *node = histList;
    while (node != NULL) {
        setbuf(stdout, 0);
        printf("[%d] %s\n", node->index, node->command);
        node = node->next;
    }
}

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
        setbuf(stdout, 0);
        printf("\n'findloc' command executed.");
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


void bye() {
    setbuf(stdout, 0);
    printf("bye");
    exit(0);
}

HistNode* createHistNode(char* command) {
    HistNode* newNode = malloc(sizeof(HistNode));
    newNode->index = histCounter++;
    newNode->command = malloc(strlen(command) + 1);
    strcpy(newNode->command, command);
    newNode->next = NULL;
    return newNode;
}

// Insert a new task into task queue
void addHistoryNode(char* command) {
    if(histList == NULL) {
        histList = createHistNode(command);
    } else {
        HistNode* prevNode = histList;
        while (prevNode->next != NULL) {
            prevNode = prevNode->next;
        }
        prevNode->next = createHistNode(command);
    }
}