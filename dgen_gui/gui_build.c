#define CATCOMP_NUMBERS
//#define CATCOMP_BLOCK
//#define CATCOMP_CODE
extern struct LocaleInfo li;

#include "includes.h"
#include "debug.h"


void CreateGUIwindow(struct DGenGUI *);
BOOL ProcessGUI(struct DGenGUI *);
int32 beginCommand(char *romfile);
void updateList(struct DGenGUI *);

extern uint32 DoMessage(char *message, char reqtype, STRPTR buttons);
extern int32 GetRoms(STRPTR romsdir, struct List *);
extern void LaunchRom(struct DGenGUI *);
extern void ShowPreview(struct DGenGUI *);
extern STRPTR DupStr(CONST_STRPTR str, int32 length);
extern VOID FreeString(STRPTR *string);
extern int32 SaveToolType(STRPTR iconname, STRPTR ttpName, STRPTR ttpArg);


extern struct IconIFace *IIcon;
extern struct DOSIFace *IDOS;
extern struct IntuitionIFace *IIntuition;
extern struct GraphicsIFace *IGraphics;
extern struct UtilityIFace *IUtility;

// the class pointer
extern Class *ClickTabClass, *ListBrowserClass, *ButtonClass, *LabelClass, *GetFileClass,
             *CheckBoxClass, *ChooserClass, *BitMapClass, *LayoutClass, *WindowClass,
             *RequesterClass, *SpaceClass, *IntegerClass, *GetFileClass;;
// some interfaces needed
//extern struct ListBrowserIFace *IListBrowser;
//extern struct ClickTabIFace *IClickTab;
//extern struct LayoutIFace *ILayout;
//extern struct ChooserIFace *IChooser;

extern struct WBStartup *WBenchMsg;


Object *Objects[LAST_NUM];
uint32 res_prev; // avoid "reload" already selected ROM


#define CMDLINE_LENGTH 2048
int32 beginCommand(char *romfile)
{
	STRPTR cmdline;
	int32 res_value = 0;
	STRPTR savestate = NULL;
	struct Node *res_nod;
DBUG("beginCommand()\n",NULL);
	// Check if romfile exists
	struct ExamineData *dat = IDOS->ExamineObjectTags(EX_StringNameInput,romfile, TAG_END);
	if(dat == NULL) { return -1; }
DBUG("  ExamineObjectTags(): '%s' %s%lld bytes\n",dat->Name,"",dat->FileSize);
	IDOS->FreeDosObject(DOS_EXAMINEDATA, dat);

	cmdline = IExec->AllocVecTags(CMDLINE_LENGTH, TAG_DONE);

	// Add romfile (and savestate) to commandline string
	IIntuition->GetAttr(CHOOSER_Selected, OBJ(OID_DGENEXE), (uint32*)&res_value);

	IIntuition->GetAttrs(OBJ(OID_SAVESTATES), CHOOSER_Selected,(uint32*)&savestate, CHOOSER_SelectedNode,(uint32*)&res_nod, TAG_DONE);
	if(savestate != NULL) {
		IChooser->GetChooserNodeAttrs(res_nod, CNA_Text,&savestate, TAG_DONE);
DBUG("  load savestate %lc\n",savestate[0]);
		IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "DGEN-SDL%ld -s %lc \"%s\"",res_value+1,savestate[0],romfile);
	}
	else { IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "DGEN-SDL%ld \"%s\"",res_value+1,romfile); }

	//IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "DGEN-SDL%ld \"%s\"",res_value+1,romfile);
DBUG("  %s\n",cmdline);

	// Launch 'DGen'
	res_value = IDOS->SystemTags(cmdline, SYS_Input,NULL, SYS_Output,NULL, //SYS_Error,NULL,
	                             NP_Priority,0, SYS_Asynch,FALSE, TAG_END);

	IExec->FreeVec(cmdline);

	return res_value;
}

