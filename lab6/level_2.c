#include "level_2.h"

/**** globals defined in main_1.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char gpath[256];
extern char *name[64];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256], parameter[256];
extern OFT oft[NOFT];

///*** Level 2 functions ///***

int open_file(int mode)
{
    int i = 0, j = 0, success = 0;
    char temp[256], *bname, *dname;
    //gets ino for file to open
    strcpy(temp, pathname);
    //dname = dirname(temp);
    //bname = basename(temp);
    //printf("parent=%s child=%s\n", dname, bname);

    //int ino = getino(dname);
    int ino = getino(temp);
	OFT *file = 0;
    if(!ino)
    {
        printf("error: file %s not found\n", temp);
        return -1;
    }
    MINODE * mip = iget(dev, ino);
    //ino = search(mip, bname);

    if(!S_ISREG(mip->INODE.i_mode))
    {
        printf("error: %s is not a regular file\n", temp);
        return -1;
    }
    while (i < NOFT)
    {
        if(oft[i].refCount != 0 && oft[i].minodePtr->ino == mip->ino)
        {
			if(oft[i].mode != 0 || mode != 0)
			{
				//Already open but not for read
				printf("error: file %s is already open with a conflicting mode\n", pathname);
				return -1;
			}
			else
			{
				// File already opened, but both will be for read
				file = &oft[i];
                oft[i].refCount++;
			}
        }
        i+=1;
    }
    i = 0;
    while(i < NOFT)
    {
        // allocate a FREE OpenFileTable (OFT) and fill in values
        if(oft[i].refCount == 0)
        {
            oft[i].mode = mode;
            oft[i].refCount = 1;
            oft[i].minodePtr = mip;
            oft[i].offset = (mode == 3 ? mip->INODE.i_size : 0);
			file = &oft[i];
		}
		if(file)
		{
			// Add file to the process
            while(j < NFD && !success)
            {
                if(!running->fd[j])
                {
					running->fd[j] = file;
					success = 1;
                    break;
                    n+=1;
                }
                j++;
            }
			if(!success)
			{
				printf("current process has too many files open\n");
				return -1;
			}

			// Prepare the inode for the format it's being opened in
            if(mode == 1)
            {
                // W: truncate file to 0 size
                truncate(mip);
            }
            //update INODE's time field
            time_t t = time(0L);
            //for R: touch atime. 
            mip->INODE.i_atime = t;
            if(mode != 0)
            {
                //for W|RW|APPEND mode : touch atime and mtime
                mip->INODE.i_mtime = t;
            }
            mip->dirty = 1;
            return j;//return j as the file descriptor

        }
        i+=1;
    }
    return -1;
}

void myopen()
{
    int open_mode;
    int fd;
    //checks mode
    if(strcmp(parameter, "R") == 0)
    {
        open_mode = 0;
    }
    else if(strcmp(parameter, "W") == 0)
    {
        open_mode = 1;
    }
    else if(strcmp(parameter, "RW") == 0)
    {
        open_mode = 2;
    }
    else if(strcmp(parameter, "APPEND") == 0)
    {
        open_mode = 3;
    }
    else{
        open_mode = -1;
        printf("usage: open filename mode\nmodes: R (read), W (write), RW (read and write), APPEND\n");
        return;
    }
    //open file
    fd = open_file(open_mode);
    if(fd >= 0)
    {
        if(fd < NFD)
        {
            printf("file %s opened with fd %d\n", pathname, fd);
        }
    }
}

int close_file(int fd)
{
	if(fd < 0 || fd >= NFD)
	{
		printf("File descriptor out of range (0-%d)\n", NFD);
		return -1;
	}

	if(!running->fd[fd])
	{
		printf("File descriptor %d is not in use\n", fd);
		return -1;
	}

	OFT *oftp = running->fd[fd];

	// Remove fd from the process
	running->fd[fd] = 0;

	if(--oftp->refCount == 0)
	{
		oftp->minodePtr->dirty = 1;
		iput(oftp->minodePtr);
	}

	return 0;
}

int myclose()
{
	if(!isdigit(pathname[0]))
	{
		printf("Usage: close [FILE DESCRIPTOR]\n");
		return -1;
	}

	int fd = atoi(pathname);
	
	close_file(fd);

	return 0;
}

int mylseek()
{
	if(!isdigit(pathname[0]) || !isdigit(parameter[0]))
	{
		printf("Usage: lseek [FILE DESCRIPTOR] [POSITION]\n");
		return -1;
	}

	int fd = atoi(pathname), position = atoi(parameter);
	if(fd < 0 || fd > NFD)
	{
		printf("Offset out of range\n");
		return -1;
	}

	OFT *file = running->fd[fd];
	int old_offset = file->offset;
	
	if(position >= 0 && position < file->minodePtr->INODE.i_size)
	{
		file->offset = position;
	}

	return old_offset;
}

int pfd()
{
	int i;
	char *modes[4] = {"READ", "WRITE", "R/W", "APPEND"};
	printf("fd\tmode\toffset\tINODE\n");

	for(i = 0; i < NFD; i++)
	{
		if(running->fd[i])
			printf("%d\t%s\t%d\t[%d, %d]\n", i, modes[running->fd[i]->mode], running->fd[i]->offset, running->fd[i]->minodePtr->dev, running->fd[i]->minodePtr->ino);
	}
	putchar('\n');
}

void dup(int fd)
{
    OFT * oftp = (running->fd[fd]);
    //if(running->fd[fd].refCount <= 0)
    if(oftp->refCount <= 0)
    {
        printf("error: file descriptor not opened\n");
        return;
    }
    else
    {
        int i = 0;
        while(running->fd[i])
        {
            //get to FIRST empty fd[ ] slot
            i += 1;
        }
        //duplicates (copy) fd[fd] into FIRST empty fd[ ] slot;
        running->fd[i]->mode = oftp->mode;
        running->fd[i]->refCount = oftp->refCount;
        running->fd[i]->minodePtr = oftp->minodePtr;
        running->fd[i]->offset = oftp->offset;
        //increment OFT's refCount by 1;
        oftp->refCount +=1;
    }
}

void dup2(int fd, int gd)
{
    //close_file(gd);
    running->fd[gd]->mode = running->fd[fd]->mode;
    running->fd[gd]->refCount = running->fd[fd]->refCount;
    running->fd[gd]->minodePtr = running->fd[fd]->minodePtr;
    running->fd[gd]->offset = running->fd[fd]->offset;

}
//

void zero_block(int dev, int blk)
{
    char buf[BLKSIZE];
    memset(buf, 0, BLKSIZE);
    put_block(dev, blk, buf);
}

void write_file()
{
    //ask for a fd   and   a text string to write;
    int fd = atoi(pathname);
    printf("fd: %d\n", fd);
    //verify fd is indeed opened for WR or RW or APPEND mode
    if(running->fd[fd]->refCount == 0)
    {
        printf("File descriptor not open\n");
        return;
    }
    if(running->fd[fd]->mode == 0)
    {
        printf("File is open for read\n");
        return;
    }
    //copy the text string into a buf[] and get its length as nbytes.
    char buffer[256];
    strcpy(buffer, parameter);
    mywrite(fd, buffer, strlen(buffer));
}

int mywrite(int fd, char buf[], int nbytes)
{
    OFT * oftp = (running->fd[fd]);
    int original_nbytes = nbytes;
    MINODE * mip = oftp->minodePtr;
    char * cq = buf;
    while(nbytes > 0)
    {
        // compute LOGICAL BLOCK (lbk) and the startByte in that lbk:
        int lbk = oftp->offset / BLKSIZE;
        int startByte = oftp->offset % BLKSIZE;
        int blk;

        if(lbk < 12)// direct block
        {
            if(mip->INODE.i_block[lbk] == 0) // if no data block yet
                {
                    mip->INODE.i_block[lbk] = balloc(mip->dev);// MUST ALLOCATE a block
                }
            blk = mip->INODE.i_block[lbk]; // blk should be a disk block now
        }
        else if(lbk >= 12 && lbk < 256 + 12)// INDIRECT blocks 
        {
            if(mip->INODE.i_block[12] == 0)
            {
                //allocate a block for it;
                mip->INODE.i_block[12] = balloc(mip->dev);
                //zero out the block on disk
                zero_block(mip->dev, mip->INODE.i_block[12]);
            }
            //get i_block[12] into an int ibuf[256];
            uint32_t ibuf[256];
            get_block(mip->dev, mip->INODE.i_block[12], (char*)ibuf);
            blk = ibuf[lbk - 12];
            if(blk == 0)
            {
                //allocate a disk block;
                if((blk = ibuf[lbk - 12] = balloc(mip->dev)) == 0)
                {
                    printf("error: ran out of disk space\n");
                    return original_nbytes - nbytes;
                }
                //record it in i_block[12];
                put_block(mip->dev, mip->INODE.i_block[12], (char*)ibuf);
            }
        }
        else
        {
            // double indirect blocks */
            int indirect1 = (lbk - 256 - 12) / 256;
            int indirect2 = (lbk - 256 - 12) % 256;
            //get i_block[13] into an int ibuf[256];
            uint32_t ibuf[256];
            if(mip->INODE.i_block[13] == 0)
            {
                 //allocate a disk block;
                mip->INODE.i_block[13] = balloc(mip->dev);
                 //zero out the block on disk
                zero_block(mip->INODE.i_block[13], mip->INODE.i_block[13]);
            }
            //get i_block[13] into an int ibuf[256]
            get_block(mip->dev, mip->INODE.i_block[13], (char*)ibuf);
            if(ibuf[indirect1] == 0)// if no data block yet
            {
                //allocate a disk block;
                ibuf[indirect1] = balloc(mip->dev);
                   //zero out the block on disk
                zero_block(mip->dev, ibuf[indirect1]);
                //record it in i_block[13];
                put_block(mip->dev, mip->INODE.i_block[13], (char*)ibuf);
            }
            uint32_t ibuf2[256];
            //get i_block[indirect2] into an int ibuf[256]
            get_block(mip->dev, ibuf[indirect1], (char*)ibuf2);
            if(ibuf2[indirect2] == 0)
            {
                //allocate a disk block;
                ibuf2[indirect2] = balloc(mip->dev);
                  //zero out the block on disk
                zero_block(mip->dev, ibuf2[indirect2]);
                //record it in i_block[indirect1];
                put_block(mip->dev, ibuf[indirect1], (char*)ibuf2);
            }
            blk = ibuf2[indirect2];
        }

        char wbuf[BLKSIZE];
        //zero out the block on disk
        //zero_block(mip->dev, blk);
        get_block(mip->dev, blk, wbuf);// read disk block into wbuf[ ]  
        char * cp = wbuf + startByte;// cp points at startByte in wbuf[]
        int remain = BLKSIZE - startByte; // number of BYTEs remain in this block

        while(remain > 0)// write as much as remain allows
        {
            *cp++ = *cq++;//// cq points at buf[ ]
            nbytes--;// dec counts
            remain--;
            oftp->offset++;// advance offset
            if(oftp->offset > mip->INODE.i_size)//// especially for RW|APPEND mode
            {
                mip->INODE.i_size++;// // inc file size (if offset > fileSize)
            }
                
            if(nbytes <= 0)// // if already nbytes, break
            {
                break;
            }
        }
        put_block(mip->dev, blk, wbuf);   // write wbuf[ ] to disk
    //loop back to outer while to write more .... until nbytes are written
    }
    mip->dirty = 1; // mark mip dirty for iput()
    printf("wrote %d char into the file descriptor fd=%d\n", original_nbytes, fd);
    return nbytes;
}

