#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void to_lower_case(char* str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + ('a' - 'A');
        }
    }
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
        
        to_lower_case(input);
        write_to_pipe(STDOUT_FILENO, input);
        
        free(input);
    }
    
    return 0;
}