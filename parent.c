#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void write_to_pipe(int pipe_fd, const char* str) {
    size_t len = strlen(str);
    write(pipe_fd, &len, sizeof(size_t));
    write(pipe_fd, str, len);
}

char* read_from_pipe(int pipe_fd) {
    size_t len;
    if (read(pipe_fd, &len, sizeof(size_t)) <= 0) {
        return NULL;
    }
    
    char* str = malloc(len + 1);
    if (read(pipe_fd, str, len) <= 0) {
        free(str);
        return NULL;
    }
    str[len] = '\0';
    return str;
}


int read_input(char* buffer, int size) {
    int i = 0;
    char c;
    
    while (i < size - 1) {
        if (read(STDIN_FILENO, &c, 1) <= 0) {
            break;
        }
        if (c == '\n') {
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return i;
}


void print_string(const char* str) {
    write(STDOUT_FILENO, str, strlen(str));
}

void print_result(const char* result) {
    print_string("Result: ");
    print_string(result);
    print_string("\n");
}

int main() {
    int pipe1[2];  // parent -> child1
    int pipe2[2];  // child2 -> parent
    int child_pipe[2]; // child1 -> child2
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(child_pipe) == -1) {
        return 1;
    }
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child1 process
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(child_pipe[0]);
        
        dup2(pipe1[0], STDIN_FILENO);
        dup2(child_pipe[1], STDOUT_FILENO);
        
        close(pipe1[0]);
        close(child_pipe[1]);
        
        execl("./child1", "./child1", NULL);
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child2 process
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(child_pipe[1]);
        
        dup2(child_pipe[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        
        close(child_pipe[0]);
        close(pipe2[1]);
        
        execl("./child2", "./child2", NULL);
        exit(1);
    }
    
    // Parent process
    close(pipe1[0]);
    close(pipe2[1]);
    close(child_pipe[0]);
    close(child_pipe[1]);
    
    char buffer[BUFFER_SIZE];
    
    print_string("Enter strings (empty line to exit):\n");
    
    while (1) {
        print_string("> ");
        int len = read_input(buffer, BUFFER_SIZE);
        
        if (len == 0) {
            break;
        }
        
        write_to_pipe(pipe1[1], buffer);
        
        char* result = read_from_pipe(pipe2[0]);
        if (result) {
            print_result(result);
            free(result);
        }
    }
    
    close(pipe1[1]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    close(pipe2[0]);
    
    return 0;
}