int read_file()
{

    int fd = atoi(pathname) ;
    int nbytes = atoi(parameter);//ask for a fd  and  nbytes to read;
    char buf[nbytes + 1];
    //verify that fd is indeed opened for RD or RW;
    if(running->fd[fd] == 0 || (running->fd[fd]->mode != 0 && running->fd[fd]->mode != 2))
    {
        printf("error: file descriptor %d is not open for read.\n", fd);
        return;
    }
    int ret = myread(fd, buf, nbytes);//read file
    buf[ret] = 0;//set end char to 0
    printf("%s\n", buf);//print file contents
}

int myread(int fd, char buf[], int nbytes)
{
    int cur_byte = running->fd[fd]->offset;//he offset in the OFT points 
    //to the current byte position in the file from where we wish to read nbytes.
    OFT *oftp = running->fd[fd];
    
    MINODE *mip = running->fd[fd]->minodePtr;
    int avil = mip->INODE.i_size - cur_byte;// number of bytes still available in file.
    int count = 0;
    char *cq = buf;
    uint32_t dbuf[256];
    int blk;
    while(nbytes && avil)
    {
        int cur_byte = running->fd[fd]->offset;//he offset in the OFT points 
    //to the current byte position in the file from where we wish to read nbytes.
    //Compute LOGICAL BLOCK number lbk and startByte in that block from offset
        int lbk = oftp->offset / BLKSIZE;
        int start = oftp->offset % BLKSIZE;
    
        if(lbk < 12)// lbk is a direct block
        {   
            blk = mip->INODE.i_block[lbk];// map LOGICAL lbk to PHYSICAL blk
        }
        else if(lbk >= 12 && lbk < 256 + 12)
        {
            //indirect blocks
            get_block(mip->dev,mip->INODE.i_block[12], (char*)dbuf);// Get to blocks from indirect blocks
            blk = dbuf[lbk - 12];
        }
        else
        {
            //double indirect blocks
            get_block(mip->dev,mip->INODE.i_block[13], (char*)dbuf);// Get to indirect blocks from double indirect
            get_block(mip->dev, dbuf[(lbk - 268) / 256], (char*)dbuf); // Get to indirect block
            blk = dbuf[(lbk-268) % 256];// Save direct block value to blk

        }

        /* get the data block into readbuf[BLKSIZE] */
        char readbuf[BLKSIZE];
        get_block(mip->dev, blk, readbuf);
            /* copy from startByte to buf[ ], at most remain bytes in this block */
        char *cp = readbuf + start;
        int remain = BLKSIZE - start;   // number of bytes remain in readbuf[]
        while(remain > 0)
        {
            *cq++ = *cp++;// copy byte from readbuf[] into buf[]
        // running->fd[fd].offset++;// advance offset 
            oftp->offset++;
            count++;// inc count as number of bytes read
            avil--;
            nbytes--;
            remain--;
            if(nbytes <= 0 || avil <= 0)
            {
                break;
            }
        // if one data block is not enough, loop back to OUTER while for more ...
        }
    }
    printf("myread: read %d char from file descriptor %d\n", count, fd);
    return count; // count is the actual number of bytes read
}

int mycat()
{
	int fd = open_file(0), n = 0, size = running->fd[fd]->minodePtr->INODE.i_size;
	char buf[size + 1];

	if(fd)
	{
		n = myread(fd, buf, size);
		buf[n] = 0;
		printf(buf);
	}

	return n;
}

///*** End level 2 functions ///***
