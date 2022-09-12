#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_SUB_COMMANDS 5
#define MAX_ARGS 10
#define BUFFER_SIZE 32768

static char *cmd0[] = {"ls", "-l", NULL};
static char *cmd1[] = {"wc", NULL};
static char *cmd2[] = {"/bin/ls", NULL};
extern char **environ;
char *filename = " ";
char cwd[PATH_MAX];
int flag = 1; 
int num_pipes; 
int option = 0;
char variable[50]; 
char value[50]; 
char *dirName = " "; 

struct SubCommand{
    char *line;
    char *argv[MAX_ARGS];
};

struct Command{
    struct SubCommand sub_commands[MAX_SUB_COMMANDS];
    int num_sub_commands;
    int background;
};

void emptyCommand();
void ReadCommand(char *line, struct Command *command);
void Commands(struct Command *command);
void PrintCommand(struct Command *command);
void ReadArgs(char *in, char **argv, char *delim);
void myCat();
void redIO();
void showDirectories();
void environmentVariable();
void makeDirectories(); 

int main() {
    struct Command comm;
    char line[200] = " ";
    while (1){
        there:
        printf("%s $ ", getcwd(cwd, PATH_MAX));
        fflush(stdout); 
        fgets(line, sizeof(line), stdin);
        if (line[0] == 0x0A){
            printf("[%d] finished\n", (int) getpid());
        }
        ReadCommand(line, &comm);
        Commands(&comm);
        if (flag == 0){
            flag = 1;
            goto there;
        }
        PrintCommand(&comm);
    }
    return EXIT_SUCCESS;
}

void ReadCommand(char *line, struct Command *command) {
    char *argv[MAX_SUB_COMMANDS];
    char *templine = strdup(line);
    char *tempsubline;
    memset(command, 0, sizeof(struct Command));
    ReadArgs(templine, argv, "|\n");
    int i;
    for (i = 0; i < MAX_SUB_COMMANDS && argv[i]; i++){
        tempsubline = strdup(argv[i]);
        command->sub_commands[i].line = tempsubline;
        ReadArgs(tempsubline, command->sub_commands[i].argv, " \n");
        command->num_sub_commands++;
    }
    free(templine);
    num_pipes = command->num_sub_commands;
}

void Commands(struct Command *command){
    int i ,j =0; 
    for (i = 0 ; i < command->num_sub_commands; i++){
        while(command->sub_commands[i].argv[j] != NULL){ 
            if (! strcmp(command->sub_commands[i].argv[j], "&")){
                printf("[%d]\n", (int) getpid());
                flag = 0;
            }
            if (! strcmp(command->sub_commands[i].argv[j], "cd")){
                int chd = chdir(command->sub_commands[i].argv[j+1]);
                if (chd < 0){
                    printf("cd command not successfuly executed\n");
                }
                if (! strcmp(command->sub_commands[i].argv[j], "..")){
                    chdir("..");
                    printf("%s \n", getcwd(cwd, 100));
                }
            } else if (! strcmp(command->sub_commands[i].argv[j], "cat")){
                filename = command->sub_commands[i].argv[j+1];
                myCat();
            } else if (! strcmp(command->sub_commands[i].argv[j], "ls") && command->sub_commands[i].argv[1] == NULL){
                showDirectories();
                flag = 0;
            }
            if (! strcmp(command->sub_commands[i].argv[j], "<")){
                filename = command->sub_commands[i].argv[j+1];
                option = 1;
                redIO();
            } else if (! strcmp(command->sub_commands[i].argv[j], ">")){
                filename = command->sub_commands[i].argv[j+1];
                if (filename == NULL){
                    printf("type in file for output redirection\n");
                }
                option = 2;
                redIO();
            }
            if (! strcmp(command->sub_commands[i].argv[j], "sleep")){
                int sleep_duration = atoi(command->sub_commands[i].argv[j+1]);
                sleep(sleep_duration); 
            }
            if (! strcmp(command->sub_commands[i].argv[j], "mkdir")){
                dirName = command->sub_commands[i].argv[j+1];
                makeDirectories();
            }
            if (strchr(command->sub_commands[i].argv[i], '=')){
                strcpy(variable, strtok(command->sub_commands[0].argv[0], "="));
                strcpy(value, strtok(NULL, "="));
                environmentVariable();
                flag = 0;
            }
            j++;
        }
        j= 0;
    }
}

void ReadArgs(char *in, char **argv, char *delim) {
    int argc = 0;
    char *arg;
    arg = strtok(in, delim);
    while (arg && argc < (MAX_ARGS-1)){
        argv[argc++] = strdup(arg);
        arg = strtok(NULL, delim);
    }
    argv[argc] = NULL;
}

void PrintCommand(struct Command *command){
    int i;
    for (i = 0 ; i < command->num_sub_commands; i++){
        if (command->sub_commands[i].argv[1] == NULL && command->num_sub_commands < 2){
                printf("%s: Command not found\n",command->sub_commands[0].argv[0]);
        }
    }
}

void myCat(){
    int file; 
    char buffer[BUFFER_SIZE];
    file = open(filename, O_RDWR);
    if (file < 0){
        printf("%s: File not found\n", filename);
    } else{
        read(file, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
        close(file); 
    }
}

void redIO(){
    int ret = fork();
    if (ret < 0){
        fprintf(stderr, "fork failed\n");
        exit(1); 
    } else if (ret == 0){
        if (option == 1){
            close(0);
            int fd = open(filename, O_RDONLY);
            if (fd < 0){
                printf("%s: File not found\n", filename);
                exit(1); 
            }
            execvp(cmd1[0], cmd1);
            filename = "";
            option = 0;
        } else if (option == 2){
            close(1);
            int fd = open(filename, O_WRONLY | O_CREAT, 0660);
            if (fd < 0){
                printf("%s: Cannot create file\n", filename);
                exit(1);
            }
            execvp(cmd0[0], cmd0);
            filename = "";
            option = 0;
        }
    } else{
        waitpid(ret, NULL, 0);
    }
}

void showDirectories(){
    int ret = fork(); 
    if (ret < 0){
        fprintf(stderr, "fork failed\n");
        exit(1); 
    } else if (ret == 0){
        close(0);
        execvp(cmd2[0], cmd2);
    } else{
        waitpid(ret, NULL, 0);
    }
}

void environmentVariable(){
    setenv(variable, value, 1); 
    int i = 0; 
    printf("Printing all environment variables: \n");
    while(environ[i]){
        printf("%s\n", environ[i++]);
    }
}

void makeDirectories(){
    int ret = 0; 
    ret = mkdir(dirName, 0777);
    if (ret == 0){
        printf("Directory '%s' created\n", dirName);
    } else{
        printf("Unable to create directory %s \n", dirName);
        exit(1);
    }
}
