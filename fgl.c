/***************************************************************************
 * fgl.c
 * Character-based parser for Fgl definitions
 **************************************************************************/
/* INCLUDE FILES */
#include "general.h"    /* always include first */

#include <string.h>     /* to declare strxxx() functions */
#include <ctype.h>      /* to define isxxx() macros */

#include "parse.h"      /* always include */
#include "read.h"       /* to define file fileReadLine() */

/* DATA DEFINITIONS */
typedef enum eFglKinds {
    K_FUNCTION, K_PREPARE, K_CURSOR, K_MODULE
} fglKind;

static kindOption FglKinds [] = {
    { TRUE, 'f', "function", "functions" },
    { TRUE, 's', "prepare", "statements" },
    { TRUE, 'c', "cursor", "cursors" },
    { TRUE, 'v', "variable", "variables" }
};

/* FUNCTION DEFINITIONS */
static void findFglTags (void)
{
    vString *name = vStringNew ();
    const unsigned char *line;
    int define_block=0;
    int in_function=0;

    while ((line = fileReadLine ()) != NULL)
    {
	const unsigned char *pos=line;
	/* advance the point to the first non whitespace char */
        while (isspace ((int) *pos))
             ++pos;

	if (define_block)
        {
            const unsigned char *cp = line;
            while (isspace ((int) *cp))
                ++cp;

	    if ( strlen(cp) == 0 ) /* there is a empty line between variable declaration under single define keyword */
	       continue;

	    if ((int) *cp != 'm' && (int) *cp != 'g') 
	    {
	       define_block=0;
	       continue;
	    }

            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
                vStringPut (name, (int) *cp);
                ++cp;
            }

            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_MODULE);
            vStringClear (name);

	    /* if the current line ends with a comma, variable declaration continues to the next line */
	    int len = strlen(line);
            const unsigned char *lastp = line+len-1;
            while (isspace ((int) *lastp))
                --lastp;

	    if ((int) *lastp == ',' )
	       define_block=1;
	    else
	       define_block=0;
            	
	}

        /* Look for a line beginning with module "define" followed by name */
        if ( strncmp (pos, "define", (size_t) 6) == 0  ||
             strncmp (pos, "Define", (size_t) 6) == 0  ||
             strncmp (pos, "DEFINE", (size_t) 6) == 0)
        {
            const unsigned char *cp = line + 7;
            while (isspace ((int) *cp))
                ++cp;
	    
	    if ( strlen(cp)==0 ) /* define only in the line */
	        define_block = 1;

	    if ((int) *cp != 'm' && (int) *cp != 'g') 
	       continue;

            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
                vStringPut (name, (int) *cp);
                ++cp;
            }

            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_MODULE);
            vStringClear (name);

	    /* if the current line ends with a comma, variable declaration continues to the next line */
	    int len = strlen(line);
	    const unsigned char *lastp = line + len -1; 
            while (isspace ((int) *lastp))
                --lastp;
	   
	    if ((int) *lastp == ',' )
	       define_block=1;
        }
	
        /* Look for a line beginning with "function" followed by name */
        if ((strncmp (pos, "function", (size_t) 8) == 0  ||
            strncmp (pos, "FUNCTION", (size_t) 8) == 0  ||
            strncmp (pos, "Function", (size_t) 8) == 0)  &&
            isspace ((int) line [8]))
        {
            in_function=1;
            const unsigned char *cp = line + 9;
            while (isspace ((int) *cp))
                ++cp;
            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
                vStringPut (name, (int) *cp);
                ++cp;
            }
            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_FUNCTION);
            vStringClear (name);
        }

        /* Look for a line beginning with "end function" followed by name */
        if ((strncmp (pos, "end function", (size_t) 8) == 0  ||
            strncmp (pos, "END FUNCTION", (size_t) 8) == 0  ||
            strncmp (pos, "End Function", (size_t) 8) == 0)  &&
            isspace ((int) line [8]))
	{
	   in_function=0;
	}

        /* Look for a line beginning with "prepare" followed by name */
        if (strncmp (pos, "prepare ", (size_t) 8) == 0  ||
            strncmp (pos, "PREPARE ", (size_t) 8) == 0  ||
            strncmp (pos, "Prepare ", (size_t) 8) == 0 )
        {
            const unsigned char *cp = pos + 8;
            while (isspace ((int) *cp))
                ++cp;
            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
                vStringPut (name, (int) *cp);
                ++cp;
            }
            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_PREPARE);
            vStringClear (name);
        }

        /* Look for a line beginning with "declare" followed by name */
        if (strncmp (pos, "declare ", (size_t) 8) == 0  ||
            strncmp (pos, "Declare ", (size_t) 8) == 0  ||
            strncmp (pos, "DECLARE ", (size_t) 8) == 0) 
        {
            const unsigned char *cp = pos + 8;
            while (isspace ((int) *cp))
                ++cp;
            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
                vStringPut (name, (int) *cp);
                ++cp;
            }
            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_CURSOR);
            vStringClear (name);
        }
    }
    vStringDelete (name);
}

/* Create parser definition stucture */
extern parserDefinition* FglParser (void)
{
    static const char *const extensions [] = { "4gl", NULL };
    parserDefinition* def = parserNew ("Fgl");
    def->kinds      = FglKinds;
    def->kindCount  = KIND_COUNT (FglKinds);
    def->extensions = extensions;
    def->parser     = findFglTags;
    return def;
}


