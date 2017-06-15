#include "libpower.h"

int main(int argc, char *argv[])
{
  check_config();
  system("> ~/.config/config_powerup/black_list_pid.conf");
  system("> ~/.config/config_powerup/refresh_list_pid.conf");
  int length, i = 0, opt= 0, long_index =0;
  char buffer[EVENT_BUF_LEN];
  time_t first_refresh, second_refresh, first_stop, second_stop;
  // bool duplicate =false;
  //  FILE *black_list_conf;
  //   char string[100];
  static struct option long_options[] = {
    {"help",        no_argument, 0,  'h' },
    {"add-refresh", no_argument, 0,  'r' },
    {"add-black",   no_argument, 0,  'b' },
    {"kill",        no_argument, 0,  'k' },
    {"list",        no_argument, 0,  'l' },
    {"toggle-black",no_argument, 0,  't' },
    {    0,             0,       0,   0  }
  };

  while ((opt = getopt_long(argc, argv,"hrbklt", long_options, &long_index )) != -1) {
    switch (opt) {
    case 'h' :
      print_usage();
      exit (0);
    case 'r' :
	system("xprop _NET_WM_PID | cut -f3 -d' ' >> ~/.config/config_powerup/refresh_list_pid.conf");
	exit(0);
    case 'b' :
	system("xprop _NET_WM_PID | cut -f3 -d' ' >> ~/.config/config_powerup/black_list_pid.conf");
	exit(0);
    case 'k' :
      system("kill `ps -e | grep latest | cut -f1 -d' '`");
      exit(0);
    case 'l' :
      printf("Below is a list of currently active applications and their respective PIDS.\n");
      system("wmctrl -l -p");
      exit(0);
    case 't' :
      // black_list_conf=fopen("~/.config/config_powerup/black_list.conf","r");
      // if(black_list_conf==NULL){
      //	perror("TToggle error");
      //  }
    //   while( !feof(black_list_conf)) {
      //	fscanf(black_list_conf, "%s", string);
      // }
      // if(duplicate == false)
      //{
      //  system("ps -e | grep `xdotool getactivewindow getwindowpid` | sed -e 's/[0-9]*//' | sed -e 's/\ .*\ //' >> ~/.config/config_powerup/black_list.conf");
      //}
      //else
      //{
      //  system("mv ~/.config/config_powerup/black_list.conf ~/.config/config_powerup/tmp.conf");
      //  system("> ~/.config/config_powerup/black_list.conf");	  
      //}*/
      exit(0);
    }
  }
  if (( pipe_popen = popen("xprop -root -spy _NET_ACTIVE_WINDOW | mawk -W interactive '{print $5}' > ~/.config/config_powerup/notif/window_change.conf", "r")) == NULL)
    {
      perror("popen");
      exit(1);
    }
  
  //system("> ~/.config/config_powerup/black_list_pid.conf");
  first_refresh = time(NULL);
  printf("\nLaunched power-up.\n");
  system("bash ~/.config/config_powerup/get_pid.sh");
  Liste *black_list = create_list("/.config/config_powerup/black_list_pid.conf");
  Liste *refresh_list = create_list("/.config/config_powerup/refresh_list_pid.conf");

  action.sa_handler = hand;
  sigaction(SIGINT,&action,NULL);
  
  fd = inotify_init();
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }

  char src[100], src2[100], notify[100], open_windows[100];
  strcpy(notify, getenv("HOME"));
  strcpy(open_windows, getenv("HOME"));
  strcpy(src, "/.config/config_powerup/notif/");
  strcpy(src2, "/.config/config_powerup/open_windows.conf");
  strcat(notify, src);
  strcat(open_windows, src2);
  
  wd = inotify_add_watch( fd, notify, IN_MODIFY); 
  fp = fopen(open_windows,"r");
  if(fp==NULL){
    perror("cannot open file open_windows.conf");
  }
  
  while(1){
    i=0;
    length = read( fd, buffer, EVENT_BUF_LEN );
    if ( length < 0 ) {
      perror( "read" );
    }  
    
    /*read the change event one by one and process it accordingly.*/
    while ( i < length ) {     struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];     if ( event->len ) {
	if ( event->mask & IN_MODIFY ) {
	  //printf( "%s has been modified, changing suspended windows.\n", event->name );
	  first_stop = time(NULL);
	  system("xdotool getwindowfocus getwindowpid > ~/.config/config_powerup/open_windows.conf");
	  system("wmctrl -l -p | cut -f4 -d' ' >> ~/.config/config_powerup/open_windows.conf");
	  system("bash ~/.config/config_powerup/get_pid.sh");
	  black_list = create_list("/.config/config_powerup/black_list_pid.conf");
	  refresh_list = create_list("/.config/config_powerup/refresh_list_pid.conf");
	  
	  fscanf(fp, "%d", &active_pid);
	  while( !feof(fp)) {
	    fscanf(fp, "%d", &pid); 
	    if (pid == active_pid){
	      kill(pid, SIGCONT);
	    }
	    else if ( !member(pid,black_list) ){
	      second_stop=time(NULL);
	      while (second_stop-first_stop < STOP_AFTER_S){
		second_stop=time(NULL);
	      }
	      kill(pid, SIGSTOP);
	    }
	  }
	}
      }
      i += EVENT_SIZE + event->len;
    }
    rewind(fp);
    second_refresh = time(NULL);
    if (second_refresh-first_refresh >= REFRESH_RATE_S){
      //printf("Waking up chosen processes\n");
      activate_list(refresh_list);
      first_refresh = time(NULL);
    }
  }
}
