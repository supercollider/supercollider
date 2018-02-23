/*

 Package: dyncall
 Library: dynload
 File: dynload/dynload_syms_mach-o.c
 Description:
 License:

   Copyright (c) 2007-2015 Olivier Chafik <olivier.chafik@gmail.com>,
                 2017-2018 refactored completely for stability, API
                           consistency and portability by Tassilo Philipp.

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/



/*

 dynamic symbol resolver for Mach-O

*/

#include "dynload.h"
#include "dynload_alloc.h"
#include "../autovar/autovar_ARCH.h"
#include "../autovar/autovar_OS.h"

#include <mach-o/dyld.h>
#include <mach-o/nlist.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>

#if defined(ARCH_X64) || defined(ARCH_PPC64) || defined(ARCH_ARM64) /*@@@ use dyncall_macros.h*/
#define MACH_HEADER_TYPE mach_header_64
#define MACH_HEADER_MAGIC_NR MH_MAGIC_64
#define SEGMEND_COMMAND_ID LC_SEGMENT_64
#define SEGMENT_COMMAND segment_command_64
#define NLIST_TYPE nlist_64
#else
#define MACH_HEADER_TYPE mach_header
#define MACH_HEADER_MAGIC_NR MH_MAGIC
#define SEGMEND_COMMAND_ID LC_SEGMENT
#define SEGMENT_COMMAND segment_command
#define NLIST_TYPE nlist
#endif


struct DLSyms_
{
	DLLib*                   pLib;
	const char*              pStringTable;
	const struct NLIST_TYPE* pSymbolTable;
	uint32_t                 symbolCount;
	uintptr_t                symOffset;
};


DLSyms* dlSymsInit(const char* libPath)
{
	DLLib* pLib;
	DLSyms* pSyms = NULL;
	uint32_t i, n;
	struct stat st0;
	const struct MACH_HEADER_TYPE* pHeader = NULL;
	const struct dysymtab_command* dysymtab_cmd = NULL;

	if(stat(libPath, &st0) == -1)
		return NULL;

	pLib = dlLoadLibrary(libPath);
	if(!pLib)
		return NULL;

	/* Loop over all dynamically linked images to find ours. */
	for(i = 0, n = _dyld_image_count(); i < n; ++i)
	{
		struct stat st1;
		const char* name = _dyld_get_image_name(i);

		if(name && (stat(name, &st1) != -1))
		{
			/* Don't rely on name comparison alone, as libPath might be relative, symlink, differently */
			/* cased, etc., but compare inode number with the one of the mapped dyld image. */
			if(st0.st_ino == st1.st_ino/*!strcmp(name, libPath)*/)
			{
				pHeader = (const struct MACH_HEADER_TYPE*) _dyld_get_image_header(i);
//@@@ slide = _dyld_get_image_vmaddr_slide(i);
				break; /* found header */
			}
		}
	}

	if(pHeader && (pHeader->magic == MACH_HEADER_MAGIC_NR) && (pHeader->filetype == MH_DYLIB) && !(pHeader->flags & MH_SPLIT_SEGS))
	{
		const char* pBase = (const char*)pHeader;
		uintptr_t slide = 0, symOffset = 0;
		const struct load_command* cmd = (const struct load_command*)(pBase + sizeof(struct MACH_HEADER_TYPE));

		for(i = 0, n = pHeader->ncmds; i < n; ++i, cmd = (const struct load_command*)((const char*)cmd + cmd->cmdsize))
		{
			if(cmd->cmd == SEGMEND_COMMAND_ID)
			{
				const struct SEGMENT_COMMAND* seg = (struct SEGMENT_COMMAND*)cmd;
				/*@@@ unsure why I used this instead of checking __TEXT: if((seg->fileoff == 0) && (seg->filesize != 0))*/
				if(strcmp(seg->segname, "__TEXT") == 0)
					slide = (uintptr_t)pHeader - seg->vmaddr; /* effective offset of segment from header */

				/* If we have __LINKEDIT segment (= raw data for dynamic linkers), use that one to find symbal table address. */
				if(strcmp(seg->segname, "__LINKEDIT") == 0) {
					/* Recompute pBase relative to where __LINKEDIT segment is in memory. */
					pBase = (const char*)(seg->vmaddr - seg->fileoff) + slide;

					/*@@@ we might want to also check maxprot and initprot here:
						VM_PROT_READ    ((vm_prot_t) 0x01)
						VM_PROT_WRITE   ((vm_prot_t) 0x02)
						VM_PROT_EXECUTE ((vm_prot_t) 0x04)*/

					symOffset = slide; /* this is also offset of symbols */
				}
			}
			else if(cmd->cmd == LC_SYMTAB && !pSyms/* only init once - just safety check */)
			{
				const struct symtab_command* scmd = (const struct symtab_command*)cmd;

				/* cmd->cmdsize must be size of struct, otherwise something is off; abort */
				if(cmd->cmdsize != sizeof(struct symtab_command))
					break;

				pSyms = (DLSyms*)dlAllocMem(sizeof(DLSyms));
				pSyms->symbolCount  = scmd->nsyms;
				pSyms->pStringTable = pBase + scmd->stroff;
				pSyms->pSymbolTable = (struct NLIST_TYPE*)(pBase + scmd->symoff);
				pSyms->symOffset    = symOffset;
				pSyms->pLib         = pLib;
			}
			else if(cmd->cmd == LC_DYSYMTAB && !dysymtab_cmd/* only init once - just safety check */)
			{
				/* @@@ unused, we'll always run over all symbols, and not check locals, globals, etc.
				if(cmd->cmdsize != sizeof(struct symtab_command)) {
					dlSymsCleanup(pSyms);
					break;
				}

				dysymtab_cmd = (const struct dysymtab_command*)cmd;*/
			}
		}
	}

	/* Got symbol table? */
	if(pSyms) {
		/* Alter symtable info if we got symbols organized in local/defined/undefined groups. */
		/* Only use local ones in that case. */
		/*@@@ don't restrict to only local symbols if(dysymtab_cmd) {
			pSyms->pSymbolTable += dysymtab_cmd->ilocalsym;
			pSyms->symbolCount = dysymtab_cmd->nlocalsym;
		}*/
		return pSyms;
	}

	/* Couldn't init syms, so free lib and return error. */
	dlFreeLibrary(pLib);
	return NULL;
}


