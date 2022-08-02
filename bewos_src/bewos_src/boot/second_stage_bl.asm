; Black & White Operating System
;
; This program is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the
; Free Software Foundation; either version 2 of the License, or (at your
; option) any later version.
;
; This program is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; General Public License for more details (in the COPYING file).
;
; You should have received a copy of the GNU General Public License along
; with this program; if not, write to the Free Software Foundation, Inc.,
; 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

;*****************************************INFO******************************************************
; Coders: WMI
; Descrizione: Passaggio dalla modialita' reale a quella protetta e avvio di BeWos
;***************************************************************************************************

[bits 16]	; Codice a 16 bit
[segment 0x1000]
[org 0]

	mov	ax, 0x1000
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	sp, 0xFFFF

	;code segment (cs) = 1000

%ifdef DEBUG
	; Stampa il messaggio di benvenuto
	mov	si, BEWOOT
	call	write
%endif

	; Floppy motor-off
	mov	dx, 0x3F2
	mov	al, 0xC
	out	dx, al

	; Disabilitamo gli interrupt
%ifdef DEBUG
	mov	si, DISINT
	call	write
%endif
	cli

	; Disabilito i NMI
	in	al, 0x70
	or	al, 0x80
	out	0x70, al

%ifdef DEBUG
	mov	si, ACTA20
	call	write
%endif
	; Abilitiamo la linea a20
wait1:
	in	al, 0x64
	test	al, 0x2
	jnz	wait1
	mov	al, 0xD1
	out	0x64, al
wait2:
	in	al, 0x64
	and	ax, byte 0x2
	jnz	wait2
	mov	al, 0xDF
  	out	0x60, al
	
	; Carichiamo la GDT
%ifdef DEBUG
	mov	si, LOADGDT
	call	write
%endif
	lgdt	[0x10000+(gdtinfo-$$)]	; Aggiungo 0x10000 perche' siamo sul segmento 0x1000 (0x1000:gdtinfo = 0x10000+gtdinfo)

	; Settiamo il bit PE
%ifdef DEBUG
	mov	si, BITPE
	call	write
%endif
	mov	eax, CR0
	or	al, 0x1
	mov	CR0, eax

	mov	ax, 0x10
	mov	ds, ax

	; Passiamo al codice a 32 bit
	jmp	dword 0x08:(0x10000+(lancia_kernel-$$))	; Aggiungo 0x10000 per lo stesso motivo di prima


[BITS 16]
; Stampa sullo schermo una stringa di testo attraverso le routine del BIOS.
; Il registro si deve contenere la locazione di memoria della stringa
write:
	lodsb
	or	al, al
	jz	write_end
	mov	ah, 0xE
	mov	bx, 0x7
	int	0x10
	jmp	write
write_end:
	ret

; Imposta la struttura base della GDT
gdtinfo:
	dw	gdtlength	; Numero di elementi presenti nella GDT
	dd	0x10000+(gdt_table-$$) ; Indirizzo della GDT
gdt_table:
	dd	0	; Il primo elemento e' nullo
	dd	0	; Il primo elemento e' nullo
.flat_code:
	dw	0xFFFF	; Limite bit 0->15
	dw	0x0000	; Base bit 0->15
	db	0x00	; Base bit 16->23
	db	0x9A	; Segmento codice con privilegio 0
	db	0xCF	; Limite bit 16->19 segmento a 32 bit step: 4096 byte
	db	0x00	; Base 24->32
.flat_data:
	dw	0xFFFF	; Limite bit   0->15
	dw	0x0000	; Base bit 0->15
	db	0x00	; Base bit 16->23
	db	0x92	; Segmento dati con privilegio 0
	db	0xCF	; Limite bit 16->19 segmento a 32 bit step: 4096 byte
	db	0x00	; Base 24->32

gdtlength	equ	$ - gdt_table	; Dimensione della GDT

[BITS 32]	; Codice a 32 bit
lancia_kernel:
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	mov	esp, 0x9FFF0	; Imposto lo stack

	; Ultima fase prima del lancio
	push	dword 0x2	; Azzero gli EFLAGS
	popfd			; Imposto gli EFLAGS
	mov	eax, 0x10000+(kernel-$$) ; Imposta la locazione di memoria del kernel

	

	jmp eax			; Eseguo il kernel

; Messaggi da stampare a video
%define NEWLINE	13,10	; Equivale a "\r\n"
%define NUL	0	; E' il nul finale delle stringhe ('\0')

%ifdef DEBUG
BEWOOT		db	NEWLINE, "Bewoot: Second stage in esecuzione", NEWLINE, NUL
DISINT		db	"Disabilitazione degli interrupts...", NEWLINE, NUL
LOADGDT		db	"Caricamento della GDT (Global Descriptor Table) temporanea...", NEWLINE, NUL
BITPE		db	"Attivazione della modalita' protetta (settaggio bit PE)...", NEWLINE, NUL
ACTA20		db	"Setup linea di indirizzamento a20", NEWLINE, NUL
%endif

align 512

; Immediatamente dopo il second stage viene aggiunto il kernel. Con la seguente etichetta indico l'inizio del kernel
kernel:
