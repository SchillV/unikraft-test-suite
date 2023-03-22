#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <bits/struct_stat.h>
#define TST_TMPFS_MAGIC    0x01021994
#define TCONF 32