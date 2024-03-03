#define CATCOMP_NUMBERS
//#define CATCOMP_BLOCK
//#define CATCOMP_CODE
extern struct LocaleInfo li;

#include "includes.h"
#include "debug.h"


void CreateGUIwindow(struct DGenGUI *);
BOOL ProcessGUI(struct DGenGUI *);
int32 beginCommand(STRPTR rom_file, STRPTR rom_ext);
void updateList(struct DGenGUI *);

extern uint32 DoMessage(char *message, char reqtype, STRPTR buttons);
extern int32 GetRoms(STRPTR romsdir, struct List *);
extern void LaunchRom(struct DGenGUI *);
extern void ShowPreview(struct DGenGUI *);
extern STRPTR DupStr(CONST_STRPTR str, int32 length);
extern VOID FreeString(STRPTR *string);
extern int32 SaveToolType(STRPTR iconname, STRPTR ttpName, STRPTR ttpArg);
extern BOOL make_chooser_list2(BOOL, struct List *, int32, int32); // using CATCOMP_NUMBERS and index


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
int32 beginCommand(STRPTR rom_file, STRPTR rom_ext)
{
	STRPTR cmdline = IExec->AllocVecTags(CMDLINE_LENGTH, TAG_END),
	       savestate_str;
	uint32 res_val;
	int32 res_value = 0;
	struct Node *res_nod;
DBUG("beginCommand()\n",NULL);
	// Check if romfile exists
	IUtility->Strlcpy(cmdline, rom_file, CMDLINE_LENGTH);
	IUtility->Strlcat(cmdline, rom_ext, CMDLINE_LENGTH);
	struct ExamineData *dat = IDOS->ExamineObjectTags(EX_StringNameInput,cmdline, TAG_END);
	if(dat == NULL) {
		IExec->FreeVec(cmdline);
		return -1;
	}
DBUG("  ExamineObjectTags(): '%s' %s%lld bytes\n",dat->Name,"",dat->FileSize);
	IDOS->FreeDosObject(DOS_EXAMINEDATA, dat);

	// Add executble to commandline string
	IIntuition->GetAttr(CHOOSER_Selected, OBJ(OID_DGENEXE), &res_val);
	IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "DGEN-SDL%ld",res_val+1);

	// Add 0:RUN/PLAY or 1:RECORD or 2:PLAYBACK to commandline string
	IIntuition->GetAttr(CHOOSER_Selected, OBJ(OID_GAME_OPTIONS), &res_val);
	if(res_val != 0) {
		IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "%s -%lc \"%s.REC\"",cmdline,res_val==1? 'd':'D',IDOS->FilePart(rom_file) );
	}
	else { // add savestate (if chosen) to commandline string
		IIntuition->GetAttrs(OBJ(OID_SAVESTATES), CHOOSER_Selected,(uint32*)&savestate_str, CHOOSER_SelectedNode,(uint32*)&res_nod, TAG_DONE);
		if(savestate_str != NULL) {
			IChooser->GetChooserNodeAttrs(res_nod, CNA_Text,&savestate_str, TAG_DONE);
DBUG("  load savestate %lc\n",savestate_str[0]);
			IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "%s -s %lc",cmdline,savestate_str[0]);
		}
	}
	// Add ROM file to commandline string
	IUtility->SNPrintf(cmdline, CMDLINE_LENGTH, "%s \"%s%s\"",cmdline,rom_file,rom_ext);
DBUG("  %s\n",cmdline);

	// Launch 'DGen'
	res_value = IDOS->SystemTags(cmdline, SYS_Input,NULL, SYS_Output,NULL, //SYS_Error,NULL,
	                             NP_Priority,0, SYS_Asynch,FALSE, TAG_END);

	IExec->FreeVec(cmdline);

	return res_value;
}

uint32 selectListEntry(struct Window *pw, uint32 res_val)
{
	IIntuition->SetAttrs(OBJ(OID_LISTBROWSER),
	                     LISTBROWSER_Selected,res_val,
	                     LISTBROWSER_MakeVisible,res_val, TAG_DONE);
	IIntuition->RefreshGadgets(GAD(OID_LISTBROWSER), pw, NULL);

	return res_val;
}

