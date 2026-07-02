/* Copyright (C) 2002-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GFC_TRANS_DESCRIPTOR_H
#define GFC_TRANS_DESCRIPTOR_H


tree gfc_conv_descriptor_dtype (tree);
tree gfc_conv_descriptor_rank (tree);
tree gfc_conv_descriptor_version (tree);
tree gfc_conv_descriptor_elem_len (tree);
tree gfc_conv_descriptor_attribute (tree);
tree gfc_conv_descriptor_type (tree);
tree gfc_get_descriptor_dimension (tree);
tree gfc_conv_descriptor_dimension (tree, tree);
tree gfc_conv_descriptor_token (tree);
tree gfc_conv_descriptor_offset (tree);

tree gfc_conv_descriptor_data_get (tree);
tree gfc_conv_descriptor_offset_get (tree);
tree gfc_conv_descriptor_span_get (tree);

tree gfc_conv_descriptor_stride_get (tree, tree);
tree gfc_conv_descriptor_lbound_get (tree, tree);
tree gfc_conv_descriptor_ubound_get (tree, tree);

void gfc_conv_descriptor_data_set (stmtblock_t *, tree, tree);
void gfc_conv_descriptor_offset_set (stmtblock_t *, tree, tree);
void gfc_conv_descriptor_span_set (stmtblock_t *, tree, tree);
void gfc_conv_descriptor_stride_set (stmtblock_t *, tree, tree, tree);
void gfc_conv_descriptor_lbound_set (stmtblock_t *, tree, tree, tree);
void gfc_conv_descriptor_ubound_set (stmtblock_t *, tree, tree, tree);

/* Build expressions for accessing components of an array descriptor.  */
void gfc_get_descriptor_offsets_for_info (const_tree, tree *, tree *, tree *,
					  tree *, tree *, tree *, tree *,
					  tree *);

/* Build a null array descriptor constructor.  */
tree gfc_build_null_descriptor (tree type);

tree gfc_conv_descriptor_size (tree, int);
tree gfc_conv_descriptor_cosize (tree, int, int);

/* Shift lower bound of descriptor, updating ubound and offset.  */
void gfc_conv_shift_descriptor_lbound (stmtblock_t*, tree, int, tree);

#endif /* GFC_TRANS_DESCRIPTOR_H */
