#include <stdbool.h>
#include <time.h>

#define FREE_STACK_MAX 10
#define MINIMAL_RESOURCE_SIZE 15

struct ScreenInfo {
	int width;
	int height;
};

typedef struct SystemInformation {
	char *user;
	char *hostname;
	char *shell;
	char *terminal;
	char *arhitecture;
	char *release;
	char *OS;
	char *CPU;
	long CPUCoresTotal;
	long CPUCoresOnline;
	double totalRAM;
	double usedRAM;
	int screenCount;
	struct ScreenInfo *screens;
	double uptime;
	struct tm *currentSystemTime;
} SystemInformation;

void fetchUser(SystemInformation *sysInfo, struct passwd *pwd);
void fetchHostname(SystemInformation *sysInfo, struct utsname OSInfo);
void fetchShell(SystemInformation *sysInfo, struct passwd *pwd);
void fetchTerminal(SystemInformation *sysInfo);
void fetchArhitecture(SystemInformation *sysInfo, struct utsname OSInfo);
void fetchRelease(SystemInformation *sysInfo, struct utsname OSInfo);
void fetchOS(SystemInformation *sysInfo, struct utsname OSInfo);
void fetchCPUInfo(SystemInformation *sysInfo);
void fetchRAMInfo(SystemInformation *sysInfo);
void fetchScreens(SystemInformation *sysInfo);
void fetchUPTime(SystemInformation *sysInfo);
void fetchCurrentTime(SystemInformation *sysInfo);

void freeSysInfo(void **freeStack, int freeStackCounter);
