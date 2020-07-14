/*
 * AlterEgo: seamless integration of chroot applications
 * Copyright (C) 2012-2020 by Jody Bruchon <jody@jodybruchon.com>
 * Distributed under The MIT License
*/

#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"

const char program_version[] = "1.2";

#ifndef DEFAULTROOT
#define DEFAULTROOT "/i386"
#endif

int main(int argc, char **argv) {
	char prog[PATH_MAX];
	static char rootdir[PATH_MAX] = DEFAULTROOT;
	char *p;	/* for parsing options */
	int status = 0;

	/* Behave differently if invoked as our own name */
	if (strncmp(basename(prog), "alterego", PATH_MAX) == 0) {
		if (argc < 2) {
			fprintf(stderr, "No parameters specified.\n");
			status = 255; goto usage;
		}

		/* Parse command-line options if present */
		if (*argv[1] == '-') {
			p = argv[1];
			p++;
			switch (*p) {
			default:
			case '\0':
				fprintf(stderr, "You must specify a valid option after the '-'\n");
				status = 255; goto usage;
			case 'r':
				if (argc < 4) {
					fprintf(stderr, "You must specify the root after the -r switch\n");
					status = 255; goto usage;
				}
				strncpy(rootdir, argv[2], PATH_MAX);
				break;
			}
		}
	}

	/* Use absolute path if slashes used in invocation */
	if (strchr(argv[0], '/') != NULL) {
		getcwd(prog, PATH_MAX);
		strcat(prog, "/");
		strcat(prog, argv[0]);
	} else {
		strcpy(prog, argv[0]);
	}

	if (chdir(rootdir)) {
		fprintf(stderr, "Change directory to %s failed.\n", rootdir);
		status = 1; goto usage;
	}
	if (chroot(rootdir)) {
		fprintf(stderr, "Change root to %s failed.\n", rootdir);
		status = 2; goto usage;
	}
	if (seteuid(getuid())) {
		fprintf(stderr, "seteuid() to the current user ID failed.\n");
		status = 3; goto usage;
	}

	if (execvp(prog, argv)) {
		fprintf(stderr, "Could not execute %s in %s\n", prog, rootdir);
		status = 4; goto usage;
	}

	/* Fall through - this should never be reached */
	fprintf(stderr, "This message indicated a bug, please report it.\n");
	status = 128;

usage:
	printf("\nAlterEgo version %s by Jody Lee Bruchon <jody@jodybruchon.com>\n", program_version);
	printf("This copy uses the following default root path: %s\n\n", DEFAULTROOT);
	printf(
	"This program attempts to execute a program with the same name\n"
	"as itself under a hard-coded chroot, for example to run a 32-bit\n"
	"userland on a 64-bit machine. It was written to allow seamless\n"
	"mixing of two potentially incompatible userlands.\n\n"
	"For example, if the machine is x86_64 and you create a complete\n"
	"i386 system under /i386, and you install a 32-bit Firefox under\n"
	"/i386/usr/bin/firefox, any x86_64 program that attempts to run\n"
	"/usr/bin/firefox (or just firefox) will run the 32-bit Firefox\n"
	"transparently with no extra configuration. Simply symlink to\n"
	"AlterEgo, i.e. ln -s /usr/bin/alterego /usr/bin/firefox\n\n"
	);
	exit(status);
}

