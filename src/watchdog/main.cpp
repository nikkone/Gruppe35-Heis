#include <ostream>
#include <unistd.h>
#include <sys/inotify.h>
#include <boost/thread.hpp>
#include <signal.h>
#include "elev.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

const int TIMEOUT = 2;

bool changed = false;
bool killed = false;

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

int main(int argc, char* argv[]){
  int pid = getppid();
  boost::thread t(&isModified);
  do{
    t.timed_join(boost::posix_time::seconds(TIMEOUT));
    if(kill(pid,0) != 0) killed = true;
  } while(changed == true && killed == false);
  elev_init();
  elev_set_motor_direction(DIRN_STOP);
  if(kill(pid,0) == 0) kill(pid,1);
  t.timed_join(boost::posix_time::seconds(0));
  std::cerr << "Timed out, restarting" << std::endl;
  int spawned = execl("./bin/heis", "heis", (char*)0);
  if(spawned == -1)
  {
    std::cerr << "Watchdog failed to start Elevator thread!" << std::endl;
  }
  return 0; 
}
