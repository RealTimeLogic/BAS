/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: mako.h 6657 2026-10-28 18:19:39Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012 - 2026
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
 *
 */
#include <barracuda.h>

#define APPNAME "mako"

struct MakoOpenAUX;

/* Function pointer to addSecret() in MakoMain.c */
typedef void (*AddSecret)(
   struct MakoOpenAUX* aux, BaBool unique, const U8* secret, size_t slen);


typedef struct MakoOpenAUX {
   lua_State* L; /* The main Lua state */
   IoIntfPtr dio; /* The IO instance named "disk" */
   IoIntfPtr hio; /* The IO instance named "home" */
   AddSecret addSecret; /* Callback to addSecret() in MakoMain.c */
   int tpmFuncRef; /* Reference to the TPM seed function in MakoTPM.lua */
} MakoOpenAUX;

/* SoftTPM extension in MakoExt2.ch; not called in PSA builds.
 */
void makoOpenAUX(MakoOpenAUX* aux);

#if SHARKSSL_PSA_CONSUMER
/* Required platform hook in PSA builds. No arguments; return int 0 on success
 * or a nonzero error code to stop startup. Initialize entropy, hardware, and
 * persistent storage in the provider's required order, including a successful
 * psa_crypto_init(). Platform prerequisites must be ready here.
 *
 * Called once per server runMako() invocation, after service/daemon setup and
 * MakoExtM1.ch, before cryptography, resource verification, Lua, or the
 * dispatcher mutex. Help and service-management commands exit before it.
 * Not called on a Lua debugger restart. Keep provider state alive through
 * shutdown and safely reuse it if runMako() is invoked again sequentially,
 * never concurrently. makoOpenAUX() is not called in PSA builds.
 * Do not use Lua/server objects or reset persistent keys here.
 * See PsaSimInit.c for the Windows simulator implementation.
 */
int makoInitPsa(void);
#endif

void makoprintf(int isErr,const char* fmt, ...);
void makovprintf(int isErr,const char* fmt, va_list argList);
void setDispExit(void);
void sendFatalError(const char* eMsg,
                    BaFatalErrorCodes ecode1,
                    unsigned int ecode2,
                    const char* file,
                    int line);
void errQuit(const char* fmt, ...);
  

void runMako(int isWinService,int argc, char* argv[], char* envp[]);

#ifdef _WIN32
#define SERVICENAME "mako"
void mustBeAdmin(void);
void instalMakoEventSource(void);
int prtok(int ok, const char* okmsg);
#endif
