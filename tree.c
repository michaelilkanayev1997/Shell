#include <stdio.h>
#include <string.h>
#include <dirent.h>

// function to recursively print the directory tree
void print_tree(char *path, int level) {
    DIR *dir; // pointer to directory stream
    struct dirent *entry;

     // open the directory stream, and if it fails, return
    if (!(dir = opendir(path))) {
        return;
    }

    // loop through each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) { // if the entry is a directory
            char subpath[1024];
            // construct the subdirectory path by concatenating the parent path and the directory name
          
            int len = snprintf(subpath, sizeof(subpath)-1, "%s/%s", path, entry->d_name); 
            subpath[len] = 0;   // set the null terminator at the end of the string
            // if the directory name is "." or "..", skip it
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // print the directory name, indented by level*2 spaces
            printf("%*s[%s]\n", level*2, "", entry->d_name);
            // recursively call print_tree with the subdirectory path and incremented level
            print_tree(subpath, level + 1);
        } else {
            // if the entry is a file, print its name, indented by level*2 spaces, with a "-" prefix
            printf("%*s- %s\n", level*2, "", entry->d_name);
        }
    }
    closedir(dir); //// close the directory stream
}

int main(int argc, char *argv[]) {
    // get the path argument from the command line
    char *path = argv[1];
    int level = 0;  // initialize the level to 0

    // call print_tree with the path and level
    print_tree(path, level);

    return 0;
}