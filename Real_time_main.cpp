/**
 *Copyright:SCUT
 *Author:kjs
 *Date:2020/5/22
 *Description: Real-time monitoring for course design project
 *File: main.cpp
*/

#include "Inotify.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    if(argc != 2){
        cerr << "Usage: " << argv[0] << " PATH" << endl;
        return -1;
    }

    // Init the Inotify before the first rsync.
    Inotify inotify_watch = Inotify();
    inotify_watch.watch_init(MASK, argv[1]);
    inotify_watch.watch_loop();
    
    return 0;
}