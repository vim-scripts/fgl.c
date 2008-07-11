/***************************************************************************
* timk
* fgl.c
* Character-based parser for Fgl definitions
* Sat Jul  5 08:06:45 EDT 2008
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

#define CMP_WORD_SIZE 15

static char lastChar(const unsigned char* ln)
{
   const unsigned char *str;
   const unsigned char *lastp;
   int len = strlen(ln);
   
   str=strchr(ln, '#');
   if (str != 0) 
      lastp=str-1;
   else
      lastp=ln+len-1;

   /* get the last non-white char */
   while (isspace ((int) *lastp))
      --lastp;

   return (int)*lastp;
}
   

static void findFglTags (void)
{
   vString *name = vStringNew ();
   const unsigned char *line;
   const unsigned char *lastchar;
   int var_define_block=0;
   int in_comment=0;
   unsigned int i;

   unsigned char pos[CMP_WORD_SIZE];

   while ((line = fileReadLine ()) != NULL)
   {
      /* advance the point to the first non whitespace char */
      while (isspace ((int) *line))
         ++line;

      /* skip comment */
      if ((int) *line == '{')  /*beginning of a comment*/
        {
           /* comment finish on the same line */
           if ((int)lastChar(line) != '}') 
              in_comment=1;
           continue;
        }

      /* check #... or #...} or alrady in a comment block*/
      if ((int) *line == '#' || in_comment) 
      {
   
         if (in_comment)
         {
            if ((int)lastChar(line) == '}') 
               in_comment=0; /* exit comment */
         }

         continue;
      }
     
     

      /* change the target keyword to lowercase */
      for (i=0; i<CMP_WORD_SIZE-1 && i<strlen(line); i++)
         pos[i]=tolower(line[i]);
    
      pos[i]='\n';


      if (var_define_block)
      {
         const unsigned char *cp = line;

         if ( strlen(cp) == 0 ) /* there is a empty line between variable declaration under single define keyword */
            continue;

         if ((int) *cp != 'm' && (int) *cp != 'g') 
         {
            var_define_block=0;
         }
         else
         {

            while (isalnum ((int) *cp)  ||  *cp == '_')
            {
               vStringPut (name, (int) *cp);
               ++cp;
            }
   
            vStringTerminate (name);
            makeSimpleTag (name, FglKinds, K_MODULE);
            vStringClear (name);

            /* if the current line ends with a comma, variable declaration continues to the next line */
            if ((int)lastChar(line) == ',') 
               var_define_block=1;
            else
               var_define_block=0;
         }
      }

      /* just 'define' on the line */
      if ( var_define_block==0 && strncmp (pos, "define", (size_t) 6) == 0 )
      {
         int len = strlen(line);
         const unsigned char *lastp = line+len-1;
         const unsigned char *cp = line + 5;

         /* get the last non-white char */
          while (isspace ((int) *lastp))
             --lastp;

         if (cp == lastp) 
         {
            var_define_block = 1;
            continue;
         }
      }

      /* Look for a line beginning with module "define" followed by name */
      if (var_define_block==0 &&  strncmp (pos, "define", (size_t) 6) == 0 )
      {
         const unsigned char *cp = line + 7;
         while (isspace ((int) *cp))
            ++cp;

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
         if ((int)lastChar(line) == ',') 
            var_define_block=1;

         continue;
      }

      /* Look for a line beginning with "function" followed by name */
      if (strncmp (pos, "function", (size_t) 8) == 0  && isspace ((int) line [8]))
      {
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
         continue;
      }


      /* Look for a line beginning with "prepare" followed by name */
      if (strncmp (pos, "prepare ", (size_t) 8) == 0 )
      {
         const unsigned char *cp = line + 8;
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
         continue;
      }

      /* Look for a line beginning with "declare" followed by name */
      if (strncmp (pos, "declare ", (size_t) 8) == 0 ) 
      {
         const unsigned char *cp = line + 8;
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
         continue;
      }
   }
   vStringDelete (name);
}

/* Create parser definition stucture */
extern parserDefinition* FglParser (void)
{
   static const char *const extensions [] = { "4gl",NULL };
   parserDefinition* def = parserNew ("Fgl");
   def->kinds      = FglKinds;
   def->kindCount  = KIND_COUNT (FglKinds);
   def->extensions = extensions;
   def->parser     = findFglTags;
   return def;
}


