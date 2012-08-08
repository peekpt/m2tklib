/*

  mn.h
  
  menu node 
  
*/

#ifndef _MN_H
#define _MN_H

typedef struct _mn_struct *mn_type;
typedef int (*mn_fn)(mn_type mn, int msg, void *arg);

#define MN_MSG_NONE 0
#define MN_MSG_OPEN 1
#define MN_MSG_CLOSE 2
#define MN_MSG_COPY 3
#define MN_MSG_GET_DISPLAY_STRING 4


struct _mn_struct
{
  mn_type n;
  mn_type d;
  mn_fn fn;
  void *data;
};

mn_type mn_Open(void);					/* mn_init.c */
void mn_Close(mn_type mn);					/* mn_init.c */
mn_type mn_OpenCopy(mn_type source);		/* mn_init.c */


mn_type mn_GetSubtree(mn_type n, int pos);		/* mn_get.c */


void mn_Del(mn_type n);					/* mn_del.c */
mn_type mn_CutSubtree(mn_type n, int pos);		/* mn_del.c */
void mn_DelSubtree(mn_type n, int pos);		/* mn_del.c */


mn_type mn_Copy(mn_type n);				/* mn_copy.c */
mn_type mn_CopySubtree(mn_type n, int pos);	/* mn_copy.c */


void mn_AppendSubtree(mn_type n, mn_type s);			/* mn_add.c */
void mn_PasteSubtree(mn_type n, int pos, mn_type s);		/* mn_add.c */

void mn_Show(mn_type n);							/* mn_show.c */

int mn_fn_empty(mn_type mn, int msg, void *arg);		/* mn_fn.c */

#endif