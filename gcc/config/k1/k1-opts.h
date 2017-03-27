/*

   Copyright (C) 2009-2014 Kalray SA.

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

#ifndef K1_OPTS_H
#define K1_OPTS_H

#define K1_BOARD_DEFAULT "csp_generic"
#define K1_BOARD_VERIF "verif"

enum k1_processing_type
{
  K1_PROCESSING_SINGLE,
  K1_PROCESSING_MULTI
};

enum k1_cluster_type
{
  K1_CLUSTER_IODDR,
  K1_CLUSTER_IODDR_DDR,
  K1_CLUSTER_IOETH,
  K1_CLUSTER_NODE,
  K1_CLUSTER_NODE_MSD
};

enum k1_arch_type
{
  K1_ARCH_C
};

/* enum k1_soc_type */
/* { */
/*   K1_SOC_BOSTAN, */
/*   K1_SOC_BOSTANS */
/* }; */

enum k1_os_type
{
  K1_OS_BARE,
  K1_OS_NODEOS,
  K1_OS_RTEMS,
  K1_OS_LINUX
};

#endif