uint32 selectListEntryNode(struct Window *pw, struct Node *n)
{
	uint32 res_val;
	IIntuition->SetAttrs(OBJ(OID_LISTBROWSER),
                      LISTBROWSER_SelectedNode,n,
                      LISTBROWSER_MakeNodeVisible,n, TAG_DONE);
	IIntuition->RefreshGadgets(GAD(OID_LISTBROWSER), pw, NULL);
	IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Selected,&res_val, TAG_DONE);

	return res_val;
}

BOOL ProcessGUI(struct DGenGUI *dgg)
{
	BOOL done = TRUE;
	uint16 code = 0;
	uint32 result = WMHI_LASTMSG, res_value = 0, res_totnode = 0, res_temp = 0,
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
		case WMHI_VANILLAKEY:
		{
DBUG("[WMHI_VANILLAKEY] = 0x%lx (0x%lx)\n",code,result&WMHI_KEYMASK);
			struct Node *node1 = NULL, *next_node1 = NULL;
			STRPTR node_val, next_n_val;
			char char_node, char_keyb;

			if(code == 0x1b) // ESC
			{
				done = FALSE;
				break;
			}

			if(code == 0x0d) // ENTER/RETURN
			{
				LaunchRom(dgg);
				break;
			}

			char_keyb = IUtility->ToUpper(result & WMHI_KEYMASK); // uppercase'd (key pressed)
DBUG("[WMHI_VANILLAKEY] '%lc' (0x%lx)\n",char_keyb,char_keyb);
			IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_SelectedNode,&node1, TAG_DONE);
			IListBrowser->GetListBrowserNodeAttrs(node1, LBNA_Column,COL_ROM, LBNCA_Text,&node_val, TAG_DONE);
			next_node1 = IExec->GetSucc(node1);
			IListBrowser->GetListBrowserNodeAttrs(next_node1, LBNA_Column,COL_ROM, LBNCA_Text,&next_n_val, TAG_DONE);
DBUG("  Actual Node -> Next Node\n",NULL);
DBUG("   0x%08lx -> 0x%08lx\n",node1,next_node1);
DBUG("          '%lC' -> '%lC'\n",*node_val,*next_n_val);
			// SELECT IT: NEXT node starts with KEY pressed and NEXT node = ACTUAL node
			if( char_keyb==IUtility->ToUpper(*next_n_val)  &&  IUtility->ToUpper(*next_n_val)==IUtility->ToUpper(*node_val) )
			{
				res_prev = selectListEntryNode(dgg->win, next_node1);
				ShowPreview(dgg);
			}
			// GO TO KEY PRESSED FIRST NODE: 1)ACTUAL node starts with KEY pressed and NEXT node != ACTUAL node
			// OR 2)pressed another KEY OR 3)reached end of listbrowser (next_node1=NULL)
			if( (char_keyb==IUtility->ToUpper(*node_val)  &&  IUtility->ToUpper(*next_n_val)!=IUtility->ToUpper(*node_val))
			   ||  char_keyb!=IUtility->ToUpper(*node_val)  ||  next_node1==NULL )
			{
				next_node1 = node1; // avoid refreshing/reloading single ROM filename entries
				for(node1=IExec->GetHead(dgg->romlist); node1!=NULL; node1=IExec->GetSucc(node1) ) {
					IListBrowser->GetListBrowserNodeAttrs(node1, LBNA_Column,COL_ROM, LBNCA_Text,&res_str, TAG_DONE);
					char_node = IUtility->ToUpper(*res_str); // uppercased (romfile 1st letter)
					if(char_node==char_keyb  &&  node1!=next_node1) {
						res_prev = selectListEntryNode(dgg->win, node1);
						ShowPreview(dgg);
						node1 = NULL;
					}
				}
			}

		}
		break;
		case WMHI_RAWKEY:
		{
			int32 sel_entry = -1; // -1: key pressed not valid
DBUG("[WMHI_RAWKEY] 0x%lx (win=0x%08lx)\n",code,dgg->win);
			/*if(code == RAWKEY_ESC)
			{
				done = FALSE;
				break;
			}*/

			IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Selected,&res_value,
			                     LISTBROWSER_TotalNodes,&res_totnode, TAG_DONE);
