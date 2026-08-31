
#include "pty.h"

#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int static make_nonblock(int master_fd) {
    int flags = fcntl(master_fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl FGETFL");
        return 1;
    }
    if (fcntl(master_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }
    return 0;
}

bool pty_enq_input(PtyInputQue *que, const char *input, size_t input_size) {
    size_t available = sizeof(que->data) - que->length;

    if (input_size > available) {
        return false;
    }
    memcpy(que->data + que->length, input, input_size);
    que->length += input_size;
    return true;
}

int pty_flush_input(int master_fd, PtyInputQue *que) {
    while (que->length > 0) {
        ssize_t bytes_written = write(master_fd, que->data, que->length);
        if (bytes_written > 0) {
            size_t remaining = que->length - (size_t)bytes_written;
            memmove(que->data, que->data + bytes_written, remaining);
            que->length = remaining;
            continue;
        }
        if (bytes_written == -1 && errno == EINTR) {
            continue;
        }
        if (bytes_written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return 0;
        }
        if (bytes_written == -1) {
            perror("write master_fd");
            return -1;
        }
    }
    return 0;
}

ssize_t write_master_fd(int master_fd, const char *input_pty,
                        size_t input_pty_size) {
    ssize_t bytes_written;

    do {
        bytes_written = write(master_fd, input_pty, input_pty_size);
    } while (bytes_written == -1 && errno == EINTR);

    return bytes_written;
}

ssize_t read_master_fd(int master_fd, char *output_pty,
                       size_t input_buff_size) {
    ssize_t bytes_read;

    do {
        bytes_read = read(master_fd, output_pty, input_buff_size);
    } while (bytes_read == -1 && errno == EINTR);

    if (bytes_read == -1 && errno == EIO) {
        return 0;
    }
    return bytes_read;
}

int start_shell(int *master_fd, pid_t *shell_pid) {
    pid_t pid = forkpty(master_fd, NULL, NULL, NULL);

    if (pid == -1) {
        printf("Failed: forkpty: %d\n", errno);
        return -1;
    }
    if (pid == 0) {
        execl("/bin/sh", "sh", NULL);
        perror("execl");
        _exit(127);
    }
    if (make_nonblock(*master_fd) == 1) {
        close(*master_fd);
        waitpid(pid, NULL, 0);
        return -1;
    }
    *shell_pid = pid;
    return 0;
}
