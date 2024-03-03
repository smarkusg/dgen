#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#define CATCOMP_CODE
struct LocaleInfo li;


#include "includes.h"
#include "debug.h"


#define FILENAME_LENGTH   1024 // LaunchRom() and ShowPreview()
#define MAX_FULLFILEPATH  MAX_DOS_PATH+MAX_DOS_FILENAME // GetRoms()


extern int32 beginCommand(STRPTR rom_file, STRPTR rom_ext);


struct Library *IconBase = NULL;
//struct Library *DOSBase;
struct Library *IntuitionBase = NULL;
struct Library *GfxBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *LocaleBase = NULL;
struct IconIFace *IIcon = NULL;
extern struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition = NULL;
struct GraphicsIFace *IGraphics = NULL;
struct UtilityIFace *IUtility = NULL;
struct LocaleIFace *ILocale = NULL;
struct Library *ClickTabBase = NULL, *ListBrowserBase = NULL, *LayoutBase = NULL,
               *ChooserBase = NULL;
// the class library base
struct ClassLibrary *ButtonBase = NULL, *BitMapBase = NULL, *CheckBoxBase = NULL,
                    *LabelBase = NULL, *WindowBase = NULL, *StringBase = NULL,
                    *RequesterBase = NULL, *SpaceBase = NULL, *IntegerBase = NULL,
                    *GetFileBase = NULL;
// the class pointer
Class *ClickTabClass, *ListBrowserClass, *ButtonClass, *LabelClass, *StringClass,
      *CheckBoxClass, *ChooserClass, *BitMapClass, *LayoutClass, *WindowClass,
      *RequesterClass, *SpaceClass, *IntegerClass, *GetFileClass;
// some interfaces needed
struct ListBrowserIFace *IListBrowser = NULL;
//struct ClickTabIFace *IClickTab = NULL;
//struct LayoutIFace *ILayout = NULL;
struct ChooserIFace *IChooser = NULL;


extern Object *Objects[LAST_NUM];


BOOL OpenLibs(void)
{
DBUG("OpenLibs() - START\n",NULL);
	DOSBase = IExec->OpenLibrary("dos.library", 52);
	IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);

	UtilityBase = IExec->OpenLibrary("utility.library", 52);
	IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityBase, "main", 1, NULL);

	IntuitionBase = IExec->OpenLibrary("intuition.library", 52);
	IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);

	GfxBase = IExec->OpenLibrary("graphics.library", 52);
	IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);

	li.li_Catalog = NULL;
	if( (LocaleBase=IExec->OpenLibrary("locale.library", 52))
	   &&  (ILocale=(struct LocaleIFace *)IExec->GetInterface(LocaleBase, "main", 1, NULL)) )
	{
		li.li_ILocale = ILocale;
		li.li_Catalog = ILocale->OpenCatalog(NULL, "dgengui.catalog",
		                                     OC_BuiltInLanguage, "english",
		                                     OC_PreferExternal, TRUE,
		                                    TAG_END);
	}
	else { IDOS->PutErrStr("Failed to use catalog system. Using built-in strings.\n"); }

	IconBase = IExec->OpenLibrary("icon.library", 52);
	IIcon = (struct IconIFace *)IExec->GetInterface(IconBase, "main", 1, NULL);

	if(DOSBase==NULL  ||  UtilityBase==NULL  ||  IntuitionBase==NULL  ||  GfxBase==NULL
	   ||  LocaleBase==NULL  ||  IconBase==NULL) { return FALSE; }

	RequesterBase = IIntuition->OpenClass("requester.class", 52, &RequesterClass);
//	IntegerBase = IIntuition->OpenClass("gadgets/integer.gadget", 52, &IntegerClass);
	SpaceBase = IIntuition->OpenClass("gadgets/space.gadget", 52, &SpaceClass);
