#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__

#include <string>
typedef std::basic_string<TCHAR> tstring;

#define ERROR_MESSAGEBOX(A) AfxMessageBox(A, MB_ICONEXCLAMATION)
#define INFO_MESSAGEBOX(A) AfxMessageBox(A, MB_ICONINFORMATION)
#define MAX_LINE_LEN    2048
#define CPPSOURCEFILE   ".cpp"
#define HEADERFILE      ".h"
#define JAVAFILE        ".java"
#define CONFFILE        ".conf"

//////////////////////////////////////////////////////////////////////////
#define  CCM	   "ccm"
#define  CDA       "cda"
#define  SSRM      "ssrm"
#define  IMAGING   "img"
#define  SDA       "sda"
#define  PM        "pm"
//Report service
#define  RPT       "rpt"
//itpamadapter
#define  ITP       "itpa"
#define  SAMA      "sama"
#define  CMDB      "cmdb"
#define  CLIC      "clic"
#define  DPMS      "dpms"
#define  EC2       "ec2"
#define  DPME      "dpme"
#define  RSI       "rsi"
//Network management adapter
#define  NMA       "nma"
#define  XEN       "xen"
#define  VCWS      "vcws"
#define  CHAR      "char"
#define  HYPERV    "hype"
#define  DPMR      "dpmr"
#define  ZONE      "zone"
#define  SNMP      "snmp"
#define  ISM       "ism"
#define  SPM       "spm"
#define  LPAR      "lpar"
#define  UCSW      "ucsw"
#define  RDAD      "rdad"
#define  MSCS      "mscs"
#define  CE        "ce"
#define  PMMC      "pmmc"
#define  APPL      "appl"
#define  DPMH      "dpmh"
#define  ISMC      "ismc"
#define  ORCHE     "orch"
#define  GETUI     "gwtu"
#define  INSTALL   "inst"
#define  VCLOUD    "vclo"
#define  KVMWS     "kvmw"
#define  POLICY    "poli"
#define  DPMP      "dpmp"
#define  GALAX     "gala"
#define  DPMO      "dpmo"
#define  DPMK      "dpmk"
#define  AOM       "aom"
#define  KPDB      "kpdb"
#define  SPS       "sps"
#define  CARESMAN  "care"
//alarmmanager
#define ALARMMANAGER "alar"
#define SCH          "sch"
//StateEngine
#define STATEENGINE  "stat"
//HelpDesk
#define HD           "hd"
//AuditService
#define AUDITSERVICE  "audi"
#define ADESWS        "ades"
//trapreceiver
#define  TRAPRECEIVER "trap"

#define WM_UPDATECOMPONENTS         WM_USER + 100
#define WM_GENERATEANALYSERESULT    WM_USER + 101
#define WM_SWITCHDOCUMENT           WM_USER + 102
#define WM_SHOWCOMPNENT             WM_USER + 103
//////////////////////////////////////////////////////////////////////////
//PS
/*
xen			xenws			\SRC\PMM\xenws\
vcws		vces			\SRC\PMM\vcws\
kvmws		kvmws			\SRC\PMM\kvmws\
zone		zone			\SRC\PMM\zonews\
gppws		gppws			\SRC\PMM\gppws\
adesws		adesws			\SRC\PMM\adesws\\
ucsws		ucsws			\SRC\PMM\ucsws\
vbkws		vbkws			\SRC\PMM\vbkws\
caserm		caserm			\SRC\PMM\caserm\
hypervws	hypervws		\SRC\PMM\hypervws\
mscsws		mscsws			\SRC\PMM\mscsws\
lpar		lpar			\SRC\PMM\lparws\
casesy		casesy			\SRC\casesy\

SC				https://yanju03-win2k8/aip/sc
AOM				https://yanju03-win2k8:443/aip/AOM
adesws			https://yanju03-win2k8:443/dpm/adesws
AuditService	https://yanju03-win2k8:443/aip/AuditService
hd
CE				https://yanju03-win2k8:443/aip/CE
Policy			https://yanju03-win2k8:443/aip/Policy
StateEngine		https://yanju03-win2k8:443/aip/StateEngine
sch				https://yanju03-win2k8:443/aip/sch
alarmmanager	https://yanju03-win2k8:443/aip/alarmmanager
trapreceiver	https://yanju03-win2k8:443/aip/trapreceiver
chargebk		https://yanju03-win2k8:443/dpm/chargebk
ccm				
ec2
itpamadapter
orchestrator	https://yanju03-win2k8:443/dpm/orchestrator
caresman		https://yanju03-win2k8:443/dpm/caresman
rsi
sda
spm
sps             \SRC\StorageProvisioningManager\
img				https://yanju03-win2k8:443/dpm/img
nma
ISM
resourcemgr		https://yanju03-win2k8:443/aip/resourcemgr
*/

#endif //__COMMON_HEADER__