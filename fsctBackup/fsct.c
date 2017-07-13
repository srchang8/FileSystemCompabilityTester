/*

fsct
File System Compabitility Test

Purpose:
Find paths that may cause incomparability issues when copying
files from one file system to another.

Main Features:
-max component for path
-max characters for path
-detect bad characters
-path with capitalization

Data Structures:
arrays and a LinkedList

We use a linked list here because we do know
the number of entries in each directory.

List includes:
Full path to file
File name
Type of file


*/


#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>


int check_length = 0;
int max_length = -1;

int check_depth = 0;
int max_depth = -1;

int check_bad_chars = 0;
char* bad_chars;

int check_case = 0;

// node of a linked list of paths
typedef struct path_node {

	// full path, starting at given base directory
	char* path;

	// local name, excluding path
	char* name;

	// 1 for directories, 0 for others
	int is_dir;

	// pointer to next node in list
	struct path_node* next;

} path_node;


// check if given path is equal ignoring case to another path in the list
// args: path, list of paths
// return: 1 if there is a matching path, 0 otherwise
// note: always returns 0 if check_case is 0
int bad_case(char* path, path_node* paths) {
	if (! check_case) return 0;
	
	path_node* node;
	for (node = paths; node != NULL; node = node->next) {
		// ignore node for given path
		if (strcmp(node->path, path) == 0) continue;
		// if the name is equal ignoring case, it's bad
		if (strcasecmp(node->path, path) == 0) return 1;
	}
	
	return 0;
}

