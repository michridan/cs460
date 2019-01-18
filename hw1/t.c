/*******************************************************
*       t.c, skeleton code from KC's example           *
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

static char * boot[] = {"boot", "mtx"};

int prints(char *s)
{
	int i = 0;

	while(s[i])
	{
		putc(s[i]);
		i++;
	}
	return i;
}

void gets(char *s)
{ 
	char c;
	while((c = getc()) != '\r')
	{
		putc((*s++ = c));
	}
	*s = 0;
}


char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
    int i;
    DIR * dp;
    char * cp;
    char temp[64];

    for(i = 0; i < 12 && ip->i_block[i] != 0; i++)
    {
        getblk((u16)ip->i_block[i], buf2);
        cp = buf2;
        dp = (DIR*)buf2;

        while(cp < buf2 + BLK)
        {
			strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';
            if(strcmp(temp, name) == 0)
                return (u16)dp->inode - 1;
            dp = (DIR*)(cp += dp->rec_len);
        }

    }

	error();
}

main()
{ 
	INODE *ip;
	int iblk, ino, i;
	u32 *up;

//1. Write YOUR C code to get the INODE of /boot/mtx
	getblk(2, buf1);
	iblk = ((GD*)buf1)->bg_inode_table;
	getblk(iblk, buf1);
	ip = (INODE *)buf1 + 1;

	for(i = 0; i < 2; i++)
	{
		ino = search(ip, boot[i]);
		getblk(iblk + ino / 8, buf1);
		ip = (INODE *)buf1 + ino % 8;
	}

   //if INODE has indirect blocks: get i_block[12] int buf2[  ]
	getblk((u16)ip->i_block[12], buf2);
	up = (u32 *)buf2;      

	setes(0x1000);  // MTX loading segment = 0x1000

//3. load 12 DIRECT blocks of INODE into memory
   for (i=0; i<12; i++){
      getblk((u16)(ip->i_block[i]), 0);
      putc('*');
      inces();
   }

//4. load INDIRECT blocks, if any, into memory
	while(*up)
	{
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
	}
  prints("ready?");getc();
}
