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

int gets(char *s)
{ 
	int i = 0;
	char c;

	while((c = getc()) != '\r')
	{
		putc(c);
		s[i] = c;
		i++;
	}
	s[i] = 0;

	return i;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
  // search for name in the data block of INODE; 

  // return its inumber if found

  // else error();
}

main()
{ 

//1. Write YOUR C code to get the INODE of /boot/mtx
  // INODE *ip --> INODE

   //if INODE has indirect blocks: get i_block[12] int buf2[  ]


//2. setes(0x1000);  // MTX loading segment = 0x1000

//3. load 12 DIRECT blocks of INODE into memory
   for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }

//4. load INDIRECT blocks, if any, into memory
   if (ip->i_block[12]){
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }
  prints("go?"); getc();
}
