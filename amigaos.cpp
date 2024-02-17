/*
** smarkusg: part of the code comes from mplayer 1.5 AOS
** License: GPL v2 or later
*/

#include <proto/dos.h>
#include <proto/icon.h>
#include <workbench/startup.h>
#include <stdio.h>
#include <string.h>
#include "amigaos.h"
#include <stdlib.h>

#include <exec/types.h>
#include <proto/timer.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/asl.h>

#include <proto/icon.h>
#include <proto/dos.h>
#include <proto/wb.h>
#include <proto/application.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/datatypes.h>
#include <libraries/application.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <workbench/startup.h>
#include <proto/locale.h>
#include <classes/infowindow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdarg>


static const char *__attribute__((used)) stackcookie = "$STACK: 500000";
static const char *__attribute__((used)) version_tag = "$VER: " AMIGA_VERSION ;



struct Library		*IconBase = NULL;
struct IconIFace		*IIcon  = NULL;

struct Library * IntuitionBase = NULL;
struct IntuitionIFace *IIntuition = NULL;

struct Library		*AslBase = NULL;
struct AslIFace		*IAsl   = NULL;


static struct Task *current_task;
APTR window_mx;
APTR pr_WindowPtr;
char fullpath_dgen[1024] = "";

static int AmigaOS_argc = 0;
static char **AmigaOS_argv = NULL;

char* SDL_FULL = NULL;
char* FULLSCREEN_START = NULL;

static void Free_Arg(void)
{
	if (AmigaOS_argv)
	{
		ULONG i;
		for (i=0; i < AmigaOS_argc; i++)
			if(AmigaOS_argv[i]) free(AmigaOS_argv[i]);

		free(AmigaOS_argv);
	}
}

BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;

	*base = OpenLibrary( name , ver);
	if (*base)
	{
		*interface = GetInterface( *base, iname, iver, TAG_END );
		if (!*interface) DebugPrintF("Unable to get %s IFace for '%s' %ld!\n",iname,name,ver);
	}
	else
	{
		DebugPrintF("Unable to open the '%s' %ld!\n",name,ver);
	}

	return (*interface) ? TRUE : FALSE;
}

#define GET_PATH(drawer,file,dest)                                                                           \
	dest = (char *) malloc( ( strlen(drawer) + strlen(file) + 2 ) * sizeof(char) );                      \
	if (dest)                                                                                            \
	{                                                                                                    \
		if ( strlen(drawer) == 0) strcpy(dest, file);                                                \
		else                                                                                         \
		{                                                                                            \
			if ( (drawer[ strlen(drawer) - 1  ] == ':')  ) sprintf(dest, "%s%s", drawer, file);  \
			else sprintf(dest, "%s/%s", drawer, file);                                           \
		}                                                                                            \
	}

void AmigaOS_ParseArg(int argc, char *argv[], int *new_argc, char ***new_argv)
{
	struct WBStartup *WBStartup = NULL;
	struct DiskObject *icon = NULL;

	*new_argc = argc;
	*new_argv = argv;

	WBStartup = (struct WBStartup *) argv;
	if (!WBStartup)
	{
		return ; // We never know !
	}

	if (WBStartup->sm_NumArgs > 1)
	{
		// The first arg is always the tool name (aka us)
		// Then if more than one arg, with have some file name
		ULONG i;

		// We will replace the original argc/argv by us
		AmigaOS_argc = WBStartup->sm_NumArgs;
		AmigaOS_argv = (char **)   malloc(AmigaOS_argc * sizeof(char *) );
		if (!AmigaOS_argv) goto fail;

		memset(AmigaOS_argv, 0x00, AmigaOS_argc * sizeof(char *) );

		for(i=0; i<AmigaOS_argc; i++)
		{
			AmigaOS_argv[i] = (char *) malloc(MAX_DOS_NAME + MAX_DOS_PATH + 1);
			if (!AmigaOS_argv[i])
			{
				goto fail;
			}
			NameFromLock(WBStartup->sm_ArgList[i].wa_Lock, AmigaOS_argv[i], MAX_DOS_PATH);
			AddPart(AmigaOS_argv[i], WBStartup->sm_ArgList[i].wa_Name, MAX_DOS_PATH+MAX_DOS_NAME);

	      *new_argc = AmigaOS_argc;
	      *new_argv = AmigaOS_argv;
	      }
         }
	 else
	 {

		ULONG i;
		struct FileRequester *AmigaOS_FileRequester = NULL;
		BPTR FavoritePath_File;
		char FavoritePath_Value[1024];
		BOOL FavoritePath_Ok = FALSE;
	        char *EXTPATTERN = strdup("#?.(bin|md)");

		FavoritePath_File = Open("PROGDIR:ROMS", MODE_OLDFILE);
		if (FavoritePath_File)
		{
			LONG size = Read(FavoritePath_File, FavoritePath_Value, sizeof(FavoritePath_Value) );
			if (size > 0)
			{
				if ( strchr(FavoritePath_Value, '\n') ) // There is an \n -> valid file
				{
					FavoritePath_Ok = TRUE;
					*(strchr(FavoritePath_Value, '\n')) = '\0';
				}
			}
			Close(FavoritePath_File);
		}

		AmigaOS_FileRequester = (struct FileRequester *)AllocAslRequest(ASL_FileRequest, NULL);
		if (!AmigaOS_FileRequester)
		{
			goto fail;
		}

		if ( ( AslRequestTags( AmigaOS_FileRequester,
					ASLFR_TitleText,      "Please select the rom to run",
					ASLFR_DoMultiSelect,  TRUE,
					ASLFR_RejectIcons,    TRUE,
					ASLFR_DoPatterns,     TRUE,
					ASLFR_InitialPattern, (ULONG)EXTPATTERN,
					ASLFR_InitialDrawer,  (FavoritePath_Ok) ? FavoritePath_Value : "",
				TAG_DONE) ) == FALSE )
		{
			FreeAslRequest(AmigaOS_FileRequester);
			AmigaOS_FileRequester = NULL;
			/* nic nie wybrano */
			*new_argc = 0;
			*new_argv = NULL;
    			return;
		}

		AmigaOS_argc = AmigaOS_FileRequester->fr_NumArgs + 1;
		AmigaOS_argv = (char **)   malloc(AmigaOS_argc * sizeof(char *) );
		if (!AmigaOS_argv) goto fail;


		AmigaOS_argv[0] = strdup(FilePart(fullpath_dgen));
		if (!AmigaOS_argv[0]) goto fail;

		for(i=1; i<AmigaOS_argc; i++)
		{
			GET_PATH(AmigaOS_FileRequester->fr_Drawer,
						AmigaOS_FileRequester->fr_ArgList[i-1].wa_Name,
						AmigaOS_argv[i]);
			if (!AmigaOS_argv[i])
			{
				FreeAslRequest(AmigaOS_FileRequester);
				AmigaOS_FileRequester = NULL;
				goto fail;
			}
		}

		*new_argc = AmigaOS_argc;
		*new_argv = AmigaOS_argv;

		FreeAslRequest(AmigaOS_FileRequester);
	}
	return;
	

fail:

	*new_argc = argc;
	*new_argv = argv;
}


