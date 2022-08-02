[BITS 16]	; Codice a 16 bit
[ORG 0x7C00]	; L'indirizzo 0x7C00 il primo settore caricato al boot

jmp short boot_start	; Salto la parte del boot sector riservata ai dati FAT
times 3-($-$$) nop	; Mi sposto al terzo byte

OEM_ID			db "MSWIN4.1"
BytesPerSector		dw 0x0200
SectorsPerCluster	db 0x01
ReservedSectors		dw 0x0001
TotalFATs		db 0x02
MaxRootEntries		dw 0x00E0
TotalSectorsSmall	dw 0x0B40
MediaDescriptor		db 0xF0
SectorsPerFAT		dw 0x0009
SectorsPerTrack		dw 0x0012
NumHeads		dw 0x0002
HiddenSectors		dd 0x00000000
TotalSectorsLarge	dd 0x00000000
DriveNumber		db 0x00
Flags			db 0x00
Signature		db 0x29		; 0x28 o 0x29
VolumeID		dd 0xFFFFFFFF
VolumeLabel		db "BEWOS      "
SystemID		db "FAT12   "

boot_start:
	xor	ax, ax		; Imposta i segmenti
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	sp, 0xFFFF	; Imposta lo stack
	mov	bp, 0xFE00	; Usato per tenere traccia dei cluster da leggere

; calcola la dimensione in settori della root directory e la salva in ax
	xor	cx, cx
	xor	dx, dx
	mov	ax, 32			; ogni entry della directory e' grande 32 bytes
	mul	word [MaxRootEntries]	; calcola (in ax) la grandezza in bytes della directory root
	div	word [BytesPerSector]	; calcola (in ax) il numero di settori occupati dalla directory
	push	ax			; salvo l'informazione appena trovata sullo stack

	mov	[BASE_OPEN_SPACE], ax	; Salvo la grandezza della root directory anche qua in modo da determinare, aggiungendo in seguito anche il base sector della root, il settore da cui inizia l'open space.

; calcolo il settore da cui inizia la directory root in ax
	xor	ax, ax
	mov	al, byte [TotalFATs]                ; metto in al il numero di FATs presenti su disco
	mul	word [SectorsPerFAT]                ; calcolo i settori occupati dalle FATs (TotalFATs*SectorsPerFAT)
	add	ax, word [ReservedSectors]          ; aggiungo il numero di settori riservati (di solito solo il bootsector)

	add	[BASE_OPEN_SPACE], ax	; Aggiungo anche il base sector della root ottenendo cosi' il settore da cui inizia l'open space.

; leggo i settori occupati dalla directory root e li salvo immediatamente dopo il codice di avvio
read_rootdir:
	mov	bx, 0x07E0
	mov	es, bx		; Imposto il segmento di destinazione
	pop	bx		; Ripristino dallo stack il numero di settori da leggere
	call	read_floppy	; Effettuo l'operazione di lettura

; cerco all'interno della directory root appena letta il file [FILENAME]
explore_rootdir:
	mov	cx, word [MaxRootEntries]	; contatore per il loop di ricerca
	mov	di, 0x7E00			; indirizzo della prima entry della directory root
.readdir:
	push	cx		; Salvo cx
	mov	cx, 11		; Imposto il numero di bytes da confrontare
	mov	si, FILENAME	; Imposto la stringa di 11 caratteri con cui effettuare il confronto (nome del file da cercare)
	call	memcmp		; Confronto
	pop	cx		; Ripristino cx
	cmp	ax, 0		; Controllo il risultato di memcmp
	je	.filefound	; Se questo e' il file che cercavo salto a .filefound
	add	di, 32		; Altrimenti passo alla entry successiva
	loop	.readdir
.filenotfound:			; Se sono arrivato qui vuol dire che non ho trovato il file
	mov	si, FILE_NOT_FOUND
	call write		; Stampo il messaggio di errore
	jmp $

.filefound:
	; Se sono arrivato qui vuol dire che il file e' stato trovato
	; In di e' salvato il puntatore all'entry della directory root che contiene le informazioni sul file
