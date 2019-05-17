// THIS IS A FUNNY SHELL TO LEARN A REAL SHELL HOW TO WORK.
// THAT'S REALLY COOL TO WRITE THIS CODE, ENJOY THIS.
// i0sh -> means i0Ek3's Shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define CWD_SIZE 1024 // 当前路径长度
#define READ_LINE_BUFSIZE 1024 // 读入缓冲区大小
#define TOKEN_BUFSIZE 64 // 令牌缓冲区大小
#define TOKEN_DELIM " \t\r\n\a" // 分隔符

// 当前路径提示
char cwd[CWD_SIZE];

// 内置命令声明
int i0_cd(char **args);
int i0_help(char **args);
int i0_exit(char **args);

// 内置命令集合
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
}; 

int (*builtin_func[]) (char **) = {
    &i0_cd,
    &i0_help,
    &i0_exit
};

// 内置命令个数
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// 内置命令的实现
int i0_cd(char **args) {
    if (args[1] == NULL) { // 缺少参数
        fprintf(stderr, "Error: Expected argument to \"cd!\"\n");
    } else {
        if (chdir(args[1]) != 0) { // 改变路径
            perror("i0sh");
        }
        getcwd(cwd, sizeof(cwd));
    }
    return 1;
}

int i0_help(char **args) {
    int i = 0;

    printf("This is i0sh, welcome to use it!\n");
    printf("Enter help to check builtin commands.\n");
    printf("The builtin commands as follows:\n");

    for (; i < num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }

    printf("Also you can use man command to check other commands.\n");
    return 1;
}

int i0_exit(char **args) {
    return 0;
}

// Shell 工作的流程分为三个阶段：
//  1. 启动并初始化Shell
//  2. 解析用户输入的命令和参数
//  3. 执行命令

// 通过fork()实现
int launch(char **args) {
    pid_t pid;
    int stat;

    pid = fork();
    if (pid == 0) { // 子进程
        signal(SIGINT, SIG_DFL); // 防止子进程复制父进程的当前信号状态
        if (execvp(args[0], args) == -1) {
            perror("i0sh");
            exit(EXIT_FAILURE); // 进行错误检查
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) { // fork() 出错
        perror("i0sh");
        exit(EXIT_SUCCESS);
    } else { // 父进程
        do {
            waitpid(pid, &stat, WUNTRACED); // 等待子进程的退出
        } while (!WIFEXITED(stat) && !WIFSIGNALED(stat)); // 正常退出或者因信号问题退出
    }
    return 1;
}

// 执行用户输入的命令
int execute(char **args) {
    int i = 0;
    if (args[0] == NULL) { // 没有输入命令
        return 1;
    }

    for (; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch(args);
}

// 解析命令和参数
char *read_line(void) {
    int bufsize = READ_LINE_BUFSIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "Error: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        if (c == EOF) { // EOF 是整数，不是字符
            exit(EXIT_SUCCESS);
        } else if (c == '\n') { // 如果换行了，则更新当前buffer的位置到\0
            buffer[pos] = '\0';
            return buffer;
        } else { // 更新buffer的位置为c
            buffer[pos] = c;        
        }
        
        pos++;

        if (pos >= bufsize) { // 持续输入命令，则动态扩充buffer
            char *tmp = realloc(buffer, bufsize + READ_LINE_BUFSIZE);
            if (!tmp) {
                fprintf(stderr, "Error: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
            buffer = tmp;
            bufsize += READ_LINE_BUFSIZE;
        }
    }   
}

// 切分命令行输入的字符
char **split_line(char *line) {
    int bufsize = TOKEN_BUFSIZE;
    int pos = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "Error: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= bufsize) {
            bufsize += TOKEN_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "Error: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

void loop(void) {
    char *line;
    char **args;
    int stat;

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Thank you for use i0sh, we'll be better!\n");
    }

    do {
        printf("[%s]-> ", cwd); // 显示当前路径
        line = read_line();
        args = split_line(line);
        stat = execute(args);

        free(line);
        free(args);
    } while (stat);
}

// Ctrl-C 信号处理
void sigint_handler(int signo) {
    printf(" !!!Caught SIGINT!!!\n");
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler);
    loop();

    return EXIT_SUCCESS;
}

