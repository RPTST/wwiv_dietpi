/**************************************************************************/
/*                                                                        */
/*                 WWIV Initialization Utility Version 5.0                */
/*             Copyright (C)1998-2014, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#ifndef __INCLUDED_WWIVINIT_H__
#define __INCLUDED_WWIVINIT_H__

#ifndef NOT_BBS
#define NOT_BBS 0 
#endif  // NOT_BBS

// defined in VARS.H net_networks_rec *net_networks;
// defined in VARS.H int initinfo.net_num_max;
#define MAX_NETWORKS 100
#define MAX_LANGUAGES 100
#define MAX_ALLOWED_PORT 8

//
// Make sure right number of OS flags are defined.  Some OSD #defines are
// defined in this file, so anything that relies on thse values MUST
// be included after testos.h has been included.
//

#include "platform/testos.h"

#include "w5assert.h"

//
// Include OSD set of standard header files
//

#include "platform/incl1.h"

//
// Normal ANSI C type includes
//

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cerrno>
#include <fcntl.h>

#include <cmath>
#include <csignal>
#include <cstdarg>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <ctime>

//
// Data
//
#pragma pack(push, 1)
typedef struct {
  short int frm_state;
  short int to_state;
  char *type;
  char *send;
  char *recv;
} autosel_data;
#pragma pack(pop)

//
// WWIV includes
//
#ifndef INIT
#define INIT
#endif  // INIT

#include "vardec.h"
#include "wconstants.h"
#include "ifcns.h"
#include "vars.h"
#include "platform/wfndfile.h"
#include "platform/curses_io.h"
#include "platform/wfile.h"

#include "init.h"

// Misc Junk for now
#define textattr(x) curatr = (x)

// STRUCTS %%TODO: move 'em
#pragma pack(push, 1)
typedef struct {
  char description[81],                     /* protocol description */
   receivefn[81],                           /* receive filename */
   sendfn[81];                              /* send filename */

  unsigned short ok1, ok2,                  /* if sent */
   nok1, nok2;                              /* if not sent */
} externalrec;


typedef struct {
  char curspeed[23];                        /* description of speed */

  char return_code[23];                     /* modem result code */

  unsigned short modem_speed,               /* speed modems talk at */
   com_speed;                               /* speed com port runs at */
} resultrec;

struct initinfo_rec {
    int numexterns;
    int numeditors;
    int num_languages;
    int net_num_max;
    int num_subs;
    int usernum;
    int nNumMsgsInCurrentSub;
};
#pragma pack(pop)

class InitSession;

__EXTRN__ WInitApp *app;
__EXTRN__ InitSession *sess;
__EXTRN__ userrec thisuser;
__EXTRN__ initinfo_rec initinfo;

#if defined(__unix__) 

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define _strupr strupr
#define _open open
#define _close close
#define _lseek lseek
#define _read read
#define _write write
#define chsize ftruncate

#endif  // __unix__

#endif // __INCLUDED_WWIVINIT_H__
