; --- Le seguenti direttive servono a determinare il numero di settori da caricare

; La dimensione del kernel viene determinata tramite DIM_SECONDSTAGE_BYTES, ricevuto tramite linea di comando al momento della compilazione
; Traformiamo la dimensione del kernel in bytes in numero di settori occupati
%xdefine DIM_KERNEL_SECTORS DIM_KERNEL_BYTES/512
%if DIM_KERNEL_BYTES % 512 > 0
%xdefine DIM_KERNEL_SECTORS DIM_KERNEL_SECTORS + 1
%endif

[BITS 16]	; Codice a 16 bit
[ORG 0x7C00]	; L'indirizzo 0x7C00 il primo settore caricato al boot
	xor	ax, ax
	mov	ds, ax
	mov	ss, ax
	mov	sp, 0xFFFF

	mov	bl, 0x7
	mov	si, BEWOOT
	call	write

	; Carica il kernel in memoria
	mov	ax, 0x1000
	mov	es, ax
	mov	ax, 1
	mov	bx, 80
	call	read_floppy	; Carico il kernel

	mov	si, LOADOK
	call write

	jmp	0x1000:0	; Ora che il kernel e' stato caricato lo eseguo!!!! :D

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

DriveNumber		db 0x00
SectorsPerTrack		dw 0x0012
NumHeads		dw 0x0002

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
	

; Messaggi da stampare a video
BEWOOT		db	'Bewos bootloader', 0xD, 0xA, 'Caricamento del second stage...', 0x0
LOADOK		db	'                                            [Go]', 0xD, 0xA, 0x0
FDC_ERROR	db	' Errore nella lettura del floppy.', 0xD, 0xA, 0xD, 0xA, 'Impossibile avviare BeWos. Installazione non corretta o floppy danneggiato', 0x0

; Azzera i byte rimanenti del bootsector e imposta i terminatori
PADDING equ 510-($-$$)
%if PADDING < 0
	%error Il first stage bootloader non puo' essere piu' grande di 512 bytes
%endif
times	510-($-$$)	db	0
dw	0xAA55
