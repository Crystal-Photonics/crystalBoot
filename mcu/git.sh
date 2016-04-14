"C:\Program Files (x86)\Git\bin\git" log -1 --format="format:#define GITHASH 0x%h"  > include/vc.h
ECHO " " >> include/vc.h
"C:\Program Files (x86)\Git\bin\git" log -1 --date=short --format="format:#define GITDATE \"%ad\"" >> include/vc.h
ECHO " " >> include/vc.h
"C:\Program Files (x86)\Git\bin\git" log -1 --date=short --format="format:#define GITUNIX %ct" >> include/vc.h