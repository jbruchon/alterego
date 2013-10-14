/*
  AlterEgo: seamless integration of chroot applications
  Copyright (C) 2012 by Jody Lee Bruchon <jody@c02ware.com>

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

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef SMALL
#include <stdio.h>
#endif

#include "config.h"

#ifndef SMALL
int usage(char **argv);
const char program_version[] = "1.0.1";
#endif

#ifndef ROOT
#define ROOT "/i386"
#endif

const char archdir[] = ROOT;

int main(int argc, char **argv) {
	char prog[PATH_MAX];

	/* Use absolute path if slashes used in invocation */
	if(strchr(argv[0], '/') != NULL) {
		getcwd(prog, PATH_MAX);
		strcat(prog, "/");
		strcat(prog, argv[0]);
	} else {
		strcpy(prog, argv[0]);
	}

	if(chdir(archdir) == -1) {
#ifndef SMALL
		printf("Change directory to %s failed.\n", archdir);
		usage(argv);
#endif
		exit(1);
	}
	if(chroot(archdir) == -1) {
#ifndef SMALL
		printf("Change root to %s failed.\n", archdir);
		usage(argv);
#endif
		exit(1);
	}
	if(seteuid(getuid()) == -1) {
#ifndef SMALL
		printf("seteuid() to the current user ID failed.\n");
		usage(argv);
#endif
		exit(1);
	}

	if(execvp(prog,argv)) {
#ifndef SMALL
		printf("Could not execute %s in %s\n", prog, archdir);
#endif
	}
#ifndef SMALL
	usage(argv);
#endif
	exit(1);
}

#ifndef SMALL
int usage(char **argv) {
	/* Program usage */
	printf("\nAlterEgo version %s by Jody Lee Bruchon <jody@c02ware.com>\n", program_version);
	printf("This program attempts to execute a program with the same name\n");
	printf("as itself under a hard-coded chroot, for example to run a 32-bit\n");
	printf("userland on a 64-bit machine. It was written to allow seamless\n");
	printf("mixing of two potentially incompatible userlands.\n\n");
	printf("For example, if the machine is x86_64 and you create a complete\n");
	printf("i386 system under /i386, and you install a 32-bit Firefox under\n");
	printf("/i386/usr/bin/firefox, any x86_64 program that attempts to run\n");
	printf("/usr/bin/firefox (or just firefox) will run the 32-bit Firefox\n");
	printf("transparently with no extra configuration. Simply symlink to\n");
	printf("AlterEgo, i.e. ln -s /usr/bin/alterego /usr/bin/firefox\n\n");
	printf("This copy uses the following root path: %s\n\n",archdir);
	exit(1);
}
#endif

