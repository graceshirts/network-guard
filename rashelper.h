#include <windows.h>
#include <Ras.h>

BOOL RasHlpConnected(HRASCONN hrasconn);
DWORD RasHlpGetEntryDialParams(LPRASDIALPARAMS lprasdialparam, char szEntryName[], LPBOOL fPassword);
DWORD RasHlpDial(LPRASDIALPARAMS lprasdialparams, LPHRASCONN lphrasconn);