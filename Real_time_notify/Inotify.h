/**
 *Copyright:SCUT
 *Author:kjs
 *Date:2020/5/22
 *Description: Real-time monitoring for course design project
 *File: Inotify.h
*/


#ifndef __INOTIFY_H__
#define __INOTIFY_H__

#include <map>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>

using namespace std;

#define EVENT_BUFSIZE 4096
#define PATH_MAX_SIZE 512

// the types of event
enum{MASK = IN_MODIFY | IN_CREATE | IN_DELETE | IN_ISDIR | IN_MOVED_FROM | IN_MOVED_TO};

// enum{MASK = IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_ACCESS | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO | IN_MOVE | IN_OPEN | IN_ISDIR | IN_UNMOUNT | IN_Q_OVERFLOW | IN_Q_OVERFLOW | IN_IGNORED | IN_ONESHOT};


class Inotify {
    public:
        Inotify();
        ~Inotify();
        int watch_init(int mask, char *root);
        void watch_loop();

    private:
        void add_watch(char *dir, int mask);
        bool delete_watch(string prefix);
        void delete_watch_recursive(char* dir);
        void CallRsync();
        map<int, string> dirset;
        int fd;
};

#endif

