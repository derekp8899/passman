/*

passman - command line password manager

author : Rubberspoon 

THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

USE THIS SOFTWARE AT YOUR OWN RISK

written in C with ncurses interface

crypto functions implemented with openssl

User master password will not be stored
autogenerated secret key and salt will be stored as SHA2/512 hashes, future possible upgrade to SHA-3

salts will be changed with each login from /dev/urandom

Will use these three elements to form the encryption keys using 60k iteration HKDF
encryption IVs will be autogenerated from /dev/urandom upon each new encrypt
HMAC - are autogenerated by the gcm mode encrypt

managed passwords will be encrypted with AES in GCM
no plaintext stored on the disk 
---------------------------------------------------------------------------------------------------------------------------------------------

CURRENT BUILD IS ONLY A TEST BED FOR THE KDF FUNCTIONS AND SECRET KEY/SALT GEN

---------------------------------------------------------------------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/kdf.h> 

//maybe pthread the crypts/decrypts?
/*******************
#include <pthread.h>
*******************/

//defaults
#define RAND_DEFAULT 25 //default length for random password generation

char* genSecret();
char* keyGenHKDF();

//to create a new user
void createUser(){
  
    FILE *passmanDB = fopen("passman","a+");
    char *userTemp=malloc(sizeof(char)*512);
    char *passTemp=malloc(sizeof(char)*512);
    unsigned char *hashTemp=malloc(sizeof(char)*512);
    char *saltTemp=malloc(sizeof(char)*512);
    char *secTemp=malloc(sizeof(char)*512);
    size_t hashSize = 512;
    genSecret();
    FILE *passmanSalt = fopen("passman.salt","r");
    getline(&saltTemp,&hashSize,passmanSalt);
    FILE *passmanSec = fopen("passman.sec","r");
    getline(&secTemp,&hashSize,passmanSec);

    printf("DEBUGGING SALT: %s\n", saltTemp);
    printf("DEBUGGING SEC: %s\n", secTemp);
    printf("You Need to create a user\n");
    printf("Input a user name(max 512 characters)\n");
    
    //creating the master password will be salted with the random salt that was already created
    
    printf("You need to create a master password\nMAKE THIS PASSWORD STRONG\nTHIS PASSWORD WILL BE USED TO ENCRYPT ALL OTHER PASSWORDS\n");
    printf("I suggest a password of at least 16 characters(max supported by this application is 512)\n");
    printf("Input a password\n");
    scanf("%s",passTemp);
    printf("you entered: %s\n",passTemp);
    passTemp = strtok(passTemp,"\n");
    hashTemp = keyGenHKDF(passTemp,secTemp,saltTemp);
    fprintf(passmanDB,"derek:");
    printf("\nyour generated hash:");
    for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
      printf("%02x", hashTemp[i]);
      fprintf(passmanDB,"%02x", hashTemp[i]);
    }
    printf("\n");
    free(userTemp);free(passTemp);free(hashTemp);free(saltTemp);free(secTemp);
    fclose(passmanDB);
    fclose(passmanSalt);
    fclose(passmanSec);
    exit(1);
}

//generates the crypto-random secret key that is used, along with the users master password, to generate the AES encryption key
//uses /dev/urandom as the randomized source
char* genSecret(){

  size_t numBytes = 512;
  char *keyBuff = malloc(sizeof(char)*512);
  FILE *urand = fopen("/dev/urandom","r");
  size_t resultKey = fread(keyBuff,1,numBytes,urand);
  FILE *passmanKey = fopen("passman.sec","w+");
  FILE *passmanSalt = fopen("passman.salt","w+");
  printw("generating your random secret key and unique salt\n These are stored in passman.sec and passman.salt\n");
  printw("DO NOT DISTRIBUTE OR EXPOSE THESE THEY ARE USED TO ENCRYPT ALL YOUR OTHER DATA\n");
  printw("your secret key hash: ");
  //openssl stores them as unsigned chars
  unsigned char inPassHash[SHA512_DIGEST_LENGTH];
  SHA512(keyBuff, 512, inPassHash);
 
  //convert from the hexdigest to printable chars
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printw("%02x", inPassHash[i]);
    fprintf(passmanKey,"%02x", inPassHash[i]);
  }
  printw("\n");
  size_t resultSalt = fread(keyBuff,1,numBytes,urand);
  
  SHA512(keyBuff, 512, inPassHash);
  printw("your salt: ");
  //convert from the hexdigest to printable chars
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printw("%02x", inPassHash[i]);
    fprintf(passmanSalt,"%02x", inPassHash[i]);
  }
  printw("\n");

  refresh();
  free(keyBuff);
  fclose(passmanKey);
  fclose(passmanSalt);
  fclose(urand);
  sleep(5);
  
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

