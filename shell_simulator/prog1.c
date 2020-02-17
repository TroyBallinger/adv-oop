#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHAR_LIMIT 1024
#define ARG_LIMIT 64

void kill_processes(int);
void exec_command(char**, short, bool);
bool running = 0;

int main(void) {
	char user_input[CHAR_LIMIT];
	char* args[ARG_LIMIT];
	unsigned short arg_count;
	bool exit, background = 0;
	char special_chars[] = {' ','&','<','>',';','|'};
	signal(SIGINT, kill_processes);
	setbuf(stdout, NULL);
	while (!exit) {
		memset(user_input, '\0', CHAR_LIMIT);
		memset(args, 0, ARG_LIMIT);
		arg_count = 0;
		background = 0;
		bool traversing_arg = 0;
		unsigned short i = 0;
		fprintf(stdout, "shell> ");
		fgets(user_input, CHAR_LIMIT, stdin);
		while (user_input[i] != '\n') {
			for (unsigned short j = 0; j < 6; j++) {
				if (user_input[i] == special_chars[j]) {
					if (j > 1) {
						args[arg_count] = &(special_chars[j]);
						arg_count++;					
					} else if (j == 1) {
						background = 1;
					}
					traversing_arg = 0;
					user_input[i] = '\0';
				}
			}
			if (!traversing_arg && user_input[i] != '\0') {
				args[arg_count] = &(user_input[i]);
				traversing_arg = 1;
				arg_count++;
			}
			i++;
		}
		user_input[i] = '\0';
		i = 0;
		unsigned short cmd_start = 0;
		while (i < arg_count) {
			if (strcmp(args[i], "exit") == 0) { 
				exit = 1; 
				i = arg_count; /* Exit the loop */
			} else if (args[i][0] == special_chars[4] || i == (arg_count - 1)) {
				if (i == (arg_count - 1)) { i++; }
				args[i] = NULL;
				exec_command(&(args[cmd_start]), (i - cmd_start), background);
				cmd_start = i + 1;
			}
			i++;
		}
	}
	return 0;
}

void kill_processes(int sig) {
	if (sig == 2) {
		kill(0,0);
		if (!running) {
			fprintf(stdout, "\nCtrl-C caught. But currently there is no foreground process running.\nshell> ");
		}
	}
}

void exec_command(char** args, short arg_count, bool background) {
	unsigned short start_index = 0;
	int child_pid = fork();
	if (child_pid == 0) {
		unsigned short k = 0;
		unsigned short f = 0;
		int fd1[2];
		int fd2[2];
		pipe(fd1);
		pipe(fd2);
		while (k < arg_count) {
			if (args[k][0] == '<') {
				int i_fd = open(args[(k+1)], O_RDONLY);
				args[k] = NULL;
				args[k + 1] = NULL;
				dup2(i_fd, 0);
				k++;
			} else if (args[k][0] == '>') {
				int o_fd = open(args[(k+1)], O_CREAT|O_TRUNC|O_RDWR, S_IRWXU);
				args[k] = NULL;
				args[k + 1] = NULL;
				dup2(o_fd, 1);
				k++;
			} else if (args[k][0] == '|') {
				args[k] = NULL;
				int p_pid = fork();
				if (p_pid == 0) {
					switch (f) {
						case 0: dup2(fd1[1], 1);
							close(fd1[0]);
							close(fd1[1]); break;
						default: dup2(fd2[1], 1);
							close(fd2[0]);
							close(fd2[1]); break;
					}
					execvp(args[start_index], &(args[start_index]));
				} else {
					waitpid(p_pid, NULL, 0);
					fprintf(stdout, "\n... child process (PID=%d) is done\n", p_pid);
					dup2(fd1[0], 0);
					switch (f) {
						case 0: dup2(fd1[0], 0);
							close(fd1[0]);
							close(fd1[1]); break;
						default: dup2(fd2[0], 0);
							close(fd2[0]);
							close(fd2[1]); break;
					}
					start_index = k + 1;
					f++;
				}
			}
			if (k == (arg_count - 1)) {
				execvp(args[start_index], &(args[start_index]));
			}
			k++;
		}
	} else {
		if (background) {
			waitpid(child_pid, NULL, WNOHANG);
		} else {
			running = 1;
			waitpid(child_pid, NULL, 0);
			running = 0;
			fprintf(stdout, "\n... child process (PID=%d) is done\n", child_pid);
			int pid;
			while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
				fprintf(stdout, "\n... child process (PID=%d) is done\n", pid);
			}
		}
	}
}