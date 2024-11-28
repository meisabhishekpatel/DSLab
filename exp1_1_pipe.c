#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 256

void concatenate_strings(char *str1, char *str2, char *result) {
	int i = 0, j = 0;
	while(str1[i] != '\0') {
		result[i] = str1[i];
		i++;
	}
	while(str2[j] != '\0') {
		result[i] = str2 [j];
		i++;
		j++;
	}
	result[i] = '\0';
}

int main() {
	int pipefd1[2], pipefd2[2];
	pipe(pipefd1);
	pipe(pipefd2);

	pid_t pid;
	char input_str[MAX_LEN];
	char received_str[MAX_LEN];
	char concatenated_str[MAX_LEN];
	const char *append_str = " - Processed by Child";

	pid = fork();

	if(pid > 0) {
		close(pipefd1[0]);
		close(pipefd2[1]);

		printf("Enter a string: ");
		fgets(input_str, MAX_LEN, stdin);
		input_str[strcspn(input_str, "\n")] = '\0';

		write(pipefd1[1], input_str, strlen(input_str) + 1);

		read(pipefd2[0], received_str, MAX_LEN);

		printf("Received from child: %s\n", received_str);

		close(pipefd1[1]);
		close(pipefd2[0]);
	}
	else {
		close(pipefd1[1]);
		close(pipefd2[0]);

		read(pipefd1[0], received_str, MAX_LEN);

		concatenate_strings(received_str, append_str, concatenated_str);

		write(pipefd2[1], concatenated_str, strlen(concatenated_str) + 1);

		close(pipefd1[0]);
		close(pipefd2[1]);
	}

	return 0;
}
