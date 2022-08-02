;Generic BootLoader with switch to pmode, Writed by Baldosoft

; NON USATO


;Condizioni iniziali
;CS: 0 
;DS: 0
;ES: 0
;SS: 0
;IP: 0x7C00
;SP: 0x400

org 0x7C00

SETTORI_DA_LEGGERE EQU 74

mov bx, Messaggio1
call puts

mov bx, Messaggio2
call puts

mov ah, 0x2 ;numero comando dell'interrupt
mov al, 0x1 ;numero di settori da leggere
mov ch, 0x0 ;numero della traccia (0->79)
mov cl, 0x2 ;numero settore (1->18) lo imposto a 0x2 perche' il primo settore e' occupato dal boot loader
mov dh, 0x0 ;numero testina
mov dl, 0x0 ;numero drive

mov bx,0x1000 ;buffer = 0x10000 omettiamo uno zero perche' es e' il segmento
mov es,bx     ;copiamo da bx perche' non e' consentito copiare direttamente in es
mov bx,0x0    ;azzerriamo l'offset, ovvero bx
mov si,SETTORI_DA_LEGGERE  ;utilizziamo si come contatore dei settori da leggere
mov ds,bx    ;azzeriamo ds che verra' usato come contatore per i segni da stampare
 
; qua viene letto 
LeggiSettore:

    pusha          ;salviamo e carichiamo i registri prima e dopo l'interrupt
                   ;perche' alcuni bios sono buggati e modificano registri che non andrebbero
                   ;modificati
    int 0x13            ;leggiamo un settore
    jc ErroreInLettura  ;se c'e' stato un errore facciamo un jmp a ErroreInLettura
    
    mov bx,es           ;aggiungiamo 512 al segmento: 
    add bx,0x20         ;0x20*0x10 (perche' il seg. va moltiplicato per 0x10) = 0x200(512)
    mov es,bx           
    
    mov bx,ds
    inc bx
    
    cmp bx,4            ;se il contatore e' arrivato a 4 lo riportiamo a 0
    je NoReset
    mov bx,0
    NoReset:
    
    mov al,8          ;stampiamo un carattere backspace
    call putchar
    
    mov al,[Segni+bx]   ;stampiamo un segno
    call putchar
    
    mov ds,bx
    popa
    
    dec si             ;decrementiamo si(contatore dei settori da leggere) e se e' 0 saltiamo alla
    cmp si,0           ;fine della lettura
    je FineLettura
    
    cmp cl,18
    je MaxSettori
    inc cl
    
    jmp LeggiSettore
    
MaxSettori:
    cmp dh,0x1
    je MaxTestina
    
    mov dh,0x1
    mov cl,1
    jmp Controllo
    
MaxTestina:
    inc ch
    mov dh,0
    mov cl,1

Controllo:
    cmp ch,80
    je FineLettura
    
    jmp LeggiSettore
    
    push bx
    mov bx,es
    add bx,0x20
    mov es,bx
    pop bx
    
    cmp cl,18
    jge MaxSettori
    
    inc cl
    jmp LeggiSettore
    
FineLettura:

    call SpegniDriveFloppy    ;spegniamo il motore del floppy
    
    mov bx,MessaggioOK
    call puts
    
    mov bx,Messaggio3
    call puts
    
    call DeterminaGrandezzaRam   ;esegue una chiamata al bios
    
    jmp InizializzaProtectMode   ;saltiamo all'inizializzazione della pmode

ErroreInLettura:
    mov bx,Errore1
    call puts
    call SpegniDriveFloppy
    cli
    hlt

SpegniDriveFloppy:
  mov dx,0x3F2
  mov al,0xC
  out dx,al
  ret

putchar:
    push ax
    mov ah,0x0E
    int 0x10
    pop ax
    ret

puts:
    push bx
    push ax
    
    StampaCarattere:
    
        mov al,byte [bx]
        
        cmp al,0
        je FineStampa
        
        call putchar
        
        inc bx
        jmp StampaCarattere
    
    FineStampa:
    
    pop ax
    pop bx
    ret
    
DeterminaGrandezzaRam:
    mov  ah,0x88
    jnc NoRamError
    mov ax,0x0
    NoRamError:
    
    mov [DimensioniRam],ax
    ret

AspettaOutput:
    in al,0x64
    test al,2
    jnz AspettaOutput
    ret
    
InizializzaProtectMode:
    cli
    
    in  al,0x70
    or  al,0x80    ;disabilito gli NMI
    out 0x70,al

; abilitiamo la linea a20
    mov al,0xD0
    out 0x64,al
    
    call AspettaOutput
    
    xor ax,ax
    in  al,0x60
    or  al,2
    push ax
    
    call AspettaOutput
    
    mov al,0xD1
    out 0x64,al
    
    call AspettaOutput
    
    pop ax
    out 0x60,al
    
    call AspettaOutput
    
;la linea a20 e' inizializzata

    lgdt [TabellaGdt]  ;carichiamo le gdt
     
    mov eax,CR0
    or  eax,1
    mov CR0,eax         ;imposto il bit PE (ProtectMode Enable)
    
    jmp dword 0x8:AvviaProtectMode 
    
;i primi 3 bit del segmento indicano l' RPL , un altro bit indica la tabella di
;riferimento(0=GDT, 1=LDT) e gli altri bit indicano l' indice nella gdt

    
[BITS 32 ]

AvviaProtectMode:
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,0x9FFF0    ;imposto lo stack
    sti                ;riabilito gli interrupt

    push dword 2       ;azzero gli EFLAG
    popfd              ;e li imposto

    push dword [DimensioniRam]
    push dword 0

    jmp 0x10000     ;salta al kernel copiato nella ram

TabellaGdt:
dw  DIMENSIONI_TAB_GDT             ;il numero di elementi presenti nella gdt
dd  DESCRITTORE_NULLO              ;l'indirizzo della gdt

DESCRITTORE_NULLO:
    dd 0
    dd 0                ;primo descrittore gdt nullo

Gdt_code:
    dw 1111111111111111b     ;segment limit 0->15
    dw 0000000000000000b     ;base address 0->15
    db 00000000b             ;base address 16->23
    db 10011010b             ;1010=type 1=S 00=DPL 1=P 
    db 11001111b             ;1111=segment limit 0=AVL 0=costante 1=D/B 1=G
    db 00000000b             ;Base address 24->31
    
Gdt_data:
    dw 1111111111111111b     ;segment limit 0->15
    dw 0000000000000000b     ;base address 0->15
    db 00000000b             ;base address 16->23
    db 10010010b             ;0010=type 1=S 00=DPL 1=P 
    db 11001111b             ;1111=segment limit 0=AVL 0=costante 1=D/B 1=G
    db 00000000b             ;Base address 24->31
    
DIMENSIONI_TAB_GDT equ $ - DESCRITTORE_NULLO

Messaggio1 db "                        BeWos Operating System",13,10,13,10,0
Messaggio2 db "Caricamento kernel... ",0
Messaggio3 db "Avvio kernel...",0
MessaggioOK db 8,"OK",13,10,0

Errore1 db "Errore nella lettura del floppy.",0
Segni db "-\|/",0

DimensioniRam dw 0

times 510-($-$$) db 0
dw 0xAA55