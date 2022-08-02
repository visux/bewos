# Black & White Operating System
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#  
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details (in the COPYING file).
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 

#*****************************************INFO******************************************************
# Coders: Black, BlackPanter(soprattutto per l'idea), WMI
# Descrizione: Compilazione di BeWos (richiama inoltre altri Makefiles per generare i dischi avviabili)
#***************************************************************************************************

include make.rules

# Elenco dei files che dovranno essere generati separati da uno spazio
# In pratica e' il nome dei files che devono essere compilati, ma con estensone .o
# Es: Se il file da compilare e' kernel/io_port.c dovra'essre inserito di seguito: "kernel/io_port.o"
# L'utility make provera' a cercare il file sorgente (con estensione .S o .c) e con il nome indicato,
# non e' quindi possibile creare due files sorgenti con lo stesso nome ma con diversa estensione
OBJ = boot/kernel_entry_point.o main.o driver/console.o libc/vsprintf.o kernel/printk.o kernel/scank.o interrupt/idt.o kernel/io_port.o interrupt/pic8259.o interrupt/pit8253.o interrupt/timer.o interrupt/irq.o driver/keyboard.o libc/string.o apps/shell.o libc/math.o driver/floppy.o libc/time.o mm/gdt.o mm/phisicmem.o mm/phisicalloc.o mm/kmalloc.o mm/malloc.o task/scheduler.o task/task.o task/semaphores.o task/signals.o mm/paging.o kernel/syscall.o kernel/delay.o kernel/system.o kernel/wait.o apps/vfloppy.o

# Regole di compilazione:
.S.o:
	@ echo "$<: Compilazione (assembly at&t)"
	@ $(CC) $(CFLAGS) -I ./include/ -o $@ $< $(shell if test "$(DEBUG)" = "yes" ; then echo "-DDEBUG"; fi)
	@ echo " "

.c.o:
	@ echo "$<: Compilazione (codice C)"
	@ $(CC) $(CFLAGS) -I ./include/ -o $@ $< $(shell if test "$(DEBUG)" = "yes"; then echo "-DDEBUG"; fi)
	@ echo " "

# Target principale della compilazione
all:
	@ echo " "
	@ echo "Compilazione di Black & White Operating System"
	@ echo " Opzioni attive:"
ifeq ($(DEBUG),yes)
	@ echo "  - DEBUG   (verra' definita in tutti i files la macro DEBUG e il kernel verra'"
	@ echo "            disassemblato con objdump in kernel.S. Inoltre tutti i warnings so-"
	@ echo "            no attivati)"
endif
ifeq ($(FILESYSTEM),raw)
	@ echo "  - FILESYSTEM: raw"
else
ifeq ($(FILESYSTEM),fat12)
	@ echo "  - FILESYSTEM: fat12"
else
	@ echo " "
	@ echo "ERRORE: Nessun filesytem specificato o filesystem non valido"
	@ false	# Questo false mi serve per generare un'errore e uscire
endif
endif
ifdef IMG_SIZE
	@ echo "  - PADDING (verra' generata una seconda immagine BeWos_padded.img grande esat-"
	@ echo "            esattamente $(IMG_SIZE) byte$(shell if test "$(IMG_SIZE)" = "1474560"; then echo " quanto un floppy"; fi)"
endif
	@ make --quiet kernel_build
	@ echo " "
	@ echo "compilazione bootloader (fs: $(FILESYSTEM))"
	@ make --quiet -I "$(shell pwd)" -f boot/bootloaders/$(FILESYSTEM)/Makefile bootloader_build
	@ echo [ DONE ]  :: bootloader compilato

clean:
	@ make --quiet kernel_clean
	@ echo " "
	@ echo "CANCELLAZIONE DI OGGETTI E BINARI DEL BOOTLOADER... "
	@ make --quiet -I "$(shell pwd)" -f boot/bootloaders/$(FILESYSTEM)/Makefile bootloader_clean
	@ $(RM) BeWos.img BeWos_padded.img
	@ echo [ DONE ]

