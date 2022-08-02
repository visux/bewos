/* Black & White Operating System
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details (in the COPYING file).
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
/******************************************INFO**********************************************
* Coders: Demone^ && Matrixina
* Descrizione: inizializzazione del sistema e dei driver
***********************************************************************************************/

#ifndef __KERNEL_H
#define __KERNEL_H

#include <types.h>
#include <io.h>
#include <stdarg.h>

/** costanti richieste 
*/
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define NULL ((void *)0)

/** estensione per il GCC 
*/
#define __init		__attribute__ ((__section__ (".text.init")))
#define __initdata	__attribute__ ((__section__ (".data.init")))
#define __init_call	__attribute__ ((unused,__section__ (".initcall.init")))
#define __noreturn	__attribute__ ((noreturn))

// Kernel tipi
typedef u_int32_t pid_t;
typedef u_int32_t block_t;
typedef u_int32_t ino_t;
typedef u_int32_t uid_t;
typedef u_int32_t gid_t;
typedef u_int32_t nlink_t;
typedef u_int32_t loff_t;
typedef u_int32_t umode_t;

/** Inizializzazione dei driver 
*/
#define driver_init(fn) __initcall(fn)
typedef void (*initcall_t)(void);
extern initcall_t __initcall_start,__initcall_end;
#define __initcall(fn) static initcall_t __initcall_##fn __init_call = fn

#endif 
/**__KERNEL_H__ 
*/ 