read_clusters: ; Ora che il file e' stato individuato, procedo creando un'array di tutti i cluster da leggere in sequenza
	mov	bx, word [di+26]	; Leggo il numero del primo cluster del file
	push	bx			; Salvo questo dato appena trovato

; Leggo in 0x7E00 la FAT
	mov	ax, [ReservedSectors]	; La FAT inizia dopo i settori riservati
	mov	bx, [SectorsPerFAT]	; Numero di settori da leggere
	mov	cx, 0x07E0
	mov	es, cx			; Segmento di destinazione
	call	read_floppy		; Leggo la FAT

	pop	bx			; Ripristino il numero del primo cluster
.loop:
	call	add_cluster		; Aggiungo il numero di cluster alla lista
	call	read_next_cluster	; Leggo il cluster successivo
	cmp	bx, 0xFF8		; Era il cluster finale? (il cluster finale e' indicato come maggiore o uguale a 0xFF8)
	jl	.loop			; Se no ripeto le operazioni per il prossimo cluster

	mov	bx, 0xFFF
	call	add_cluster	; Aggiungo all fine della lista dei cluster da leggere un marcatore che ne indica la fine (0xFFF)
	mov	bp, 0xFE00	; Ritorno al primo elemento della lista dei cluster

	jmp	load_clusters		; Dopo che ho inserito tutti i cluster da leggere nella lista salto alla prte di codice che si occupa di leggere ad uno ad uno i clusters e caricarli in memoria

; Legge dalla FAT il cluster del file successivo a quello passato tramite bx e lo salva in bx
read_next_cluster:

; Calcolo l'indirizzo di memoria dell'entry della FAT da considerare e lo salvo in bx (Indirizzo: BASE_ADDRESS_FAT+(CLUSTER * 1.5) )
	mov	si, 0x7E00	; Imposto il base address della FAT
	add	si, bx
	mov	dx, bx
	shr	dx, 1	; DX/2
	add	si, dx
	mov	dx, word [si]	; Leggo due bytes dalla FAT
	test	bx, 1	; Controllo se la divisione precedente ha avuto resto (in base a questo decido se prendere i 12 bit alti o bassi della dword appena letta)
	jnz	.take_high12
.take_low12:
	and	dx, 0xFFF	; Prendo in considerazione i 12 bits bassi
	jmp	.read_end
.take_high12:
	shr	dx, 4		; Sposto a destra di 4 bits il contenuto della dword, in modo da ottenere i 12 bits alti
.read_end:
	mov	bx, dx		; Salvo in bx il risultato
	ret

; Questa routine aggiunge un cluster alla suddetta array (cluster letto da bx)
add_cluster:
	mov	word [bp], bx
	add	bp, 2
	ret

; Questa altra routine invece legge un cluster dall'array e lo ritorna in dx. Se non ci sono piu' cluster da leggere torna 0xFFFF
read_cluster:
	mov	dx, word [bp]
	add	bp, 2
	ret

BASE_OPEN_SPACE dw 0		; Verra' riempito con il settore di base dell'open space (lo spazio che contiene i dati veri e propri dei files)
SEGMENTS_PER_CLUSTER dw 0	; Corrisponde a (settori_per_cluster*bytes_per_segmento) >> 4. Dato che la funzione read_floppy richede come argomento in es un segmento, in questo valore salvo il numero di segmenti da incrementare per ogni cluster.

load_clusters:
	mov	ax, [BytesPerSector]
	mul	word [SectorsPerCluster]
	shr	ax, 4		; Numero di segmenti da incrementare per ogni cluster letto
	mov	[SEGMENTS_PER_CLUSTER], ax

	mov	ax, 0x1000
	mov	es, ax

	xor 	bx, bx
	mov	bl, byte [SectorsPerCluster]	; Settori per cluster da leggere

	xor	cx, cx
.loop:
	call	read_cluster
	cmp	dx, 0xFFF	; Controllo se ci sono cluster da leggere
	je end			; Se non ce n'e' salto a end
	; Calcolo il settore di base con la seguente formula: settore = baseopenspace + ( (cluster-2) * SectorsPerCluster )
	pusha
	sub	dx, 2
	xor	ax, ax
	mov	al, byte [SectorsPerCluster]
	mul	dx
	add	ax, [BASE_OPEN_SPACE]

	call	read_floppy
	popa

	inc	cx

	mov	ax, [SEGMENTS_PER_CLUSTER]
	mul	cx
	add	ax, 0x1000
	mov	es, ax

	jmp .loop
end:
	jmp	0x1000:0	; Ora che il file e' stato caricato lo eseguo!!!! :D

; Stampa sullo schermo una stringa di testo attraverso le routine del BIOS.
; Il registro si deve contenere la locazione di memoria della stringa
write:
	lodsb
	or	al, al
	jz	.end
	mov	ah, 0xE
	mov	bh, 0
	mov	cx, 2
	int	0x10
	jmp	write
.end:
	ret

; Compara cx bytes di [si] con [di]
; Se le stringhe sono ugulai ax viene impostato a 0, altrimenti a 1
memcmp:
	pusha
.loop:
	mov	bl, [si]
	cmp	[di], bl
	jne	.neq
	inc	si
	inc	di
	loop	.loop
.eq:
	popa
	mov	ax, 0
	ret
.neq:
	popa
	mov	ax, 1
	ret

; Legge uno o piu' settori da un floppy
; Input:	AX	Settore logico di partenza (es: 0, 1, 242 ecc...)
;		ES	Segmento di destinazione (la scrittura sara' effettuata partendo dall'offset 0)
;		BX	Numero di settori da leggere
; Output:	ES:0	Contenuto dei settori indicati
read_floppy:
; Traduce un indirizzo logico in settore,testina,cilindro come richesto dall'int 0x13
	xor	dx,dx
	div	word [SectorsPerTrack]	; divide ax per settori_per_traccia
	inc 	dl		; incremento dl per ottenere il numero di settore (i settori vengono contato partendo da da 1)
	mov	cl, dl		; salvo in cl il numero di settore trovato
	xor	dx, dx
	div	word [NumHeads]
	mov	ch, al		; cilindro (chiamato anche traccia)
	mov	dh, dl		; testina
; Legge dal floppy a partire dal settore specificato con
	mov	di, bx		; Dimensione del kernel in settori
	mov	dl, [DriveNumber] ; Numero del drive
	xor	bx, bx		; Azzero il buffer es:bx  = 0x1000:0 = Primo Mega
.lettura:
	mov	ah, 0x2		; Numero del servizio
	mov	al, 0x1		; Numero di settori da leggere (legge un settore alla volta)
	int	0x13		; Esegue la lettura
	jc	.error		; Gestisci l'eventuale errore
	mov	ax, es		; Metto il valore di es in ax
	add	ax, 0x20	; Modifico il valore di ax
	mov	es,ax		; Riaggiorno il valore di es
	dec	di		; Decrementiamo di uno il numero dei settori
	jz 	.end		; Se non vi sono pi settori, termina
	inc	cx		; Incremento il numero del settore
	cmp	cl, [SectorsPerTrack] ; Il settore da leggere con il numero di settori
	jna	.lettura	; Se cl e' minore di [SectorsPerTrack], continua a leggere
	mov	cl, 0x1		; Altrimenti ricomincio dal primo settore
	inc	dh		; Incremento il numero della testina
	cmp	dh, [NumHeads]	; Comparo dh e il numero di testina
	jne	.lettura	; Se la testina 1 allora tutto bene
	xor	dh, dh		; Altrimenti seleziono la testina 0
	inc	ch		; Incremento il numero della traccia
	jmp 	.lettura	; Continua a leggere
.error:
	mov	si, FDC_ERROR
	call	write		; Stampa il messaggio di errore
	jmp	$
.end:
	ret			; Ritorna al chiamante
	

FDC_ERROR	db "Errore di lettura", 0
FILE_NOT_FOUND	db "File non trovato", 0


FILENAME	db "KERNEL  BIN"	; Nome del file da cercare (11 caratteri, rimepito con degli spazi, tutto maiuscolo, forma 8 + 3, omettere il punto prima dell'estensione)

times 510-($-$$) db 0
dw 0xAA55		; Boot Signature