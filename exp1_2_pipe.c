#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 256

void mat_sum(int mat1[][2], int mat2[][2], int result[][2]) {
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
			result[i][j] = mat1[i][j] + mat2[i][j];
		}
	}
}

int main() {
	int pipefd1[2], pipefd2[2];
	pipe(pipefd1);
	pipe(pipefd2);

	pid_t pid;

	int mat1[2][2], mat2[2][2], res[2][2];

	pid = fork();

	if(pid > 0) {
		close(pipefd1[0]);
		close(pipefd2[1]);

		printf("Enter matrix 1 (2x2): \n");
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++) {
				scanf("%d", &mat1[i][j]);
			}
		}

		printf("Enter matrix 2 (2x2): \n");
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++) {
				scanf("%d", &mat2[i][j]);
			}
		}

		write(pipefd1[1], mat1, sizeof(mat1));
		write(pipefd1[1], mat2, sizeof(mat2));
		read(pipefd2[0], res, sizeof(res));

		printf("Received from child: \n");
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++) {
				printf("%d ", res[i][j]);
			}
			printf("\n");
		}

		close(pipefd1[1]);
		close(pipefd2[0]);
	}
	else {
		close(pipefd1[1]);
		close(pipefd2[0]);

		read(pipefd1[0], mat1, sizeof(mat1));
		read(pipefd1[0], mat2, sizeof(mat2));

		mat_sum(mat1, mat2, res);
		write(pipefd2[1], res, sizeof(res));
		
		close(pipefd1[0]);
		close(pipefd2[1]);
	}

	return 0;
}
