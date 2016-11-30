/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*             Copyright (C)1998-2016, WWIV Software Services             */
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
#include <string>

#include "bbs/bgetch.h"
#include "bbs/com.h"
#include "bbs/datetime.h"
#include "bbs/input.h"
#include "bbs/interpret.h"
#include "bbs/wconstants.h"
#include "bbs/bbs.h"
#include "bbs/fcns.h"
#include "bbs/keycodes.h"
#include "bbs/pause.h"
#include "bbs/vars.h"
#include "core/strings.h"
#include "core/stl.h"
#include "core/wwivassert.h"

using std::string;
using namespace wwiv::strings;
using namespace wwiv::stl;

static char str_yes[81],
            str_no[81];
char  str_pause[81],
      str_quit[81];

// in pause.cpp
extern int nsp; 

bool inli(string* outBuffer, string* rollOver, string::size_type nMaxLen, bool bAddCRLF,
          bool bAllowPrevious, bool bTwoColorChatMode, bool clear_previous_line) {
  char szBuffer[4096] = {0}, szRollover[4096] = {0};
  to_char_array(szBuffer, *outBuffer);
  if (rollOver) {
    to_char_array(szRollover, *rollOver);
  } else {
    memset(szRollover, 0, sizeof(szRollover));
  }
  bool ret = inli(szBuffer, szRollover, nMaxLen, bAddCRLF, bAllowPrevious, bTwoColorChatMode, clear_previous_line);
  outBuffer->assign(szBuffer);
  if (rollOver) {
    rollOver->assign(szRollover);
  }
  return ret;
}

// returns true if needs to keep inputting this line
bool inli(char *buffer, char *rollover, string::size_type nMaxLen, bool bAddCRLF, bool bAllowPrevious,
          bool bTwoColorChatMode, bool clear_previous_line) {
  char szRollOver[255];

  WWIV_ASSERT(buffer);
  WWIV_ASSERT(rollover);

  int cm = chatting;

  size_t begx = session()->localIO()->WhereX();
  if (rollover[0] != 0) {
    char* ss = szRollOver;
    for (int i = 0; rollover[i]; i++) {
      if (rollover[i] == CC || rollover[i] == CO) {
        *ss++ = 'P' - '@';
      } else {
        *ss++ = rollover[i];
      }
      // Add a second ^P if we are working on RIP codes
      if (rollover[i] == CO && rollover[i + 1] != CO && rollover[i - 1] != CO) {
        *ss++ = 'P' - '@';
      }
    }
    *ss = '\0';
    if (charbufferpointer) {
      char szTempBuffer[255];
      strcpy(szTempBuffer, szRollOver);
      strcat(szTempBuffer, &charbuffer[charbufferpointer]);
      strcpy(&charbuffer[1], szTempBuffer);
      charbufferpointer = 1;
    } else {
      strcpy(&charbuffer[1], szRollOver);
      charbufferpointer = 1;
    }
    rollover[0] = '\0';
  }
  string::size_type cp = 0;
  bool done = false;
  unsigned char ch = '\0';
  do {
    ch = bout.getkey();
    if (bTwoColorChatMode) {
      bout.Color(bout.IsLastKeyLocal() ? 1 : 0);
    }
    if (cm) {
      if (chatting == 0) {
        ch = RETURN;
      }
    }
    if (ch >= SPACE) {
      if ((session()->localIO()->WhereX() < (session()->user()->GetScreenChars() - 1)) && (cp < nMaxLen)) {
        buffer[cp++] = ch;
        bout.bputch(ch);
        if (session()->localIO()->WhereX() == (session()->user()->GetScreenChars() - 1)) {
          done = true;
        }
      } else {
        if (session()->localIO()->WhereX() >= (session()->user()->GetScreenChars() - 1)) {
          done = true;
        }
      }
    } else switch (ch) {
      case CG:
        if (chatting && outcom) {
          bout.rputch(CG);
        }
        break;
      case RETURN:                            // C/R
        buffer[cp] = '\0';
        done = true;
        break;
      case BACKSPACE:                             // Backspace
        if (cp) {
          if (buffer[cp - 2] == CC) {
            cp -= 2;
            bout.Color(0);
          } else if (buffer[cp - 2] == CO) {
            const string interpreted = interpret(buffer[cp - 1]);
            for (auto i = interpreted.size(); i > 0; i--) {
              bout.bs();
            }
            cp -= 2;
            if (buffer[cp - 1] == CO) {
              cp--;
            }
          } else {
            if (buffer[cp - 1] == BACKSPACE) {
              cp--;
              bout.bputch(SPACE);
            } else {
              cp--;
              bout.bs();
            }
          }
        } else if (bAllowPrevious) {
          if (okansi()) {
            if (clear_previous_line) {
              bout << "\r\x1b[K";
            }
            bout << "\x1b[1A";
          } else {
            bout << "[*> Previous Line <*]\r\n";
          }
          return true;
        }
        break;
      case CX:                            // Ctrl-X
        while (session()->localIO()->WhereX() > begx) {
          bout.bs();
          cp = 0;
        }
        bout.Color(0);
        break;
      case CW:                            // Ctrl-W
        if (cp) {
          do {
            if (buffer[cp - 2] == CC) {
              cp -= 2;
              bout.Color(0);
            } else if (buffer[cp - 1] == BACKSPACE) {
              cp--;
              bout.bputch(SPACE);
            } else {
              cp--;
              bout.bs();
            }
          } while (cp && buffer[cp - 1] != SPACE && buffer[cp - 1] != BACKSPACE);
        }
        break;
      case CN:                            // Ctrl-N
        if (session()->localIO()->WhereX() && cp < nMaxLen) {
          bout.bputch(BACKSPACE);
          buffer[cp++] = BACKSPACE;
        }
        break;
      case CP:                            // Ctrl-P
        if (cp < nMaxLen - 1) {
          ch = bout.getkey();
          if (ch >= SPACE && ch <= 126) {
            buffer[cp++] = CC;
            buffer[cp++] = ch;
            bout.Color(ch - '0');
          } else if (ch == CP && cp < nMaxLen - 2) {
            ch = bout.getkey();
            if (ch != CP) {
              buffer[cp++] = CO;
              buffer[cp++] = CO;
              buffer[cp++] = ch;
              bout.bputch('\xf');
              bout.bputch('\xf');
              bout.bputch(ch);
            }
          }
        }
        break;
      case TAB: {                           // Tab
        int charsNeeded = 5 - (cp % 5);
        if ((cp + charsNeeded) < nMaxLen
            && (session()->localIO()->WhereX() + charsNeeded) < session()->user()->GetScreenChars()) {
          charsNeeded = 5 - ((session()->localIO()->WhereX() + 1) % 5);
          for (int j = 0; j < charsNeeded; j++) {
            buffer[cp++] = SPACE;
            bout.bputch(SPACE);
          }
        }
      }
      break;
      }
  } while (!done && !hangup);

  if (hangup) {
    // Caller isn't here, so we are not saving any message.
    return false;
  }

  if (ch != RETURN) {
    string::size_type lastwordstart = cp - 1;
    while (lastwordstart > 0 && buffer[lastwordstart] != SPACE && buffer[lastwordstart] != BACKSPACE) {
      lastwordstart--;
    }
    if (lastwordstart > static_cast<string::size_type>(session()->localIO()->WhereX() / 2)
        && lastwordstart != (cp - 1)) {
      string::size_type lastwordlen = cp - lastwordstart - 1;
      for (string::size_type j = 0; j < lastwordlen; j++) {
        bout.bputch(BACKSPACE);
      }
      for (string::size_type j = 0; j < lastwordlen; j++) {
        bout.bputch(SPACE);
      }
      for (string::size_type j = 0; j < lastwordlen; j++) {
        rollover[j] = buffer[cp - lastwordlen + j];
      }
      rollover[lastwordlen] = '\0';
      cp -= lastwordlen;
    }
    buffer[cp++] = CA;
    buffer[cp] = '\0';
  }
  if (bAddCRLF) {
    bout.nl();
  }
  return false;
}