BOOL ProcessGUI(struct DGenGUI *dgg)
{
	BOOL done = TRUE;
	uint16 code = 0;
	uint32 result = WMHI_LASTMSG, res_value = 0, res_temp = 0,
	       siggot = 0, wsigmask = 0;
	STRPTR res_str = NULL;

	IIntuition->GetAttr(WINDOW_SigMask, OBJ(OID_MAIN), &wsigmask);
	siggot = IExec->Wait(wsigmask|SIGBREAKF_CTRL_C);

	if(siggot & SIGBREAKF_CTRL_C) { return FALSE; }

	while( (result=IIntuition->IDoMethod(OBJ(OID_MAIN), WM_HANDLEINPUT, &code)) != WMHI_LASTMSG )
	{
//DBUG("result=0x%lx\n",result);
		switch(result & WMHI_CLASSMASK)
		{
			case WMHI_CLOSEWINDOW:
				done = FALSE;
			break;
			case WMHI_ICONIFY:
DBUG("WMHI_ICONIFY (win=0x%08lx)\n",dgg->win);
				if( IIntuition->IDoMethod(OBJ(OID_MAIN), WM_ICONIFY) ) {
					dgg->win = NULL;
				}
			break;
			case WMHI_UNICONIFY:
				if( (dgg->win=(struct Window *)IIntuition->IDoMethod(OBJ(OID_MAIN), WM_OPEN, NULL)) ) {
DBUG("WMHI_UNICONIFY (win=0x%08lx)\n",dgg->win);
					dgg->screen = dgg->win->WScreen;
					IIntuition->ScreenToFront(dgg->screen);
				}
				else { done = FALSE; }
			break;
			/*case WMHI_JUMPSCREEN:
				IIntuition->GetAttrs(OBJ(OID_MAIN), WA_PubScreen,&dgg->screen, TAG_DONE);
				//IIntuition->SetAttrs(OBJ(OID_MAIN), WA_PubScreen,dgg->screen, TAG_DONE);
		break;*/
		case WMHI_GADGETUP:
DBUG("[WMHI_GADGETUP] code = %ld (0x%08lx)\n",code,code);
			switch(result & WMHI_GADGETMASK)
			{
				case OID_ROMDRAWER:
					res_value = IIntuition->IDoMethod(OBJ(OID_ROMDRAWER), GFILE_REQUEST, dgg->win);
					if(res_value) {
						updateList(dgg);
						// Sort and refresh new list and chooser info
						IIntuition->DoGadgetMethod(GAD(OID_LISTBROWSER), dgg->win, NULL,
						                           LBM_SORT, NULL, COL_ROM, LBMSORT_FORWARD, NULL);
						IIntuition->RefreshGadgets(GAD(OID_LISTBROWSER), dgg->win, NULL);
						// Reset selected rom to 1st entry
						res_prev = 0;
						ShowPreview(dgg);
					}
				break;
				case OID_LISTBROWSER:
					IIntuition->GetAttrs( OBJ(OID_LISTBROWSER), LISTBROWSER_RelEvent,&res_value, LISTBROWSER_Selected,&res_temp, TAG_DONE);
					if(res_value == LBRE_DOUBLECLICK) { LaunchRom(dgg); }
					else
					{// avoid "reload" already selected ROM (and launching if no ROMs in listbrowser)
DBUG("  Selected: [old]%ld == [new]%ld\n",res_prev,res_temp);
						if(res_temp!=-1  &&  res_prev!=res_temp) {
							res_prev = res_temp;
							ShowPreview(dgg);
						}
					}
				break;
				case OID_PREVIEW_BTN:
					LaunchRom(dgg);
				break;
				case OID_ABOUT:
				{
					char text_buf[256] = "";
					IUtility->SNPrintf(text_buf, sizeof(text_buf), (STRPTR)GetString(&li,MSG_GUI_ABOUT_TEXT),VERS,DATE);
					DoMessage(text_buf, REQIMAGE_INFO, NULL);
				}
				break;
				case OID_SAVE:
					IIntuition->GetAttr(GETFILE_Drawer, OBJ(OID_ROMDRAWER), (uint32*)&res_str);
					SaveToolType(dgg->wbs->sm_ArgList->wa_Name, "ROMS_DRAWER", res_str);
				break;
				case OID_QUIT:
					done = FALSE;
				break;
			}
		} // END switch
	} // END while( (result..

	return done;
}


void CreateGUIwindow(struct DGenGUI *dgg)
{
	struct MsgPort *gAppPort = NULL;
	struct ColumnInfo *columninfo;
	uint32 res_totnode;
	//struct Node *node;
DBUG("CreateGUIwindow()\n",NULL);
	STRPTR chooser_array[] = {"SDL1", "SDL2", NULL};
	WORD max_w_ext = IGraphics->TextLength(&dgg->screen->RastPort, GetString(&li,MSG_GUI_TITLE_COL_FMT), IUtility->Strlen(GetString(&li,MSG_GUI_TITLE_COL_FMT))+1); // max rom extension pixel width

	gAppPort = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END);

