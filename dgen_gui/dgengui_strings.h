#ifndef DGENGUI_STRINGS_H
#define DGENGUI_STRINGS_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_CODE
#ifndef CATCOMP_BLOCK
#define CATCOMP_ARRAY
#endif
#endif

#ifdef CATCOMP_ARRAY
#ifndef CATCOMP_NUMBERS
#define CATCOMP_NUMBERS
#endif
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif

#ifdef CATCOMP_BLOCK
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_ERROR_NOROMS 1
#define MSG_ERROR_REQUIRED 2
#define MSG_ERROR_ROMDRAWER 3
#define MSG_ERROR_LAUNCHING 4
#define MSG_ERROR_GUI_NOPREVIEW 5
#define MSG_OK_GAD 50
#define MSG_GUI_ABOUT_TEXT 51
#define MSG_GUI_TITLE_COL_ROM 100
#define MSG_GUI_TITLE_COL_FMT 101
#define MSG_GUI_ROMDRAWER 102
#define MSG_GUI_ROMDRAWER_TITLE 103
#define MSG_GUI_SAVESTATES 104
#define MSG_GUI_SAVESTATES_NO 105
#define MSG_GUI_TOTALROMS 106
#define MSG_GUI_GAME_RUN 107
#define MSG_GUI_GAME_REC 108
#define MSG_GUI_GAME_PLAY_REC 109
#define MSG_GUI_ABOUT_BTN 110
#define MSG_GUI_SAVE_BTN 111
#define MSG_GUI_QUIT_BTN 112
#define MSG_GUI_ROMDRAWER_HELP 500
#define MSG_GUI_DGENEXEX_HELP 501
#define MSG_GUI_LISTBROWSER_HELP 502
#define OID_PREVIEW_BTN_HELP 503
#define MSG_GUI_GAME_OPTIONS_HELP 504
#define MSG_GUI_SAVESTATES_HELP 505
#define MSG_GUI_ABOUT_BTN_HELP 506
#define MSG_GUI_SAVE_BTN_HELP 507
#define MSG_GUI_QUIT_BTN_HELP 508

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_ERROR_NOROMS_STR "No roms found."
#define MSG_ERROR_REQUIRED_STR "Couldn't find required library/classes."
#define MSG_ERROR_ROMDRAWER_STR "\033cFailed to load ROM drawer:\n%s."
#define MSG_ERROR_LAUNCHING_STR "Error launching dgen executable!"
#define MSG_ERROR_GUI_NOPREVIEW_STR "Preview not available"
#define MSG_OK_GAD_STR "_OK"
#define MSG_GUI_ABOUT_TEXT_STR "\033c\033b%s %s\033n\n\nGUI for DGen, OS4 port by Marek Glogowski\n\nAcknowlegdments: kas1e, samo79,\nsmarkusg and everyone I forget.\n\n(using built-in strings)"
#define MSG_GUI_TITLE_COL_ROM_STR "Game"
#define MSG_GUI_TITLE_COL_FMT_STR "Format"
#define MSG_GUI_ROMDRAWER_STR " ROMS:"
#define MSG_GUI_ROMDRAWER_TITLE_STR "Select ROMs drawer"
#define MSG_GUI_SAVESTATES_STR " Load saved slot"
#define MSG_GUI_SAVESTATES_NO_STR "No"
#define MSG_GUI_TOTALROMS_STR " %ld roms availables"
#define MSG_GUI_GAME_RUN_STR "Play selected game"
#define MSG_GUI_GAME_REC_STR "Play & Record selected game"
#define MSG_GUI_GAME_PLAY_REC_STR "Playback recorded game"
#define MSG_GUI_ABOUT_BTN_STR "About..."
#define MSG_GUI_SAVE_BTN_STR "Save settings"
#define MSG_GUI_QUIT_BTN_STR "Quit"
#define MSG_GUI_ROMDRAWER_HELP_STR "Select drawer with Genesis/MD ROMs."
#define MSG_GUI_DGENEXEX_HELP_STR "Choose between SDL1 or SDL2 (uses\nSDL1.2compat) versions of executable."
#define MSG_GUI_LISTBROWSER_HELP_STR "List of Genesis/MD ROMs.\nDouble click is like clicking\non image/button."
#define OID_PREVIEW_BTN_HELP_STR "Click to \"launch\" ROM.\nDepends on below option (play,\nplay&rec. or playback)."
#define MSG_GUI_GAME_OPTIONS_HELP_STR "\033bPlay\033n: loads rom for playing.\n\033bPlay&Record\033n: record a demo of\n the game you are playing.\n\033bPlayback\033n: play back a previously\n recorded demo."
#define MSG_GUI_SAVESTATES_HELP_STR "List availables saved \"slots\"."
#define MSG_GUI_ABOUT_BTN_HELP_STR "Show about DGenGUI"
#define MSG_GUI_SAVE_BTN_HELP_STR "Save tooltypes:\n\033bROMS_DRAWER\033n"
#define MSG_GUI_QUIT_BTN_HELP_STR "Quit DGenGUI"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG         cca_ID;
    CONST_STRPTR cca_Str;
};

