/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

main()
{ 
  u16    i, iblk;
  char   *cp, temp[64];

  prints("read block# 2 (GD)\n\r");
  getblk(2, buf1);

// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = buf1;
  iblk = gp->bg_inode_table;
  prints("inode_block="); putc(iblk+'0'); prints("\n\r"); 

// 2. WRITE YOUR CODE to get root inode
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;
  prints("read inodes begin block to get root inode\n\r");

// 3. WRITE YOUR CODE to step through the data block of root inode
   prints("read data block of root DIR\n\r");

   for(i = 0; i < 12 && ip->i_block[i]; i++)
   {
	   getblk((u16)ip->i_block[i], buf2);
	   dp = (DIR *)buf2;
	   cp = buf2;

	   while(cp - buf2 < BLK)
	   {
// 4. print file names in the root directory /
		   strncpy(temp, dp->name, dp->name_len);
		   temp[dp->name_len] = 0;
		   prints(temp); prints("\n\r");
		   cp += dp->rec_len;
		   dp = (DIR *)cp;
	   }
   }
   getc();
   return 1;
}  

int prints(char *s)
{
	int i = 0;
	while(s[i])
		putc(s[i++]);
	return i;
}

int gets(char *s) 
{ 
	int i = 0;
	char c;
	while((c = getc()) != '\r')
	{
		putc(c);
		s[i++] = c;
	}
	s[i] = 0;
	return i;
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}