DBUG("  sel=%ld  nodes=%ld\n",res_value,res_totnode);
			// HOME key
			if(code==RAWKEY_HOME  &&  res_value!=0) {
				sel_entry = 0;
			}
			// END key
			if(code==RAWKEY_END  &&  res_value!=res_totnode-1) {
				sel_entry = res_totnode - 1;
			}
			// CuRSOR UP key
			if(code==CURSORUP  &&  res_value!=0) {
						sel_entry = res_value - 1;
			}
			// PAGE UP key
			if(code==RAWKEY_PAGEUP  &&  res_value!=0) {
				IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Top,&res_value, LISTBROWSER_Bottom,&res_temp, TAG_DONE);
				sel_entry = res_temp - res_value;
				sel_entry = res_value - sel_entry;
//DBUG("  %ld\n",sel_entry);
				if(sel_entry < 0) { sel_entry = 0; }
			}
			// CURSOR DOWN key
			if(code==CURSORDOWN  &&  res_value!=res_totnode-1) {
				sel_entry = res_value + 1;
			}
			// PAGE DOWN key
			if(code==RAWKEY_PAGEDOWN  &&  res_value!=res_totnode-1) {
				IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Bottom,&res_value, TAG_DONE);
				sel_entry = res_value;
			}

			if(sel_entry != -1) {
				res_prev = selectListEntry(dgg->win, sel_entry);
				ShowPreview(dgg);
			}

			// RETURN/ENTER key
			/*if(code==RAWKEY_RETURN  ||  code==RAWKEY_ENTER)
			{
				LaunchRom(dgg);
			}*/

		}
		break;
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
				case OID_GAME_OPTIONS:
					IIntuition->RefreshSetGadgetAttrs(GAD(OID_SAVESTATES), dgg->win, NULL, GA_Disabled,code==0? FALSE:TRUE, TAG_DONE);
				break;
				/*case OID_PLAY_RECGAME:
					//IIntuition->RefreshSetGadgetAttrs(GAD(OID_RECGAME_FILE), dgg->win, NULL, GA_Disabled,code==1? FALSE:TRUE, TAG_DONE);
				break;*/
				case OID_SAVE:
					IIntuition->GetAttr(GETFILE_Drawer, OBJ(OID_ROMDRAWER), (uint32*)&res_str);
DBUG("  OID_SAVE: '%s' (0x%08lx)\n",res_str,dgg->wbs);
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

