#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <pthread.h>
#include <string.h>

#define MAX_FILE_NAME_LEN 256

// gcc -o mapreduce mapreduce.c -lpthread

// Structure to hold the file information for each file
typedef struct {
    char file_name[MAX_FILE_NAME_LEN];
    long file_size;
    char owner_name[MAX_FILE_NAME_LEN];
} FileInfo;

// Global variables
FileInfo *file_infos = NULL;
int file_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to get the owner of the file
char* get_owner_name(const char *file_path) {
    struct stat statbuf;
    struct passwd *pw;

    if (stat(file_path, &statbuf) == -1) {
        perror("stat failed");
        return NULL;
    }

    pw = getpwuid(statbuf.st_uid);
    if (pw == NULL) {
        perror("getpwuid failed");
        return NULL;
    }

    return pw->pw_name;
}

// Function to process each file (map function)
void* process_file(void *arg) {
    char *file_name = (char*)arg;
    struct stat statbuf;
    char file_path[MAX_FILE_NAME_LEN];

    // Construct file path
    getcwd(file_path, sizeof(file_path));
    strcat(file_path, "/");
    strcat(file_path, file_name);

    if (stat(file_path, &statbuf) == -1) {
        perror("stat failed");
        return NULL;
    }

    // Get file size and owner
    long file_size = statbuf.st_size;
    char *owner = get_owner_name(file_path);
    if (owner == NULL) {
        return NULL;
    }

    // Lock mutex to safely update shared file_infos array
    pthread_mutex_lock(&mutex);
    file_infos = realloc(file_infos, sizeof(FileInfo) * (file_count + 1));
    strncpy(file_infos[file_count].file_name, file_name, MAX_FILE_NAME_LEN);
    file_infos[file_count].file_size = file_size;
    strncpy(file_infos[file_count].owner_name, owner, MAX_FILE_NAME_LEN);
    file_count++;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Function to perform the reduce operation (find max file size and owner)
void find_max_file_and_owner() {
    long max_size = 0;
    char *max_owner = NULL;

    for (int i = 0; i < file_count; i++) {
        if (file_infos[i].file_size > max_size) {
            max_size = file_infos[i].file_size;
            max_owner = file_infos[i].owner_name;
        } else if (file_infos[i].file_size == max_size) {
            printf("Multiple owners of the largest file: %s\n", file_infos[i].owner_name);
        }
    }

    if (max_owner != NULL) {
        printf("Owner of the largest file: %s\n", max_owner);
    } else {
        printf("No files found in the directory.\n");
    }
}

// Main function
int main() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir failed");
        return 1;
    }

    // Read all files in the directory
    pthread_t threads[1024];
    int thread_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Ignore directories and hidden files
        if (entry->d_type == DT_REG && entry->d_name[0] != '.') {
            if (pthread_create(&threads[thread_count], NULL, process_file, (void*)entry->d_name) != 0) {
                perror("pthread_create failed");
                return 1;
            }
            thread_count++;
        }
    }

    closedir(dir);

    // Wait for all threads to complete
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Perform reduce step (find max file size and owner)
    find_max_file_and_owner();

    // Clean up
    free(file_infos);

    return 0;
}

