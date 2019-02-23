#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int statusPadre, statusFiglio;
void handlerPadre(int signum); // Gestisce terminazione Nipote
void handlerFiglio(int signum);// Gestisce teminazione Figli
int main(int argc, char *argv[]){
	if(argc!=1){
		printf("Sintassi sbagliata\n");
		exit(0);
	}
	printf("Pid:%d\n", getpid());
	signal(SIGUSR1, handlerPadre);
	signal(SIGUSR2, handlerPadre);
	signal(SIGCHLD, handlerPadre);
	if(fork()==0){ //figlio 1
		printf("Blocco Traffico diretto verso Fog2\n");
		execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@dropFog2.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/1",(char *)0 );
		//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml
	}
	while(1)
		wait(&statusPadre); //Attesa terminazione figlii

}


void handlerPadre(int signum){
	if(signum==SIGUSR1){
		if(fork()==0){ //figlio 2
			signal(SIGCHLD, handlerFiglio);
			if(fork()==0){ //nipote
				printf("Blocco traffico diretto verso Fog1\n");
				execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@dropFog1.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/2",(char *)0 ); 
				//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml	
			}
			wait(&statusFiglio); // attende terminazione nipote 
		}
	}
			
	if(signum==SIGUSR2){
		printf("Blocco traffico proveniente da dispositivi Edge\n");
		if(fork()==0){ //figlio 3
			execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@dropEdge.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/4",(char *)0 );
			//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml
		} 

	}
	else if(signum==SIGCHLD){
		printf("In attesa di altri segnali...\n");
	}

}

void handlerFiglio(int signum){
	printf("Ridirigo traffico verso Fog2\n");
	execl("/usr/bin/curl","curl","-u","admin:admin","-H","Content-Type: application/yang.data+xml", "-H", "Accept:application/XML", "-X", "PUT", "-d","@forward.xml","http://192.168.56.1:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:6/table/0/flow/3",(char *)0 ); 
	//assicurarsi che la table e l'id presenti nella URI corrispondando a table e id del file.xml
}

