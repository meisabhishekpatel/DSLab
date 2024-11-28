#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
	int pipefd[2];
	pipe(pipefd);

	pid_t pid = fork();

	if(pid > 0) {
		close(pipefd[1]);

		wait(NULL);

		char result[1024];
		ssize_t bytesRead = read(pipefd[0], result, sizeof(result));
		if(bytesRead > 0) {
			result[bytesRead] = '\0';
			printf("CPU Load on Node B: %s\n", result);
		}

		close(pipefd[0]);
	}
	else {
		close(pipefd[0]);
		FILE *fp = popen("uptime", "r");
		
		char result[1024];
		while(fgets(result, sizeof(result), fp) != NULL) {
			write(pipefd[1], result, strlen(result) + 1);
		}

		fclose(fp);
		close(pipefd[1]);
		exit(0);
	}
	
	return 0;
}
