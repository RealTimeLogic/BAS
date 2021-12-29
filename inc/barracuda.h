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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2020
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
 *               https://realtimelogic.com
 ****************************************************************************
 *
 * Container header file making it possible to use code with either
 * the autogenerated and amalgamated barracuda.h and the standard BAS
 * SDK using this header file.
 */
#ifndef __barracuda_h
#define __barracuda_h

#include <balua.h>
#include <AuthenticatedUser.h>
#include <BaTimer.h>
#include <HttpCmdThreadPool.h>
#include <HttpResRdr.h>
#include <HttpTrace.h>
#include <IoIntfCspReader.h>
#include <ZipIo.h>

#ifndef NO_BAIO_DISK
#include <BaDiskIo.h>
#endif

#include <FileCspReader.h>
#include <FileZipReader.h>
#include <NetIo.h>
#include <lxrc.h>


#endif