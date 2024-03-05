/*
** smarkusg: part of the code comes from mplayer 1.5 AOS
** License: GPL v2 or later
*/

#include <exec/types.h>

#define AMIGA_VERSION "DGen/SDL 1.33 (5.03.2024)"

int AmigaOS_Open(int argc, char *argv[]); // returns -1 if a problem
void AmigaOS_Close(void);
void AmigaOS_ParseArg(int argc, char *argv[], int *new_argc, char ***new_argv);
void VARARGS68K EasyRequester(CONST_STRPTR text, CONST_STRPTR button, ...);
