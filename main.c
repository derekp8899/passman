/*

passman - command line password manager simple and effective

author : Rubberspoon 

written in C with ncurses interface

local user account password stored as SHA2-512 hashes, future possible upgrade to SHA-3

managed passwords will be encrypted with a still TBD algorithm (AES?) will be able to type the password or randomize one
no plaintext stored on the disk 

*/

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>


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

//generates the SHA2-512 hash for login checks
char* genLoginHash(char *inPass, char* loginHash){

  printf("%s",inPass);//verify the input
  
}

//to verify the login
int verifyLogin(char *inPassHash, char *loginHash){



}

int main(){

  //open the files we need dont check since we will just create if they are not there
  FILE *passmanKeys = fopen("passman.keys","a+");
  FILE *loginFp = fopen("passman","a+");

  // -TODO- add in support for multi users, for now we are assuming a single user per install
  //malloc the input buffers for the login process
  
  char **login = (char**)malloc(sizeof(char*));
  *login = (char*)malloc(sizeof(char)*(1024)+1);//allow for user and password has to be 512 chars each +1 for separator
  char *user = malloc(sizeof(char)*512);//to store the username
  char *uPHash = malloc(sizeof(char)*512);//to store the username password hash
  char *passInTemp = malloc(sizeof(char)*512);
  
  //define the buffer sizes for the getlines and buffer operations
  
  size_t userSize = 1025;
  size_t passSize = 512;

  //grab the user name and hash from the login hash file, and split, it is saved in the format user:hash
  if(getline(login,&userSize,loginFp)){

  }
  user =strtok(*login,":");
  uPHash = strtok(NULL, "\n");
  
  printf("%s %s\n",user,uPHash);
  printf("Please Enter your password: ");
  
  //grab the password form input
  int inPassSize = getline(&passInTemp,&passSize,stdin);
  //remove trailing newline
  char *passIn = strtok(passInTemp,"\n");
  //  passIn[sizeof(passIn)-1]='\0';
  //  printf("you entered: %s\n",passIn);
  printf("your hash: ");
  unsigned char inPassHash[SHA512_DIGEST_LENGTH];
  SHA512(passIn, inPassSize-1, inPassHash);
  char *inHashBuffer = (char*)malloc(sizeof(char)*128*2);
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printf("%02x", inPassHash[i]);
    sprintf(&(inHashBuffer[i*2]),"%02x",inPassHash[i]);
  }
  printf("\n");
  printf("printing inHashBuffer: %s\n", inHashBuffer);

  //test if the password hashes match
  for (int i = 0; i < SHA512_DIGEST_LENGTH;++i){
    if(inHashBuffer[i]!=uPHash[i]){
      printf("Password incorrect! TRY AGAIN\n- EXITING -\n");
      exit(0);
    }
  }
  printf("LOGIN SUCCESSFUL\nLogged in as: %s\n",user);
 
  sleep(4);
  /* start the window */


  /*----------------------------------- debugging test bed for now --------------- */
  /*
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
  //endwin();
  /*
  //free all buffers
  free(*(login));
  free(login);
  //-TODO- why these are invalid pointers?
  //free(uPHash);
  //free(user);
  for(int j = 0;j<i;j++)
    free(keyBuffs[j]);
  free(keyBuffs);
  */
  /* exit program no error */
  exit(0);

}
