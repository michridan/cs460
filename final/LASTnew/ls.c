#include "ucode.c"
#include <dirent.h>

int ls_file(int ino)
{
  MINODE *mip = iget(dev, ino);
	INODE stat = mip->INODE;
    // Use INODE contents to ls it 
	char ftime[64], mods[3] = { 'x', 'w', 'r' };
	int i;

	// print mode
	if (S_ISREG(stat.i_mode))
		putchar('-');
	else if (S_ISDIR(stat.i_mode))
		putchar('d');
	else if (S_ISLNK(stat.i_mode))
		putchar('l');

	for (i=8; i >= 0; i--)	
	{
		if (stat.i_mode & (1 << i)) // print r|w|x
			putchar(mods[i % 3]);
		else
			putchar('-');
	}

	// print links, owners, and size
	printf("%4d ", stat.i_links_count);
	printf("%4d ", stat.i_uid);
	printf("%4d ", stat.i_gid);
	printf("%8d ", stat.i_size);

	// can't figure out how to print the time
	//strcpy(ftime, ctime(&stat.i_atime));
	//ftime[strlen(ftime)-1] = 0;
	//printf("%s  ",ftime);

	// print name at the upper level
	return 0;
}

int ls_dir(char *dirname)
{
	char buf[BLKSIZE];
    int ino;
    MINODE *mip;
	int i;
	char temp[256], *cp;

	if(!strcmp(dirname, ""))
	{
		mip = running->cwd;
	}
	else
	{
		ino = getino1(dirname);
		mip = iget(dev, ino);
	}

	for (i=0; i < 12; i++)
	{ 
		if (mip->INODE.i_block[i] == 0)
			break;
		get_block(dev, mip->INODE.i_block[i], buf);
	    dp = (DIR *)buf;
		cp = buf;

	    while (cp < buf + BLKSIZE)
		{
		   strncpy(temp, dp->name, dp->name_len);
	       temp[dp->name_len] = 0;

		   ls_file(dp->inode);
		   printf("%s\n", temp);
	       cp += dp->rec_len;
		   dp = (DIR *)cp;
		   
	    }
	}
	return 0;
}

int ls(char *name)
{
    int ino;
    MINODE *mip;
	INODE check;

	if(!strcmp(name, ""))
	{
		mip = running->cwd;
		check = mip->INODE;
	}
	else
	{
		ino = getino1(name);
		mip = iget(dev, ino);
		check = mip->INODE;
	}
	// Check if reg or dir, use correct ls
	if (S_ISDIR(check.i_mode))
	{
		ls_dir(name);
	}
	else
	{
		ls_file(ino);
		// Print name
		printf("%s\n", basename(name));
	}

	return 1;
}

int main(int argc, char *argv[])
{
    char temp[256];
    int ino;
    if(argc > 2)
    {
        strcpy(pathname, ".");
        ino = running->cwd->ino;
        //gets ino for cwd to print contents of current path
    }
    else
    {
        strcpy(temp, pathname);
        ino = getino(temp);
        //get ino for specified path
    }

    MINODE * mip = iget(dev, ino);
    //gets MINODE for specified path
    if(S_ISDIR(mip->INODE.i_mode))
    {
        //get ino for the DIR
        int d_ino = getino(pathname);
        //path leads to DIR so ls the DIR
        DIR_ls(pathname, d_ino);
    }
    else
    {
        //path leads to file so ls the file
        file_ls(pathname, ino);
    }
   iput(mip);
 
}
