#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mntpoint"
#define T_MSG "this is a test string"

struct test_case {
	char *desc;
	char *path;
	int flags;
	mode_t mode;
	int err;
} tc[] = {
		.desc = "Open regular file O_RDONLY",
		.path = T_REG_EMPTY,
		.flags = O_RDONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open regular file O_WRONLY",
		.path = T_REG_EMPTY,
		.flags = O_WRONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open regular file O_RDWR",
		.path = T_REG_EMPTY,
		.flags = O_RDWR,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open regular file O_RDWR | O_SYNC",
		.path = T_REG_EMPTY,
		.flags = O_RDWR | O_SYNC,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open regular file O_RDWR | O_TRUNC",
		.path = T_REG_EMPTY,
		.flags = O_RDWR | O_TRUNC,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open dir O_RDONLY",
		.path = T_DIR,
		.flags = O_RDONLY,
		.mode = 0755,
		.err = 0,
	},
		.desc = "Open dir O_RDWR, expect EISDIR",
		.path = T_DIR,
		.flags = O_RDWR,
		.mode = 0755,
		.err = EISDIR,
	},
		.desc = "Open regular file O_DIRECTORY, expect ENOTDIR",
		.path = T_REG_EMPTY,
		.flags = O_RDONLY | O_DIRECTORY,
		.mode = 0644,
		.err = ENOTDIR,
	},
		.desc = "Open hard link file O_RDONLY",
		.path = T_LINK_REG,
		.flags = O_RDONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open hard link file O_WRONLY",
		.path = T_LINK_REG,
		.flags = O_WRONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open hard link file O_RDWR",
		.path = T_LINK_REG,
		.flags = O_RDWR,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open sym link file O_RDONLY",
		.path = T_SYMLINK_REG,
		.flags = O_RDONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open sym link file O_WRONLY",
		.path = T_SYMLINK_REG,
		.flags = O_WRONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open sym link file O_RDWR",
		.path = T_SYMLINK_REG,
		.flags = O_RDWR,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open sym link dir O_RDONLY",
		.path = T_SYMLINK_DIR,
		.flags = O_RDONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open sym link dir O_WRONLY, expect EISDIR",
		.path = T_SYMLINK_DIR,
		.flags = O_WRONLY,
		.mode = 0644,
		.err = EISDIR,
	},
		.desc = "Open sym link dir O_RDWR, expect EISDIR",
		.path = T_SYMLINK_DIR,
		.flags = O_RDWR,
		.mode = 0644,
		.err = EISDIR,
	},
		.desc = "Open device special file O_RDONLY",
		.path = T_DEV,
		.flags = O_RDONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open device special file O_WRONLY",
		.path = T_DEV,
		.flags = O_WRONLY,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open device special file O_RDWR",
		.path = T_DEV,
		.flags = O_RDWR,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open non-existing regular file in existing dir",
		.path = T_DIR"/"T_NEW_REG,
		.flags = O_RDWR | O_CREAT,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open link file O_RDONLY | O_CREAT",
		.path = T_LINK_REG,
		.flags = O_RDONLY | O_CREAT,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open symlink file O_RDONLY | O_CREAT",
		.path = T_SYMLINK_REG,
		.flags = O_RDONLY | O_CREAT,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open regular file O_RDONLY | O_CREAT",
		.path = T_REG_EMPTY,
		.flags = O_RDONLY | O_CREAT,
		.mode = 0644,
		.err = 0,
	},
		.desc = "Open symlink dir O_RDONLY | O_CREAT, expect EISDIR",
		.path = T_SYMLINK_DIR,
		.flags = O_RDONLY | O_CREAT,
		.mode = 0644,
		.err = EISDIR,
	},
		.desc = "Open dir O_RDONLY | O_CREAT, expect EISDIR",
		.path = T_DIR,
		.flags = O_RDONLY | O_CREAT,
		.mode = 0644,
		.err = EISDIR,
	},
		.desc = "Open regular file O_RDONLY | O_TRUNC, "
			"behaviour is undefined but should not oops or hang",
		.path = T_REG_EMPTY,
		.flags = O_RDONLY | O_TRUNC,
		.mode = 0644,
		.err = -1,
	},
		.desc = "Open regular file(non-empty) O_RDONLY | O_TRUNC, "
			"behaviour is undefined but should not oops or hang",
		.path = T_REG,
		.flags = O_RDONLY | O_TRUNC,
		.mode = 0644,
		.err = -1,
	},
};

int  verify_open(unsigned int n)
{
	if (tc[n].err > 0) {
		TST_EXP_FAIL2(open(tc[n].path, tc[n].flags, tc[n].mode),
		             tc[n].err, "%s", tc[n].desc);
	} else if (tc[n].err == 0) {
		TST_EXP_FD(open(tc[n].path, tc[n].flags, tc[n].mode),
		           "%s", tc[n].desc);
	} else {
open(tc[n].path, tc[n].flags, tc[n].mode);
		tst_res(TPASS, "%s", tc[n].desc);
	}
	if (TST_RET > 0)
		close(TST_RET);
}

void setup(void)
{
	int fd;
	int ret;
	fd = open(T_REG, O_WRONLY | O_CREAT, 0644);
	ret = write(fd, T_MSG, sizeof(T_MSG));
	if (ret == -1) {
		close(fd);
		tst_brk(TBROK | TERRNO, "Write %s failed", T_REG);
	}
	close(fd);
	touch(T_REG_EMPTY, 0644, NULL);
	link(T_REG, T_LINK_REG);
	symlink(T_REG, T_SYMLINK_REG);
	mkdir(T_DIR, 0755);
	symlink(T_DIR, T_SYMLINK_DIR);
	mknod(T_DEV, S_IFCHR, makedevT_DEV, S_IFCHR, makedev1, 5));
}

void main(){
	setup();
	cleanup();
}
