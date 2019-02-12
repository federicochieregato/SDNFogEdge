/* Server che riceve da edge in insieme di valori e ne fa la media ogni 10 inviando il risultato al cloud center*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>



/********************************************************
void gestore(int signo){
  int stato;
  printf("esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}
/********************************************************/

int main(int argc, char **argv){
	int sd, sd2, portFog, portCloud, len, num1,ris; //Num1 contatore x controllo interi
	struct sockaddr_in cliaddr, servaddrFog, servaddrCloud;
	struct hostent *clienthost;
	struct hostent *cloudhost;
	int req, num; //intero ricevuto da edge tradotto
	int totale;
	float media, media_trad;
	int count;
	const int on=1;
	

	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=4){
		printf("Error: %s port Fog, IP cloud, port Cloud\n", argv[0]);
		exit(1);
	}
	else{
		num1=0;
		while( argv[1][num1]!= '\0' ){
			if((argv[1][num1] < '0') || (argv[1][num1] > '9')){
				printf("Secondo argomento non intero\n");
				printf("Error: %s port\n", argv[0]);
				exit(2);
			}
			num1++;
		}  

		num1=0;
		while( argv[3][num1]!= '\0' ){
			if((argv[3][num1] < '0') || (argv[3][num1] > '9')){
				printf("Quarto argomento non intero\n");
				printf("Error: %s port\n", argv[0]);
				exit(2);
			}
			num1++;
		} 	
	  	portFog = atoi(argv[1]);
  		if (portFog < 1024 || portFog > 65535){
		      printf("Error: %s port\n", argv[0]);
		      printf("1024 <= port <= 65535\n");
		      exit(2);  	
  		}

		portCloud = atoi(argv[3]);
  		if (portCloud < 1024 || portCloud > 65535){
		      printf("Error: %s port\n", argv[0]);
		      printf("1024 <= port <= 65535\n");
		      exit(2);  	
  		}
	}

	//Configurazione Fog

	/* INIZIALIZZAZIONE INDIRIZZO SERVER ---------------------------------- */
	memset ((char *)&servaddrFog, 0, sizeof(servaddrFog));
	servaddrFog.sin_family = AF_INET;
	servaddrFog.sin_addr.s_addr = INADDR_ANY;  
	servaddrFog.sin_port = htons(portFog);  

	//Configurazione Cloud

	memset((char *)&servaddrCloud, 0, sizeof(struct sockaddr_in));
	servaddrCloud.sin_family = AF_INET;
	cloudhost = gethostbyname(argv[2]);

	if (cloudhost == NULL){
		printf("%s not found in /etc/hosts\n", argv[2]);
		exit(2);
	}else{
		servaddrCloud.sin_addr.s_addr=((struct in_addr *)(cloudhost->h_addr))->s_addr;
		servaddrCloud.sin_port = htons(portCloud);
	}

	/* CREAZIONE, SETAGGIO OPZIONI E CONNESSIONE SOCKET -------------------- */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd <0){perror("creazione socket "); exit(1);}
	printf("Server: creata la socket, sd=%d\n", sd);

	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
	{perror("set opzioni socket "); exit(1);}
	printf("Server: set opzioni socket ok\n");

	if(bind(sd,(struct sockaddr *) &servaddrFog, sizeof(servaddrFog))<0)
	{perror("bind socket "); exit(1);}
	printf("Server: bind socket ok\n");


	/* CICLO DI RICEZIONE RICHIESTE ------------------------------------------ */
	totale=0;
	media=0;
	count=0;
	for(;;){
		len=sizeof(struct sockaddr_in);
		if (recvfrom(sd, &req, sizeof(int), 0, (struct sockaddr *)&cliaddr, &len)<0)
		{perror("recvfrom "); continue;}

		/* trattiamo le conversioni possibili */
		num=ntohl(req);

		clienthost=gethostbyaddr( (char *) &cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
		if (clienthost == NULL) printf("client host information not found\n");
		else printf("Operazione richiesta da: %s %i\n", clienthost->h_name,(unsigned)ntohs(cliaddr.sin_port)); 

		printf("Intero ricevuto: %d\n", num);
		printf("Server : eseguo la somma\n");
			
		/*EXEC*/
		totale=num+totale;
		count++;
		printf("Count:%d Totale:%d \n", count, totale);
		
		
		if(count == 10) {
			if(fork()==0){
				printf("Figlio connessione cloud creato\n"); 
				media=totale/count;
				media_trad=htonl(media);
		
			/* CREAZIONE SOCKET STREAM CLOUD------------------------------------ */
				sd2=socket(AF_INET, SOCK_STREAM, 0);
				if(sd2<0) {perror("apertura socket"); exit(1);}
				printf("Fog: creata la socket stream verso Cloud Center sd=%d\n", sd2);
	
			/* Operazione di BIND implicita nella connect */
				if(connect(sd2,(struct sockaddr *) &servaddrCloud, sizeof(struct sockaddr))<0)
				{perror("connect"); exit(1);}
		
				printf("Fog: connect ok\n");
				printf("Invio a Cloud Center: %5.2f\n", media);			
				write(sd2,&media_trad,sizeof(float));
				printf("Invio effettuato\n");
				close(sd2);
				printf("Socket Stream Cloud chiusa\n");
				printf("Figlio:esco\n");
				exit(0);
			}
			totale=0;
			media=0;
			count=0;
			}

	} //for

}

