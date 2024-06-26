#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../include/SystemInformation.h"
#include "../include/config.h"

#define MAX_BUFFER 256

double bytesMB(unsigned long long bytes) {
    return (double)bytes / (1024 * 1024);
}

double bytesGB(unsigned long long bytes) {
    return (double)bytes / (1024 * 1024 * 1024);
}

// should not be needed only in case strdup fails
// which is highly unlikely and if adding this
// add it also to hostname and all other places that use strdup
// if (sysInfo->user == NULL) sysInfo->user = "(?)";
// if (pwd) should be used only in main program not here

void fetchUser(SystemInformation *sysInfo, struct passwd *pwd) {
	sysInfo->user = strdup(pwd->pw_name);
}

void fetchHostname(SystemInformation *sysInfo, struct utsname OSInfo) {
	sysInfo->hostname = strdup(OSInfo.nodename);
}

void fetchShell(SystemInformation *sysInfo, struct passwd *pwd) {
	sysInfo->shell = strdup(pwd->pw_shell);
}

void fetchTerminal(SystemInformation *sysInfo) {
	char *term = getenv("TERM");
	if (term) {
		sysInfo->terminal = strdup(term);
	}
}

void fetchArhitecture(SystemInformation *sysInfo, struct utsname OSInfo) {
		sysInfo->arhitecture = strdup(OSInfo.machine);
}

void fetchRelease(SystemInformation *sysInfo, struct utsname OSInfo) {
	sysInfo->release = strdup(OSInfo.release);
}

void fetchOS(SystemInformation *sysInfo, struct utsname OSInfo) {
	sysInfo->OS = strdup(OSInfo.sysname);
}

void fetchCPUInfo(SystemInformation *sysInfo) {
	long totalCores, onlineCores;
	totalCores = sysconf(_SC_NPROCESSORS_CONF);
	onlineCores = sysconf(_SC_NPROCESSORS_CONF);

	// not really needed but leave it be
	if (totalCores == -1) {
		totalCores = 0;
	}
	if (onlineCores == -1) {
		onlineCores = 0;
	}

	sysInfo->CPUCoresTotal = totalCores;
	sysInfo->CPUCoresOnline = onlineCores;

	FILE *fp;
	char buffer[MAX_BUFFER];
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
		return;
    }

	while(fgets(buffer, MAX_BUFFER, fp) != NULL) {
    	if (strncmp(buffer, "model name", 10) == 0) {
        	sysInfo->CPU = strdup(strchr(buffer, ':'));
            break;
    	}
    }
    fclose(fp);
}

void fetchRAMInfo(SystemInformation *sysInfo) {
	long pageSize, numPages, availablePages;

	pageSize = sysconf(_SC_PAGESIZE);
	numPages = sysconf(_SC_PHYS_PAGES);
	availablePages = sysconf(_SC_AVPHYS_PAGES);

	if (pageSize == -1 || numPages == -1 || availablePages == -1) {
		sysInfo->totalRAM = 0;
		sysInfo->usedRAM = 0;
	} else {
		unsigned long long totalRAMBytes = numPages * pageSize;
		unsigned long long freeRAMBytes = availablePages * pageSize;
		unsigned long long usedRAMBytes = totalRAMBytes - freeRAMBytes;

		if(SHOW_IN_MB) {
			sysInfo->totalRAM = bytesMB(totalRAMBytes);
			sysInfo->usedRAM = bytesMB(usedRAMBytes);
		} else {
			sysInfo->totalRAM = bytesGB(totalRAMBytes);
			sysInfo->usedRAM = bytesGB(usedRAMBytes);
		}
	}
}

void fetchScreens(SystemInformation *sysInfo) {
	Display *display = XOpenDisplay(NULL);
	if(display == NULL) {
		return;
	}

	int screenCount = XScreenCount(display);
	sysInfo->screenCount = screenCount;

	sysInfo->screens = malloc(screenCount * sizeof(struct ScreenInfo));
	if (sysInfo->screens == NULL) {
		XCloseDisplay(display);
		return;
	}

	for(int i = 0; i < screenCount; i++) {
		Screen *screen = XScreenOfDisplay(display, i);
		sysInfo->screens[i].width = WidthOfScreen(screen);
		sysInfo->screens[i].height = HeightOfScreen(screen);
	}

	XCloseDisplay(display);
}

void fetchUPTime(SystemInformation *sysInfo) {
	FILE *fp;
	double uptime;

    fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
		return;
    }

    if (fscanf(fp, "%lf", &uptime) != 1) {
        fclose(fp);
		return;
    }
    fclose(fp);

	sysInfo->uptime = uptime/60;
}

void fetchCurrentTime(SystemInformation *sysInfo) {
	time_t currentTime;

	currentTime = time(NULL);

	sysInfo->currentSystemTime = localtime(&currentTime);
}

void freeSysInfo(void **freeStack, int freeStackCounter) {
	for(int i = 0; i < freeStackCounter; i++) {
		if (freeStack[i]) {
			free(freeStack[i]);
		}
	}
}
