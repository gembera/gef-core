/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gfile.h"
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <glob.h>

#define DEFAULT_DIR_MODE 0777

#define WINDOWS_PATH_SEPARATOR	 "\\"
#define LINUX_PATH_SEPARATOR	 "/"

gstring g_file_get_normalize_path(gstring path)
{
	gint pos;
	pos = g_strindexof(path, "://", 0);
	if (pos > 0)
		path = path + pos + 3;
	path = g_strreplace(path, WINDOWS_PATH_SEPARATOR, LINUX_PATH_SEPARATOR);
	if (strlen(path) > 1 && path[1] == ':' && path[0] >= 'A' && path[0] <= 'Z')
	{
		path[0] = path[0] + 'a' - 'A';
	}
	return path;
}

gint g_file_delete(gstring path)
{
	g_return_val_if_fail(path != NULL, -1);
	gint ret = 0;
	if (g_file_exists(path)) {
		path = g_file_get_normalize_path(path);
		ret = unlink(path);
		if(ret != 0) {
			g_log_info("g_file_delete: %s %d %s\n", path, ret, strerror(errno));
		}
		g_free(path);
	}
	return ret;
}
gint g_file_rename(gstring oldpath, gstring newpath)
{
	gint r;
	gstring noldpath, nnewpath;
	g_return_val_if_fail(oldpath != NULL && newpath != NULL, -1);
	g_file_delete(newpath);
	noldpath = g_file_get_normalize_path(oldpath);
	nnewpath = g_file_get_normalize_path(newpath);
	r = rename(oldpath, newpath);
	if(r != 0) {
		g_log_info("g_file_rename: %s %s %d %s\n", oldpath, nnewpath, r, strerror(errno));
	}
	g_free(noldpath);
	g_free(nnewpath);
	return r;
}
gbool g_dir_exists(gstring path)
{
	return access(path, F_OK) == 0;
}
gbool g_file_exists(gstring path)
{
	return access(path, F_OK) == 0;
}
gint g_file_get_size(gstring path)
{
	struct stat st;
	stat(path, &st);
	return (gint)st.st_size;
}
static gint _g_file_make_full_directory(gstring path, gbool root) {
	gint r = 0;
	gint pos = 0;
	gstring tmp = g_file_get_normalize_path(path);
	pos = g_strindexof(tmp, LINUX_PATH_SEPARATOR, pos);
	// printf("tmp: %s\n", tmp);
	while((pos = g_strindexof(tmp, LINUX_PATH_SEPARATOR, pos + 1)) > 0)
	{
		char c = tmp[pos];
		tmp[pos] = '\0';
		// printf("tmp: %s\n", tmp);
		if (!g_dir_exists(tmp))
		{
			r = mkdir(tmp, DEFAULT_DIR_MODE);
			g_log_debug("mkdir : %d, %s\n", r, tmp);
			if(r != 0) break;
		}
		tmp[pos] = c;
	}
	if(r == 0 && root) {
		if (!g_dir_exists(tmp)) {
			r = mkdir(tmp, DEFAULT_DIR_MODE);
			g_log_debug("mkdir : %d, %s\n", r, tmp);
		}
	}
	g_free(tmp);
	return r;
}
gint g_file_make_full_directory_for_file(gstring path)
{
	gint r = _g_file_make_full_directory(path, FALSE);
	return r;
}

gint g_file_make_full_directory(gstring path)
{
	gint r = _g_file_make_full_directory(path, TRUE);
	return r;
}


#define MAXLEN 1024
int g_dir_travel(gstring root, GCallback func, gpointer user_data) {
	struct dirent* ent = NULL;
	DIR* dir = NULL;
	dir = opendir(root);
	
	if(dir == NULL) {
		/* could not open directory */
		g_log_error("could not open directory: %s", root);
		return -1;
	}
	else {
		char path[MAXLEN];
		struct stat st;
		while((ent = readdir(dir)) != NULL) {
			char* fn = ent->d_name;
			char out[MAXLEN] = {0};
			snprintf(path, sizeof(path),"%s/%s", root, fn);
			lstat(path, &st);
			if(S_ISDIR(st.st_mode)) {
				if(strcmp(fn, ".") == 0 || strcmp(fn, "..") == 0) continue;
			}
			else if(S_ISREG(st.st_mode)) {
				if(func) {
					func(path, user_data);
				}
			}
			else if(S_ISLNK(st.st_mode)) {
				
			}
		}
		closedir(dir);
	}
	
	return 0;
}


static void _g_dir_files_count(gpointer  data, gpointer  user_data) {
	int* c = (int*)user_data;
	*c = *c + 1;
}

int g_dir_files(gstring root) {
	int ret = 0;
	int c = 0;
	ret = g_dir_travel(root, _g_dir_files_count, &c);
	if(ret == 0) {
		ret = c;
	}
	return ret;
}

static void _g_dir_file_size_count(gpointer  data, gpointer  user_data) {
	int* c = (int*)user_data;
	int size = g_file_get_size((gstring)data);
	*c = *c + size;
}

gint g_dir_file_size(gstring root) {
	int ret = 0;
	int c = 0;
	ret = g_dir_travel(root, _g_dir_file_size_count, &c);
	if(ret == 0) {
		ret = c;
	}
	return ret;
}

gint g_file_glob(gstring pattern, GCallback func, gpointer user_data) {
	g_return_val_if_fail(pattern != NULL, -1);
	g_return_val_if_fail(func != NULL, -1);

	glob_t buf;
    int i, found = 0;
    glob(pattern, GLOB_NOSORT, NULL, &buf);

    for (i = 0; i < buf.gl_pathc; i++)
    {
		func(buf.gl_pathv[i], user_data);
        found++;
    }

    globfree(&buf);

    return found;
}

gint g_file_glob_first(gstring pattern, char* out_buffer, gint out_buffer_len) {
	g_return_val_if_fail(pattern != NULL, -1);
	g_return_val_if_fail(out_buffer != NULL, -1);
	g_return_val_if_fail(out_buffer_len > 0, -1);

	glob_t buf;
    int i, found = 0;
    glob(pattern, GLOB_NOSORT, NULL, &buf);

    for (i = 0; i < buf.gl_pathc; i++)
    {
		strncpy(out_buffer, buf.gl_pathv[i], out_buffer_len);
		found++;
		break;
    }

    globfree(&buf);

    return found;
}


gint g_dir_free_space(gstring root) {
	g_return_val_if_fail(root != NULL, -1);

	int ret = 0;
	struct statvfs vfs;

    ret = statvfs(root, &vfs);
	if(ret == 0) {
		ret = vfs.f_bsize * vfs.f_bfree;
	}
	else {
		g_log_error("statvfs: %s %d %s\n", root, ret, strerror(errno));
	}
	return ret;
}
