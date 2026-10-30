#ifndef MAIN_H
#define MAIN_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <fcntl.h> 

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

char *externals[] ={"bash","bunzip2","busybox","bzcat","bzcmp","bzdiff","bzegrep","bzexe","bzfgrep","bzgrep",
                        "bzip2","bzip2recover","bzless","bzmore","cat","chacl","chgrp","chmod","chown","chvt",
                        "cp","cpio","dash","date","dbus-cleanup-sockets","dbus-daemon","dbus-uuidgen","dd","df",
                        "dir","dmesg","dnsdomainname","domainname","dumpkeys","echo","ed","egrep","false","fgconsole",
                        "fgrep","findmnt","fuser","fusermount","getfacl","grep","gunzip","gzexe","gzip","hostname","ip",
                        "kbd_mode","kill","kmod","less","lessecho","lessfile","lesskey","lesspipe","ln","loadkeys","login",
                        "loginctl","lowntfs-3g","ls","lsblk","lsmod","mkdir","mknod","mktemp","more","mount","mountpoint",
                        "mt","mt-gnu","mv","nano","nc","nc.openbsd","netcat","netstat","nisdomainname","ntfs-3g","ntfs-3g.probe",
                        "ntfs-3g.secaudit","ntfs-3g.usermap","ntfscat","ntfsck","ntfscluster","ntfscmp","ntfsdump_logfile",
                        "ntfsfix","ntfsinfo","ntfsls","ntfsmftalloc","ntfsmove","ntfstruncate","ntfswipe","open","openvt",
                        "pidof","ping","ping6","plymouth","plymouth-upstart-bridge","ps","pwd","rbash","readlink","red","rm",
                        "rmdir","rnano","running-in-container","run-parts","sed","setfacl","setfont","setupcon","sh","sh.distrib",
                        "sleep","ss","static-sh","stty","su","sync","tailf","tar","tempfile","touch","true","udevadm","ulockmgr_server",
                        "umount","uname","uncompress","unicode_start","vdir","vmmouse_detect","which","whiptail","ypdomainname","zcat",
                        "zcmp","zdiff","zegrep","zfgrep","zforce","zgrep","zless","zmore","znew",NULL};


#define Builtin_cmd 0
#define External_cmd 1
#define No_cmd 2

extern int status;
extern pid_t shell_pid;
extern char prompt[50];

int validate_prompt(char str[]);
//void extract_external();
int check_cmd_type(char *str);
int check_pipe(char *str[]);

void execute_external_cmd(char* commands[]);
void execute_builtin_cmd(char *commands[]);
void execute_n_pipe(char *commands[]);
void print_n_pipe_usage(void);
#endif
