/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
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

#include "wwiv.h"
#include "printfile.h"
#include "wcomm.h"
#include "core/strings.h"


void wwivnode(WUser *pUser, int mode) {
  char sysnum[6], s[81];
  int nUserNumber, nSystemNumber;

  if (!mode) {
    GetSession()->bout.NewLine();
    GetSession()->bout << "|#7Are you an active WWIV SysOp (y/N): ";
    if (!yesno()) {
      return;
    }
  }
  GetSession()->bout.NewLine();
  GetSession()->bout << "|#7Node:|#0 ";
  input(sysnum, 5);
  if (sysnum[0] == 'L' && mode) {
    print_net_listing(false);
    GetSession()->bout << "|#7Node:|#0 ";
    input(sysnum, 5);
  }
  if (sysnum[0] == '0' && mode) {
    pUser->SetForwardNetNumber(0);
    pUser->SetHomeUserNumber(0);
    pUser->SetHomeSystemNumber(0);
    return;
  }
  sprintf(s, "1@%s", sysnum);
  parse_email_info(s, &nUserNumber, &nSystemNumber);
  if (nSystemNumber == 0) {
    GetSession()->bout << "|#2No match for " << sysnum << "." << wwiv::endl;
    pausescr();
    return;
  }
  net_system_list_rec *csne = next_system(nSystemNumber);
  sprintf(s, "Sysop @%u %s %s", nSystemNumber, csne->name, GetSession()->GetNetworkName());
  std::string ph, ph1;
  if (!mode) {
    ph1 = pUser->GetDataPhoneNumber();
    input_dataphone();
    ph = pUser->GetDataPhoneNumber();
    pUser->SetDataPhoneNumber(ph1.c_str());
    enter_regnum();
  } else {
    ph = csne->phone;
  }
  if (ph != csne->phone) {
    GetSession()->bout.NewLine();
    if (printfile(ASV0_NOEXT)) {
      // failed
      GetSession()->bout.NewLine();
      pausescr();
    }
    sprintf(s, "Attempted WWIV SysOp autovalidation.");
    pUser->SetNote(s);
    if (pUser->GetSl() < syscfg.newusersl) {
      pUser->SetSl(syscfg.newusersl);
    }
    if (pUser->GetDsl() < syscfg.newuserdsl) {
      pUser->SetDsl(syscfg.newuserdsl);
    }
    return;
  }
  sysoplog("-+ WWIV SysOp");
  sysoplog(s);
  pUser->SetRestriction(GetSession()->asv.restrict);
  pUser->SetExempt(GetSession()->asv.exempt);
  pUser->SetAr(GetSession()->asv.ar);
  pUser->SetDar(GetSession()->asv.dar);
  if (!mode) {
    GetSession()->bout.NewLine();
    if (printfile(ASV1_NOEXT)) {
      // passed
      GetSession()->bout.NewLine();
      pausescr();
    }
  }
  if (wwiv::strings::IsEquals(pUser->GetDataPhoneNumber(),
                              reinterpret_cast<char*>(csne->phone))) {
    if (pUser->GetSl() < GetSession()->asv.sl) {
      pUser->SetSl(GetSession()->asv.sl);
    }
    if (pUser->GetDsl() < GetSession()->asv.dsl) {
      pUser->SetDsl(GetSession()->asv.dsl);
    }
  } else {
    if (!mode) {
      GetSession()->bout.NewLine();
      if (printfile(ASV2_NOEXT)) {
        // data phone not bbs
        GetSession()->bout.NewLine();
        pausescr();
      }
    }
  }
  pUser->SetForwardNetNumber(GetSession()->GetNetworkNumber());
  pUser->SetHomeUserNumber(1);
  pUser->SetHomeSystemNumber(nSystemNumber);
  if (!mode) {
    print_affil(pUser);
    pausescr();
  }
  pUser->SetForwardUserNumber(pUser->GetHomeUserNumber());
  pUser->SetForwardSystemNumber(pUser->GetHomeSystemNumber());
  if (!mode) {
    GetSession()->bout.NewLine();
    if (printfile(ASV3_NOEXT)) {
      \
      // mail forwarded
      GetSession()->bout.NewLine();
      pausescr();
    }
  }
  if (!pUser->GetWWIVRegNumber()) {
    enter_regnum();
  }
  changedsl();
}
