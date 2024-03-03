#include <workbench/startup.h>


extern int gui_main(struct WBStartup *wbs);


int main(int argc, char **argv)
{
	//struct WBStartup *wbs = NULL;

/* whatever you need to DO */

	if(argc == 0) { gui_main( (struct WBStartup *)argv ); }

	//gui_main(wbs);

/* whatever you need to UNDO */

	return RETURN_OK;
}