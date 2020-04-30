#include "rpage/aos/inc.prl"
#include "rpage/frwk.h"

enum locale {locale_fr, locale_en, locale_es};
extern char *locale_ext[3];

UBYTE *load_pak_locale_to_array(char *text_array[], UWORD array_size, char *filename);
