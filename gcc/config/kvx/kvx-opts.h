/*

   Copyright (C) 2009-2018 Kalray SA.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef KVX_OPTS_H
#define KVX_OPTS_H

enum kvx_arch_type
{
  KVX_ARCH_KV3_1,
  KVX_ARCH_KV3_2
};

#define KV3_1 (kvx_arch_name == KVX_ARCH_KV3_1)
#define KV3_2 (kvx_arch_name == KVX_ARCH_KV3_2)

#endif
