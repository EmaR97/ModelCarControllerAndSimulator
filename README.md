## Istruzioni per l'uso

Nel file "[main.c](main.c)" sono evidenziate diverse parti del codice esclusive del simulatore e del micro-controllore:
- L'inizio e la fine dei segmenti di codice relativi alla simulazione sono evidenziati dalla presenza di due commenti (START SIMULATION PART, END SIMULATION PART). Queste parti vanno commentate se si vuole usare il programma er controllare il micro-controllore perché vanno a sovrascrivere alcune funzioni essenziali per il funzionamento del micro.
- L'inizio e la fine dei segmenti di codice relativi solo al micro_controllore sono evidenziati dalla presenza di due commenti (MICRO ONLY START, MICRO ONLY END).queste parti vanno commentate se si vuole eseguire la parte della simulazione.

Il file "[Array_ostacoli.txt](Array_ostacoli.txt)" contiene una sequenza di ostacoli, che rappresentano un percorso di prova. Possono essere caricati all'inizio del programma di simulazione per provare il funzionamento del programma per la selezione del percorso.

Il file "[percorso di prova.png](percorso%20di%20prova.png)" è una rappresentazione del percorso di prova usato per il testing del l'algoritmo di selezione del percorso

## Funzionamento 

Per il corretto funzionamento del programma occorre conoscere la velocità e la velocità angolare a regime del modellino, queste informazioni vengono ricavate tramite una fase di calibrazione. I risultati di questa fase possono essere visualizzati e inseriti nelle costanti del codice o, se le condizioni lo permettono, è possibile effettuare la fase di calibrazione all avvio del modellino. Per tenere conto del esaurimento della batteria, che influisce sulla potenza erogata dai motori e quindi sulla velocità, effettuare la calibrazione all'accensione. Occorre effettuare la calibrazione al variare del terreno su cui si adopera il modellino per tenere conto di eventuali slittamenti delle ruote su superfici lisce. 

## Calibrazione velocità 

Per eseguire la calibrazione completa(complete_calibrate), occorre posizionare la macchina appoggiata perpendicolarmente a un muro dritto di lunghezza sufficiente (2 metri circa). Per il corretto funzionamento occorre assicurarsi che il sensore TOF sia correttamente calibrato. La fase di calibrazione sara composta dalle seguenti fasi:
- Il modellino si allontanerà dal muro di una 20 di centimetri e rileverà la distanza dal muro(distanza_iniziale).