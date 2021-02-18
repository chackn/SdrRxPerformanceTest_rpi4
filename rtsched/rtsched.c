#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <limits.h>
#include <errno.h>

int main(int argc, char **argv)
{
	struct sched_param sp;
	int policy;
	int ret;
	char path[PATH_MAX];
	char *cmd, *p, *p2;
	int len;
	
	if (argc < 3) {
		fprintf(stderr, "usage: %s rr/fifo <prio> [<command> [<arg> ...]]\n", argv[0]);
		exit(1);
	}

	if (strcasecmp(argv[1], "rr") == 0)
		policy = SCHED_RR;
	else if (strcasecmp(argv[1], "fifo") == 0)
		policy = SCHED_FIFO;
	else
		policy = SCHED_OTHER;

	sp.sched_priority = atoi(argv[2]);
	ret = sched_setscheduler(0, policy, &sp);
	if (ret) {
		perror("sched_setscheduler");
		return ret;
	}
	
	if (argc == 3) {
		ret = execl("/bin/sh", NULL);
		perror("execl");
		return ret;
	}
	
	cmd = argv[3];
	if (index(cmd, '/')) {
		ret = execv(cmd, argv+3);
		perror("execv");
		return ret;
	}
	
	ret = execv(cmd, argv+3);

	p2 = getenv("PATH");
	while (p = p2) {
		p2 = index(p, ':');
		len = p2 ? p2 - p : strlen(p);
		if (len < PATH_MAX-2) { // ignore too long path
			memcpy(path, p, len);
			path[len] = '/';
			path[len+1] = 0;
			strncat(path, cmd, PATH_MAX-1);
			ret = execv(path, argv+3);
		}
		if (p2) p2++; // next directory in the PATH
	}
	
	fprintf(stderr, "%s: command not found\n", cmd);
	return errno;
	
}
