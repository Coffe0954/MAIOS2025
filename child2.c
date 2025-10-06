#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Custom isspace function without ctype.h
int is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

void remove_double_spaces(char* str) {
    char* dest = str;
    char* src = str;
    int prev_space = 0;
    
    while (*src) {
        if (is_space(*src)) {
            if (!prev_space) {
                *dest++ = ' ';
                prev_space = 1;
            }
        } else {
            *dest++ = *src;
            prev_space = 0;
        }
        src++;
    }
    *dest = '\0';
}

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

int main() {
    while (1) {
        char* input = read_from_pipe(STDIN_FILENO);
        if (!input) {
            break;
        }
        
        remove_double_spaces(input);
        write_to_pipe(STDOUT_FILENO, input);
        
        free(input);
    }
    
    return 0;
}