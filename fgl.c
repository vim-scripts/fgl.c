/* INCLUDE FILES */
#include "general.h"    /* always include first */
#include "parse.h"      /* always include */

/* FUNCTION DEFINITIONS */

static void installFglRegex (const langType language)
{
    addTagRegex (language, "^[ \t]*[Ff][Uu][Nn][Cc][Tt][Ii][Oo][Nn][ \t]+([a-zA-Z0-9_]+)[ \t\(]+.*$", "\\1", "f,function", NULL);
}

/* Create parser definition stucture */
extern parserDefinition* FglParser (void)
{
    static const char *const extensions [] = { "4gl", NULL };
    parserDefinition* const def = parserNew ("Fgl");
    def->extensions = extensions;
    def->initialize = installFglRegex;
    def->regex      = TRUE;
    return def;
}


