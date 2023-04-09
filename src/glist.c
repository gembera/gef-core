/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"


//static GList	    *free_list = NULL;

GList*
g_list_alloc (void)
{
  GList *new_list = g_new (GList, 1);
  
  new_list->data = NULL;
  new_list->next = NULL;
  new_list->prev = NULL;
  
  return new_list;
}

void
g_list_free (GList *list)
{
  GList *next;
  
  for (; list != NULL; list = next)
    {
		next = list->next;
		g_free(list);
    }
}

void
g_list_free_1 (GList *list)
{
  if (list)
    {
		g_free(list);
    }
}

GList*
g_list_append (GList	*list,
	       gpointer	 data)
{
  GList *new_list;
  GList *last;
  
  new_list = g_list_alloc ();
  new_list->data = data;
  
  if (list)
    {
      last = g_list_last (list);
      /* g_assert (last != NULL); */
      last->next = new_list;
      new_list->prev = last;

      return list;
    }
  else
    return new_list;
}

GList*
g_list_prepend (GList	 *list,
		gpointer  data)
{
  GList *new_list;
  
  new_list = g_list_alloc ();
  new_list->data = data;
  
  if (list)
    {
      if (list->prev)
	{
	  list->prev->next = new_list;
	  new_list->prev = list->prev;
	}
      list->prev = new_list;
      new_list->next = list;
    }
  
  return new_list;
}

GList*
g_list_insert (GList	*list,
	       gpointer	 data,
	       gint	 position)
{
  GList *new_list;
  GList *tmp_list;
  
  if (position < 0)
    return g_list_append (list, data);
  else if (position == 0)
    return g_list_prepend (list, data);
  
  tmp_list = g_list_nth (list, position);
  if (!tmp_list)
    return g_list_append (list, data);
  
  new_list = g_list_alloc ();
  new_list->data = data;
  
  if (tmp_list->prev)
    {
      tmp_list->prev->next = new_list;
      new_list->prev = tmp_list->prev;
    }
  new_list->next = tmp_list;
  tmp_list->prev = new_list;
  
  if (tmp_list == list)
    return new_list;
  else
    return list;
}

GList *
g_list_concat (GList *list1, GList *list2)
{
  GList *tmp_list;
  
  if (list2)
    {
      tmp_list = g_list_last (list1);
      if (tmp_list)
	tmp_list->next = list2;
      else
	list1 = list2;
      list2->prev = tmp_list;
    }
  
  return list1;
}

GList*
g_list_remove (GList	*list,
	       gpointer	 data)
{
  GList *tmp;
  
  tmp = list;
  while (tmp)
    {
      if (tmp->data != data)
	tmp = tmp->next;
      else
	{
	  if (tmp->prev)
	    tmp->prev->next = tmp->next;
	  if (tmp->next)
	    tmp->next->prev = tmp->prev;
	  
	  if (list == tmp)
	    list = list->next;
	  
	  g_list_free_1 (tmp);
	  
	  break;
	}
    }
  return list;
}

GList*
g_list_remove_link (GList *list,
		    GList *link)
{
  if (link)
    {
      if (link->prev)
	link->prev->next = link->next;
      if (link->next)
	link->next->prev = link->prev;
      
      if (link == list)
	list = list->next;
      
      link->next = NULL;
      link->prev = NULL;
    }
  
  return list;
}

GList*
g_list_reverse (GList *list)
{
  GList *last;
  
  last = NULL;
  while (list)
    {
      last = list;
      list = last->next;
      last->next = last->prev;
      last->prev = list;
    }
  
  return last;
}

GList*
g_list_nth (GList *list,
	    guint  n)
{
  while ((n-- > 0) && list)
    list = list->next;
  
  return list;
}

gpointer
g_list_nth_data (GList     *list,
		 guint      n)
{
  while ((n-- > 0) && list)
    list = list->next;
  
  return list ? list->data : NULL;
}

GList*
g_list_find (GList    *list,
	     gpointer  data)
{
  while (list)
    {
      if (list->data == data)
	break;
      list = list->next;
    }
  
  return list;
}

GList*
g_list_find_custom (GList       *list,
		    gpointer     data,
		    GCompareFunc func)
{
  g_return_val_if_fail (func != NULL, list);

  while (list)
    {
      if (! func (list->data, data))
	return list;
      list = list->next;
    }

  return NULL;
}


gint
g_list_position (GList *list,
		 GList *link)
{
  gint i;

  i = 0;
  while (list)
    {
      if (list == link)
	return i;
      i++;
      list = list->next;
    }

  return -1;
}

gint
g_list_index (GList   *list,
	      gpointer data)
{
  gint i;

  i = 0;
  while (list)
    {
      if (list->data == data)
	return i;
      i++;
      list = list->next;
    }

  return -1;
}

GList*
g_list_last (GList *list)
{
  if (list)
    {
      while (list->next)
	list = list->next;
    }
  
  return list;
}

GList*
g_list_first (GList *list)
{
  if (list)
    {
      while (list->prev)
	list = list->prev;
    }
  
  return list;
}

guint
g_list_length (GList *list)
{
  guint length;
  
  length = 0;
  while (list)
    {
      length++;
      list = list->next;
    }
  
  return length;
}

void
g_list_foreach (GList	 *list,
		GFunc	  func,
		gpointer  user_data)
{
  while (list)
    {
      (*func) (list->data, user_data);
      list = list->next;
    }
}


GList*
g_list_insert_sorted (GList        *list,
                      gpointer      data,
                      GCompareFunc  func)
{
  GList *tmp_list = list;
  GList *new_list;
  gint cmp;

  g_return_val_if_fail (func != NULL, list);
  
  if (!list) 
    {
      new_list = g_list_alloc();
      new_list->data = data;
      return new_list;
    }
  
  cmp = (*func) (data, tmp_list->data);
  
  while ((tmp_list->next) && (cmp > 0))
    {
      tmp_list = tmp_list->next;
      cmp = (*func) (data, tmp_list->data);
    }

  new_list = g_list_alloc();
  new_list->data = data;

  if ((!tmp_list->next) && (cmp > 0))
    {
      tmp_list->next = new_list;
      new_list->prev = tmp_list;
      return list;
    }
   
  if (tmp_list->prev)
    {
      tmp_list->prev->next = new_list;
      new_list->prev = tmp_list->prev;
    }
  new_list->next = tmp_list;
  tmp_list->prev = new_list;
 
  if (tmp_list == list)
    return new_list;
  else
    return list;
}