//	StringBase = IIntuition->OpenClass("gadgets/string.gadget", 52, &StringClass);
//	CheckBoxBase = IIntuition->OpenClass("gadgets/checkbox.gadget", 52, &CheckBoxClass);
	ButtonBase = IIntuition->OpenClass("gadgets/button.gadget", 52, &ButtonClass);
	BitMapBase = IIntuition->OpenClass("images/bitmap.image", 52, &BitMapClass);
	LabelBase = IIntuition->OpenClass("images/label.image", 52, &LabelClass);
	GetFileBase = IIntuition->OpenClass("gadgets/getfile.gadget", 52, &GetFileClass);
	WindowBase = IIntuition->OpenClass("window.class", 52, &WindowClass);

	ListBrowserBase = (struct Library *)IIntuition->OpenClass("gadgets/listbrowser.gadget", 52, &ListBrowserClass);
	//ClickTabBase = (struct Library *)IIntuition->OpenClass("gadgets/clicktab.gadget", 52, &ClickTabClass);
	LayoutBase = (struct Library *)IIntuition->OpenClass("gadgets/layout.gadget", 52, &LayoutClass);
	ChooserBase = (struct Library *)IIntuition->OpenClass("gadgets/chooser.gadget", 52, &ChooserClass);

	if(RequesterBase==NULL  /*||  IntegerBase==NULL*/  ||  SpaceBase==NULL  /*||  StringBase==NULL*/
	   /*||  CheckBoxBase==NULL*/  ||  ButtonBase==NULL  ||  BitMapBase==NULL  ||  LabelBase==NULL
	   ||  GetFileBase==NULL  ||  WindowBase==NULL  ||  ListBrowserBase==NULL  /*||  ClickTabBase==NULL*/
	   ||  LayoutBase==NULL  ||  ChooserBase==NULL) { return FALSE; }

	IListBrowser = (struct ListBrowserIFace *)IExec->GetInterface( (struct Library *)ListBrowserBase, "main", 1, NULL );
//	IClickTab = (struct ClickTabIFace *)IExec->GetInterface( (struct Library *)ClickTabBase, "main", 1, NULL );
//	ILayout = (struct LayoutIFace *)IExec->GetInterface( (struct Library *)LayoutBase, "main", 1, NULL );
	IChooser = (struct ChooserIFace *)IExec->GetInterface( (struct Library *)ChooserBase, "main", 1, NULL );
DBUG("OpenLibs() - END\n",NULL);
	return TRUE;
}

void CloseLibs(void)
{
DBUG("CloseLibs()\n",NULL);
	if(IIntuition) {
		IIntuition->CloseClass(RequesterBase);
//		IIntuition->CloseClass(IntegerBase);
		IIntuition->CloseClass(SpaceBase);
//		IIntuition->CloseClass(StringBase);
		IIntuition->CloseClass(LabelBase);
		IIntuition->CloseClass(GetFileBase);
		IIntuition->CloseClass(BitMapBase);
		IIntuition->CloseClass(ButtonBase);
//		IIntuition->CloseClass(CheckBoxBase);
		IIntuition->CloseClass(WindowBase);

		IExec->DropInterface( (struct Interface *)IChooser );
		IIntuition->CloseClass( (struct ClassLibrary *)ChooserBase );
//		IExec->DropInterface( (struct Interface *)ILayout );
		IIntuition->CloseClass( (struct ClassLibrary *)LayoutBase );
//		IExec->DropInterface( (struct Interface *)IClickTab );
//		IIntuition->CloseClass( (struct ClassLibrary *)ClickTabBase );
		IExec->DropInterface( (struct Interface *)IListBrowser );
		IIntuition->CloseClass( (struct ClassLibrary *)ListBrowserBase );
	}

	if(ILocale)
	{
		ILocale->CloseCatalog(li.li_Catalog);
		IExec->DropInterface( (struct Interface *)ILocale );
	}
	IExec->CloseLibrary( (struct Library *)LocaleBase );

	IExec->DropInterface( (struct Interface *)IIcon );
	IExec->CloseLibrary(IconBase);
	IExec->DropInterface( (struct Interface *)IGraphics );
	IExec->CloseLibrary(GfxBase);
	IExec->DropInterface( (struct Interface *)IIntuition );
	IExec->CloseLibrary(IntuitionBase);

	IExec->DropInterface( (struct Interface *)IUtility );
	IExec->CloseLibrary(UtilityBase);

	IExec->DropInterface( (struct Interface *)IDOS );
	IExec->CloseLibrary(DOSBase);
}

