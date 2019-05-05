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
  
    FILE *loginFp = fopen("passman","a+");
    char *userTemp=malloc(sizeof(char)*512);
    char *passTemp=malloc(sizeof(char)*512);
    char *hashTemp=malloc(sizeof(char)*128*2);
    
    printf("You Need to create a user\n");
    printf("Input a user name(max 512 characters)\n");
    
    printf("You need to create a master password\nMAKE THIS PASSWORD STRONG\nTHIS PASSWORD WILL BE STORED ON THE MACHINE AS A SHA2-512 HASH\n");
    printf("I suggest a password of at least 16 characters(max support by this application is 512)\n");
    printf("Input a password\n");

    free(userTemp);free(passTemp);free(hashTemp);
    
}

//to set the new user password
void writeUserPass(FILE *passmanFP, char *userTemp, char* passTemp){

  

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
  char *inHashBuffer = (char*)malloc(sizeof(char)*128*2);
  
  //define the buffer sizes for the getlines and buffer operations
  
  size_t userSize = 1025;
  size_t passSize = 512;

  //init the screen window
  initscr();
  refresh();
  
  //grab the user name and hash from the login hash file, and split, it is saved in the format user:hash
  if(getline(login,&userSize,loginFp)){//if the login data already exists in passman file

      user =strtok(*login,":");
      uPHash = strtok(NULL, "\n");
    
  }
  else//else we need to create the user login
    createUser();
  
  //debug prints for file io and string split
  printw("%s %s\n",user,uPHash);

  //prompt password
  printw("Please Enter your password: ");
  refresh();

  //here we grab the password from input
  char keyIn='\0';
  //turn off input echo
  noecho();
  
  keyIn = getch();
  int index = 0;

  while(1){
    if(keyIn == '\n')
      break;
    passInTemp[index]=keyIn;
    keyIn=getch();
    index++;

  }
  index++;
  passInTemp[index]='\n';
  printw("you entered the password: ");
  printw("%s\n",passInTemp);

  int inPassSize = index;
  //remove trailing newline
  //  char *passIn = "testdata\n";
  char *passIn = strtok(passInTemp,"\n");
  //hash the input and store in the buffer, for now debug print the hash to the screen
  printw("your hash: ");
  //opensll stores them as unsigned chars
  unsigned char inPassHash[SHA512_DIGEST_LENGTH];
  SHA512(passIn, inPassSize-1, inPassHash);
 
  //convert from the hexdigest to printable chars
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printw("%02x", inPassHash[i]);
    sprintf(&(inHashBuffer[i*2]),"%02x",inPassHash[i]);
  }
  refresh();
  
  //debug dump to term
  printw("\n");
  printw("printing inHashBuffer: %s\n", inHashBuffer);
  refresh();
  //test if the password hashes match
  for (int i = 0; i < SHA512_DIGEST_LENGTH;++i){
    if(inHashBuffer[i]!=uPHash[i]){
      printw("Password incorrect! TRY AGAIN\n- EXITING -\n");
      refresh();
      exit(0);
    }
  }
  printw("LOGIN SUCCESSFUL\nLogged in as: %s\n",user);
  refresh();
  free(passInTemp);//no longer need temp pass storage
 
  sleep(4);
  
  /*----------------------------------- debugging test bed/logins for now --------------- */
  /*
      
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
  endwin();
  exit(0);

}
