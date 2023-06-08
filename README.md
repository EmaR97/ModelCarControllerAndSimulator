## Istruzioni per l'uso

Questa cartella contiene in codice usato relativo alla simulazione e l'archivio con il codice per il micro controllore.

Nel file "[main.c](main.c)" sono evidenziate diverse parti del codice esclusive del simulatore e del micro-controllore:
- L'inizio e la fine dei segmenti di codice relativi alla simulazione sono evidenziati dalla presenza di due commenti (START SIMULATION PART, END SIMULATION PART). Queste parti vanno commentate se si vuole usare il programma er controllare il micro-controllore perché vanno a sovrascrivere alcune funzioni essenziali per il funzionamento del micro.
- L'inizio e la fine dei segmenti di codice relativi solo al micro_controllore sono evidenziati dalla presenza di due commenti (MICRO ONLY START, MICRO ONLY END).queste parti vanno commentate se si vuole eseguire la parte della simulazione.

Il file "[Array_ostacoli.txt](Array_ostacoli.txt)" contiene una sequenza di ostacoli, che rappresentano un percorso di prova. Possono essere caricati all'inizio del programma di simulazione per provare il funzionamento del programma per la selezione del percorso.

Il file "[percorso di prova.png](percorso%20di%20prova.png)" è una rappresentazione del percorso di prova usato per il testing del l'algoritmo di selezione del percorso

L'eseguibile per provare la simulazione è nella cartella "[cmake-build-debug](cmake-build-debug)"

L'archivio "[ferrari_st-master](ferrari_st-master.zip)" contiene il progetto da compilare e caricare sulla macchinina tramite AutoDevKit (le parti di codice relative alla simulazione sono state gia rimosse).


## Funzionamento 

Per il corretto funzionamento del programma occorre conoscere la velocità e la velocità angolare a regime del modellino, queste informazioni vengono ricavate tramite una fase di calibrazione. I risultati di questa fase possono essere visualizzati e inseriti nelle costanti del codice o, se le condizioni lo permettono, è possibile effettuare la fase di calibrazione all avvio del modellino. Per tenere conto del esaurimento della batteria, che influisce sulla potenza erogata dai motori e quindi sulla velocità, effettuare la calibrazione all'accensione. Occorre effettuare la calibrazione al variare del terreno su cui si adopera il modellino per tenere conto di eventuali slittamenti delle ruote su superfici lisce. 

### Calibrazione velocità 

Per eseguire la calibrazione completa(complete_calibrate), occorre posizionare la macchina appoggiata perpendicolarmente a un muro dritto di lunghezza sufficiente (2m circa). Per il corretto funzionamento occorre assicurarsi che il sensore TOF sia correttamente calibrato. La fase di calibrazione sara composta dalle seguenti fasi:
- Il modellino si allontanerà dal muro di una 20cm e rileverà la distanza dal muro(distanza_iniziale).
- Il modellino si muoverà all'indietro per un tempo fisso(initial_time), che si ritiene essere sufficiente per il raggiungimento della velocità di regime. Allo spegnimento si rileva nuovamente la distanza percorsa. Il confronto tra le due misure ci dà la distanza minima che si può percorrere con un unico movimento(initial_value). 
- Mantenendo i motori accesi in direzione inversa per lo stesso tempo(initial_time) torno alla posizione di partenza.
- Il modellino si muoverà all'indietro per un tempo(final_time) maggiore rispetto a quello iniziale(initial_time) e concluso il movimento si rileverà la distanza percorsa(final_value).
- Calcolo la velocità media come:(final_value-initial_value)/(final_time-initial_time). Se lo slittamento delle ruote non è eccessivo la stima sulla velocità si rivela abbastanza valida.
- Per verificare il corretto funzionamento si usano i valori acquisiti per far percorre alla macchina distanze note che possono essere verificate dall'operatore umano.
- Alla fine della fase di verifica il modellino dovrebbe trovarsi nelle alla distanza di 20 cm dal muro e perpendicolare a esso.