//DBUG("screentitle height=%ld\n",dggn->screen->BarHeight + 1);
//DBUG("screenfont='%s'/%ld\n",dgg->screen->Font->ta_Name,dgg->screen->Font->ta_YSize);

	columninfo = IListBrowser->AllocLBColumnInfo(LAST_COL,
	                            LBCIA_Column,COL_ROM, LBCIA_Title, GetString(&li, MSG_GUI_TITLE_COL_ROM), //"Game",
	                                                  LBCIA_AutoSort,TRUE, LBCIA_Sortable,TRUE,
	                                                  LBCIA_Weight, 100,
	                            LBCIA_Column,COL_FMT, LBCIA_Title, GetString(&li, MSG_GUI_TITLE_COL_FMT), //"Format",
	                                                  LBCIA_AutoSort,TRUE, LBCIA_Sortable,TRUE,
	                                                  LBCIA_Width, max_w_ext,
	                           TAG_DONE);

	//node = IChooser->AllocChooserNode(CNA_Text,GetString(&li,MSG_GUI_SAVESTATES_NO), TAG_DONE);
	//IExec->AddTail(dgg->savestates_list, node);

	OBJ(OID_MAIN) = IIntuition->NewObject(WindowClass, NULL, //"window.class",
        WA_ScreenTitle, VERS" "DATE,
        WA_Title,       "DGenGUI",
        WA_PubScreen,         dgg->screen,
        WA_PubScreenFallBack, TRUE,
        WA_DragBar,     TRUE,
        WA_CloseGadget, TRUE,
        WA_SizeGadget,  TRUE,
        WA_DepthGadget, TRUE,
        WA_Activate,    TRUE,
        WINDOW_IconifyGadget, TRUE,
        WINDOW_AppPort,       gAppPort,
        WINDOW_Icon,          dgg->iconify,
        WINDOW_Position,    WPOS_CENTERSCREEN,
        WINDOW_PopupGadget, TRUE,
        //WINDOW_JumpScreensMenu, TRUE,
        WINDOW_Layout, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
         LAYOUT_Orientation,    LAYOUT_ORIENT_VERT,
         LAYOUT_SpaceOuter,     TRUE,
         LAYOUT_HorizAlignment, LALIGN_CENTER,
// BANNER
         LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
           GA_ReadOnly, TRUE,
           //BUTTON_Transparent, TRUE,
           BUTTON_BevelStyle,  BVS_BOX,
           BUTTON_RenderImage, OBJ(OID_BANNER_IMG) = IIntuition->NewObject(BitMapClass, NULL, //"bitmap.image",
             //IA_Scalable, TRUE,
             BITMAP_Screen,     dgg->screen,
             //BITMAP_Transparent, TRUE,
             BITMAP_SourceFile, "PROGDIR:DgenConf/dgen_banner.png",
           TAG_DONE),
         TAG_DONE),
         CHILD_WeightedHeight, 0,
         CHILD_WeightedWidth,  0,
// ROM DRAWER + DGEN EXECUTABLE
        LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
          //LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
          //LAYOUT_SpaceOuter,  TRUE,
          LAYOUT_AddChild, OBJ(OID_ROMDRAWER) = IIntuition->NewObject(GetFileClass, NULL,
            GA_ID,        OID_ROMDRAWER,
            GA_RelVerify, TRUE,
            GETFILE_ReadOnly,    TRUE,
            GETFILE_RejectIcons, TRUE,
            GETFILE_TitleText,   GetString(&li, MSG_GUI_ROMDRAWER_TITLE),
            GETFILE_DrawersOnly, TRUE,
            GETFILE_Drawer,      dgg->myTT.romsdrawer,
          TAG_DONE),
          CHILD_Label, IIntuition->NewObject(LabelClass, NULL,
            LABEL_Text, GetString(&li, MSG_GUI_ROMDRAWER),//" ROM drawer:",
          TAG_DONE),
          LAYOUT_AddChild, OBJ(OID_DGENEXE) = IIntuition->NewObject(ChooserClass, NULL,
            GA_Underscore, 0,
            CHOOSER_LabelArray, chooser_array,
          TAG_DONE),
          CHILD_WeightedWidth, 0,
          CHILD_Label, IIntuition->NewObject(LabelClass, NULL,
            LABEL_Text, " D_Gen:",
          TAG_DONE),
         TAG_DONE),
         CHILD_WeightedHeight, 0,
