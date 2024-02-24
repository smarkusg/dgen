#ifndef INCLUDE_H
#define INCLUDE_H


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/listbrowser.h>
#include <proto/clicktab.h>
#include <proto/chooser.h>
#include <proto/layout.h>
#include <proto/graphics.h>
#include <proto/icon.h>
//#include <proto/locale.h>

#include <workbench/icon.h>
#include <workbench/startup.h>
#include <libraries/keymap.h> // RAWKEY_#? codes
#include <classes/window.h>
#include <classes/requester.h>
#include <gadgets/clicktab.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <gadgets/checkbox.h>
#include <gadgets/chooser.h>
#include <gadgets/string.h>
#include <gadgets/space.h>
#include <gadgets/integer.h>
#include <gadgets/getfile.h>
#include <images/label.h>
#include <images/bitmap.h>

#include "dgengui_rev.h"
#include "dgengui_strings.h"


#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]

#define ROMS      "PROGDIR:ROMS"
#define PREVIEWS  "PROGDIR:PREVIEWS"
#define SCRSHOTS  "PROGDIR:DgenConf/screenshots"
#define SAVES     "PROGDIR:DgenConf/saves"


enum {
 COL_ROM = 0,
 COL_FMT, // ZIP BIN SMC ... (extensions)
 LAST_COL
};

enum {
 OID_MAIN = 0,
 OID_BANNER_IMG,
 OID_ROMDRAWER,
 OID_DGENEXE,
 OID_LISTBROWSER,
 OID_PREVIEW_BTN,
 OID_PREVIEW_IMG, // MUST BE after OID_PREVIEW_BTN [updateButtonImage()]
 OID_TOTALROMS,
 OID_SAVESTATES,
 // Buttons
 OID_ABOUT,
 OID_SAVE,
 OID_QUIT,
 LAST_NUM
};


// Listbrowser's pens
/*enum {
 ROW_O = 0,
 ROW_E,
 TXT_R,
 LAST_PEN
};
#define RGB8to32(RGB) ( (uint32)(RGB) * 0x01010101UL )*/


struct myToolTypes {
	STRPTR romsdrawer;  // ROMS_DRAWER=<path>
	int32 last_rom_run; // LAST_ROM_LAUNCHED=<value>

	//STRPTR newttp, ttpBuf1, ttpBuf2; // only needed if using SaveToolType()
};

struct DGenGUI {
	struct Screen *screen; // PUSBCREEN=<screen_name>
	struct DiskObject *iconify;
	struct List *romlist;
	struct Window *win;
	struct myToolTypes myTT;
	//STRPTR dgen_path;
	struct WBStartup *wbs;
	struct List *savestates_list;
};


#endif
