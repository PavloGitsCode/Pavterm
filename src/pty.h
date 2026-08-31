#ifndef PAVTERM_PTY_H
#define PAVTERM_PTY_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#define PTY_INPUT_CAPACITY 8192

typedef struct {
    char data[PTY_INPUT_CAPACITY];
    size_t length;
} PtyInputQue;

bool pty_enq_input(PtyInputQue *que, const char *input, size_t input_size);

int pty_flush_input(int master_fd, PtyInputQue *que);

ssize_t write_master_fd(int master_fd, const char *input_pty,
                        size_t input_pty_size);

ssize_t read_master_fd(int master_fd, char *input_buff, size_t input_buff_size);

int start_shell(int *master_fd, pid_t *shell_pid);

#endif