// ROM LIST + PREVIEW BUTTON
         LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
           //LAYOUT_Orientation,    LAYOUT_ORIENT_VERT,
           LAYOUT_SpaceOuter,     TRUE,
           //LAYOUT_SpaceInner,     FALSE,
           //LAYOUT_HorizAlignment, LALIGN_CENTER,
           //LAYOUT_BevelStyle,     BVS_SBAR_VERT,
           //LAYOUT_Label,          GetString(&li, MSG_GUI_GENERAL_GROUP),//"Game / Preview",
           LAYOUT_AddChild, OBJ(OID_LISTBROWSER) = IIntuition->NewObject(ListBrowserClass, NULL, //"listbrowser.gadget",
             GA_ID,        OID_LISTBROWSER,
             GA_RelVerify, TRUE,
             //LISTBROWSER_SortColumn,     COL_ROM,
             //LISTBROWSER_AutoFit,        TRUE,
             LISTBROWSER_Labels,         NULL,
             LISTBROWSER_ColumnInfo,     columninfo,
             LISTBROWSER_ColumnTitles,   TRUE,
             LISTBROWSER_ShowSelected,   TRUE,
             LISTBROWSER_Selected,       -1,
             LISTBROWSER_MinVisible,     15,
             //LISTBROWSER_Striping,       LBS_ROWS,
             LISTBROWSER_TitleClickable, TRUE,
             //LISTBROWSER_HorizontalProp, TRUE,
           TAG_DONE),
           CHILD_MinWidth, 200,
           //CHILD_MaxWidth, 800, // pixels width of listbrowser
           CHILD_WeightedWidth, 60,

// BUTTON/IMAGE + [SAVESTATES + TOTALROMS]
             LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
               LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
               LAYOUT_SpaceOuter,  FALSE,

// BUTTON/IMAGE
               LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
                 //LAYOUT_Orientation,    LAYOUT_ORIENT_VERT,
                 LAYOUT_HorizAlignment, LALIGN_CENTER,
                 LAYOUT_SpaceOuter,  FALSE,
                 LAYOUT_AddChild, IIntuition->NewObject(SpaceClass, NULL, //"space.gadget",
                   SPACE_MinWidth, 20,
                 TAG_DONE),
                 //CHILD_WeightedWidth, 0,
                 LAYOUT_AddChild, OBJ(OID_PREVIEW_BTN) = IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
                   GA_ID,         OID_PREVIEW_BTN,
                   GA_RelVerify,  TRUE,
                   GA_Underscore, 0,
                   //GA_Text,       "RUN GAME",
                   BUTTON_BevelStyle,  BVS_THIN,
                   //BUTTON_Transparent, TRUE,
                   //BUTTON_BackgroundPen, BLOCKPEN,
                   //BUTTON_FillPen,       BLOCKPEN,
                   BUTTON_RenderImage, OBJ(OID_PREVIEW_IMG) = IIntuition->NewObject(BitMapClass, NULL, //"bitmap.image",
                    //IA_Scalable, TRUE,
                    BITMAP_Screen, dgg->screen,
                    //BITMAP_Masking, TRUE,
                   TAG_DONE),
                 TAG_DONE),
                 CHILD_MaxWidth, 256+2, // pixels width of preview + button border
                 CHILD_MinWidth, 256+2, // pixels width of preview + button border
                 CHILD_MaxHeight, 224+2, // pixels height of preview + button border
                 CHILD_MinHeight, 224+2, // pixels height of preview + button border
                 //CHILD_NoDispose, TRUE,
                 LAYOUT_AddChild, IIntuition->NewObject(SpaceClass, NULL, //"space.gadget",
                   SPACE_MinWidth, 20,
                 TAG_DONE),
                 //CHILD_WeightedWidth, 0,
               TAG_DONE), // END of BUTTON/IMAGE
