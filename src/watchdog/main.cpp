#include <ostream>
#include <unistd.h>
#include <sys/inotify.h>
#include <boost/thread.hpp>
#include "elev.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

const int TIMEOUT = 2;

bool changed = false;

void isModified(){
  char buffer[BUF_LEN];
  changed = false;
  int fd = inotify_init();

  if ( fd < 0 ) {
    return;
  }

  int wd = inotify_add_watch( fd, "backup.txt", IN_MODIFY);
  int length = read( fd, buffer, BUF_LEN );  
  if ( length < 0 ) {
    return;
  }
  changed = true;
  inotify_rm_watch( fd, wd );

  close( fd );
  return;
}

int main(){
  system("gnome-terminal -e \"./bin/Heis\" &");
  while(true) {
    do{
      boost::thread t(&isModified);
      t.timed_join(boost::posix_time::seconds(TIMEOUT));
    } while(changed == true);
    elev_init();
    elev_set_motor_direction(DIRN_STOP);
    try {
      system("killall -9 Heis &");
    } catch(...){
      std::cerr << "No process found" << std::endl;
    }
    system("gnome-terminal -e \"./bin/Heis\" &");
    std::cerr << "Timed out, restarting" << std::endl;
    sleep(2);
  }
  return 0;
}
