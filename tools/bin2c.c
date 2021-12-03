/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Embedded Web-Server
 ****************************************************************************
 *            PROGRAM MODULE
 *
 *   $Id: bin2c.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2018
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 */
#include <stdio.h>
#include <stdlib.h>


static const char fmtZipReader[] = {
   "#include <barracuda.h>\n"
   "static int\n"
   "DataZipReader_diskRead(\n"
   "   CspReader* o,void* data,U32 offset,U32 size,int blockStart)\n"
   "{\n"
   "   (void)o;\n"
   "   (void)blockStart;\n"
   "   memcpy(data, cspPages+offset, size);\n"
   "   return 0;\n"
   "}\n"
   "#ifdef __cplusplus\nextern \"C\"\n#endif\n" /* For SMX */
   "ZipReader* %s(void);\n"
   "ZipReader* %s(void)\n"
   "{\n"
   "   static ZipReader zipReader;\n"
   "   ZipReader_constructor(&zipReader,DataZipReader_diskRead,sizeof(cspPages));\n"
   "   CspReader_setIsValid(&zipReader);\n"
   "   return &zipReader;\n"
   "}\n"
};


static const char fmtZipDir[] = {
   "#ifdef __cplusplus\nextern \"C\"\n#endif\n" /* For SMX */
   "HttpDir* %s(const char* name, AllocatorIntf* alloc);\n"
   "HttpDir* %s(const char* name, AllocatorIntf* alloc)\n"
   "{\n"
   "   static HttpResRdr dir;\n"
   "   static ZipIo io;\n"
   "   ZipIo_constructor(&io, getZipReader(), 1024, alloc);\n"
   "   if(ZipIo_getECode(&io) ==  ZipErr_NoError) {\n"
   "      HttpResRdr_constructor(&dir, (IoIntf*)&io, name, alloc, 0);\n"
   "      HttpResRdr_setMaxAge(&dir, 365*24*60*60);\n"
   "      return (HttpDir*)&dir;\n"
   "   }\n"
   "   return 0;\n"
   "}\n"
};


static const char fmtCspReader[] = {
   "#include <CspRunTm.h>\n"
   "static int\n"
   "DataCspReader_diskRead(\n"
   "   struct CspReader* o, void* data, U32 offset, U32 size, int blockStart)\n"
   "{\n"
   "#ifndef NDEBUG\n"
   "   if(blockStart)\n"
   "   {\n"
   "      U32 magicNo;\n"
   "      memcpy(&magicNo, cspPages+offset-4, 4);\n"
   "      baAssert(baNtohl(magicNo) == HTTP_MAGIC_NO);\n"
   "   }\n"
   "#endif\n"
   "   memcpy(data, cspPages+offset, size); \n"
   "   return 0;\n"
   "}\n"
   "#ifdef __cplusplus\nextern \"C\"\n#endif\n" /* For SMX */
   "CspReader* %s(void);\n"
   "CspReader* %s(void)\n"
   "{\n"
   "   static struct CspReader cspReader;\n"
   "   CspReader_constructor(&cspReader, DataCspReader_diskRead);\n"
   "   CspReader_setIsValid(&cspReader);\n"
   "   return &cspReader;\n"
   "}\n"
};


void baFatalEf(int ecode1, unsigned int ecode2,
               const char* file, int line)
{
   fprintf(stderr,"Fatal err: %d %d: %s:%d\n",ecode1,ecode2,file,line);
   exit(1);
}


int
main(int argc, char* argv[])
{
   FILE* fIn;
   FILE* fOut;
   int ch, l=1;
   const char* readerFmt=0;
   char* arrayName = "cspPages";
   char* getterName = 0;
   int i;
   if(argc < 3)
   {
      printf(
         "Usage: bin2c [flag] [include files]"
         " inFile outfile\n"
         "\n"
         "You can only use one flag.\n"
         "flags:\n"
         "  -c name"
         "          Insert a CSP reader.\n"
         "                   Name is the CspReader getter function.\n"
         "  -z name"
         "          Insert a ZIP reader.\n"
         "                   Name is the ZipReader getter function.\n"
         "  -Z name"
         "          Insert a HttpResRdr and ZIP reader.\n"
         "                   Name is the HttpResRdr getter function.\n"
         "  -d name"
         "          Name of the C Data array"
         "\n");
      return 1;
   }
   if(argc > 4 && argv[1][0] == '-')
   {
      switch(argv[1][1])
      {
         case 'c': readerFmt = fmtCspReader; break;
         case 'z': readerFmt = fmtZipReader; break;
         case 'Z': readerFmt = fmtZipDir; break;
         case 'd': arrayName = argv[2]; break;
         default:
            fprintf(stderr, "Unknown flag %d\n", argv[1][1]);
            return 1;
      }
      getterName = argv[2];
      argc-=2;
      argv+=2;
   }

   fOut = fopen(argv[argc-1], "w");
   if(!fOut)
   {
      fprintf(stderr, "Cannot open outFile %s", argv[argc-1]);
      perror("");
      return 1;
   }

   for(i = 1; i < argc-2 ; i++)
   {
      FILE* incFp = fopen(argv[i], "r");
      if( !incFp )
      {
         fprintf(stderr, "Cannot open include file %s", argv[i]);
         perror("");
         return 1;
      }
      fprintf(fOut, "\n\n/****** Include file %s ******/\n", argv[i]);
      while( (ch = getc(incFp)) != EOF)
         fprintf(fOut, "%c", ch);
      fclose(incFp);
   }

   fIn = fopen(argv[argc-2], "rb");
   if(!fIn)
   {
      fprintf(stderr, "Cannot open inFile %s", argv[argc-2]);
      perror("");
      return 1;
   }
   fprintf(fOut,
           "\n\n/****** The C array below contains data from file: %s *****/\n"
           "#include <barracuda.h>\n",
           argv[argc-2]);
   if(readerFmt)
      fprintf(fOut, "static const U8 %s[] = {\n", arrayName);
   else
      fprintf(fOut,
              "#ifndef STATIC\n"
              "#define STATIC\n"
              "#endif\n"
              "STATIC const U8 %s[] = {\n", arrayName);

   fprintf(fOut, "(U8)0x%02X", getc(fIn));
   while( (ch = getc(fIn)) != EOF)
   {
      fprintf(fOut, ",(U8)0x%02X", ch);
      if(++l == 8)
      {
         fprintf(fOut, "\n");
         l = 0;
      }
   }
   fprintf(fOut, "\n};\n\n\n");
   if(readerFmt)
   {
      if(readerFmt != fmtZipDir)
      {
         fprintf(fOut, "#ifndef STATIC\n#define STATIC\n#endif\n");
         fprintf(fOut, readerFmt, getterName,getterName);
      }
      else
      {
         fprintf(fOut, "%s","#undef STATIC\n#define STATIC static\n");
         fprintf(fOut, "%s","#include <HttpResRdr.h>\n#include <ZipIo.h>\n");
         fprintf(fOut, fmtZipReader, "getZipReader","getZipReader");
         fprintf(fOut, fmtZipDir, getterName,getterName);
      }
   }
   fclose(fIn);
   fclose(fOut);
   return 0;
}
