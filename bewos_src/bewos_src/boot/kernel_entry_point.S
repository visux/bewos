/* Questo e' l'entry point del kernel, caricato subito dopo il bootloader second stage */

.extern main	/* Importo il simbolo della funzione main del kernel */
.global _start	/* Rendo l'entry point _start globale */

_start: /* Prima funzione lanciata dal bootloader second stage */
	call main	/* Lancio la funzione che si occupa di inizializzare il kernel */
	jmp .		/* Loop infinito nel caso la funzione ritorni (non divrebbe succedere) */