char* keyGenHKDF(char* inPassBuff, char* secBuff, char* saltBuff){

  //init the crypto context
  EVP_PKEY_CTX *pctx;
  unsigned char *out=malloc(sizeof(unsigned char)*512);
  unsigned char *keyTemp=malloc(sizeof(unsigned char)*512);
  unsigned char *catTemp=malloc(sizeof(unsigned char)*1024);
  size_t outlen = 512;
  printw("%d\n",outlen);//verify the size
  //  refresh();
  strcpy(catTemp,inPassBuff);
  strcat(catTemp,secBuff);
  printf("DEBUG CATTEMP:\n%s",catTemp);
  SHA512(catTemp, 1024, keyTemp);
  pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

  if (EVP_PKEY_derive_init(pctx) <= 0)
    /* Error */
    return NULL;
  if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha512()) <= 0)
    /* Error */
    return NULL;
  if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, saltBuff, 512) <= 0)
    /* Error */
    return NULL;
  if (EVP_PKEY_CTX_set1_hkdf_key(pctx, keyTemp, 512) <= 0)
    /* Error */
    return NULL;
  if (EVP_PKEY_CTX_add1_hkdf_info(pctx, "passk", 5) <= 0)
    /* Error */
    return NULL;
  EVP_PKEY_derive(pctx,out,&outlen);
  for(int j = 0; j < 60000;++j){//60k iters
    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, out, outlen) <= 0)
      /* Error */
      return NULL;
    if (EVP_PKEY_derive(pctx, out, &outlen) <= 0)
      /* Error */
      return NULL;
  }
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printw("%02x",out[i]);
  }
  printw("\n");
  refresh();
  return out;
}

int main(){

  //open the files we need dont check since we will just create if they are not there
  FILE *passmanKeys = fopen("passman.keys","a+");
  FILE *loginFp = fopen("passman","a+");
  FILE *saltFP = fopen("passman.salt","r");
  FILE *secFP = fopen("passman.sec","r");

  //malloc the input buffers for the login process
  
  char **login = (char**)malloc(sizeof(char*));
  *login = (char*)malloc(sizeof(char)*(1024)+1);//allow for user and password has to be 512 chars each +1 for separator
  char *user = malloc(sizeof(char)*512);//to store the username
  char *uPHash = malloc(sizeof(char)*512);//to store the username password hash
  char *passInTemp = malloc(sizeof(char)*512);
  char *inHashBuffer = (char*)malloc(sizeof(char)*512);
  char *passmanSec = malloc(sizeof(char)*512);
  char *passmanSalt = malloc(sizeof(char)*512);
			     
  //define the buffer sizes for the getlines and buffer operations
  
  size_t userSize = 1025;
  size_t passSize = 512;
  size_t hashSize= 512;

  //init the screen window
  //  initscr();
  //  refresh();

  //  genSecret();
  // -TODO- if there is a db file prompt for master password and attempt decrypt verify the decrypt
  //grab the user name and hash from the login hash file, and split, it is saved in the format user:hash
  if(getline(login,&userSize,loginFp)!=-1){//if the login data already exists in passman file

      user =strtok(*login,":");
      uPHash = strtok(NULL, "\n");
      getline(&passmanSec,&hashSize,secFP);
      getline(&passmanSalt,&hashSize,saltFP);
      
  }
  else//else we need to create the user login/passworddb file
    createUser();
  
  //debug prints for file io and string split
  printf("PRINTING USER AND PASS HASH FROM THE DB FILE\n%s %s\n",user,uPHash);
  printf("DEBUG SALT AND SEC:\n%s\n%s\n",passmanSalt, passmanSec);
  //prompt password
  printf("Please Enter your password: ");
  //  refresh();
  /*
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
  */
  scanf("%s",passInTemp);
  printf("you entered the password: ");
  printf("%s\n",passInTemp);

  //  int inPassSize = index;
  //remove trailing newline
  char *passIn = strtok(passInTemp,"\n");
  //hash the input and store in the buffer, for now debug print the hash to the screen

  //openssl stores them as unsigned chars
  unsigned char *inPassHash = malloc(sizeof(char)*512);
  //  SHA512(passIn, inPassSize-1, inPassHash);
  inPassHash = keyGenHKDF(passIn,passmanSec,passmanSalt);
  //convert from the hexdigest to printable chars
  printf("\nyour hash:\n ");
  for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
    printf("%02x", inPassHash[i]);
    sprintf(&(inHashBuffer[i*2]),"%02x",inPassHash[i]);
  }
  //  refresh();
  
  //debug dump to term
  printf("\n");
  //  printf("printing inHashBuffer: %s\n", inHashBuffer);
  //  refresh();
  //test if the password hashes match
  for (int i = 0; i < SHA512_DIGEST_LENGTH;++i){
    if(inHashBuffer[i]!=uPHash[i]){
      printf("Password incorrect! TRY AGAIN\n- EXITING -\n");
      //      refresh();
      exit(0);
    }
  }
  printf("LOGIN SUCCESSFUL\nLogged in as: %s\n",user);
  //  refresh();
  free(passInTemp);//no longer need temp pass storage
 
  sleep(5);
  
  /*----------------------------------- debugging test bed/logins for now -----------------------------------*/
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
