/* KVX-specific backend routines.
   Copyright (C) 2009-2016 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

   Copyright (C) 2019 Kalray

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#include "bfd.h"
#include "elf-bfd.h"
#include "stdint.h"


extern bfd_reloc_status_type
_bfd_kvx_elf_put_addend (bfd *, bfd_byte *, bfd_reloc_code_real_type,
			     reloc_howto_type *, bfd_signed_vma);

extern bfd_boolean
_bfd_kvx_elf_add_symbol_hook (bfd *, struct bfd_link_info *,
				  Elf_Internal_Sym *, const char **,
				  flagword *, asection **, bfd_vma *);

bfd_vma
_bfd_kvx_elf_resolve_relocation (bfd_reloc_code_real_type r_type,
				bfd_vma place, bfd_vma value,
				bfd_vma addend, bfd_boolean weak_undef_p);

bfd_boolean
kvx_elf32_init_stub_bfd (struct bfd_link_info *info,
			bfd *stub_bfd);
bfd_boolean
kvx_elf64_init_stub_bfd (struct bfd_link_info *info,
			bfd *stub_bfd);


#define elf_backend_add_symbol_hook	_bfd_kvx_elf_add_symbol_hook
