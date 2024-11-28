#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

// Structure to hold file size and owner information
typedef struct {
    char owner[100];
    off_t size;
} FileData;

// Function to get the owner of a file using stat and getpwuid
void get_file_owner(struct stat file_stat, char *owner) {
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    if (pwd != NULL) {
        strcpy(owner, pwd->pw_name);
    } else {
        strcpy(owner, "Unknown");
    }
}

// Function to simulate the Map phase and reduce the maximum file size with its owners
void find_max_file_owner(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    FileData *file_list = NULL;
    size_t file_count = 0;
    size_t max_size_index = -1;
    off_t max_size = 0;
    
    // Open directory
    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }
    
    // Map phase: Read files and gather their sizes and owners
    while ((entry = readdir(dir)) != NULL) {
        // Skip the special entries "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Get file information
        if (stat(entry->d_name, &file_stat) == 0) {
            // If it's a regular file, add it to the list
            if (S_ISREG(file_stat.st_mode)) {
                file_list = realloc(file_list, (file_count + 1) * sizeof(FileData));
                get_file_owner(file_stat, file_list[file_count].owner);
                file_list[file_count].size = file_stat.st_size;
                
                // Update the maximum size and index
                if (file_stat.st_size > max_size) {
                    max_size = file_stat.st_size;
                    max_size_index = file_count;
                }
                
                file_count++;
            }
        } else {
            perror("Error with stat");
        }
    }
    
    // Close the directory
    closedir(dir);
    
    // Reduce phase: Find the file with the maximum size and print its owners
    if (max_size_index != -1) {
        printf("Maximum file size: %lld bytes\n", max_size);
        printf("Owner(s) of the largest file:\n");
        
        // Print the owner(s) of the file with the maximum size
        for (size_t i = 0; i < file_count; i++) {
            if (file_list[i].size == max_size) {
                printf("User: %s\n", file_list[i].owner);
            }
        }
    } else {
        printf("No regular files found in the directory.\n");
    }
    
    // Free the allocated memory for the file list
    free(file_list);
}

int main() {
    // Current working directory
    char cwd[1024];
    
    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
        find_max_file_owner(cwd);
    } else {
        perror("Error getting current working directory");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