/* Get screen at front, used by DoMessage()/ObtainColors()/ReleasePens() */
struct Screen *FrontMostScr(void)
{
	ULONG intuition_lock;
	struct Screen *front_screen_address, *public_screen_address = NULL;
	struct List *public_screen_list;
	struct PubScreenNode *public_screen_node;

	intuition_lock = IIntuition->LockIBase(0L);

	front_screen_address = ((struct IntuitionBase *)IntuitionBase)->FirstScreen;
	if( (front_screen_address->Flags & PUBLICSCREEN) || (front_screen_address->Flags & WBENCHSCREEN) ) {
		IIntuition->UnlockIBase(intuition_lock);

		public_screen_list = IIntuition->LockPubScreenList();
		public_screen_node = (struct PubScreenNode *)public_screen_list->lh_Head;
		while(public_screen_node) {
			if(public_screen_node->psn_Screen == front_screen_address) {
				public_screen_address = public_screen_node->psn_Screen;
				break;
			}

			public_screen_node = (struct PubScreenNode *)public_screen_node->psn_Node.ln_Succ;
		}

		IIntuition->UnlockPubScreenList();
	}
	else {
		IIntuition->UnlockIBase(intuition_lock);
	}

	if(!public_screen_address) {
		public_screen_address = IIntuition->LockPubScreen(NULL);
		IIntuition->UnlockPubScreen(NULL, public_screen_address);
	}

//DBUG("%lx\n", (int)public_screen_address);
	return public_screen_address;
}

/* Show Errors/Warnings/... */
uint32 DoMessage(char *message, char reqtype, STRPTR buttons)
{
	uint32 button;
	Object *requester;

	if(IIntuition == NULL) { IDOS->PutErrStr(message); return 0; }

	requester = IIntuition->NewObject(RequesterClass, NULL, //"requester.class",
	                                 REQ_Image,      reqtype,
	                                 REQ_TitleText,  "dgenGUI",
	                                 REQ_BodyText,   message,
	                                 REQ_GadgetText, buttons? buttons : GetString(&li,MSG_OK_GAD),
	                                 //REQ_StayOnTop, TRUE,
	                                TAG_DONE);

	button = IIntuition->IDoMethod( requester, RM_OPENREQ, NULL, NULL, FrontMostScr() );
	IIntuition->DisposeObject(requester);

	return button;
}