// Returns 1 if current user has sysop access (permanent or temporary), else
// returns 0.
bool so() {
  return (session()->GetEffectiveSl() == 255);
}

/**
 * Checks for Co-SysOp status
 * @return true if current user has limited co-sysop access (or better)
 */
bool cs() {
  if (so()) {
    return true;
  }

  return (getslrec(session()->GetEffectiveSl()).ability & ability_cosysop) ? true : false;
}


/**
 * Checks for limitied Co-SysOp status
 * <em>Note: Limited co sysop status may be for this message area only.</em>
 *
 * @return true if current user has limited co-sysop access (or better)
 */
bool lcs() {
  if (cs()) {
    return true;
  }

  if (getslrec(session()->GetEffectiveSl()).ability & ability_limited_cosysop) {
    if (*qsc == 999) {
      return true;
    }
    return (*qsc == static_cast<uint32_t>(session()->current_user_sub().subnum)) ? true : false;
  }
  return false;
}

/**
 * Checks to see if user aborted whatever he/she was doing.
 * Sets abort to true if control-C/X or Q was pressed.
 * returns the value of abort
 */
bool checka() {
  bool ignored_abort = false;
  bool ignored_next = false;
  return checka(&ignored_abort, &ignored_next);
}

/**
 * Checks to see if user aborted whatever he/she was doing.
 * Sets abort to true if control-C/X or Q was pressed.
 * returns the value of abort
 */
bool checka(bool *abort) {
  bool ignored_next = false;
  return checka(abort, &ignored_next);
}

/**
 * Checks to see if user aborted whatever he/she was doing.
 * Sets next to true if control-N was hit, for zipping past messages quickly.
 * Sets abort to true if control-C/X or Q was pressed.
 * returns the value of abort
 */