Conclusa la fase di calibrazione della velocità si procede alla rilevazione della velocità angolare. La procedura è la stessa, l'unica differenza è che la macchina ruota su se stessa. Il muro piatto e la posizione perpendicolare a questo della macchina permettono il calcolo degli angoli di rotazione. 

### Controllo movimento

Modellino ruota su se stesso a velocità costante fino a compiere un giro completo. Durante la rotazione scansiona gli ostacoli a intervalli costanti, la posizione degli ostacoli rispetto al modellino viene calcolata in base alla velocità angolare stimata, al tempo trascorso dall'accensione dei motori e alla distanza rilevata dal TOF. Non tutti i punti rilevati vengono salvati. 
Vengono definiti dei range di distanza entro i quali i punti vengono salvati al percorrere di un certo angolo, ad esempio i punti entro 40cm vengono salvati ogni 45° mentre quelli a 2m vengono salvati ogni 5°. Questa tecnica permette di ridurre il tempo impiegato in elaborazione. Gli intervalli scelti sono calcolati in modo che il programma interpreti correttamente gli ostacoli anche se definiti tramite un numero ridotto di punti. Salvo il punto anche al variare del range distanza rispetto al precedente punto salvato, per avere un maggiore dettagli sulla forma degli ostacoli. Se non vengono rilevati ostacoli si salva l'ultimo punto rilevato che rappresenta l'estremo di un ostacolo.

Ogni due punti salvati controllo se il corridoio compreso tra questi è sufficiente largo per il passaggio della macchinina. Questa dimensione minima viene fissata pari alla diagonale della macchina più una distanza di sicurezza pari a 10cm, per assicurarsi che il modellino abbia spazio sufficiente per ruotare su se stesso.

Al termine della rotazione viene scelto il passaggio più vicino alla direzione base che la macchina deve seguire. Tramite le stime sulla velocità il modellino può stimare la propria posizione e la direzione verso cui è puntato, considerando la posizione di partenza come il punto 0,0 e la direzione iniziale quella di angolo 0. A meno di deviazioni dovute all'imprecisione sulla stima della velocità, il modellino è in grado di riconoscere la direzione a 0° e di scegliere il percorso che la porti più lontano possibile in questa direzione. L'algoritmo impedisce, a meno che non sia l'unica soluzione possibile(caso di vicoli cechi) retrocedere lungo i propri passi. Quando il modellino è costretto a tornare indietro la posizione precedente viene salvata come vicolo ceco in modo da essere evitata in iterazione successive della funzione di controllo del movimento. 

Per compensare parzialmente l'accumulo degli errori sul calcolo della posizione dovuti alla stima sulla velocità angolare, la fase di rotazione e scansione avviene in direzione inversa a cicli alterni. 

### Simulazione

Le parti che dipendo dal sensore e dal micro-controllore sono state riprodotte per testare il funzionamento dell'algoritmo di controllo del movimento realizzato. 

Lo scorrere del tempo viene approssimato im modo da simulare il funzione delle varie componenti il cui comportamento è strettamente legata a questo, come TOF e motori. 

La posizione della macchina viene calcolata come nel caso reale in base alle informazioni sulla velocità e al tempo che i motori rimangono accesi. 

Gli ostacoli, la macchina e il vettore di visione del TOF sono stati modellati come un insieme di segmenti:
- se, successivamente a un movimento, i segmenti rappresenti la macchina si sovrappongono a quelli rappresentanti un ostacolo, il modellino ha subito una collisione con un ostacolo.
- mantenendo aggiornata la posizione della macchina, creo un segmento che si estende davanti a questa che rappresenta e il vettore di visione del TOF e rilevo l'ostacolo come la sovrapposizione tra questo e i segmenti che rappresentano gli ostacoli.

Il comportamento simulato è abbastanza fedele a quello reale nel caso in cui i valori di velocità ottenuti durante la fase di calibrazione rispecchino correttamente quelli reali del modellino.