int32 GetRoms(STRPTR romsdir, struct List *list)
{
	APTR context;
	int32 resul = 0;
	char rom[MAX_DOS_FILENAME], ext[4] = ""; // extension + '\0'
	struct Node *n;
	STRPTR pattern_ms = IExec->AllocVecTags(256, TAG_END),
	       romfullpath = IExec->AllocVecTags(MAX_FULLFILEPATH, AVT_ClearWithValue,0, TAG_END);
DBUG("GetRoms() '%s'\n",romsdir);
	IDOS->ParsePatternNoCase("#?.(zip|smd|bin|md)", pattern_ms, 64);
	IUtility->Strlcpy(romfullpath, ROMS, MAX_FULLFILEPATH);
	IDOS->AddPart(romfullpath, romsdir, MAX_FULLFILEPATH);
	context = IDOS->ObtainDirContextTags(EX_StringNameInput, romfullpath,
	                                     EX_DataFields, (EXF_NAME|EXF_TYPE),
	                                     EX_MatchString, pattern_ms,
	                                    TAG_END);
	if(context)
	{
		struct ExamineData *dat;
		int32 len, newpos;

		// Re-read theme drawer, but first "clean" listbrowser list
		IListBrowser->FreeListBrowserList(list);

		while( (dat=IDOS->ExamineDir(context)) )
		{
			if( EXD_IS_FILE(dat) )
			{
				len = IUtility->Strlen(dat->Name) - sizeof(ext);
				newpos = IDOS->SplitName(dat->Name, '.', rom, len, MAX_DOS_FILENAME);
DBUG("  rom = '%s'\n",dat->Name);
				// Get extension
				ext[0] = IUtility->ToUpper( *(dat->Name + newpos) );
				ext[1] = IUtility->ToUpper( *(dat->Name + newpos + 1) );
				ext[2] = IUtility->ToUpper( *(dat->Name + newpos + 2) );
DBUG("    extension = '%s'\n",ext);
				IUtility->Strlcpy(rom, dat->Name, newpos);
DBUG("    name = '%s'\n",rom);
				n = IListBrowser->AllocListBrowserNode(LAST_COL,
				                                       LBNA_Column,COL_ROM,
				                                        //LBNA_Flags,LBFLG_CUSTOMPENS, LBNCA_FGPen,PenArray[TXT_R],
				                                        //LBNA_UserData,,
				                                         LBNCA_CopyText,TRUE, LBNCA_Text,rom,
				                                       LBNA_Column,COL_FMT,
				                                        //LBNA_Flags,LBFLG_CUSTOMPENS, LBNCA_FGPen,PenArray[TXT_R],
				                                         LBNCA_CopyText,TRUE, LBNCA_Text,ext,
				                                         LBNCA_HorizJustify, LCJ_CENTER,
				                                      TAG_DONE);
				IExec->AddTail(list, n);
				resul++;
DBUG("  %4ld:'%s' [0x%08lx] added.\n",resul,dat->Name,n);
			}
		}

		if(IDOS->IoErr() != ERROR_NO_MORE_ENTRIES)
		{
			IDOS->Fault(IDOS->IoErr(), NULL, rom, 1024);
			DoMessage(rom, REQIMAGE_ERROR, NULL);
		}
	} // END if(context)
	else
	{
		IDOS->Fault(IDOS->IoErr(), NULL, pattern_ms, 1024); // 'pattern_ms' used as temp-buffer
		IUtility->SNPrintf(rom, 1024, GetString(&li,MSG_ERROR_ROMDRAWER),pattern_ms);
		DoMessage(rom, REQIMAGE_ERROR, NULL);
	}

	IDOS->ReleaseDirContext(context);
	IExec->FreeVec(romfullpath);
	IExec->FreeVec(pattern_ms);
DBUG("GetRoms() = %ld\n",resul);
	return resul;
}

BOOL updateButtonImage(STRPTR fn, CONST_STRPTR fb_str, uint32 OID_btn, struct Window *pw)
{
	Object *newobj = NULL;
	struct ExamineData *datF;
	uint32 OID_img = OID_btn + 1;
DBUG("updateButtonImage() '%s'\n",fn);
	// Check if filename exists
	datF = IDOS->ExamineObjectTags(EX_StringNameInput,fn, TAG_END);
	if(datF==NULL  &&  fb_str==NULL) { return FALSE; }

	if(datF) {
DBUG("  ExamineObjectTags(): '%s' %s%lld bytes\n",datF->Name,"",datF->FileSize); // FileSize is int64
		IDOS->FreeDosObject(DOS_EXAMINEDATA, datF);
		// Create and set button/image
		newobj = IIntuition->NewObject(BitMapClass, NULL, //"bitmap.image",
		                               IA_Scalable, TRUE,
		                               IA_Width,256, IA_Height,224,
		                               //BITMAP_Masking, TRUE,
		                               BITMAP_Screen,           pw->WScreen,
		                               BITMAP_SourceFile,       fn,
		                               BITMAP_SelectSourceFile, PREVIEWS"/_launchROM.png",
		                              TAG_DONE);
	}
DBUG("  new image: 0x%08lx\n",newobj);
	if(newobj) {
		IIntuition->SetGadgetAttrs(GAD(OID_btn), pw, NULL, BUTTON_RenderImage,newobj, TAG_DONE);
	}
	else
	{// No image found -> show fallback string (fb_str)
		IIntuition->SetGadgetAttrs(GAD(OID_btn), pw, NULL, GA_Text,fb_str, TAG_DONE);
	}

DBUG("  old image: 0x%08lx (disposing)\n",OBJ(OID_img));
	IIntuition->DisposeObject( OBJ(OID_img) );
	OBJ(OID_img) = newobj;
	IIntuition->RefreshGadgets(GAD(OID_btn), pw, NULL);

	return TRUE;
}

