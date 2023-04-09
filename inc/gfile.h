/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_FILE_H_
#define _G_FILE_H_
#include "glib.h"

gstring g_file_get_normalize_path(gstring path);
gboolean g_file_exists(gstring path);
gboolean g_dir_exists(gstring path);
gint g_file_get_size(gstring path);
gint g_file_make_full_directory(gstring path);
gint g_file_make_full_directory_for_file(gstring path);
gint g_file_delete(gstring path);
gint g_file_rename(gstring oldpath, gstring newpath);

/**
 * @description: 遍历指定目录的文件 (不支持递归子目录)
 * @param {root} 根目录
 * @param {func} 回调函数
 * @param {user_data} 用户上下文
 * @return: 
 */
gint g_dir_travel(gstring root, GFunc func, gpointer user_data);
/**
 * @description: 返回指定目录的文件数 (不支持递归子目录)
 * @param {root} 根目录
 * @return: -1 失败, 0 或 大于 0 表示匹配目录文件的总数
 */
gint g_dir_files(gstring root);

/**
 * @description: 返回指定目录的文件总大小 (不支持递归子目录)
 * @param {root} 根目录
 * @return: -1 失败, 0 或 大于 0 表示匹配目录文件占用空间大小(单位bytes)
 */
gint g_dir_file_size(gstring root);

/**
 * @description: 根据 pattern 遍历指定目录的文件
 * @param {pattern} 匹配服,例如 /dev/*, /tmp/carota/dm/*_XXXXXX.tmp.info
 * @param {func} 回调函数
 * @param {user_data} 用户上下文
 * @return: -1 失败, 0 或 大于 0 表示找到匹配文件的总数
 */
gint g_file_glob(gstring pattern, GFunc func, gpointer user_data);

/**
 * @description: 根据 pattern 遍历指定目录的文件, 并返回第一个匹配文件
 * @param {pattern} 匹配服,例如 /dev/*, /tmp/carota/dm/*_XXXXXX.tmp.info
 * @param {out_buffer} 用于接收找到的第一个文件的buffer
 * @param {out_buffer_len} out_buffer 的长度
 * @return: -1 失败, 0 或 大于 0 表示找到匹配文件的总数
 */
gint g_file_glob_first(gstring pattern, char* out_buffer, gint out_buffer_len);

/**
 * @description: 返回指定目录(所在分区)的剩余空间
 * @param {root} 根目录
 * @return: -1 失败, 0 或 大于 0 表示指定目录(所在分区)的剩余空间(单位bytes)
 */
gint g_dir_free_space(gstring root);

#endif