#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


void handler(int signum);
int main(int argc, char *argv[]){
	int status;
	if(argc!=1){
		printf("Sintassi sbagliata\n");
		exit(0);
	}
	printf("Pid:%d\n", getpid());
	signal(SIGUSR1, handler);
	signal(SIGUSR2, handler);
	signal(SIGCHLD, handler);
	printf("In attesa di un segnale...\n");
	while(1)
		wait(&status); //Attesa terminazione figlio

}


void handler(int signum){
	if(signum==SIGUSR1){
		printf("Blocco traffico diretto verso Fog1\n");
		if(fork()==0){
			execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@dropFog1.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/1",(char *)0 );
			//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml
		}

	}
			
	if(signum==SIGUSR2){
		printf("Blocco traffico proveniente da dispositivi Edge\n");
		if(fork()==0){
			execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@dropEdge.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/2",(char *)0 );
			//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml
		}
	}
	else if(signum==SIGCHLD){
		printf("In attesa di altri segnali...\n");
	}

}