int32 SaveToolType(STRPTR iconname, STRPTR ttpName, STRPTR ttpArg)
{
	int32 i = 0;
	//char newttp[1024], ttpBuf1[256], ttpBuf2[256];
	STRPTR newttp = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,0, TAG_END),
	       ttpBuf1 = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,0, TAG_END),
	       ttpBuf2 = IExec->AllocVecTags(MAX_DOS_PATH, AVT_ClearWithValue,0, TAG_END);
	struct DiskObject *micon = NULL;
DBUG("SaveToolType() - START\n",NULL);
	if(ttpArg)
	{
		IUtility->SNPrintf(newttp, MAX_DOS_PATH, "%s=",ttpName); // "<tooltype>="
		IUtility->SNPrintf(ttpBuf2, MAX_DOS_PATH, "(%s=",ttpName); // BUF2 = "(<tooltype>="
	}
	else
	{
		IUtility->Strlcpy(newttp, ttpName, MAX_DOS_PATH); // "<tooltype>"
		IUtility->SNPrintf(ttpBuf2, MAX_DOS_PATH, "(%s)",ttpName); // BUF2 = "(<tooltype>)"
	}
	IUtility->Strlcpy(ttpBuf1, newttp, MAX_DOS_PATH); // BUF1 = NEWTTP

	micon = IIcon->GetDiskObject(iconname);
	if(micon)
	{
		STRPTR *oldttp = NULL;
DBUG("  micon = 0x%08lx (do_ToolTypes=0x%08lx)\n",micon,micon->do_ToolTypes);
		if(micon->do_ToolTypes) // icon with tooltypes..
		{
			for(; micon->do_ToolTypes[i]!=NULL; i++) //..parse tooltypes
			{
DBUG("  %2ld'%s'\n",i,micon->do_ToolTypes[i]);
				if( !IUtility->Strnicmp(micon->do_ToolTypes[i],ttpBuf1,IUtility->Strlen(ttpBuf1))
				   || !IUtility->Strnicmp(micon->do_ToolTypes[i],ttpBuf2,IUtility->Strlen(ttpBuf2)) )
				{// Found tooltype
					IUtility->Strlcat(newttp, ttpArg, MAX_DOS_PATH);
//DBUG("  '%s' == '%s'?\n",micon->do_ToolTypes[i],newttp);
				// Normal case tooltype -> tooltype=value OR (tooltype=value)
					if( ttpArg  &&  IUtility->Stricmp(micon->do_ToolTypes[i],newttp) )
					{// Tooltype loaded diffs. from icon's tooltype -> modify tooltype
						oldttp = micon->do_ToolTypes;
						micon->do_ToolTypes[i] = newttp;
						IIcon->PutDiskObject(iconname, micon);
						micon->do_ToolTypes = oldttp;
DBUG("  Tooltype modified: '%s'\n",newttp);
					}
/*
					// Special case tooltype (toggle/switch) -> 'tooltype' OR '(tooltype)'
					// NOTE: if it doesn't exists it will be created as "enabled" -> 'tooltype'
					if(!ttpArg)
					{
						oldttp = micon->do_ToolTypes;
						if(IUtility->Stricmp(micon->do_ToolTypes[i],ttpBuf1) == 0) {
							IUtility->SNPrintf(ttpBuf1, MAX_DOS_PATH, "(%s)",ttpName); // BUF1 = "(<tooltype>)"
						}
						else {
							IUtility->Strlcpy(ttpBuf1, newttp, MAX_DOS_PATH); // BUF1 = "<tooltype>"
						}

						micon->do_ToolTypes[i] = ttpBuf1;
						IIcon->PutDiskObject(iconname, micon);
						micon->do_ToolTypes = oldttp;
DBUG("  Tooltype toggled: '%s'\n",ttpBuf1);
					}
*/
					break;
				} // END if( !Strnicmp(micon->..
			} // END for
		} // END if(micon->do_ToolTypes)

		if(!micon->do_ToolTypes  ||  !micon->do_ToolTypes[i]) // No tooltypes or doesn't exists..
		{//..create tooltype entry and END
			STRPTR *newtts = IExec->AllocVecTags( (i+2)*sizeof(STRPTR), TAG_END );
//DBUG("  Tooltype '%s' NOT found (i=%ld)\n",newttp,i);
			IUtility->Strlcat(newttp, ttpArg, MAX_DOS_PATH);
			oldttp = micon->do_ToolTypes;
			IExec->CopyMem( oldttp, newtts, i*sizeof(STRPTR) ); // clone tooltypes
			newtts[i] = newttp;
			newtts[i+1] = NULL;
			micon->do_ToolTypes = newtts;
			IIcon->PutDiskObject(iconname, micon);
			micon->do_ToolTypes = oldttp;
			IExec->FreeVec(newtts);
DBUG("  Tooltype created: '%s'\n",newttp);
			i = -1;
		}

		IIcon->FreeDiskObject(micon);
	}
