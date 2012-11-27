#ifndef CORE_SYSDEF_H
#define CORE_SYSDEF_H

#ifdef ARCH_X86_32
#	define ARCH_X86
#	define ARCH_32B
#	define ARCH_32B_PTR
#	define ARCH_PTR_SIZE 32
#endif //ARCH_X86_32

#ifdef ARCH_X86_64
#	define ARCH_X86
#	define ARCH_64B
#	define ARCH_64B_PTR
#	define ARCH_PTR_SIZE 64
#endif //ARCH_X86_64

#ifdef ARCH_CELL_PPU
#	define ARCH_CELL
#	define ARCH_32B
#	define ARCH_32B_PTR
#	define ARCH_PTR_SIZE 32
#endif //ARCH_CELL_PPU

#ifdef ARCH_CELL_SPU
#	define ARCH_CELL
#	define ARCH_32B
#	define ARCH_32B_PTR
#	define ARCH_PTR_SIZE 32
#endif //ARCH_CELL_SPU

#endif //CORE_SYSDEF_H
