/*

passman - command line password manager simple and effective

author : Rubberspoon 

written in C with ncurses interface

local user account stored as SHA-3 hashes
managed passwords will be encrypted with a still TBD algorithm will be able to type the password or randomize one
no plaintext stored on the disk 

*/

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

//maybe pthread the crypts/decrypts?
/*******************
#include <pthread.h>
*******************/

int main(){

  //open the files we need dont check since we will just create if they are not there
  FILE *passmanKeys = fopen("passman.keys","a+");
  FILE *loginFp = fopen("passman","a+");

  // -TODO- add in support for multi users, for now we are assuming a single user per install
  char **login = (char**)malloc(sizeof(char*));
  *login = (char*)malloc(sizeof(char)*(1024)+1);//allow for user and password has to be 512 chars each +1 for separator
  char *user = malloc(sizeof(char)*512);//to store the username
  char *uPHash = malloc(sizeof(char)*512);//to store the username password hash
  size_t userSize = 1025;

  //grab the user name and hash, and split, it is saved in the format user:hash
  getline(login,&userSize,loginFp);
  user =strtok(*login,":");
  uPHash = strtok(NULL, "\n");
  
  printf("%s %s\n",user,uPHash);

  sleep(2);
  /* start the window */
  initscr();
  refresh();
  
    
  //support of up to 100 keys for now
  char **keyBuffs = (char**)malloc(sizeof(char*)*100);
  int i = 0;
  
  //init the key hash buffers
  for(i = 0; i < 100; ++i)
    keyBuffs[i]=(char*)malloc(sizeof(char)*512);
  size_t strSize = 513;

  i = 0;

  while(getline(&(keyBuffs[i]),&strSize,passmanKeys)!=-1 && i < 100){  

    printw("%s",keyBuffs[i]);
    
    //refresh();
    //sleep(1);
    ++i;
  }
  printw("%d",i);
  refresh();
  sleep(2);
  

 
  
  /* end the window */
  endwin();
  exit(0);

}