DBUG("SaveToolType() - END\n",NULL);
	IExec->FreeVec(newttp);
	IExec->FreeVec(ttpBuf1);
	IExec->FreeVec(ttpBuf2);

	return i;
}

/*void createDrawer(STRPTR fullpath)
{
	BPTR lock;
	struct ExamineData *data;
DBUG("createDrawer() '%s'\n",fullpath);
	if( (data=IDOS->ExamineObjectTags(EX_StringNameInput,fullpath, TAG_END)) )
	{
		//if( EXD_IS_FILE(data) ) {}
		//if( EXD_IS_DIRECTORY(data) ) {}
		IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		return;
	}
DBUG("  creating '%s'\n",fullpath);
	lock = IDOS->CreateDir(fullpath);
	IDOS->UnLock(lock);
}*/

/*void free_chooserlist_nodes(struct List *list)
{
	struct Node *nextnode, *node = IExec->GetHead(list);
DBUG("free_chooserlist_nodes()\n",NULL);
	while(node)
	{
//DBUG(" node = 0x%08lx\n",node);
		nextnode = IExec->GetSucc(node);
		IExec->Remove(node);
		IChooser->FreeChooserNode(node);
		node = nextnode;
	}
}*/
void free_chooserlist_nodes(struct List *list)
{
	struct Node *node;

	while( (node=IExec->RemTail(list)) ) {
		IChooser->FreeChooserNode(node);
	}
}

