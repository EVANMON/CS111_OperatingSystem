//
//  main.c
//  lab 3A
//
//  Created by Zixuan Wang on 5/28/18.
//  Copyright © 2018 Zoe. All rights reserved.
//

#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "ext2_fs.h"

struct ext2_super_block superblock;
struct ex2_group_desc groupdesc;
int fd = -1;

void sum_superblock()
{
    if (pread(fd, &superblock, sizeof(struct ext2_super_block), 1024) < 0)
    {
        fprintf(stderr, "error reading superblock. \n");
        exit(2);
    }
    printf("SUPERBLOCK,");
    printf("%d,%d,%d,%hd,%d,%d,%d\n", superblock.s_blocks_count, superblock.s_inodes_count, 1024 << superblock.s_log_block_size, superblock.s_inode_size, superblock.s_blocks_per_group, superblock.s_inodes_per_group,superblock.s_first_ino);
}

void sum_group()
{
    if (pread(fd, &groupdesc, 32, 2048) < 0)
    {
        fprintf(stderr, "error reading group. \n");
        exit(2);
    }
    printf("GROUP,");
    printf("%d,", 0);
    printf("%d,%d,%hd,%hd,%d,%d,%d\n", superblock.s_blocks_count, superblock.s_inodes_count, groupdesc.bg_free_blocks_count, groupdesc.bg_free_inodes_count, groupdesc.bg_block_bitmap, groupdesc.bg_inode_bitmap, groupdesc.bg_inode_table);
}

void sum_freeblock()
{
    int bBitmap_size = 1024;
    unsigned char* bBitmap_buffer = malloc(bBitmap_size);
    if (bBitmap_buffer == NULL)
    {
        fprintf(stderr, "error allocating bitmap buffer. \n");
        exit(2);
    }
    int bBitmap_offset = (1024 << superblock.s_log_block_size) * groupdesc.bg_block_bitmap;
    if (pread(fd, bBitmap_buffer, bBitmap_size, bBitmap_offset) < 0)
    {
        fprintf(stderr, "error reading block bitmap. \n");
        exit(2);
    }
    int i;
    int j;
    int n_freeb = 0;
    for (i = 0; i < bBitmap_size; i++)
    {
        char c = bBitmap_buffer[i];
        for (j = 0; j < 8; j++)
        {
            int aixinnan = (1 << j);
            if ((aixinnan & c) == 0)
            {
                printf("BFREE,");
                printf("%d\n", i*8+j+1);
            }
        }
    }
}

void sum_freeinode()
{
    int iBitmap_size = 1024;
    unsigned char* iBitmap_buffer = malloc(iBitmap_size);
    if (iBitmap_buffer == NULL)
    {
        fprintf(stderr, "error allocating bitmap buffer. \n");
        exit(2);
    }
    int iBitmap_offset = (1024 << superblock.s_log_block_size) * groupdesc.bg_inode_bitmap;
    if (pread(fd, iBitmap_buffer, iBitmap_size, iBitmap_offset) < 0)
    {
        fprintf(stderr, "error reading inode bitmap. \n");
        exit(2);
    }
    int allocated[superblock.s_inodes_per_group];
    int i;
    int j;
    for(i = 0; i < iBitmap_size; i++)
    {
        char c = iBitmap_buffer[i];
        for (j = 0; j < 8; j++)
        {
            int num_inodes = i * 8 + j + 1;
            if (num_inodes <= superblock.s_inodes_per_group)
            {
                int aixinnan (1 << j);
                if (!(aixinnan & c))
                {
                    printf("IFREE,");
                    printf("&d\n", num_inodes);
                    allocated[num_inodes-1] = 0;
                }
                else
                {
                    allocated[num_inodes-1] = 1;
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    fd = open(argv[1], O_RDONLY);
    void sum_superblock();
    void sum_group();
    void sum_freeblock();
    void sum_freeinode();
    return 0;
}