void dlSymsCleanup(DLSyms* pSyms)
{
	if(pSyms) {
		dlFreeLibrary(pSyms->pLib);
		dlFreeMem(pSyms);
	}
}

int dlSymsCount(DLSyms* pSyms)
{
	return pSyms ? pSyms->symbolCount : 0;
}


const char* dlSymsName(DLSyms* pSyms, int index)
{
	const struct NLIST_TYPE* nl;
	unsigned char t;

	if(!pSyms)
		return NULL;

	nl = pSyms->pSymbolTable + index;
	t = nl->n_type & N_TYPE;

	/* Return name by lookup through it's address. This guarantees to be consistent with dlsym and dladdr */
	/* calls as used in dlFindAddress and dlSymsNameFromValue - the "#if 0"-ed code below returns the */
	/* name directly, but assumes wrongly that everything is prefixed with an underscore on Darwin. */

	/* only handle symbols that are in a section and aren't symbolic debug entries */
	if((t == N_SECT) && (nl->n_type & N_STAB) == 0)
		return dlSymsNameFromValue(pSyms, (void*)(nl->n_value + pSyms->symOffset));

	return NULL; /* @@@ handle N_INDR, etc.? */

#if 0
	/* Mach-O manual: Symbols with an index into the string table of zero */
	/* (n_un.n_strx == 0) are defined to have a null ("") name. */
	if(nl->n_un.n_strx == 0)
		return NULL; /*@@@ have return pointer to some static "" string? */

	/* Skip undefined symbols. @@@ should we? */
	if(t == N_UNDF || t == N_PBUD) /* @@@ check if N_PBUD is defined, it's not in the NeXT manual, but on Darwin 8.0.1 */
		return NULL;

	/*TODO skip more symbols based on nl->n_desc and nl->n_type ? */

	/* Return name - handles lookup of indirect names. */
	return &pSyms->pStringTable[(t == N_INDR ? nl->n_value : nl->n_un.n_strx)
#if defined(OS_Darwin)
		+ 1 /* Skip '_'-prefix */
#endif
	];
#endif
}


const char* dlSymsNameFromValue(DLSyms* pSyms, void* value)
{
	Dl_info info;
	if (!dladdr(value, &info) || (value != info.dli_saddr))
		return NULL;

	return info.dli_sname;
}