void GetSavestates(struct DGenGUI *dgg, STRPTR fn)
{
	APTR context;
	struct Node *node;
	STRPTR filestate = IExec->AllocVecTags(MAX_DOS_FILENAME, TAG_END),
	       pattern_ms = IExec->AllocVecTags(2+MAX_DOS_FILENAME*2, TAG_END);
DBUG("GetSavestates()\n",NULL);
	IUtility->Strlcpy(filestate, fn, MAX_DOS_FILENAME);
	IUtility->Strlcat(filestate, ".gs[0-9]", MAX_DOS_FILENAME);
DBUG("  searching '%s':\n",filestate);

	// Detach chooser list
	IIntuition->SetAttrs(OBJ(OID_SAVESTATES), CHOOSER_Labels,0, TAG_DONE);
	// Remove previous rom savestates chooser list..
	free_chooserlist_nodes(dgg->savestates_list);
	//..and generate new
	node = IChooser->AllocChooserNode(CNA_Text,GetString(&li,MSG_GUI_SAVESTATES_NO), TAG_DONE);
	IExec->AddTail(dgg->savestates_list, node);

	// Search selected/active rom savestates
	IDOS->ParsePatternNoCase(filestate, pattern_ms, 2+MAX_DOS_FILENAME*2);
	context = IDOS->ObtainDirContextTags(EX_StringNameInput, SAVES,
	                                     EX_DataFields, (EXF_NAME|EXF_TYPE),
	                                     EX_MatchString, pattern_ms,
	                                    TAG_END);
	if(context) {
		struct ExamineData *dat;

		while( (dat=IDOS->ExamineDir(context)) )
		{
			if( EXD_IS_FILE(dat) )
			{
				char slot[2] = "";
DBUG("    '%s'\n",dat->Name);
				slot[0] = dat->Name[IUtility->Strlen(dat->Name)-1];
DBUG("    slot #%s\n",slot); // only need the slot number (aka last char)
				node = IChooser->AllocChooserNode(CNA_CopyText,TRUE, CNA_Text,slot, TAG_DONE);
				if(node) { IExec->AddTail(dgg->savestates_list, node); }
			}
		}
		//if(IDOS->IoErr() == ERROR_NO_MORE_ENTRIES) {}
	}

	// Re-attach chooser list
	IIntuition->SetAttrs(OBJ(OID_SAVESTATES), CHOOSER_Labels,dgg->savestates_list, GA_Disabled,FALSE, TAG_DONE);
	IIntuition->RefreshGadgets(GAD(OID_SAVESTATES), dgg->win, NULL);

	IDOS->ReleaseDirContext(context);
	IExec->FreeVec(pattern_ms);
	IExec->FreeVec(filestate);
}

void ShowPreview(struct DGenGUI *dgg)
{
	uint32 res_n;
	STRPTR res_s, filename;

	IIntuition->GetAttr(LISTBROWSER_SelectedNode, OBJ(OID_LISTBROWSER), (uint32 *)&res_n);
	if(res_n == 0) { return; }

	filename = IExec->AllocVecTags(FILENAME_LENGTH, TAG_END);

	IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_n, LBNA_Column,COL_ROM, LBNCA_Text,&res_s, TAG_DONE );
DBUG("res_n=0x%08lx -> res_s='%s'\n",res_n,res_s);

	GetSavestates(dgg, res_s);

	IUtility->Strlcpy(filename, PREVIEWS, FILENAME_LENGTH);
	IDOS->AddPart(filename, res_s, FILENAME_LENGTH);      // add ROM filename
	IUtility->Strlcat(filename, ".png", FILENAME_LENGTH); // add extension
DBUG("PREVIEW: '%s'\n",filename);
	if(updateButtonImage(filename, NULL, OID_PREVIEW_BTN, dgg->win) == FALSE) // .PNG (previews drawer)
	{
		IUtility->Strlcpy(filename, SCRSHOTS, FILENAME_LENGTH);
		IDOS->AddPart(filename, res_s, FILENAME_LENGTH);      // add ROM filename
		IUtility->Strlcat(filename, ".png", FILENAME_LENGTH); // add extension

		if(updateButtonImage(filename, NULL, OID_PREVIEW_BTN, dgg->win) == FALSE) // .TGA (screenshoots drawer)
		{
			updateButtonImage(PREVIEWS"/not_available.png", GetString(&li,MSG_ERROR_GUI_NOPREVIEW), OID_PREVIEW_BTN, dgg->win); // fallback img/txt
		}
	}

	// Reset default options group
	//IIntuition->SetAttrs(OBJ(OID_GAME_OPTIONS),CHOOSER_Selected,0, TAG_DONE);
	//IIntuition->RefreshGadgets(GAD(OID_OPTIONS_GROUP), dgg->win, NULL);
	IIntuition->RefreshSetGadgetAttrs(GAD(OID_GAME_OPTIONS), dgg->win, NULL, CHOOSER_Selected,0, TAG_DONE);

	IExec->FreeVec(filename);
}