STATIC CONST struct CatCompArrayType CatCompArray[] =
{
    {MSG_ERROR_NOROMS,(CONST_STRPTR)MSG_ERROR_NOROMS_STR},
    {MSG_ERROR_REQUIRED,(CONST_STRPTR)MSG_ERROR_REQUIRED_STR},
    {MSG_ERROR_ROMDRAWER,(CONST_STRPTR)MSG_ERROR_ROMDRAWER_STR},
    {MSG_ERROR_LAUNCHING,(CONST_STRPTR)MSG_ERROR_LAUNCHING_STR},
    {MSG_ERROR_GUI_NOPREVIEW,(CONST_STRPTR)MSG_ERROR_GUI_NOPREVIEW_STR},
    {MSG_OK_GAD,(CONST_STRPTR)MSG_OK_GAD_STR},
    {MSG_GUI_ABOUT_TEXT,(CONST_STRPTR)MSG_GUI_ABOUT_TEXT_STR},
    {MSG_GUI_TITLE_COL_ROM,(CONST_STRPTR)MSG_GUI_TITLE_COL_ROM_STR},
    {MSG_GUI_TITLE_COL_FMT,(CONST_STRPTR)MSG_GUI_TITLE_COL_FMT_STR},
    {MSG_GUI_ROMDRAWER,(CONST_STRPTR)MSG_GUI_ROMDRAWER_STR},
    {MSG_GUI_ROMDRAWER_TITLE,(CONST_STRPTR)MSG_GUI_ROMDRAWER_TITLE_STR},
    {MSG_GUI_SAVESTATES,(CONST_STRPTR)MSG_GUI_SAVESTATES_STR},
    {MSG_GUI_SAVESTATES_NO,(CONST_STRPTR)MSG_GUI_SAVESTATES_NO_STR},
    {MSG_GUI_TOTALROMS,(CONST_STRPTR)MSG_GUI_TOTALROMS_STR},
    {MSG_GUI_GAME_RUN,(CONST_STRPTR)MSG_GUI_GAME_RUN_STR},
    {MSG_GUI_GAME_REC,(CONST_STRPTR)MSG_GUI_GAME_REC_STR},
    {MSG_GUI_GAME_PLAY_REC,(CONST_STRPTR)MSG_GUI_GAME_PLAY_REC_STR},
    {MSG_GUI_ABOUT_BTN,(CONST_STRPTR)MSG_GUI_ABOUT_BTN_STR},
    {MSG_GUI_SAVE_BTN,(CONST_STRPTR)MSG_GUI_SAVE_BTN_STR},
    {MSG_GUI_QUIT_BTN,(CONST_STRPTR)MSG_GUI_QUIT_BTN_STR},
    {MSG_GUI_ROMDRAWER_HELP,(CONST_STRPTR)MSG_GUI_ROMDRAWER_HELP_STR},
    {MSG_GUI_DGENEXEX_HELP,(CONST_STRPTR)MSG_GUI_DGENEXEX_HELP_STR},
    {MSG_GUI_LISTBROWSER_HELP,(CONST_STRPTR)MSG_GUI_LISTBROWSER_HELP_STR},
    {OID_PREVIEW_BTN_HELP,(CONST_STRPTR)OID_PREVIEW_BTN_HELP_STR},
    {MSG_GUI_GAME_OPTIONS_HELP,(CONST_STRPTR)MSG_GUI_GAME_OPTIONS_HELP_STR},
    {MSG_GUI_SAVESTATES_HELP,(CONST_STRPTR)MSG_GUI_SAVESTATES_HELP_STR},
    {MSG_GUI_ABOUT_BTN_HELP,(CONST_STRPTR)MSG_GUI_ABOUT_BTN_HELP_STR},
    {MSG_GUI_SAVE_BTN_HELP,(CONST_STRPTR)MSG_GUI_SAVE_BTN_HELP_STR},
    {MSG_GUI_QUIT_BTN_HELP,(CONST_STRPTR)MSG_GUI_QUIT_BTN_HELP_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

STATIC CONST UBYTE CatCompBlock[] =
{
    "\x00\x00\x00\x01\x00\x10"
    MSG_ERROR_NOROMS_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x28"
    MSG_ERROR_REQUIRED_STR "\x00"
    "\x00\x00\x00\x03\x00\x22"
    MSG_ERROR_ROMDRAWER_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x22"
    MSG_ERROR_LAUNCHING_STR "\x00\x00"
    "\x00\x00\x00\x05\x00\x16"
    MSG_ERROR_GUI_NOPREVIEW_STR "\x00"
    "\x00\x00\x00\x32\x00\x04"
    MSG_OK_GAD_STR "\x00"
    "\x00\x00\x00\x33\x00\x92"
    MSG_GUI_ABOUT_TEXT_STR "\x00"
    "\x00\x00\x00\x64\x00\x06"
    MSG_GUI_TITLE_COL_ROM_STR "\x00\x00"
    "\x00\x00\x00\x65\x00\x08"
    MSG_GUI_TITLE_COL_FMT_STR "\x00\x00"
    "\x00\x00\x00\x66\x00\x08"
    MSG_GUI_ROMDRAWER_STR "\x00\x00"
    "\x00\x00\x00\x67\x00\x14"
    MSG_GUI_ROMDRAWER_TITLE_STR "\x00\x00"
    "\x00\x00\x00\x68\x00\x12"
    MSG_GUI_SAVESTATES_STR "\x00\x00"
    "\x00\x00\x00\x69\x00\x04"
    MSG_GUI_SAVESTATES_NO_STR "\x00\x00"
    "\x00\x00\x00\x6A\x00\x16"
    MSG_GUI_TOTALROMS_STR "\x00\x00"
    "\x00\x00\x00\x6B\x00\x14"
    MSG_GUI_GAME_RUN_STR "\x00\x00"
    "\x00\x00\x00\x6C\x00\x1C"
    MSG_GUI_GAME_REC_STR "\x00"
    "\x00\x00\x00\x6D\x00\x18"
    MSG_GUI_GAME_PLAY_REC_STR "\x00\x00"
    "\x00\x00\x00\x6E\x00\x0A"
    MSG_GUI_ABOUT_BTN_STR "\x00\x00"
    "\x00\x00\x00\x6F\x00\x0E"
    MSG_GUI_SAVE_BTN_STR "\x00"
    "\x00\x00\x00\x70\x00\x06"
    MSG_GUI_QUIT_BTN_STR "\x00\x00"
    "\x00\x00\x01\xF4\x00\x24"
    MSG_GUI_ROMDRAWER_HELP_STR "\x00"
    "\x00\x00\x01\xF5\x00\x48"
    MSG_GUI_DGENEXEX_HELP_STR "\x00"
    "\x00\x00\x01\xF6\x00\x48"
    MSG_GUI_LISTBROWSER_HELP_STR "\x00"
    "\x00\x00\x01\xF7\x00\x4E"
    OID_PREVIEW_BTN_HELP_STR "\x00"
    "\x00\x00\x01\xF8\x00\x94"
    MSG_GUI_GAME_OPTIONS_HELP_STR "\x00\x00"
    "\x00\x00\x01\xF9\x00\x20"
    MSG_GUI_SAVESTATES_HELP_STR "\x00\x00"
    "\x00\x00\x01\xFA\x00\x14"
    MSG_GUI_ABOUT_BTN_HELP_STR "\x00\x00"
    "\x00\x00\x01\xFB\x00\x20"
    MSG_GUI_SAVE_BTN_HELP_STR "\x00"
    "\x00\x00\x01\xFC\x00\x0E"
    MSG_GUI_QUIT_BTN_HELP_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


#ifndef PROTO_LOCALE_H
 #ifndef __NOLIBBASE__
  #define _NLB_DEFINED_
  #define __NOLIBBASE__
 #endif
 #ifndef __NOGLOBALIFACE__
  #define _NGI_DEFINED_
  #define __NOGLOBALIFACE__
 #endif
 #include <proto/locale.h>
 #ifdef _NLB_DEFINED_
  #undef __NOLIBBASE__
  #undef _NLB_DEFINED_
 #endif
 #ifdef _NGI_DEFINED_
  #undef __NOGLOBALIFACE__
  #undef _NGI_DEFINED_
 #endif
#endif

struct LocaleInfo
{
#ifndef __amigaos4__
    struct Library     *li_LocaleBase;
#else
    struct LocaleIFace *li_ILocale;
#endif
    struct Catalog     *li_Catalog;
};


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef CATCOMP_CODE

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
#ifndef __amigaos4__
    struct Library     *LocaleBase = li->li_LocaleBase;
#else
    struct LocaleIFace *ILocale    = li->li_ILocale;
#endif
    LONG         *l;
    UWORD        *w;
    CONST_STRPTR  builtIn = NULL;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum && l < (LONG *)(&CatCompBlock[sizeof(CatCompBlock)]))
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    if (*l == stringNum)
    {
        builtIn = (CONST_STRPTR)((ULONG)l + 6);
    }

#ifndef __amigaos4__
    if (LocaleBase)
    {
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
    }
#else
    if (ILocale)
    {
#ifdef __USE_INLINE__
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#else
        return ILocale->GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#endif
    }
#endif
    return builtIn;
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* DGENGUI_STRINGS_H */
