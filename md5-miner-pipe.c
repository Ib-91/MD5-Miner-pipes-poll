// ldd: -lcrypto
#include <openssl/md5.h> //pour le hachage
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Pour le processus et le pipe
#include <signal.h>	//Pour le kill
#include <poll.h>
#include <sys/wait.h>

// = {0} => all the array is reset to zero (only works for zero!)
char hash[1 + 2 * MD5_DIGEST_LENGTH] = {0};

// Calcule le hash MD5 de la chaîne 'str'
char *md5hash(char *str) {
    unsigned char md5[MD5_DIGEST_LENGTH] = {0};
    MD5((unsigned char *)str, strlen(str), md5);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(hash + 2 * i, "%02x", md5[i]);
    }
    return hash;
}

int length(int num) {
    int len = 0;
    if (num == 0) {
        return 1;
    }
    while (num > 0) {
        num /= 10;
        len++;
    }
    return len;
}


int zeros (char* s, int n) {//chaine commence par n fois '0'
    for (int i = 0; i < n; i++) {
        if (s[i] != '0') {
           return 0;}
    }
    return 1;
}


void bruteforce (int pipe, int first, int step, int zero){
  char* reptext;//représentation textuelle
  char* hach;
  
  while(1){
    
    reptext= (char*)malloc((length(first)+1)* sizeof(char));
    sprintf(reptext, "%d",first);//Convertir l'entier en chaine
    hach=md5hash(reptext);
    
    if (zeros(hach, zero)==1){   
    write(pipe, reptext, strlen(reptext));
   	free(reptext);
   	exit(EXIT_SUCCESS);
    }else{
    free(reptext);
    first+=step;
    }
   }
}



int main(void) {
    int pipes[10][2]; //[0] lecture et [1] écriture
    pid_t child[10];
    int nonces, val, cpt=0;
    struct pollfd fds[10]; //structure permettant d'utiliser poll()
    char buffer[32];
    int i=0;
    while(i<10){
        pipe(pipes[i]);//création du pipe
        fds[i].fd=pipes[i][0];//le pipe lecture est attribuée à fds[i].fd
        fds[i].events = POLLIN; //savoir quand des données sont prêtes à êtres lues dans le pipe
        child[i]=fork();
        if(child[i]==0){
        bruteforce(pipes[i][1],i,10,3);
        }
        i++;
}
  while(cpt<5){
      val=poll(fds, 10, -1);
      if(val<0){
        perror("error on the poll");
        exit(EXIT_FAILURE);
      }
      for(int j=0;j<10;j++){
        if(fds[j].revents & POLLIN){ //revents == 0 rien ne s'est produit
        //&POLLIN donnée à lire dans le fds[j]
          nonces=read(fds[j].fd,buffer,32);//stock les données lus dans le buffer
          printf("nonce %i: %s\n", 1 + cpt, buffer);
          cpt++;
          break;
        }
      }
    }

  for(int i=0; i < 10; i++){
      kill(child[i], SIGKILL);        
  }
}