void LaunchRom(struct DGenGUI *dgg)
{
	uint32 res_n;
	STRPTR res_s = NULL, res_e = NULL, filename;
	char rom_ext[5] = ".";
DBUG("LaunchRom()\n",NULL);
	IIntuition->GetAttr(LISTBROWSER_SelectedNode, OBJ(OID_LISTBROWSER), &res_n);
	if(res_n == 0) { return; }

	filename = IExec->AllocVecTags(FILENAME_LENGTH, TAG_END);

	IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_n, LBNA_Column,COL_ROM, LBNCA_Text,&res_s, TAG_DONE );
	IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_n, LBNA_Column,COL_FMT, LBNCA_Text,&res_e, TAG_DONE );
DBUG("  res_n=0x%08lx -> res_s='%s' (res_e='%s')\n",res_n,res_s,res_e);
	IUtility->Strlcpy(filename, ROMS, FILENAME_LENGTH);
	IUtility->Strlcat( rom_ext, res_e, sizeof(rom_ext) );

	IDOS->AddPart(filename, ROMS, FILENAME_LENGTH);
	IDOS->AddPart(filename, res_s, FILENAME_LENGTH);       // add ROM filename
	//IUtility->Strlcat(filename, rom_ext, FILENAME_LENGTH); // add extension
DBUG("  Starting ROM '%s%s'...\n",filename,rom_ext);
	updateButtonImage(NULL, "", OID_PREVIEW_BTN, dgg->win); // "unlock" preview image/button
DBUG("  WM_CLOSE (win=0x%08lx)\n",dgg->win);
	IIntuition->IDoMethod(OBJ(OID_MAIN), WM_CLOSE);
	dgg->win = NULL;
	// LAUNCH ROM
	if(beginCommand(filename,rom_ext) != 0) {
		DoMessage( (STRPTR)GetString(&li,MSG_ERROR_LAUNCHING), REQIMAGE_ERROR, NULL ); //"Error launching 'snes9x-sdl'!"
	}

	if( (dgg->win=(struct Window *)IIntuition->IDoMethod(OBJ(OID_MAIN), WM_OPEN, NULL)) )
	{
DBUG("  WM_OPEN (win=0x%08lx)\n",dgg->win);
		dgg->screen = dgg->win->WScreen;
		//IIntuition->SetAttrs(OBJ(OID_MAIN), WA_PubScreen,dgg->screen, TAG_DONE);
		ShowPreview(dgg); // "reload" preview (maybe user created a snapshoot)
		IIntuition->ScreenToFront(dgg->screen);
//DBUG("  dgg->wbs = 0x%08lx\n",dgg->wbs);
		if(dgg->wbs)
		{
			IIntuition->GetAttrs(OBJ(OID_LISTBROWSER), LISTBROWSER_Selected,&dgg->myTT.last_rom_run, TAG_DONE);
			IUtility->SNPrintf(rom_ext, sizeof(rom_ext), "%ld",dgg->myTT.last_rom_run); // using 'rom_ext' as temp buffer
			SaveToolType(dgg->wbs->sm_ArgList->wa_Name, "LAST_ROM_LAUNCHED", rom_ext);
		}
	}

	IExec->FreeVec(filename);
}

/************************************************************/
/* This part of code is from Andy Broad's dockapps examples */
STRPTR DupStr(CONST_STRPTR str, int32 length)
{
	STRPTR dup;
	uint32 str_len = 0;

	if(!str) return NULL;

	str_len = IUtility->Strlen(str);
	if(length < 0)
	{
		length = str_len;
	}

	if( (dup=IExec->AllocVecTags(length+1, AVT_ClearWithValue,0, TAG_END)) )
	{
		uint32 copy_len = str_len;

		if(length < str_len)
		{
			copy_len = length;
		}
		IUtility->MoveMem(str, dup, copy_len);
		dup[length] = '\0';
	}

	return dup;
}

VOID FreeString(STRPTR *string)
{
	if(string)
	{
		if(*string)
		{
			IExec->FreeVec( (APTR)*string );
		}
		string = NULL;
	}
}
/* This part of code is from Andy Broad's dockapps examples */
/************************************************************/
