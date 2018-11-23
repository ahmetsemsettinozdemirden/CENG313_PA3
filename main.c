#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
        input[strcspn(input, "\n")] = 0;

        if(*input == '\0' || *input == '\n')
            continue;

        // add command to history
        addHistoryNode(input);

        // tokenize
        for (int i = 0; i < 10; ++i) {
            tokenizedInput[i] = NULL;
        }
        char copyInput[100];
        strcpy(copyInput, input);
        tokenizerCounter = 0;
        char* token = strtok(input, " ");
        while(token != NULL && tokenizerCounter < 10) {
            tokenizedInput[tokenizerCounter] = token;
            token = strtok(NULL, " ");
            tokenizerCounter++;
        }

        // false is represented by 0
        // TODO: builtin as a background task
        if(isBuiltIn(tokenizedInput) != 0) { // TODO: how to do pipe `|` operation
            execute(tokenizedInput);
        } else {
            pid_t pid = fork();

            if(pid == 0) {

                // background process
                if(*tokenizedInput[tokenizerCounter - 1] == '&') {
                    tokenizedInput[tokenizerCounter - 1] = NULL;
                }

                execvp(tokenizedInput[0], tokenizedInput);
            } else if(*tokenizedInput[tokenizerCounter - 1] != '&'){
                wait(NULL);
            } else {
                printf("pid: %d", pid);
            }
        }
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

    int shiftItems = histCounter - 11;
    while(shiftItems > 0 && node != NULL) {
        node = node->next;
        shiftItems--;
    }

    while (node != NULL) {
        setbuf(stdout, 0);
        printf("[%d] %s\n", node->index, node->command);
        node = node->next;
    }
}

void findloc() {
    // TODO: findloc
    setbuf(stdout, 0);
    printf("'findloc' command executed.");
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