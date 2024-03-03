#define CATCOMP_NUMBERS
//#define CATCOMP_BLOCK
//#define CATCOMP_CODE
extern struct LocaleInfo li;


#include "includes.h"
#include "debug.h"


extern BOOL OpenLibs(void);
extern void CloseLibs(void);
extern void CreateGUIwindow(struct DGenGUI *);
//extern uint32 DoMessage(char *, char, STRPTR);
extern struct Screen *FrontMostScr(void);
//extern void GetGUIGadgets(void);
//extern int32 GetRoms(STRPTR romsdir, struct List *);
extern STRPTR DupStr(CONST_STRPTR str, int32 length);
extern VOID FreeString(STRPTR *string);
extern void free_chooserlist_nodes(struct List *);
//extern void createDrawer(STRPTR fullpath);


const char *version = VERSTAG;


extern struct IconIFace *IIcon;
extern struct DOSIFace *IDOS;
extern struct IntuitionIFace *IIntuition;
//extern struct GraphicsIFace *IGraphics;
extern struct UtilityIFace *IUtility;

extern struct ListBrowserIFace *IListBrowser;

//extern Object *Objects[LAST_NUM];


/************/
/* GUI MAIN */
/************/
//int main(int argc, char **argv)
int gui_main(struct WBStartup *wbs)
{
	int32 error = -1;
	//char text_buf[256];
	struct DiskObject *micon = NULL;
	STRPTR ttp;
DBUG("*** START DGENGUI ***\n",NULL);

	if(OpenLibs() == TRUE) {
		struct DGenGUI *DGenG = IExec->AllocVecTags(sizeof(struct DGenGUI), AVT_ClearWithValue,NULL, TAG_END);

		DGenG->myTT.romsdrawer = DupStr( ROMS, sizeof(ROMS) );
		//DGenG->myTT.newttp = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,NULL, TAG_END);
		//DGenG->myTT.ttpBuf1 = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,NULL, TAG_END);
		//DGenG->myTT.ttpBuf2 = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,NULL, TAG_END);
		DGenG->myTT.show_hints = FALSE;

DBUG("wbs = 0x%08lx\n",wbs);
		if(wbs) // launched from WB/icon
		{
			DGenG->wbs = wbs;
			//IUtility->Strlcpy( text_buf, wbs->sm_ArgList->wa_Name, sizeof(text_buf) );

			// Reset icon X/Y positions so it iconifies properly on Workbench
			DGenG->iconify = IIcon->GetIconTags(wbs->sm_ArgList->wa_Name, ICONGETA_FailIfUnavailable,FALSE, TAG_END);
			DGenG->iconify->do_CurrentX = NO_ICON_POSITION;
			DGenG->iconify->do_CurrentY = NO_ICON_POSITION;

			// Read tooltypes
			micon = IIcon->GetDiskObjectNew(wbs->sm_ArgList->wa_Name);
//DBUG("micon 0x%08lx\n",micon);
			if(micon)
			{
				ttp = IIcon->FindToolType(micon->do_ToolTypes, "PUBSCREEN");
				if(ttp)
				{
DBUG("SCREEN tooltype set to '%s'\n",ttp);
					DGenG->screen = IIntuition->LockPubScreen(ttp);
				}

				ttp = IIcon->FindToolType(micon->do_ToolTypes, "ROMS_DRAWER");
				if(ttp)
				{
					FreeString(&DGenG->myTT.romsdrawer);
					DGenG->myTT.romsdrawer = DupStr( ttp, IUtility->Strlen(ttp) );
				}
DBUG("ROMS_DRAWER set to '%s'\n",DGenG->myTT.romsdrawer);

				ttp = IIcon->FindToolType(micon->do_ToolTypes, "LAST_ROM_LAUNCHED");
				if(ttp)
				{
					IDOS->StrToLong(ttp, &DGenG->myTT.last_rom_run);
DBUG("LAST_ROM_LAUNCHED tooltype set to %ld\n",DGenG->myTT.last_rom_run);
				}

				ttp = IIcon->FindToolType(micon->do_ToolTypes, "SHOW_HINTS");
				if(ttp) { DGenG->myTT.show_hints = TRUE; }

				IIcon->FreeDiskObject(micon);
			}
		}

		if(DGenG->screen == NULL) { DGenG->screen = IIntuition->LockPubScreen(NULL); }
DBUG("pubscreen=0x%lx '%s'\n",DGenG->screen,DGenG->screen->Title);

		// Create "needed" drawers in 'PROGDIR:DGenConf' in case don't exists
		/*createDrawer(SCRSHOTS);
		createDrawer(SAVES);
		createDrawer(RECGAMES);*/

		DGenG->romlist = IExec->AllocSysObject(ASOT_LIST, NULL);
		DGenG->savestates_list = IExec->AllocSysObject(ASOT_LIST, NULL);
		DGenG->game_opts_list = IExec->AllocSysObject(ASOT_LIST, NULL);

		CreateGUIwindow(DGenG);

		free_chooserlist_nodes(DGenG->savestates_list);
		IExec->FreeSysObject(ASOT_LIST, DGenG->savestates_list);
		DGenG->savestates_list = NULL;

		free_chooserlist_nodes(DGenG->game_opts_list);
		IExec->FreeSysObject(ASOT_LIST, DGenG->game_opts_list);
		DGenG->game_opts_list = NULL;

		IListBrowser->FreeListBrowserList(DGenG->romlist);
		IExec->FreeSysObject(ASOT_LIST, DGenG->romlist);
		DGenG->romlist = NULL;

		IIntuition->UnlockPubScreen(NULL, DGenG->screen);

		FreeString(&DGenG->myTT.romsdrawer);

		//IExec->FreeVec(DGenG->myTT.newttp);
		//IExec->FreeVec(DGenG->myTT.ttpBuf1);
		//IExec->FreeVec(DGenG->myTT.ttpBuf2);

		IExec->FreeVec(DGenG);
	}

	CloseLibs();
DBUG("*** END DGENGUI ***\n",NULL);

	return error;
}
