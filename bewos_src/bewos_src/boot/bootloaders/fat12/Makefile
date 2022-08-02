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
# Descrizione: Compilazione del bootsector per dischi con filesytem fat12
#***************************************************************************************************

include make.rules

bootloader_build:
	$(NASM) -fbin boot/bootloaders/fat12/bootsector.asm -o boot/bootloaders/fat12/bootsector.bin $(shell if test "$(DEBUG)" = "yes"; then echo "-DDEBUG"; fi)

bootloader_clean:
	$(RM) boot/bootloaders/fat12/bootsector.bin

install: boot/bootloaders/fat12/bootsector.bin kernel.bin
	@ # Attivo temporaneamente il permesso di esecuzione
	@ chmod +x boot/bootloaders/fat12/install_noformat
	@ echo " "
	@ echo "Installazione di BeWos sul floppy in uso..."
	@ exec boot/bootloaders/fat12/install_noformat
	@ # Disattivo il permesso di esecuzione
	@ chmod -x boot/bootloaders/fat12/install_noformat

floppy_img:
	@ echo "Per creare questo target e' necessario ottenere i privilegi di root."
	@ mkdir -p boot/bootloaders/fat12/mountpoint
	@ su -c 'dd if=/dev/zero bs=512 count=2879 | cat boot/bootloaders/fat12/bootsector.bin - > BeWos.img && \
		mount -o loop BeWos.img boot/bootloaders/fat12/mountpoint/ && \
		cp kernel.bin boot/bootloaders/fat12/mountpoint/kernel.bin && \
		umount BeWos.img'