// check if given path is too long
// args: path
// return: 1 path is longer than max_length, 0 otherwise
// note: always returns 0 if check_length is 0
int path_too_long(char* path) {
	if (! check_length) return 0;
	
	if ( strlen(path) > max_length )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// calculate depth of given path by counting slashes
// args: path
// return: number of slashes in oath
int path_depth(char* path) {
	int depth = 0;
	int i;
	for ( i = 0; i < strlen(path); i++)
	{
		if ( path[i] == '/' )
		{
			depth++;
		}
	}
	
	return depth;
}

// check if given path is too deep
// args: path
// return: 1 path contains max_depth slashes or more, 0 otherwise
// note: always returns 0 if check_depth is 0
int path_too_deep(char* path) {
	if (! check_depth) return 0;
	
	if ( path_depth(path) >= max_depth )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// check if given path contains bad characters
// args: path
// return: 1 if path contains bad characters, 0 otherwise
// note: always returns 0 if check_bad_chars is 0
int has_bad_chars(char const *name)
{
	if (! check_bad_chars) return 0;
	
	int i;
	int j;

	for ( i = 0; i < strlen(name); i++)
	{
		for( j = 0; j < strlen(bad_chars); j++)
		{
			if ( name[i] == bad_chars[j] )
			{
				return 1;	
			}
		}
	}
	
	return 0;
}

// check if given path fits one or more of the compatibility criteria selected by the user.
// args: current path and list of all paths in current directory
// return: 1 if path fits one or more selected criteria, 0 otherwise
int path_is_bad(path_node* node, path_node* paths) {
	return bad_case(node->path, paths)
	|| path_too_deep(node->path)
	|| path_too_long(node->path)
	|| has_bad_chars(node->name);
}

// print given path if it fits one or more of the compatibility criteria selected by the user
// args: current path and list of all paths in current directory
void print_if_bad(path_node* node, path_node* paths)
{
	if (path_is_bad(node, paths))
	{
		printf("%s\n", node->path);
	}
}

// check if file name is equal to "." or ".."
// args: file name
// return: 1 if name is equal to "." or "..", 0 otherwise
int is_dot_or_dot_dot(char const* name)
{
   return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 );
}


char* make_copy(char const* str) {
	char* copy = malloc(strlen(str) + 1);
	strcpy(copy, str);
	return copy;
}

// concat prefix, "/", and suffix
// args: path prefix and suffix
// return: concatenation of prefix, "/", and suffix
// note: caller must free() the returned pointer
// note: if prefix is empty, simply return copy of suffix, do not prepend "/" 
char* concat_path(char const* prefix, char const* suffix) {
	if (strlen(prefix) == 0) return make_copy(suffix);
	char* path = malloc(strlen(prefix) + strlen(suffix) + 2);
	strcpy(path, prefix);
	strcat(path, "/");
	strcat(path, suffix);
	return path;
}

//create note using oppendir entries
path_node* make_node(char const* dirname, char const* name, unsigned char type) {
	// allocate space for new node
	path_node* node = malloc(sizeof (path_node));
	// TODO: error handling
	
	// store data in node
	node->name = make_copy(name);
	node->path = concat_path(dirname, name);
	node->is_dir = (type == DT_DIR) ? 1 : 0;
	
	return node;
}

//mirrors make_node to make sure we free up used space
void free_node(path_node* node) {
	free(node->name);
	free(node->path);
	free(node);
}

//Takes directory entries
// creates a linked list by turning entries into nodes
//each local name as their own node
path_node* get_paths(char const* dirname) {

	DIR* dirp = opendir(dirname);
	if (dirp == NULL) {
		perror(dirname);
		return NULL;
	}
	
	//pointer to start node
	path_node* paths = NULL;
	struct dirent *ent;
	while ((ent = readdir(dirp)) != NULL)
	{
		if (is_dot_or_dot_dot(ent->d_name)) continue;
		
		path_node* node = make_node(dirname, ent->d_name, ent->d_type);
		// TODO: error handling
		
		// prepend node to list
		node->next = paths;
		paths = node;
	}
	
	// close directory
	closedir(dirp);
	
	return paths;
}

//mirrors get_paths to free up space
void free_paths(path_node* paths) {
	path_node* node = paths;
	while (node != NULL) {
		path_node* next = node->next;
		free_node(node);
		node = next;
	}
}

//iterate through nodes
//check if they should be printed
//recursively go into directories
void process_dir(char const* dirname)
{
	// get linked list of path nodes for all files in this dir
	path_node* paths = get_paths(dirname);

	//iterate nodes
	path_node* node = paths;
	while (node != NULL)
	{
		//check if we should print path
		print_if_bad(node, paths);
	
		// if directory then do recursive on it
		if (node->is_dir)
		{
			// recursively ist the contents of the subdirectory.
			process_dir(node->path);
		}
		
		node = node->next;
	}
	
	free_paths(paths);
}

int main (int ac, char *av[])
{
	char* dirs[ac];
	int cnt = 0;
	
	int i;
	//get directory names
	for (i = 1; i < ac; i++) {
		if (av[i][0] == '-') break;
		dirs[cnt++] = av[i];
	}
	
	if (cnt == 0) {
		printf("missing directory name");
		exit(1);
	}
		
	//get command line parameters
	//i is not reset
	for (; i < ac; i++){
		
		if ( strcmp(av[i], "-maxdepth" ) == 0 && i+1 < ac) {
			check_depth = 1;
			max_depth = atoi(av[i+1]);
			// printf("%d maxdepth entered \n", max_depth);
		}
		if ( strcmp(av[i], "-pathmax" ) == 0 && i+1 < ac) {
			check_length = 1;
			max_length = atoi(av[i+1]);
			// printf("%d pathmax entered \n", max_length);
		}
		if ( strcmp(av[i], "-badchars" ) == 0 && i+1 < ac) {
			check_bad_chars = 1;
			bad_chars = av[i+1];
			// printf("badChars[] print first %c \n", badChars[0]);
		}
		if ( strcmp(av[i], "-nocasesens" ) == 0) {
			check_case = 1;
		}
	}

	for (i = 0; i < cnt; i++) {
		char* dirname = dirs[i];
		path_node* dirnode = make_node("", dirname, DT_DIR);
		print_if_bad(dirnode, NULL);
		free_node(dirnode);
		process_dir(dirname);
	}
	
}//ends main