BOOL make_chooser_list2(BOOL mode, struct List *list, int32 str_num, int32 index)
{
	struct Node *node;
	int32 j;

	if(mode == NEW_LIST) { IExec->NewList(list); }
	for(j=0; j<index; j++)
	{
		node = IChooser->AllocChooserNode(CNA_CopyText, TRUE,
		                                  CNA_Text, GetString(&li, str_num+j),
		                                 TAG_DONE);
		if(node) { IExec->AddTail(list, node); }
		else { return FALSE; }
	}

	return TRUE;
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

	make_chooser_list2(NEW_LIST, dgg->game_opts_list, MSG_GUI_GAME_RUN, 3);

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
        WA_IDCMP, IDCMP_VANILLAKEY | IDCMP_RAWKEY,
        WINDOW_IconifyGadget, TRUE,
        WINDOW_AppPort,       gAppPort,
        WINDOW_Icon,          dgg->iconify,
        WINDOW_Position,    WPOS_CENTERSCREEN,
        WINDOW_PopupGadget, TRUE,
        //WINDOW_JumpScreensMenu, TRUE,
        WINDOW_GadgetHelp, dgg->myTT.show_hints,
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
             BITMAP_SourceFile, "PROGDIR:DGenConf/dgen_banner.png",
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
            GA_HintInfo,  GetString(&li, MSG_GUI_ROMDRAWER_HELP),
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
            GA_HintInfo,  GetString(&li, MSG_GUI_DGENEXEX_HELP),
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
             GA_HintInfo,  GetString(&li, MSG_GUI_LISTBROWSER_HELP),
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

// BUTTON/IMAGE + [RECGAME + SAVESTATES] + TOTALROMS
             LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
               LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
               LAYOUT_SpaceOuter,  FALSE,

// BUTTON/IMAGE
               LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
                 //LAYOUT_Orientation,    LAYOUT_ORIENT_VERT,
                 LAYOUT_HorizAlignment, LALIGN_CENTER,
                 LAYOUT_SpaceOuter,     FALSE,
                 LAYOUT_AddChild, IIntuition->NewObject(SpaceClass, NULL, //"space.gadget",
                   SPACE_MinWidth, 20,
                 TAG_DONE),
                 //CHILD_WeightedWidth, 0,
                 LAYOUT_AddChild, OBJ(OID_PREVIEW_BTN) = IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
                   GA_ID,         OID_PREVIEW_BTN,
                   GA_RelVerify,  TRUE,
                   GA_Underscore, 0,
                   //GA_Text,       "RUN GAME",
                   GA_HintInfo,  GetString(&li, OID_PREVIEW_BTN_HELP),
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
// [RECGAME + SAVESTATES]
//               LAYOUT_AddChild, OBJ(OID_OPTIONS_GROUP) = IIntuition->NewObject(LayoutClass, NULL, //"layout.gadget",
//                 LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
                 //LAYOUT_SpaceOuter,  FALSE,
                 //LAYOUT_SpaceInner,  TRUE,
                 //LAYOUT_BevelStyle,  BVS_GROUP,
                 //LAYOUT_Label,       GetString(&li, MSG_GUI_OPTIONS_GROUP),//"Options",
                 LAYOUT_AddChild, OBJ(OID_GAME_OPTIONS) = IIntuition->NewObject(ChooserClass, NULL, //"chooser.gadget",
                   GA_ID,         OID_GAME_OPTIONS,
                   GA_RelVerify,  TRUE,
                   GA_Underscore, 0,
                   GA_HintInfo,  GetString(&li, MSG_GUI_GAME_OPTIONS_HELP),
                   CHOOSER_Labels,   dgg->game_opts_list,
                   CHOOSER_Selected, 0,
                 TAG_DONE),
                 LAYOUT_AddChild, IIntuition->NewObject(SpaceClass, NULL, //"space.gadget",
                   SPACE_MinHeight, 10,
                 TAG_DONE),
                 LAYOUT_AddChild, OBJ(OID_SAVESTATES) = IIntuition->NewObject(ChooserClass, NULL, //"chooser.gadget",
                   GA_ID,         OID_SAVESTATES,
                   GA_RelVerify,  TRUE,
                   GA_Underscore, 0,
                   GA_HintInfo,  GetString(&li, MSG_GUI_SAVESTATES_HELP),
                   CHOOSER_Labels,   dgg->savestates_list,
                   CHOOSER_Selected, 0,
                 TAG_DONE),
                 //CHILD_WeightedWidth, 0,
                 CHILD_Label, IIntuition->NewObject(LabelClass, NULL,// "label.image",
                  LABEL_Text, GetString(&li, MSG_GUI_SAVESTATES),//"Load savestate",
                 TAG_DONE),
//               TAG_DONE), // END of // [RECGAME + SAVESTATES]
               LAYOUT_AddChild, IIntuition->NewObject(SpaceClass, NULL, //"space.gadget",
                 SPACE_MinHeight, 10,
               TAG_DONE),
// TOTALROMS
               LAYOUT_AddChild, OBJ(OID_TOTALROMS) = IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
                 //GA_ID,         OID_TOTALROMS,
                 //GA_RelVerify,  TRUE,
                 GA_ReadOnly,   TRUE,
                 GA_Underscore, 0,
                 GA_Text,       GetString(&li, MSG_GUI_TOTALROMS),
                 //GA_HintInfo,  GetString(&li, MSG_GUI_DGENEXEX_HELP),
                 BUTTON_Justification, BCJ_LEFT,
                 BUTTON_BevelStyle,    BVS_NONE,
                 BUTTON_Transparent,   TRUE,
               TAG_DONE),
               //CHILD_WeightedWidth,  0,
               CHILD_WeightedHeight, 0,

             TAG_DONE), // END of BUTTON/IMAGE + [RECGAME + SAVESTATES] + TOTALROMS
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
            GA_Text,      GetString(&li, MSG_GUI_ABOUT_BTN),//"About..."
            GA_HintInfo,  GetString(&li, MSG_GUI_ABOUT_BTN_HELP),
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
            GA_ID,        OID_SAVE,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_GUI_SAVE_BTN),//"Save settings"
            GA_HintInfo,  GetString(&li, MSG_GUI_SAVE_BTN_HELP),
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL, //"button.gadget",
            GA_ID,        OID_QUIT,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_GUI_QUIT_BTN),//"Quit",
            GA_HintInfo,  GetString(&li, MSG_GUI_QUIT_BTN_HELP),
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
