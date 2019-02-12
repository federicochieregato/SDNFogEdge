
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>

#define LINE_LENGTH 256


int main(int argc, char **argv)
{
	struct hostent *foghost;
	struct sockaddr_in servaddr;
	int  port, port2, sd, num1, num2, len, ris, ok, nread;
	char okstr[LINE_LENGTH];
	char c;
	int changedFog=0;



	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=5){
		printf("Error:%s serverAddress serverPort serverAddress2 serverPort2\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZI SERVER -------------------------- */
	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	foghost = gethostbyname(argv[1]);

	/*VERIFICA PORTE*/
	nread=0;
	while( argv[2][nread]!= '\0' ){
		if( (argv[2][nread] < '0') || (argv[2][nread] > '9') ){
			printf("Secondo argomento non intero\n");
			exit(2);
		}
		nread++;
	}
	port = atoi(argv[2]);

	nread=0;
	while( argv[4][nread]!= '\0' ){
		if( (argv[4][nread] < '0') || (argv[4][nread] > '9') ){
			printf("Secondo argomento non intero\n");
			exit(2);
		}
		nread++;
	}

	port2 = atoi(argv[4]);

	/* VERIFICA PORT e HOST */
	if (port < 1024 || port > 65535){
		printf("%s = porta scorretta...\n", argv[2]);
		exit(2);
	}

	if (port2 < 1024 || port2 > 65535){
		printf("%s = porta scorretta...\n", argv[4]);
		exit(2);
	}

	if (foghost == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}else{
		servaddr.sin_addr.s_addr=((struct in_addr *)(foghost->h_addr))->s_addr;
		servaddr.sin_port = htons(port);
	}



	/* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente */
	printf("Inserire intero, EOF per terminare: ");

	
	while ((ok=scanf("%i", &num1)) != EOF )
	{
		if( ok != 1){
			/* Problema nell'implementazione della scanf. Se l'input contiene PRIMA
			* dell'intero altri caratteri la testina di lettura si blocca sul primo carattere
			* (non intero) letto. Ad esempio: ab1292\n
			*				  ^     La testina si blocca qui
			* Bisogna quindi consumare tutto il buffer in modo da sbloccare la testina.
			*/
			do {c=getchar(); printf("%c ", c);}
			while (c!= '\n');
			printf("Inserire intero da inviare, EOF per terminare: ");
			continue;
		}

    		// quando arrivo qui l'input e' stato letto correttamente
		num2=htonl(num1);
		// Consumo il new line, ed eventuali altri caratteri
		// immessi nella riga dopo l'intero letto
		gets(okstr); 

		/* CREAZIONE SOCKET ------------------------------------ */
		sd=socket(AF_INET, SOCK_STREAM, 0);
		if(sd<0) {perror("apertura socket"); exit(1);}
		printf("Client: creata la socket sd=%d\n", sd);


		/* Operazione di BIND implicita nella connect */
		if(changedFog==0){
			if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0){
				changedFog=1;
				//RINIZIALIZZO INDIRIZZO SERVER
				printf("Change Fog\n");
				memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
				servaddr.sin_family = AF_INET;
				foghost = gethostbyname(argv[3]);
				if (foghost == NULL){
					printf("%s not found in /etc/hosts\n", argv[3]);
					exit(2);
				}
				servaddr.sin_addr.s_addr=((struct in_addr *)(foghost->h_addr))->s_addr;	
				servaddr.sin_port = htons(port2);
			}			
		}
		if(changedFog==1){
			if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0){
				perror("connect"); exit(1);
			}
		}
		
		printf("Client: connect ok\n");

		/*INVIO File*/
		printf("Client: Invio intero \n");
		write(sd,&num2,sizeof(int));	//invio
		printf("Client: Intero inviato correttamente \n");
		close(sd);

	printf("Inserire intero da inviare, EOF per terminare: ");
	
	} // while gets
	
	//CLEAN OUT
	printf("\nClient: termino...\n");  
	exit(0);
}
