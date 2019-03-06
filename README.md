# SDNFogEdge
Progetto di strumenti per la gestione di architetture fog/edge computing attraverso SDN

L'obbiettivo che si pone questo progetto consiste nel trovare possibili soluzioni di sicurezza in un'architettura di Fog/Edge computing attraverso l'implementazione di Software-Defined Network.

L'architettura prevista per un architettura di questo genere consiste in un deployment multi-node di Openstack (1 compute node per edge, 2 per fog e 1 per cloud center).

A causa delle scarse risorse computazionali a disposizione si è cercato di simulare questo ambiente con Mininet.

Viene utilizzato Opendalight come controller SDN

Step 1 - Configurazione Opendaylight

- Scaricare controller Opendaylight https://docs.opendaylight.org/en/stable-fluorine/downloads.html
- Assicurarsi di avere JDK 1.8 nel PC ed avviare il controller attraverso il comando ./bin/karaf
- Da console karaf installare le features l2switch, dlux e restconf

Step 2 - Congigurazione Architettura

- Installare e avviare Mininet http://mininet.org/download/
- Avviare lo script miniedit.py presente nella cartella /miniedit/example/miniedit.py
- Configurare l'architettura come nell'immagine (da sinistra a destra Edge-Fog1-Fog2-CCC).
- Assegnare ad ogni host i seguenti IP (Edge:10.0.1.1 Fog1:10.0.2.1 Fog2:10.0.2.129 CCC:10.0.3.1)
- Configurare il Controller SDN inserendo IP e porta (6633) del controller SDN.

Step 3 - APP

X=1: Prima soluzione
X=2: Seconda soluzione

- EdgeX.c: invia una serie di interi ad uno dei due dispositivi Fog.
- FogX.c: una volta ricevuti una decina di interi ne viene fatta la media e inviata al Cloud Center.
- CCC.c: Salva la media in persistenza. (identico per entrambe le soluzioni)
- SDNAppX.c: Alla ricezione di un determinato segnale interviene sul traffico in corso.
- dropFog1.xml: Istruzioni per bloccare il traffico diretto verso il primo dispositivo di Fog.
- dropFog2.xml: Istruzioni per bloccare il traffico proveniente dai dispositivi di Edge diretto verso il secondo dispositivo di Fog.
- foward.xml: Istruzioni per inoltrare il traffico proveniente dai dispositivi di edge verso il secondo dispositivo di Fog.
- dropEdge.xml: Istruzioni per bloccare il traffico proveniente dai dispositivi di edge.

Differenze tra soluzione1 e soluzione2: La prima instaura una connessione TCP  tra da Edge e il primo Fog, la seconda due connessioni UDP verso entrambi i fog (SDN instraderà il traffico verso solo uno dei due).

- Avviare controller SDN e architettura Mininet
- Lanciare gli applicativi (EdgeX.c, FogX.c, CCC.c) nei rispettivi Host mininet e l'applicativo SDNAPPX.c nell'host in cui è presente il controller SDN.
- Simulare un traffico dati inserendo interi in input ad EdgeX.c
- Assicurarsi che i file.xml siano configurati opportunamente.
- Simulare una rilevazione d'attacco lanciando un segnale al processo che gestisce l'applicazione SDN.
- SIGUSR1 bloccherà il traffico diretto verso il primo fog, l'applicazione SDN instaurerà un nuova connessione verso il secondo.
- SIGUSR2 bloccherà il traffico proveniente dal dispositivo di edge.
