/*
fsct

//TODO inum only stores the file name but not the whole file path



        $ fsct e28/hw -maxdepth 8 -pathmax 50 -badchars ’:*?|’ -nocasesens
        e28/hw/hw1/tests/test1/data/orig/jun/12
        e28/hw/hw1/tests/test1/data/orig/jun/12/wtmp.1
        e28/hw/project3-terminal-control-interface/draft1/driver.c
        e28/hw/hw2/Notes
        e28/hw/hw2/notes
        e28/hw/hw3/version-09:05PM

*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>



void showDir(char const* dirname, int const pathDepth, int const pathMax);
int is_dot_or_dot_dot(char const* name);
int countPath(char *path );
int countChar( char *path );
int pathContains( char const *path, char charArr[]);

int pathMax = 0;

int main ( int ac, char *av[] )
{
	ino_t	myinode ;
	dev_t	mydev   ;

	int maxDepth = 0;
	//int pathMax = 0;
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

	

	printf(" path max %d\n", pathMax);
	
	//printf(" number of char in path is %d\n", countChar(av[1]));
	
	//pathMax += countChar(av[1]);		
	
	//add num of path from parameter	
	//maxDepth += countPath(av[1]);
	
	//printf(" num of bad chars in path is %d ", pathContains(av[1], badChars));
	
	//showDir(av[1], maxDepth, pathMax);
	
}//ends main




int is_dot_or_dot_dot(char const* name)
{
   return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 );
}




void showDir(char const* dirname, int const pathDepth, int const pathMax)
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
			currFilNam = malloc(strlen(dirname) + strlen(curr_ent->d_name) + 2);

			// build path string
			strcpy(currFilNam, dirname);
			strcat(currFilNam, "/");
			strcat(currFilNam, curr_ent->d_name);
		
		//TODO how will it work when one of the parameters are missing from command
				//print if maxPath requirement is met
				if (pathDepth != 0)
				{
					if ( countPath(currFilNam) > pathDepth )
					{
						printf("%s\n", currFilNam);
					}
				}
				
				//print if num of char requirement is met
				if ( pathMax != 0 )
				{
					if ( countChar(currFilNam) > pathMax )
					{
						printf("%s\n", currFilNam);
					}
				}
				
				
				
			  //printf("%s\n", currFilNam);

			  // if directory and not equal to . and .. then do recursive on it
			  if ( curr_ent->d_type == DT_DIR && !(is_dot_or_dot_dot(curr_ent->d_name)) )
			  {
				 // recursively ist the contents of the subdirectory.
				 showDir(currFilNam, pathDepth, pathMax);
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
	
	for ( i = 0; i < strlen(path); i++)
	{
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

//check if path contains any char from array
//return 1 if true
int pathContains( char const *path, char charArr[] )
{

	int i = 0;
	int j = 0;
	int true = 0;

	for ( i = 0; i < strlen(path); i++)
	{
		for( j = 0; j < strlen(charArr); j++)
		{
			if ( path[i] == charArr[j] )
			{
				true++;	
			}
		}
	}
	
	return true;
}