bool checka(bool *abort, bool *next) {
  if (nsp == -1) {
    *abort = true;
    clearnsp();
  }
  while (bkbhit() && !*abort && !hangup) {
    CheckForHangup();
    char ch = bgetch();
    switch (ch) {
    case CN:
      bout.clear_lines_listed();
      *next = true;
    case CC:
    case SPACE:
    case CX:
    case 'Q':
    case 'q':
      bout.clear_lines_listed();
      *abort = true;
      break;
    case 'P':
    case 'p':
    case CS:
      bout.clear_lines_listed();
      ch = bout.getkey();
      break;
    }
  }
  return *abort;
}

// Prints an abortable string (contained in *text). Returns 1 in *abort if the
// string was aborted, else *abort should be zero.
void pla(const string& text, bool *abort) {
  if (CheckForHangup()) {
    *abort = true;
  }

  for (const auto& c : text) {
    if (checka(abort)) {
      break;
    }
    bout.bputch(c, true);
  }
  bout.flush();
  if (!*abort) {
    bout.nl();
  }
}

void plal(const string& text, string::size_type limit, bool *abort) {
  CheckForHangup();
  if (hangup) {
    *abort = true;
  }

  checka(abort);

  limit += text.length() - stripcolors(text).length();
  string::size_type nCharsDisplayed = 0;
  for (auto iter = text.begin(); iter != text.end() && nCharsDisplayed++ < limit
       && !*abort; ++iter) {
    if (*iter != '\r' && *iter != '\n') {
      bout.bputch(*iter, true);
    }
    checka(abort);
  }

  bout.flush();
  if (!*abort) {
    bout.nl();
  }
}

// Returns 1 if sysop is "chattable", else returns 0. Takes into account
// current user's chat restriction (if any) and sysop high and low times,
// if any, as well as status of scroll-lock key.
bool sysop2() {
  bool ok = sysop1();
  if (session()->user()->IsRestrictionChat()) {
    ok = false;
  }
  if (syscfg.sysoplowtime != syscfg.sysophightime) {
    if (syscfg.sysophightime > syscfg.sysoplowtime) {
      if (timer() <= (syscfg.sysoplowtime * SECONDS_PER_MINUTE) ||
          timer() >= (syscfg.sysophightime * SECONDS_PER_MINUTE)) {
        ok = false;
      }
    } else if (timer() <= (syscfg.sysoplowtime * SECONDS_PER_MINUTE) &&
               timer() >= (syscfg.sysophightime * SECONDS_PER_MINUTE)) {
      ok = false;
    }
  }
  return ok;
}

// Returns 1 if ANSI detected, or if local user, else returns 0. Uses the
// cursor position interrogation ANSI sequence for remote detection.
// If the user is asked and choosed NO, then -1 is returned.
int check_ansi() {
  if (!incom) {
    return 1;
  }

  while (bkbhitraw()) {
    bgetchraw();
  }

  bout.rputs("\x1b[6n");

  long l = timer1() + 36 + 18;

  while ((timer1() < l) && (!hangup)) {
    CheckForHangup();
    char ch = bgetchraw();
    if (ch == '\x1b') {
      l = timer1() + 18;
      while (timer1() < l && !hangup) {
        if ((timer1() + 1820) < l) {
          l = timer1() + 18;
        }
        CheckForHangup();
        ch = bgetchraw();
        if (ch) {
          if ((ch < '0' || ch > '9') && ch != ';' && ch != '[') {
            return 1;
          }
        }
      }
      return 1;
    } else if (ch == 'N') {
      return -1;
    }
    if ((timer1() + 1820) < l) {
      l = timer1() + 36;
    }
  }
  return 0;
}


// Sets current language to language index n, reads in menus and help files,
// and initializes stringfiles for that language. Returns false if problem,
// else returns true.
bool set_language_1(int n) {
  size_t idx = 0;
  for (idx = 0; idx < session()->languages.size(); idx++) {
    if (session()->languages[idx].num == n) {
      break;
    }
  }

  if (idx >= session()->languages.size() && n == 0) {
    idx = 0;
  }

  if (idx >= session()->languages.size()) {
    return false;
  }

  session()->set_language_number(n);

  strncpy(str_yes, "Yes", sizeof(str_yes) - 1);
  strncpy(str_no, "No", sizeof(str_no) - 1);
  strncpy(str_quit, "Quit", sizeof(str_quit) - 1);
  strncpy(str_pause, "More? [Y/n/c]", sizeof(str_pause) - 1);
  str_yes[0] = upcase(str_yes[0]);
  str_no[0] = upcase(str_no[0]);
  str_quit[0] = upcase(str_quit[0]);

  return true;
}

// Sets language to language #n, returns false if a problem, else returns true.
bool set_language(int n) {
  if (session()->language_number() == n) {
    return true;
  }

  int nOldCurLang = session()->language_number();

  if (!set_language_1(n)) {
    if (nOldCurLang >= 0) {
      if (!set_language_1(nOldCurLang)) {
        set_language_1(0);
      }
    }
    return false;
  }
  return true;
}

const char *YesNoString(bool bYesNo) {
  return (bYesNo) ? str_yes : str_no;
}