// SAVESTATES + TOTALROMS
               LAYOUT_AddChild, OBJ(OID_SAVESTATES) = IIntuition->NewObject(ChooserClass, NULL, //"chooser.gadget",
                 GA_ID,         OID_SAVESTATES,
                 GA_RelVerify,  TRUE,
                 GA_Underscore, 0,
                 CHOOSER_Labels,   dgg->savestates_list,
                 CHOOSER_Selected, 0,
               TAG_DONE),
               //CHILD_WeightedWidth, 0,
               CHILD_Label, IIntuition->NewObject(LabelClass, NULL,// "label.image",
                LABEL_Text, GetString(&li, MSG_GUI_SAVESTATES),//"Load savestate",
               TAG_DONE),
               LAYOUT_AddChild, OBJ(OID_TOTALROMS) = IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
                 //GA_ID,         OID_TOTALROMS,
                 //GA_RelVerify,  TRUE,
                 GA_ReadOnly,   TRUE,
                 GA_Underscore, 0,
                 GA_Text,       GetString(&li, MSG_GUI_TOTALROMS),
                 BUTTON_Justification, BCJ_LEFT,
                 BUTTON_BevelStyle,    BVS_NONE,
                 BUTTON_Transparent,   TRUE,
               TAG_DONE),
               //CHILD_WeightedWidth,  0,
               CHILD_WeightedHeight, 0,

             TAG_DONE), // END of BUTTON/IMAGE + [SAVESTATES + TOTALROMS]
             CHILD_WeightedWidth, 40,

         TAG_DONE), // END of ROM LIST + PREVIEW BUTTON
// BUTTONS
          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
           LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
           LAYOUT_SpaceOuter,  TRUE,
           LAYOUT_BevelStyle,  BVS_SBAR_VERT,
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
            GA_ID,        OID_ABOUT,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_GUI_ABOUT_BTN),//"About"
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
            GA_ID,        OID_SAVE,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_GUI_SAVE_BTN),//"Save settings"
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
            GA_ID,        OID_QUIT,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_GUI_QUIT_BTN),//"Quit",
           TAG_DONE),
          TAG_DONE), // END of BUTTONS
          CHILD_WeightedHeight, 0,

        TAG_DONE), // END of window layout group
	TAG_END);

	if( OBJ(OID_MAIN) ) {
		updateList(dgg);
		// Select last launched ROM
		IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_TotalNodes,&res_totnode, TAG_DONE);
		if(dgg->myTT.last_rom_run<0  ||  dgg->myTT.last_rom_run>=res_totnode-1) { dgg->myTT.last_rom_run = 0; }
DBUG("last_rom_run=%ld (of %ld)\n",dgg->myTT.last_rom_run,res_totnode);
		IIntuition->SetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Selected,dgg->myTT.last_rom_run,
		                     LISTBROWSER_MakeVisible,dgg->myTT.last_rom_run, TAG_DONE);
		res_prev = dgg->myTT.last_rom_run;
	}

	if( (dgg->win=(struct Window *)IIntuition->IDoMethod(OBJ(OID_MAIN), WM_OPEN, NULL)) )
	{
		IIntuition->ScreenToFront(dgg->win->WScreen);

		// Show preview image
		ShowPreview(dgg);

		while(ProcessGUI(dgg) != FALSE);
	} // END if( (dgg->win=..

	IIntuition->DisposeObject( OBJ(OID_MAIN) );
	OBJ(OID_MAIN) = NULL;
	IIntuition->DisposeObject( OBJ(OID_BANNER_IMG) );
	IIntuition->DisposeObject( OBJ(OID_PREVIEW_IMG) );

	IListBrowser->FreeLBColumnInfo(columninfo);
	IExec->FreeSysObject(ASOT_PORT, gAppPort);
}

void updateList(struct DGenGUI *dgg)
{
	STRPTR romdrawer = NULL;
	uint32 res_tot;
DBUG("updateList()\n",NULL);
	// Detach the listbrowser list first
	IIntuition->SetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Labels,NULL, TAG_END);
	// Re-generate romlist with new rom drawer
	IIntuition->GetAttr(GETFILE_Drawer, OBJ(OID_ROMDRAWER), (uint32*)&romdrawer);
	FreeString(&dgg->myTT.romsdrawer);
	dgg->myTT.romsdrawer = DupStr(romdrawer, -1);
	res_tot = GetRoms(dgg->myTT.romsdrawer, dgg->romlist);
	// Re-attach the listbrowser
	IIntuition->SetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_SortColumn,COL_ROM,
	                     LISTBROWSER_Labels,dgg->romlist, LISTBROWSER_Selected,0, TAG_END);
	IIntuition->RefreshSetGadgetAttrs(GAD(OID_TOTALROMS), dgg->win, NULL,
	                                  BUTTON_VarArgs,&res_tot, TAG_DONE);
}
