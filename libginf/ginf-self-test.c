/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2014-2016 Richard Hughes <richard@hughsie.com>
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

#include "config.h"

#include <glib.h>
#include <stdlib.h>

#include "ginf-common.h"

static gchar *
as_test_get_filename (const gchar *filename)
{
	g_autofree gchar *path = NULL;

	/* try the source then the destdir */
	path = g_build_filename (TESTDIRSRC, filename, NULL);
	if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
		g_free (path);
		path = g_build_filename (TESTDIRBUILD, filename, NULL);
	}
	return realpath (path, NULL);
}

static void
as_test_inf_func (void)
{
	GError *error = NULL;
	GKeyFile *kf;
	const gchar *data;
	gboolean ret;
	gchar *tmp;
	guint64 ts;
	g_autoptr(GDir) dir = NULL;
	g_autofree gchar *filename = NULL;
	g_autofree gchar *infs = NULL;

	/* case insensitive */
	kf = g_key_file_new ();
	ret = ginf_keyfile_load_data (kf, "[Version]\nClass=Firmware",
				GINF_KEYFILE_LOAD_FLAG_CASE_INSENSITIVE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "version", "class", NULL);
	g_assert_cmpstr (tmp, ==, "Firmware");
	g_free (tmp);

	/* section merging */
	ret = ginf_keyfile_load_data (kf, "[Version]\nPnpLockdown=1", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "version", "class", NULL);
	g_assert_cmpstr (tmp, ==, "Firmware");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Version", "PnpLockdown", NULL);
	g_assert_cmpstr (tmp, ==, "1");
	g_free (tmp);

	/* dequoting */
	ret = ginf_keyfile_load_data (kf, "[Version]\n"
				    "PnpLockdown  =  \" 2 \"\n"
				    "Empty = \"\"", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "PnpLockdown", NULL);
	g_assert_cmpstr (tmp, ==, " 2 ");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Version", "Empty", NULL);
	g_assert_cmpstr (tmp, ==, "");
	g_free (tmp);

	/* autovalues */
	ret = ginf_keyfile_load_data (kf, "[RandomList]\nfilename.cap", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "RandomList", "value000", NULL);
	g_assert_cmpstr (tmp, ==, "filename.cap");
	g_free (tmp);

	/* autovalues with quotes */
	ret = ginf_keyfile_load_data (kf, "[RandomList2]\n\"SomeRandomValue=1\"", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "RandomList2", "value000", NULL);
	g_assert_cmpstr (tmp, ==, "SomeRandomValue=1");
	g_free (tmp);

	/* Dirids */
	ret = ginf_keyfile_load_data (kf, "[DriverServiceInst]\nServiceBinary=%12%\\service.exe", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "DriverServiceInst", "ServiceBinary", NULL);
	g_assert_cmpstr (tmp, ==, "/tmp/service.exe");
	g_free (tmp);

	/* comments */
	data =	"; group priority\n"
		"[Metadata] ; similar to [Version]\n"
		"Vendor=Hughski ; vendor name\n"
		"Owner=\"Richard; Ania\"\n"
		"; device name priority\n"
		"Device=\"ColorHug\" ; device name";
	ret = ginf_keyfile_load_data (kf, data, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Metadata", "Vendor", NULL);
	g_assert_cmpstr (tmp, ==, "Hughski");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Metadata", "Device", NULL);
	g_assert_cmpstr (tmp, ==, "ColorHug");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Metadata", "Owner", NULL);
	g_assert_cmpstr (tmp, ==, "Richard; Ania");
	g_free (tmp);

	/* comment values */
	tmp = g_key_file_get_comment (kf, "Metadata", "Vendor", NULL);
	g_assert_cmpstr (tmp, ==, "vendor name\n");
	g_free (tmp);
	tmp = g_key_file_get_comment (kf, "Metadata", "Device", NULL);
	g_assert_cmpstr (tmp, ==, "device name priority\n");
	g_free (tmp);
	tmp = g_key_file_get_comment (kf, "Metadata", "Owner", NULL);
	g_assert_cmpstr (tmp, ==, NULL);
	g_free (tmp);
//	tmp = g_key_file_get_comment (kf, "metadata", NULL, NULL);
//	g_assert_cmpstr (tmp, ==, "group priority");
//	g_free (tmp);

	/* strings substitution */
	data =	"[Version]\n"
		"Provider=Vendor was %Provider% now %Unknown% Ltd\n"
		"[Strings]\n"
		"Provider=Hughski\n";
	ret = ginf_keyfile_load_data (kf, data, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "Vendor was Hughski now %Unknown% Ltd");
	g_free (tmp);

	/* continued lines */
	data =	"[Version]\n"
		"Provider=Richard & \\\n"
		"Ania & \\\n"
		"Friends";
	ret = ginf_keyfile_load_data (kf, data, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "Richard & Ania & Friends");
	g_free (tmp);

	/* multiline value */
	data =	"[Metadata]\n"
		"UpdateDescription = \"This is a very \"long\" update \"  |\n"
		"                    \"description designed to be shown \" |\n"
		"                    \"to the end user.\"";
	ret = ginf_keyfile_load_data (kf, data, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Metadata", "UpdateDescription", NULL);
	g_assert_cmpstr (tmp, ==, "This is a very \"long\" update description "
				  "designed to be shown to the end user.");
	g_free (tmp);

	/* substitution as the key name */
	data =	"[Version]\n"
		"%Manufacturer.Foo% = \"Devices\"\n"
		"[Strings]\n"
		"Manufacturer.Foo = \"Hughski\"\n";
	ret = ginf_keyfile_load_data (kf, data, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Hughski", NULL);
	g_assert_cmpstr (tmp, ==, "Devices");
	g_free (tmp);

	/* key names with double quotes */
	ret = ginf_keyfile_load_data (kf, "[\"Firmware\"]\nFoo=Bar\n", 0, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Firmware", "Foo", NULL);
	g_assert_cmpstr (tmp, ==, "Bar");
	g_free (tmp);

	/* unbalanced quotes */
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = \"Hughski\n",
			        GINF_KEYFILE_LOAD_FLAG_STRICT, &error);
	g_assert_error (error, GINF_ERROR, GINF_ERROR_FAILED);
	g_assert (!ret);
	g_clear_error (&error);
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = \"Hughski\n",
			        GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "Hughski");
	g_free (tmp);

	/* missing string replacement */
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = \"%MISSING%\"\n",
			        GINF_KEYFILE_LOAD_FLAG_STRICT, &error);
	g_assert_error (error, GINF_ERROR, GINF_ERROR_FAILED);
	g_assert (!ret);
	g_clear_error (&error);
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = \"%MISSING%\"\n",
			        GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "%MISSING%");
	g_free (tmp);

	/* invalid UTF-8 */
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = Hughski \x99\n",
			        GINF_KEYFILE_LOAD_FLAG_STRICT, &error);
	g_assert_error (error, GINF_ERROR, GINF_ERROR_FAILED);
	g_assert (!ret);
	g_clear_error (&error);
	ret = ginf_keyfile_load_data (kf, "[Version]\nProvider = Hughski \x99\n",
			        GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "Hughski ?");
	g_free (tmp);

	g_key_file_unref (kf);

	/* parse file */
	kf = g_key_file_new ();
	filename = as_test_get_filename ("example.inf");
	ret = ginf_keyfile_load_file (kf, filename, 0, &error);
	g_assert_no_error (error);
	g_assert (ret);

	/* simple */
	tmp = g_key_file_get_string (kf, "Version", "Class", NULL);
	g_assert_cmpstr (tmp, ==, "Firmware");
	g_free (tmp);

	/* HK */
	tmp = g_key_file_get_string (kf, "Firmware_AddReg", "HKR_FirmwareId", NULL);
	g_assert_cmpstr (tmp, ==, "{84f40464-9272-4ef7-9399-cd95f12da695}");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Firmware_AddReg", "HKR_FirmwareVersion_0x00010001", NULL);
	g_assert_cmpstr (tmp, ==, "0x00020002");
	g_free (tmp);
	tmp = g_key_file_get_string (kf, "Firmware_AddReg", "HKR_FirmwareFilename", NULL);
	g_assert_cmpstr (tmp, ==, "firmware.bin");
	g_free (tmp);

	/* key replacement */
	tmp = g_key_file_get_string (kf, "Firmware_CopyFiles", "value000", NULL);
	g_assert_cmpstr (tmp, ==, "firmware.bin");
	g_free (tmp);

	/* double quotes swallowing */
	tmp = g_key_file_get_string (kf, "Strings", "FirmwareDesc", NULL);
	g_assert_cmpstr (tmp, ==, "ColorHug Firmware");
	g_free (tmp);

	/* string replacement */
	tmp = g_key_file_get_string (kf, "Version", "Provider", NULL);
	g_assert_cmpstr (tmp, ==, "Hughski");
	g_free (tmp);

	/* valid DriverVer */
	ret = ginf_keyfile_load_data (kf, "[Version]\n"
				    "DriverVer = \"03/01/2015,2.0.0\"\n",
				GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = ginf_keyfile_get_driver_version (kf, &ts, &error);
	g_assert_no_error (error);
	g_assert_cmpstr (tmp, ==, "2.0.0");
	g_assert_cmpint ((gint32) ts, ==, 1425168000);
	g_free (tmp);

	/* invalid DriverVer date */
	ret = ginf_keyfile_load_data (kf, "[Version]\n"
				    "DriverVer = \"13/01/2015,2.0.0\"\n",
				GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = ginf_keyfile_get_driver_version (kf, &ts, &error);
	g_assert_error (error, GINF_ERROR, GINF_ERROR_INVALID_TYPE);
	g_assert_cmpstr (tmp, ==, NULL);
	g_clear_error (&error);

	/* no DriverVer date */
	ret = ginf_keyfile_load_data (kf, "[Version]\n"
				    "DriverVer = \"2.0.0\"\n",
				GINF_KEYFILE_LOAD_FLAG_NONE, &error);
	g_assert_no_error (error);
	g_assert (ret);
	tmp = ginf_keyfile_get_driver_version (kf, &ts, &error);
	g_assert_error (error, GINF_ERROR, GINF_ERROR_INVALID_TYPE);
	g_assert_cmpstr (tmp, ==, NULL);
	g_clear_error (&error);

	g_key_file_unref (kf);

	/* test every .inf file from my Windows XP installation disk */
	infs = as_test_get_filename ("infs");
	if (infs != NULL) {
		dir = g_dir_open (infs, 0, NULL);
		while ((data = g_dir_read_name (dir)) != NULL) {
			g_autofree gchar *path = NULL;
			path = g_build_filename (infs, data, NULL);
			kf = g_key_file_new ();
			ret = ginf_keyfile_load_file (kf, path,
						GINF_KEYFILE_LOAD_FLAG_NONE,
						&error);
			if (g_error_matches (error,
					     GINF_ERROR,
					     GINF_ERROR_INVALID_TYPE)) {
				g_clear_error (&error);
			} else {
				if (error != NULL)
					g_print ("Failed to process: %s\n",
						 path);
				g_assert_no_error (error);
				g_assert (ret);
			}
			g_key_file_unref (kf);
		}
	}
}

int
main (int argc, char **argv)
{
	g_test_init (&argc, &argv, NULL);

	/* only critical and error are fatal */
	g_log_set_fatal_mask (NULL, G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL);

	/* tests go here */
	g_test_add_func ("/libginf/inf", as_test_inf_func);


	return g_test_run ();
}