int AmigaOS_Open(int argc, char *argv[])
{
	AmigaOS_argv = NULL;

	if ( ! open_lib( "dos.library",        0L, "main", 1, &DOSBase, (struct Interface **) &IDOS ) ) return -1;
	if ( ! open_lib( "utility.library",   51L, "main", 1, &UtilityBase, (struct Interface **) &IUtility ) ) return -1;
	if ( ! open_lib( "intuition.library", 51L, "main", 1, &IntuitionBase, (struct Interface **) &IIntuition ) ) return -1;
	if ( ! open_lib( "asl.library",           0L, "main", 1, &AslBase, (struct Interface **) &IAsl ) ) return -1;
	if ( ! open_lib( "icon.library",         51L, "main", 1, &IconBase, (struct Interface **) &IIcon ) ) return -1;

	Forbid();
	current_task = FindTask(NULL);
	Permit();

return 0;
}

void AmigaOS_Close(void)
{
	Forbid();
	current_task = FindTask(NULL);
	Permit();
	((struct Process *) current_task) -> pr_WindowPtr = pr_WindowPtr;	// enable insert disk.

	Free_Arg();

	if (IntuitionBase) CloseLibrary(IntuitionBase); IntuitionBase = 0;
	if (IIntuition) DropInterface((struct Interface*) IIntuition); IIntuition = 0;

	if (IconBase) CloseLibrary(IconBase); IconBase = 0;
	if (IIcon) DropInterface((struct Interface*)IIcon); IIcon = 0;

	if (AslBase) CloseLibrary(AslBase); AslBase = 0;
	if (IAsl) DropInterface((struct Interface*)IAsl); IAsl = 0;

	if (DOSBase) CloseLibrary(DOSBase); DOSBase = 0;
	if (IDOS) DropInterface((struct Interface*)IDOS); IDOS = 0;
}

struct Screen *FrontMostScr(void)
{
    struct Screen *front_screen_address, *public_screen_address = NULL;
    ULONG intuition_lock;
    struct List *public_screen_list;
    struct PubScreenNode *public_screen_node;

    intuition_lock = LockIBase(0L);
    front_screen_address = ((struct IntuitionBase *)IntuitionBase)->FirstScreen;
    if( (front_screen_address->Flags & PUBLICSCREEN) || (front_screen_address->Flags & WBENCHSCREEN) )
    {
        UnlockIBase(intuition_lock);
        public_screen_list = LockPubScreenList();
        public_screen_node = (struct PubScreenNode*)public_screen_list->lh_Head;
        while(public_screen_node)
        {
            if(public_screen_node->psn_Screen == front_screen_address)
            {
                public_screen_address = public_screen_node->psn_Screen;
                break;
            }

            public_screen_node = (struct PubScreenNode*)public_screen_node->psn_Node.ln_Succ;
        }

        UnlockPubScreenList();
    }
    else
    {
        UnlockIBase(intuition_lock);
    }

    if(!public_screen_address)
    {
        public_screen_address = LockPubScreen(NULL);
        UnlockPubScreen(NULL, public_screen_address);
    }
    return(public_screen_address);
}

void VARARGS68K EasyRequester(CONST_STRPTR text, CONST_STRPTR button, ...)
{
  struct EasyStruct easyreq = { 0 };
  va_list parameters;
  STRPTR vargs = NULL;

  va_startlinear(parameters, button);
  vargs = va_getlinearva(parameters, STRPTR);

  easyreq.es_StructSize   = sizeof(struct EasyStruct);
  easyreq.es_Flags        = ESF_SCREEN | ESF_EVENSIZE;// | ESF_TAGGED;
  easyreq.es_Title        = AMIGA_VERSION ;
  easyreq.es_TextFormat   = text;
  easyreq.es_GadgetFormat = button;
  easyreq.es_Screen       = FrontMostScr();

  EasyRequestArgs(NULL, &easyreq, NULL, vargs);

  va_end(parameters);
}