help:
	@ echo " "
	@ echo "Ci sono 5 targets disponibili:"
	@ echo " - make all"
	@ echo "        Compila il kernel e il bootloader ma non crea l'immagine del floppy"
	@ echo " - make clean"
	@ echo "        Ripulisce i files creati durante la compilazione"
	@ echo " - make floppy_img"
	@ echo "        Crea un'immagine del floppy avviabile"
	@ echo " - make install"
	@ echo "        Installa BeWos su floppy senza riformattare (solo FAT12)"
	@ echo " - make format_and_install"
	@ echo "        Installa BeWos su floppy riformattando (il contenuto del floppy verra'"
	@ echo "        cancellato)"
	@ echo " "
	@ echo "Esempio di alcune linee di comando utili:"
	@ echo " - Genera l'immagine del floppy in BeWos.img"
	@ echo "     make all && make floppy_img"
	@ echo " - Installa BeWos in un floppy senza riformattare"
	@ echo "     make all && make install"
	@ echo " - Genera l'immagine del floppy in BeWos.img e la installa riformattando"
	@ echo "     make all && make format_and_install"
	@ echo " "

floppy_img: kernel.bin
	@ # Questo target crea in BeWos.img un'immagine di floppy avviabile. Se IMG_SIZE e' definito verra' creata una seconda immagine BeWos_padded.img
	@ # Richiama il target floppy_img specifico del filesystem
	@ make --quiet -I "$(shell pwd)" -f boot/bootloaders/$(FILESYSTEM)/Makefile floppy_img
	@ make --quiet floppy_img_padding

floppy_img_padding: BeWos.img
ifdef IMG_SIZE
	@ echo "padding dell'immagine in BeWos_padded.img"
	@ dd if=/dev/zero bs=$(shell let bs=$(IMG_SIZE)-$(shell stat --format=%s BeWos.img); echo $$bs) count=1 2>/dev/null | cat BeWos.img - > BeWos_padded.img
endif

install: kernel.bin
	@ # Questo target si limita a chiamare l'install specifico di un filesystem senza riformattare
	@ make --quiet -I "$(shell pwd)" -f boot/bootloaders/$(FILESYSTEM)/Makefile install

format_and_install: floppy_img
	@ # Questo target copia l'immagine genarata da floppy_img su dischetto /dev/fd0
	@ dd if=BeWos.img of=/dev/fd0

# I seguenti 3 target (kernel_all, kernel_link, kernel_clean) servono a compilare il kernel e ad inserirvi all'inizio il second stage

kernel_build:
	@ echo " "
	@ echo INIZO DELLA COMPILAZIONE DEL KERNEL
	@ echo " "
	@ echo "boot/second_stage_bl.asm: Compilazione (assembly intel)"
	$(NASM) -fbin boot/second_stage_bl.asm -o boot/second_stage_bl.bin $(shell if test "$(DEBUG)" = "yes"; then echo "-DDEBUG"; fi)
	@ echo " "
	@ make --quiet $(OBJ)
	@ echo linking del kernel in corso ...
	@ make --quiet kernel_link
ifeq ($(DEBUG),yes)
	@ echo esecuzione di objdump ...
	objdump -D kernel.elf > kernel.S
	@ echo " "
endif
	@ echo creazione immagine del kernel ...
	cat boot/second_stage_bl.bin kernel_tmp.bin > kernel.bin
	@ $(RM) kernel_tmp.bin
	@ echo [ DONE ]  :: kernel.bin

kernel_link: boot/second_stage_bl.bin $(OBJ)
ifeq ($(DEBUG),yes)
	$(LD) $(LDFLAGS) -Ttext 0x$(shell printf %x $(shell let x=0x10000+$(shell stat --format=%s boot/second_stage_bl.bin); echo $$x)) -e _start -o kernel.elf -Map kernel.map $(OBJ) --oformat elf32-i386
	@ echo "linking: creato kernel.elf per consentire il disassemblaggio tramite objdump"
	$(LD) $(LDFLAGS) -Ttext 0x$(shell printf %x $(shell let x=0x10000+$(shell stat --format=%s boot/second_stage_bl.bin); echo $$x)) -o kernel_tmp.bin kernel.elf --oformat binary
else
	$(LD) $(LDFLAGS) -Ttext 0x$(shell printf %x $(shell let x=0x10000+$(shell stat --format=%s boot/second_stage_bl.bin); echo $$x)) -e _start -o kernel_tmp.bin -Map kernel.map $(OBJ) --oformat binary
endif

kernel_clean:
	@ echo "CANCELLAZIONE DI OGGETTI E BINARI DEL KERNEL... "
	@ $(RM) boot/second_stage_bl.bin kernel.bin kernel.map kernel.elf kernel.S $(OBJ)
	@ echo [ DONE ]
