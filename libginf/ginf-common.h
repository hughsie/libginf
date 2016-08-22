/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2014 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#if !defined (__LIBGINF_H) && !defined (GINF_COMPILATION)
#error "Only <libginf.h> can be included directly."
#endif

#ifndef __GINF_H
#define __GINF_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * GInfKeyfileError:
 * @GINF_ERROR_FAILED:				Generic failure
 * @GINF_ERROR_INVALID_TYPE:			Invalid type
 * @GINF_ERROR_NOT_FOUND:			Data not found
 *
 * The error type.
 **/
typedef enum {
	GINF_ERROR_FAILED,
	GINF_ERROR_INVALID_TYPE,
	GINF_ERROR_NOT_FOUND,
	/*< private >*/
	GINF_ERROR_LAST
} GInfKeyfileError;

#define	GINF_ERROR				ginf_keyfile_error_quark ()

/**
 * GInfKeyfileLoadFlags:
 * @GINF_KEYFILE_LOAD_FLAG_NONE:		No flags set
 * @GINF_KEYFILE_LOAD_FLAG_STRICT:		Be strict when loading the file
 * @GINF_KEYFILE_LOAD_FLAG_CASE_INSENSITIVE:	Load keys and groups case insensitive
 *
 * The flags used when loading INF files.
 **/
typedef enum {
	GINF_KEYFILE_LOAD_FLAG_NONE			= 0,
	GINF_KEYFILE_LOAD_FLAG_STRICT			= 1 << 0,
	GINF_KEYFILE_LOAD_FLAG_CASE_INSENSITIVE		= 1 << 1,
	/*< private >*/
	GINF_KEYFILE_LOAD_FLAG_LAST
} GInfKeyfileLoadFlags;

GQuark		 ginf_keyfile_error_quark	(void);
gboolean	 ginf_keyfile_load_data		(GKeyFile	*keyfile,
						 const gchar	*data,
						 GInfKeyfileLoadFlags	 flags,
						 GError		**error);
gboolean	 ginf_keyfile_load_file		(GKeyFile	*keyfile,
						 const gchar	*filename,
						 GInfKeyfileLoadFlags	 flags,
						 GError		**error);
gchar		*ginf_keyfile_get_driver_version (GKeyFile	*keyfile,
						 guint64	*timestamp,
						 GError		**error);

G_END_DECLS

#endif /* __GINF_H */
