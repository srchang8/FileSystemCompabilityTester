#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>

/**
 **	a simplified version of pwd
 **
 **	Starts in current directory and recursively
 **	   climbs up to root of filesystem, prints top part
 **	   then prints current part
 **	Uses readdir() to get info about each thing
 **/

void	get_info(char*, dev_t *, ino_t *);
void    printpathto(dev_t, ino_t);
void	inum_to_name(dev_t , ino_t , char *);

int main ( int ac, char *av[] )
{
	ino_t	myinode ;
	dev_t	mydev   ;
	

	//connects inode and dev for the specified path
	get_info(".", &mydev, &myinode);
	
	//takes in dev and inode to print out path to get there
	printpathto( mydev, myinode );		/* print path to here	*/
	
	putchar('\n');				/* then add newline	*/
	return 0;
}



void printpathto( dev_t this_dev, ino_t this_inode )
/*
 *	prints path leading down to an object with this dev,inode
 */
{
	ino_t	myinode , par_inode;
	dev_t	mydev,    par_dev;
	char	its_name[BUFSIZ];

	chdir( ".." );					/* up one dir	*/

	inum_to_name(this_dev,this_inode,its_name );	/* get its name	*/

	get_info(".",&mydev,&myinode);			/* print head	*/
	get_info("..",&par_dev,&par_inode);
	
	if ( myinode != par_inode || mydev != par_dev )
		printpathto( mydev, myinode );		/* recursively	*/
	printf("/%s", its_name );			/* now print	*/
							/* name of this	*/
}

/*
 * gets inode and dev for the specified path, uses lstat
 */
void get_info(char *path, dev_t *dp, ino_t *inp)
{
	struct stat info;

	//get information on path and put it in info
	if ( lstat(path, &info) == -1 ){
		perror(path);
		exit(2);
	}
	*inp = info.st_ino;
	*dp  = info.st_dev;
}

//get file name using inode from current directory	
void inum_to_name(dev_t dev_to_find, ino_t inode_to_find , char *namebuf)
/*
 *	looks through current directory for a file with this inode
 *	number and copies its name into namebuf
 */
{
	DIR		*dir_ptr;		/* the directory */
	struct dirent	*direntp;		/* each entry	 */
	ino_t		cur_inum;
	dev_t		cur_dev;

	dir_ptr = opendir( "." );
	if ( dir_ptr == NULL ){
		fprintf(stderr, "cannot open a directory\n");
		exit(1);
	}
	/*
	 * search directory for a file with specified inum and device
	 */
	while ( ( direntp = readdir( dir_ptr ) ) != NULL ){
		get_info(direntp->d_name, &cur_dev, &cur_inum);
		if ( cur_inum == inode_to_find && cur_dev == dev_to_find )
		{
			strcpy( namebuf, direntp->d_name );
			closedir( dir_ptr );
			return ;
		}
	}
	fprintf(stderr, "error looking for inum %d\n", (int) inode_to_find);
	exit(1);
}
