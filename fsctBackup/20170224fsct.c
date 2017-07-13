/*
fsct

//TODO inum only stores the file name but not the whole file path
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>



void showDir(char const* dirname, int const pathDepth);
int is_dot_or_dot_dot(char const* name);
int countPath(char *path );
int countChar( char *path );


int main ( int ac, char *av[] )
{
	ino_t	myinode ;
	dev_t	mydev   ;

	int maxDepth = 0;
	int pathMax = 0;
	char badChars[50]; //To do: size?
	int caseSense = 0;//1 means true, 0 means false
	int i = 0;
	int j = 0;
	
	
	//get command line parameters
	for (i = 1; i < ac; i++){
		
			if ( strcmp(av[i], "-maxdepth" ) == 0 && i+1 < ac) {
				maxDepth = atoi(av[i+1]);
				printf("%d maxdepth entered \n", maxDepth);
			}
			if ( strcmp(av[i], "-pathmax" ) == 0 && i+1 < ac) {
				pathMax = atoi(av[i+1]);
				printf("%d path entered \n", pathMax);
			}
			if ( strcmp(av[i], "-badchars" ) == 0 && i+1 < ac) {
				strcpy(badChars, av[i+1]);
				printf("badChars[] print first %c \n", badChars[0]);
			}
			if ( strcmp(av[i], "-nocasesens" ) == 0) {
				caseSense = 1;
			}
			
	}

	
	//maxDepth += countPath(av[1]);
	//int numOfPath = 0;
	//numOfPath = countPath(av[1]);		
	//printf("maxDepth %d\n", maxDepth);	
	//printf("number of paths %d\n ", countPath(av[1]));
	
	
printf(" number of char in path is %d\n", countChar(av[1]));
			
	//maxDepth += countPath(av[1]);
	//showDir(av[1], maxDepth);
	
}//ends main




int is_dot_or_dot_dot(char const* name)
{
   return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 );
}




void showDir(char const* dirname, int const pathDepth)
{
	
	char* currFilNam;
	struct dirent *curr_ent;
	
	DIR* dirp = opendir(dirname);

	if ( dirp == NULL )
	{
		return;
	}

		while ( (curr_ent = readdir(dirp)) != NULL )
		{ 
   
			//TODO funtion
			//caller should be responsible for the freeing of the memeory
			// Allocate memory for the subdirectory.
			// 1 additional for the '/' and the second additional for '\0'.
			currFilNam = malloc(strlen(dirname) + strlen(curr_ent->d_name) + 2);

			// build path string
			strcpy(currFilNam, dirname);
			strcat(currFilNam, "/");
			strcat(currFilNam, curr_ent->d_name);
		
			//printf("  pathdepth %d\n", pathDepth);
			//printf(" currFilNam depth %d\n", currFilNam);

			//printf("countPath %d\n ", countPath(currFilNam));
			//printf("path depth plus current depth %d\n ", (pathDepth + countPath(currFilNam)));
			
			
				if ( countPath(currFilNam) <= pathDepth)
				{
					printf("%s\n", currFilNam);
				}
			
	

			  //printf("%s\n", currFilNam);

			  // if directory and not equal to . and .. then do recursive on it
			  if ( curr_ent->d_type == DT_DIR && !(is_dot_or_dot_dot(curr_ent->d_name)) )
			  {
				 // recursively ist the contents of the subdirectory.
				 showDir(currFilNam, pathDepth);
			  }

			// Free the allocated memory.
			free(currFilNam);
		}

   // Close the directory
   closedir(dirp);
}



//counts the number of components there are in a path
int countPath( char *path )
{
	
	int compCount = 0;
	int i = 0;
	
	//printf(" %c\n", path[0]);
	//printf(" %d\n", strcmp(path[0], "/" ));
	
	for ( i = 0; i < strlen(path); i++)
	{
		//printf(" %c\n", path[0]);
		if ( path[i] == '/' )
		{
			compCount++;
		}
	}
	
	return compCount;
}

int countChar( char *path )
{
	int charCount = 0;
	int i = 0;
	
	for ( i = 0; i < strlen(path); i++)
	{
		charCount++;
	}
	
	return charCount;

}










