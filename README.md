# SDNFogEdge
Design of tools for the management of fog / edge computing architectures using SDN

L'obbiettivo che si pone questo progetto consiste nel trovare possibili soluzioni di sicurezza in un'architettura di Fog/Edge computing attraverso l'implementazione di Software-Defined Network.

L'architettura prevista per un architettura di questo genere consiste in un deployment multi-node di Openstack (1 compute node per edge, 2 per fog e 1 per cloud center).

A causa delle scarse risorse computazionali a disposizione si è cercato di simulare questo ambiente con Mininet.

Viene utilizzato Opendalight come controller SDN

Step 1 - Configurazione Opendaylight

- Scaricare controller Opendaylight https://docs.opendaylight.org/en/stable-fluorine/downloads.html
- Assicurarsi di avere JDK 1.8 nel PC ed avviare il controller attraverso il comando ./bin/karaf
- Scaricare le features l2switch, dlux e restconf

Step 2 - Congigurazione Architettura

- Installare e avviare Mininet http://mininet.org/download/
- Avviare lo script miniedit.py presente nella cartella /miniedit/example/miniedit.py
- Configurare l'architettura come nell'immagine (da sinistra a destra Edge-Fog1-Fog2-CCC).
- Assegnare ad ogni host una rete diversa
- Configurare il Controller SDN inserendo IP e porta (6633) del controller SDN.

Step 3 - APP

- Lanciare gli applicativi, EdgeX.c, FogX.c, CCC.c) nei rispettivi Host mininet e l'applicativo SDNAPPX.c (X=1 prima soluzione, X=2 seconda soluzione)
- Simulare un traffico dati inserendo interi in input ad EdgeX.c
- Simulare una rilevazione d'attacco lanciando un segnale al processo che gestisce l'applicazione SDN.
- SIGUSR1 bloccherà il traffico diretto verso il primo fog, l'applicazione instaurerà un nuova connessione verso il secondo.
- SIGUSR2 bloccherà il traffico proveniente dal dispositivo di edge.

Differenze tra soluzione1 e soluzione2: La prima instaura una connessione TCP da Edge al primo Fog, la seconda due connessioni UDP verso entrambi i fog (SDN instraderà il traffico verso solo uno dei due).
