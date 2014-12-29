/*
  AlterEgo: seamless integration of chroot applications
  Copyright (C) 2012, 2013 by Jody Lee Bruchon <jody@c02ware.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"

const char program_version[] = "1.1";

#ifndef DEFAULTROOT
#define DEFAULTROOT "/i386"
#endif

int usage(char **argv, int status) {
	/* Program usage */
	printf("\nAlterEgo version %s by Jody Lee Bruchon <jody@c02ware.com>\n", program_version);
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

int main(int argc, char **argv) {
	char prog[PATH_MAX];
	char temp[PATH_MAX];
	char rootdir[PATH_MAX] = DEFAULTROOT;
	char *p;	/* for parsing options */
	int oc;		/* which option is the program to exec? */

	strncpy(temp, argv[0], PATH_MAX);
	if (strncmp(basename(prog), "alterego", PATH_MAX) == 0) {
		if (argc < 2) {
			fprintf(stderr, "No parameters specified.\n");
			usage(argv, 255);
		}

		/* Parse command-line options if present */
		if (*argv[1] == '-') {
			p = argv[1];
			p++;
			if (*p == '\0') {
				fprintf(stderr, "You must specify an option after the '-'\n");
				usage(argv,255);
			}
			if (*p == 'r') {
				if (argc < 4) {
					fprintf(stderr, "");
					usage(argv, 255);
				}
				strncpy(rootdir, argv[2], PATH_MAX);
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
		printf("Change directory to %s failed.\n", rootdir);
		usage(argv, 1);
	}
	if (chroot(rootdir)) {
		printf("Change root to %s failed.\n", rootdir);
		usage(argv, 2);
	}
	if (seteuid(getuid())) {
		printf("seteuid() to the current user ID failed.\n");
		usage(argv, 3);
	}

	if (execvp(prog,argv)) {
		printf("Could not execute %s in %s\n", prog, rootdir);
	}
	usage(argv, 4);
}

