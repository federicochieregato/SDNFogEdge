
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
	struct hostent *foghost2;
	struct sockaddr_in servaddr, clientaddr, servaddr2;
	int  port, port2, sd, sd2, num1, num2, len, ris, ok, nread;
	char okstr[LINE_LENGTH];
	char c;
	const int on=1;



	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=4){
		printf("Error:%s serverAddress1 serverPort serverAddress2\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZO CLIENT E SERVER --------------------- */
	memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;

	/* Passando 0 ci leghiamo ad un qualsiasi indirizzo libero,
	* ma cio' non funziona in tutti i sistemi.
	* Se nel nostro sistema cio' non funziona come si puo' fare?
	*/
	clientaddr.sin_port = 0;

	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	foghost = gethostbyname (argv[1]);
	

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


	/* VERIFICA PORT e HOST */
	if (port < 1024 || port > 65535){
		printf("%s = porta scorretta...\n", argv[2]);
		exit(2);
	}

	
	if (foghost == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}else{
		servaddr.sin_addr.s_addr=((struct in_addr *)(foghost->h_addr))->s_addr;
		servaddr.sin_port = htons(port);
	}

	foghost2 = gethostbyname (argv[3]);
		
	if (foghost2 == NULL){
		printf("%s not found in /etc/hosts\n", argv[3]);
		exit(2);
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




		if(fork()==0){
			
		/* CREAZIONE SOCKET ------------------------------------ */
		sd2=socket(AF_INET, SOCK_DGRAM, 0);
		if(sd2<0) {perror("apertura socket"); exit(1);}
		printf("Figlio: creata la socket sd=%d\n", sd2);

		/* BIND SOCKET, a una porta scelta dal sistema --------------- */
		if(bind(sd2,(struct sockaddr *) &clientaddr, sizeof(clientaddr))<0)
		{perror("bind socket "); exit(1);}
		printf("Figlio: bind socket ok, alla porta %i\n", clientaddr.sin_port);


			
		memset((char *)&servaddr2, 0, sizeof(struct sockaddr_in));
		servaddr2.sin_family = AF_INET;
		servaddr2.sin_addr.s_addr=((struct in_addr *)(foghost2->h_addr))->s_addr;
		servaddr2.sin_port = htons(port);
	
		len=sizeof(servaddr2);
		
		if(sendto(sd2, &num2, sizeof(int), 0, (struct sockaddr *)&servaddr2, len)<0){
			perror("sendto");
			continue;
		}


		printf("Figlio: Intero inviato correttamente \n");
	

		close(sd2);
		exit(0);


		}


		/* CREAZIONE SOCKET ------------------------------------ */
		sd=socket(AF_INET, SOCK_DGRAM, 0);
		if(sd<0) {perror("apertura socket"); exit(1);}
		printf("Client: creata la socket sd=%d\n", sd);

		/* BIND SOCKET, a una porta scelta dal sistema --------------- */
		if(bind(sd,(struct sockaddr *) &clientaddr, sizeof(clientaddr))<0)
		{perror("bind socket "); exit(1);}
		printf("Client: bind socket ok, alla porta %i\n", clientaddr.sin_port);



		len=sizeof(servaddr);

		/*INVIO intero*/
		printf("Client: Invio intero \n");
		if(sendto(sd, &num2, sizeof(int), 0, (struct sockaddr *)&servaddr, len)<0){
			perror("sendto");
			continue;
		}
		printf("Client: Intero inviato correttamente \n");

	

		close(sd);

	printf("Inserire intero da inviare, EOF per terminare: ");
	
	} // while gets
	
	//CLEAN OUT
	printf("\nClient: termino...\n");  
	exit(0);
}
