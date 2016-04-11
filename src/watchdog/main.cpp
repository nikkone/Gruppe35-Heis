#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <signal.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#define TIMEOUT     3

using namespace std;

bool changed = false;

// Hvordan starte ilag?
// Kjøre system("./bin/watchdog"); i Heis?

void isModified(){
  char buffer[BUF_LEN];
  changed = false;
  int fd = inotify_init();

  if ( fd < 0 ) {
    return;
  }

  int wd = inotify_add_watch( fd, "backup.txt", IN_MODIFY);
  //Blocking read
  int length = read( fd, buffer, BUF_LEN );  
  if ( length < 0 ) {
    return;
  }
  changed = true;
  inotify_rm_watch( fd, wd );

  //closing the INOTIFY instance
  close( fd );
  return;
}

int main(){
  system("xterm -e \"./bin/Heis\" &");
  while(true) {
    do{
      boost::thread t(&isModified);
      t.timed_join(boost::posix_time::seconds(TIMEOUT));
    } while(changed == true);
    cout << "timed out" << endl;
    try {
      system("killall -9 Heis");
    } catch(...){}
    cout << "timed out" << endl;
    system("xterm -e \"./bin/Heis\" &");
    cout << "timed out" << endl;
  }
  return 0;
}