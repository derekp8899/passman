/*

passman - command line password manager simple and effective

author : Rubberspoon 

written in C with ncurses interface

local user account password stored as SHA-3 hashes
managed passwords will be encrypted with a still TBD algorithm (AES?) will be able to type the password or randomize one
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

//defaults
#define RAND_DEFAULT 25 //default length for random password generation

//to create a new user
void createUser(){

}

//to set the new user password
void writeUserPass(){

}

// generates a ranomized password of specified SIZE default is 25 characters

char* genRandom(char *dest, int size){


}

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

  sleep(1);
  /* start the window */
  initscr();
  refresh();
  
    
  //support of up to 100 keys for now
  char **keyBuffs = (char**)malloc(sizeof(char*)*100);
  int i = 0;
  
  //init the key hash buffers
  for(i = 0; i < 100; ++i)
    keyBuffs[i]=(char*)malloc(sizeof(char)*512*2);//to store up to 512 chars for the pass word and 512 for id(name asigned at creation)
  size_t strSize = 513;

  //pointers for the passwd and ids
  char **pwdIds = (char**)malloc(sizeof(char*)*100);
  for(int j = 0; j < 100; ++j)
    pwdIds[j]=(char*)malloc(sizeof(char)*512);//each id can be up to 512 chars long

  char **passwd = (char**)malloc(sizeof(char*)*100);
  for(int j = 0; j < 100; ++j)
    passwd[j]=(char*)malloc(sizeof(char)*512);//each password is max of 512 chars long
  
  
  i = 0;
  
  while(getline(&(keyBuffs[i]),&strSize,passmanKeys)!=-1 && i < 100){  

    passwd[i] = strtok(keyBuffs[i],":");
    pwdIds[i] = strtok(NULL,"\n");

    printw("%s %s\n",passwd[i],pwdIds[i]);
    refresh();
    sleep(1);
    ++i;
  }
  printw("%d",i);
  refresh();
  //  sleep(2);
  

 
  
  /* end the window */
  endwin();

  //free all buffers
  free(*(login));
  free(login);
  //-TODO- why these are invalid pointers?
  //free(uPHash);
  //free(user);
  for(int j = 0;j<i;j++)
    free(keyBuffs[j]);
  free(keyBuffs);
  
  /* exit program no error */
  exit(0);

}
