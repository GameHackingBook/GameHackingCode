#pragma comment(lib, "allegro-5.0.10-static-md.lib")

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#pragma comment(lib, "winmm.LIB")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "psapi.lib")

#ifdef _ALLEGRO_FONT_DLL
#include <allegro5/allegro_ttf.h>
#pragma comment(lib, "freetype-2.4.8-static-md.lib")
#pragma comment(lib, "allegro_font-5.0.10-static-md.lib")
#pragma comment(lib, "allegro_ttf-5.0.10-static-md.lib")
#endif

#ifdef _ALLEGRO_DIALOG_DLL
#pragma comment(lib, "allegro_dialog-5.0.10-static-md.lib")
#endif

#ifdef _ALLEGRO_PRIM_DLL
#pragma comment(lib, "allegro_primitives-5.0.10-static-md.lib")
#endif