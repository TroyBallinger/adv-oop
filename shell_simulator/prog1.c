#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define CHAR_LIMIT 1024
#define ARG_LIMIT 64

// Helper function to handle commands 
void exec_command(char**, unsigned short, unsigned short);
// Helper function to handle pipelines 
void exec_pipeline(char**, unsigned short, unsigned short);

// Parses commands that are inputted by the user 
int main(void) {    
	// Buffer of user input
	char user_input[CHAR_LIMIT];

	char* args[ARG_LIMIT];
	unsigned short arg_count;
	bool exit = 0;

	// Define special chars, give them each a reference address 
	char pipeline = '|';
	char end_command = ';';
	char background = '&';
	char input = '<';
	char output = '>';

	// Continue prompting for user input until "exit" is typed 
	while (!exit) {
		// Reset variables 
		memset(user_input, '\0', CHAR_LIMIT);
		memset(args, 0, ARG_LIMIT);
		arg_count = 0;

		// Prompt user 
		printf("shell_sim> ");

		// Pull user input 
		fgets(user_input, CHAR_LIMIT, stdin);

		// Counter variable 
		unsigned short arg_finder = 0; 

		// True while traversing a non-special argument string 
		bool traversing_arg = 0; 

		// The first char address of every argument is stored in args[]. 
		while(user_input[arg_finder] != '\n') {
			// Special chars count as their own arg
			if (user_input[arg_finder] == '|') {
				args[arg_count] = &pipeline; 
			} else if (user_input[arg_finder] == ';') {
				args[arg_count] = &end_command; 
			} else if (user_input[arg_finder] == '&') {
				args[arg_count] = &background; 
			} else if (user_input[arg_finder] == '<') {
				args[arg_count] = &input; 
			} else if (user_input[arg_finder] == '>') {
				args[arg_count] = &output; 
			} else if (!traversing_arg && user_input[arg_finder] != ' ') {
				// Non-special arg found 
				args[arg_count] = &(user_input[arg_finder]);
				traversing_arg = 1;
				arg_count++;
			}
			// Convert space to a '\0' - denotes end of arg 
			switch (user_input[arg_finder]) {
				case '&': 
				case '|': 
				case ';': 
				case '<': 
				case '>': 
					arg_count++;
				case ' ': 
					traversing_arg = 0;
					user_input[arg_finder] = '\0';
				default: break;
			}
			arg_finder++;
		}

		// Set last char (newline) to null terminator 
		user_input[arg_finder] = '\0';

		// Handle separate commands 
		unsigned short args_in_command = 0;
		unsigned short command_start = 0;
		for (int i = 0; i < arg_count; i++) {
			if (args[i][0] == ';') {
				// Handle pipelines 
				exec_pipeline(args, command_start, args_in_command);
				printf("\n\n"); // Denote a new command 
				args_in_command = 0;
				command_start = i + 1;
			} else {
				if (strcmp(args[i], "exit") == 0) {
					exit = 1;
				} else {
					args_in_command++;
				}
			}
		}
		exec_pipeline(args, command_start, args_in_command);
	}
}

void exec_command(char** args, unsigned short arg_start, unsigned short arg_count) {
	if (arg_count == 0) { return; }
	// Print the command 
	printf("COMMAND: %s", args[arg_start]);
	if (arg_count > 1) { printf(", "); }
	// Print arguments after 
	unsigned short null_args = 0;
	for (int i = 1; i < arg_count; i++) {
		if (args[i] != 0) {
			printf("arg-%d: %s", (i - null_args), args[arg_start + i]);
			if (i < (arg_count - 1)) { printf(", "); }
		} else {
			// Adjust for removed arguments
			null_args++;
			arg_count++;
		}
	}
	printf("\n");
}

void exec_pipeline(char** args, unsigned short command_start, unsigned short args_in_command) {
	unsigned short args_in_pipe = 0;
	unsigned short pipe_start = command_start;
	bool background = 0;
	char* input = 0;
	char* output = 0;
	for (int j = command_start; j < command_start + args_in_command; j++) {
		if (args[j][0] == '|') {
			exec_command(args, pipe_start, args_in_pipe);
			if (input != 0) {
				printf("... input of the above command will be redirected from file \"%s\".\n", input);
			}
			if (output != 0) {
				printf("... output of the above command will be redirected to file \"%s\".\n", output);
			}
			printf("... output of the above command will be redrecited to serve as the input of the following command ...\n");
			input = 0;
			output = 0;
			args_in_pipe = 0;
			pipe_start = j + 1;
		} else if (args[j][0] == '&') { 
			// Set to background execution 
			background = 1;
		} else if (args[j][0] == '<') {
			// Remove input argument 
			input = args[j + 1];
			args[j] = 0;
			args[j + 1] = 0; 
			j++;
		} else if (args[j][0] == '>') {
			// Remove output argument 
			output = args[j + 1];
			args[j] = 0;
			args[j + 1] = 0; 
			j++;
		} else {
			args_in_pipe++;
		}
	}
	// Execute last pipe 
	exec_command(args, pipe_start, args_in_pipe); 
	if (input != 0) {
		printf("... input of the above command will be redirected from file \"%s\".\n", input);
	}
	if (output != 0) {
		printf("... output of the above command will be redirected to file \"%s\".\n", output);
	}
	if (background) {
		printf("... the above command will be executed in background.\n");
	}
}
