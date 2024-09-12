/* COPYRIGHT:  Real Time Logic
 *
 * This program generates a BAS binary password either from a
 * specified input file or randomly if no file is provided. It emits
 * the binary password in a C array format and also converts it to the
 * BAS password string format you use when setting the password on the
 * AES-protected ZIP file.
 *
 * See: https://realtimelogic.com/ba/doc/en/lua/lua.html#io_setpasswd
 *
 * Compile:
 *   Linux: gcc -o binpwd2str binpwd2str.c
 *   Windows: cl binpwd2str.c
 *
 * Usage:
 *   binpwd2str [input-file]
 *   - If an input-file is provided, the binary password from that file is used.
 *   - If no input-file is provided, a random binary password is
 *     generated. The raw binary data is also saved as "binpwd.bin"
 * 
 * The program also includes a help option (-h or --help)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
static unsigned int GetTickCount(void)
{
#ifdef __APPLE__
   struct timeval t;
   if(gettimeofday(&t, NULL))
      return 0;
   return 1000*t.tv_sec + t.tv_usec/1000;
#else
   struct timespec t;
#ifdef CLOCK_REALTIME
   /* BaTimer.c works better with MONOTONIC when system time is updated */
   if(clock_gettime(CLOCK_MONOTONIC, &t))
      return time(NULL);
#else
   if(clock_gettime(0, &t))
      return time(NULL);
#endif
   return 1000*t.tv_sec + t.tv_nsec/1000000;
#endif
}
#endif


// Function to generate a random binary array
static uint8_t* createRandomBinaryArray(int length)
{
   uint8_t* binaryArray = (uint8_t*) malloc(length * sizeof(uint8_t));
   if(binaryArray == NULL)
   {
      printf("Memory allocation failed.\n");
      exit(2);
   }
   // Generate binary random numbers
   for(int i = 0; i < length; i++)
   {
      binaryArray[i] = rand() % 256;  // Full byte (0-255)
   }
   FILE *out = fopen("binpwd.bin", "wb");
   if(out)
   {
      fwrite(binaryArray, length, 1, out);
      fclose(out);
   }
   return binaryArray;
}


static uint8_t* readFileIntoBuffer(const char *inputFile, size_t *fileSize)
{
   struct stat fileStat;
   if(stat(inputFile, &fileStat) != 0)
   {
      printf("Error: could not get file info for %s.\n", inputFile);
      exit(1);
   }
   *fileSize = fileStat.st_size;
   FILE *in = fopen(inputFile, "rb");
   if(in == NULL)
   {
      printf("Error: could not open input file %s.\n", inputFile);
      exit(1);
   }
   uint8_t *buffer = (uint8_t*) malloc(*fileSize * sizeof(uint8_t));
   if(buffer == NULL)
   {
      printf("Memory allocation failed.\n");
      fclose(in);
      exit(1);
   }
   size_t bytesRead = fread(buffer, sizeof(uint8_t), *fileSize, in);
   if(bytesRead != *fileSize)
   {
      printf("Error: could not read the entire file.\n");
      free(buffer);
      fclose(in);
      exit(1);
   }
   fclose(in);
   return buffer;
}


static void binpwd2str(uint8_t* binpwd, size_t size)
{
   uint16_t i;
   uint8_t* p = (uint8_t*)malloc(size);
   if(!p)
   {
      printf("Memory allocation failed.\n");
      exit(2);
   }
   for(i = 0; i < size; i++)
   {
      p[i] = 'A' + (binpwd[i] & 0x0F) + (binpwd[i] >> 4);
      if(binpwd[i] & 0x01)
         p[i] += 'c' - 'A';
      if((p[i] < 'A') || (p[i] > 'W'))
         if((p[i] < 'a') || (p[i] > 'y'))
            p[i] = 'b' + (binpwd[i] & 0x1F);
      while(p[i] > 'x')
         p[i] -= 9;
   }
   printf("\npassword(%d): ",(int)size);
   for(i = 0; i < size; i++) printf("%c", p[i]);
   printf("\n");
}


static void printHelp()
{
   printf("Usage: binpwd2str [input-file]\n");
   printf("If no input-file with a binary password is provided, "
          "a binary random array of random length (between 60 and 80) "
          "will be generated.\n");
}


int main(int argc, char *argv[])
{
   uint8_t* binpwd;
   size_t size,i;
   if(2 == argc && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
   {
      printHelp();
      return 1;
   }
   srand(GetTickCount());
   if(2==argc)
  
   {
      binpwd=readFileIntoBuffer(argv[1], &size);
   }
   else
   {
      size=60 + rand() % 21;
      binpwd=createRandomBinaryArray(size);
   }
   printf("const uint8_t binpwd[] ={\n");
   for(i = 0; i < size; i++)
   {
      if(i % 10 == 0 && i != 0)
         printf("\n");
      printf("0x%02X", binpwd[i]);
      if(i < size - 1)
         printf(", ");
   }
   printf("\n};\n\nUse the following password for the AES encrypted ZIP file.");
   binpwd2str(binpwd, size);

   return 0;
}
