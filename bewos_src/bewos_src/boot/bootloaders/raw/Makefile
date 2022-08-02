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
# Coders: WMI
# Descrizione: Compilazione del bootsector per dischi privi di filesytem (raw)
#***************************************************************************************************

include make.rules

bootloader_build: kernel.bin
	$(NASM) -fbin boot/bootloaders/raw/bootsector.asm -o boot/bootloaders/raw/bootsector.bin $(shell if test "$(DEBUG)" = "yes"; then echo "-DDEBUG"; fi) -DDIM_KERNEL_BYTES=$(shell stat --format=%s kernel.bin)

bootloader_clean:
	$(RM) boot/bootloaders/raw/bootsector.bin BeWos.img BeWos_padded.img

install:
	@ echo "ERRORE: install e' impossibile per dischi raw"

floppy_img:
	@ echo "creazione immagine di disco avviabile ..."
	cat boot/bootloaders/raw/bootsector.bin kernel.bin > BeWos.img
