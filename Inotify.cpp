/**
 *Copyright:SCUT
 *Author:kjs
 *Date:2020/5/22
 *Description: Real-time monitoring for course design project
 *File: Inotify.cpp
*/

#include "Inotify.h"

Inotify::Inotify(){
    fd = -1;
}

Inotify::~Inotify(){
}

int Inotify::watch_init(int mask, char *root){
    if(fd == -1){
        fd = inotify_init();
        if (fd == -1) {
            cout<< "Error: inotify_init" << endl;
            return fd;
        }
    }
    add_watch(root, mask);
    return fd;
}

void Inotify::add_watch(char *dir, int mask){
    char subdir[PATH_MAX_SIZE];
    DIR *odir;
    struct dirent *dent;

    if ((odir = opendir(dir)) == NULL){
        cout<< "dir: " << dir << endl;
        cerr << "Error: fail to open root dir" << endl;
        return;
    }

    int dir_wd;
    dir_wd = inotify_add_watch(fd, dir, mask);
    dirset.insert(make_pair(dir_wd, string(dir)));

    if (dir_wd == -1) {
        cerr << "Error: inotify_add_watch";
        close(fd);
    }

    errno = 0;
    while ((dent = readdir(odir)) != NULL)
    {
        if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            continue;
        if (dent->d_type == DT_DIR)
        {
            sprintf(subdir, "%s/%s", dir, dent->d_name);
            add_watch(subdir, mask);
        }
    }

    if (errno != 0)
    {
        cerr << "Error: fail to read dir"<<endl;
        exit(1);
    }
}

bool Inotify::delete_watch(string prefix){
    map<int, string>::reverse_iterator iter;
    for(iter = dirset.rbegin(); iter != dirset.rend(); iter++){
        if(iter->second.find(prefix) == 0){
            int result = inotify_rm_watch(fd, iter->first);
            if (result == -1) {
                cerr << "Error: fail to remove watch." << endl;
                exit(1);
            }
            if(dirset.erase(iter->first) == 0){
                cerr << "Error: erase filed." << endl;
                exit(1);
            }
            return false;
        }
    }
    return true;
}

void Inotify::delete_watch_recursive(char* dir){
    string prefix = dir;

    while (true)
    {
        if(delete_watch(prefix))
            break;
    }
    
}

// Call the Rsync execl
// fixed: need to use the cofig class
void Inotify::CallRsync()
{
	pid_t pid = fork();
	if (pid < 0)
	{
		cout << "Error: failed to fork!" << endl;
	}
	else if (pid == 0)
	{
		//run the executable file
		if (execl("./hello", "./hello", "1111", NULL) < 0)
			cout << "Error: execl error!" << endl;
	}
	else
	{
		int status;
		if (wait(&status) == -1)
			cout << "Error: failed to wait!" << endl;
	}
}

void Inotify::watch_loop(){
    int len = 0;
    char buf[EVENT_BUFSIZE], *cur = buf, *end;

    // Rsync at first.
    CallRsync();

    while (true) {
        len = read(fd, cur, EVENT_BUFSIZE - len);
        if (len <= 0) {
            cerr << "Error: read inotify event";
            close(fd);
        }

        end = cur + len;

        while (cur + sizeof(struct inotify_event) <= end) {
            struct inotify_event* e = (struct inotify_event*)cur;

            if (cur + sizeof(struct inotify_event) + e->len > end)
                break;

            if (e->mask & IN_MOVED_FROM || e->mask & IN_DELETE) {
                if (e->mask & IN_ISDIR){
                    char new_dir[512];
                    sprintf(new_dir, "%s/%s", dirset.find(e->wd)->second.c_str(), e->name);
                    delete_watch_recursive(new_dir);
                }
            }
            
            if (e->mask & IN_CREATE || e->mask & IN_MOVED_TO) {
                if (e->mask & IN_ISDIR){
                    char new_dir[512]; 
                    sprintf(new_dir, "%s/%s", dirset.find(e->wd)->second.c_str(), e->name);
                    add_watch(new_dir, MASK);
                }
                
            }
            cur += sizeof(struct inotify_event) + e->len;
        }

        if (cur >= end) {
            cur = buf;
            len = 0;
        } else {
            len = end - cur;
            memmove(buf, cur, len);
            cur = buf + len;
        }

        CallRsync();
    }

    close(fd);
}
