/*
 * Copyright 2008 sempr <iamsempr@gmail.com>
 *
 * Refacted and modified by zhblue<newsclan@gmail.com> 
 * Bug report email newsclan@gmail.com
 * 
 * This file is part of HUSTOJ.
 *
 * HUSTOJ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HUSTOJ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HUSTOJ. if not, see <http://www.gnu.org/licenses/>.
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1024
#define STD_MB 1048576
#define OJ_CI 2

static char oj_home  [] = "/home/judge";
static int port_number = 80;
static int max_running = 2;
static char http_baseurl[BUFFER_SIZE] = "http://172.40.25.231";
static char http_username[BUFFER_SIZE] = "judger";
static char http_password[BUFFER_SIZE] = "123456";
static bool STOP=false;

void call_for_exit(int s)
{
	STOP = true;
	printf("Stopping judged...\n");
}

void run_client(int runid,int clientid){
	char buf[BUFFER_SIZE], runidstr[BUFFER_SIZE];
	struct rlimit LIM;
	LIM.rlim_max = 800;
	LIM.rlim_cur = 800;
	setrlimit(RLIMIT_CPU,&LIM);

	LIM.rlim_max = 80*STD_MB;
	LIM.rlim_cur = 80*STD_MB;
	setrlimit(RLIMIT_FSIZE,&LIM);

	LIM.rlim_max = STD_MB<<11;
	LIM.rlim_cur = STD_MB<<11;
	setrlimit(RLIMIT_AS,&LIM);
		
	LIM.rlim_cur = LIM.rlim_max = 200;
	setrlimit(RLIMIT_NPROC, &LIM);

	sprintf(runidstr, "%d", runid);
	sprintf(buf, "%d", clientid);

	execl("/usr/bin/judge_client", "/usr/bin/judge_client", runidstr,buf, oj_home, (char *)NULL);
}

FILE * read_cmd_output(const char * fmt, ...) {
	char cmd[BUFFER_SIZE];

	FILE *  ret =NULL;
	va_list ap;

	va_start(ap, fmt);
	vsprintf(cmd, fmt, ap);
	va_end(ap);

	ret = popen(cmd,"r");
	
	return ret;
}

int read_int_http(FILE * f){
	char buf[BUFFER_SIZE];
	fgets(buf, BUFFER_SIZE-1, f);
	return atoi(buf);
}
bool check_login(){
	const char *cmd="wget --post-data=\"checklogin=1\" --load-cookies=cookie --save-cookies=cookie --keep-session-cookies -q -O - \"%s/admin/problem_judge.php\"";
	int ret = 0;
	
	FILE * fjobs=read_cmd_output(cmd, http_baseurl);
	ret=read_int_http(fjobs);
	pclose(fjobs);
	
	return ret>0;
}
void login(){
	if(!check_login()){
		char cmd[BUFFER_SIZE];
		sprintf(cmd, "wget --post-data=\"user_id=%s&password=%s\" --load-cookies=cookie --save-cookies=cookie --keep-session-cookies -q -O - \"%s/login.php\"",http_username, http_password, http_baseurl);
		system(cmd);
	}	
}

int get_jobs(int *jobs){
	login();
	int ret = 0;
	int i = 0;
	char buf[BUFFER_SIZE];
	const char *cmd="wget --post-data=\"getpending=1&max_running=%d\" --load-cookies=cookie --save-cookies=cookie --keep-session-cookies -q -O - \"%s/admin/problem_judge.php\"";
	FILE *fjobs = read_cmd_output(cmd, max_running, http_baseurl);
	while(fscanf(fjobs, "%s", buf) != EOF){
		int sid = atoi(buf);
		if (sid > 0)
			jobs[i ++]=sid;
	}
	pclose(fjobs);
	ret = i;
	while(i <= max_running * 2) jobs[i ++] = 0;
	return ret;
}

bool check_out(int solution_id, int result){
	login();
	const char *cmd="wget --post-data=\"checkout=1&sid=%d&result=%d\" --load-cookies=cookie --save-cookies=cookie --keep-session-cookies -q -O - \"%s/admin/problem_judge.php\"";
	int ret = 0;
	FILE *fjobs = read_cmd_output(cmd, solution_id, result, http_baseurl);
	fscanf(fjobs, "%d", &ret);
	pclose(fjobs);
	return ret;
}

int work(){
	static int retcnt = 0, workcnt = 0;
	static pid_t ID[100];
	int runid = 0, i = 0;
	int jobs[max_running * 2 + 1];
	pid_t tmp_pid = 0;

	/* get the database info */
	if(!get_jobs(jobs)) retcnt=0;
	/* exec the submit */
	for (int j = 0; jobs[j] > 0; j ++){
		runid = jobs[j];
		if (workcnt >= max_running){           // if no more client can running
			tmp_pid = waitpid(-1, NULL, 0);     // wait 4 one child exit
			workcnt --;
			retcnt ++;
			for (i = 0; i < max_running; i ++)     // get the client id
				if (ID[i] == tmp_pid) break; // got the client id
			ID[i] = 0;
		}else{                 // have free client
			for (i = 0; i < max_running; i ++)     // find the client id
				if (ID[i] == 0) break;    // got the client id
		}
		if(workcnt < max_running && check_out(runid, OJ_CI)){
			workcnt ++;
			ID[i] = fork();             // start to fork
			if (ID[i] == 0){
				run_client(runid, i);    // if the process is the son, run it
				exit(0);
			}
		}else{
			ID[i] = 0;
		}
	}
	while ((tmp_pid = waitpid(-1, NULL, WNOHANG))>0){
		workcnt --;
		retcnt ++;
		for (i = 0; i < max_running; i ++)     // get the client id
			if (ID[i] == tmp_pid) break; // got the client id
		ID[i] = 0;
	}
	return retcnt;
}

int main(int argc, char** argv){
	chdir(oj_home);// change the dir

	signal(SIGQUIT, call_for_exit);
	signal(SIGKILL, call_for_exit);
	signal(SIGTERM, call_for_exit);
	
	while (!STOP){
		work();
		sleep(1);
	}
	return 0;
}
