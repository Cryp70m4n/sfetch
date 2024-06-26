#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>

#include "../include/SystemInformation.h"
#include "../include/config.h"



char **readFileLines(const char *filename, int *numLines) {
    FILE *file;
    char **lines = NULL;
    char buffer[256];
    int count = 0;
    int capacity = (10 > MINIMAL_RESOURCE_SIZE) ? 10 : MINIMAL_RESOURCE_SIZE;

    file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    lines = malloc(capacity * sizeof(char *));
    if (lines == NULL) {
        fclose(file);
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        lines[count] = malloc((strlen(buffer) + 1) * sizeof(char));
        if (lines[count] == NULL) {
            for (int i = 0; i < count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }
        strcpy(lines[count], buffer);
        count++;

        if (count >= capacity) {
            capacity *= 2;
            char **temp = realloc(lines, capacity * sizeof(char *));
            if (temp == NULL) {
                for (int i = 0; i < count; i++) {
                    free(lines[i]);
                }
                free(lines);
                fclose(file);
                return NULL;
            }
            lines = temp;
        }
    }

    fclose(file);
    *numLines = count;

    return lines;
}

void freeLines(char **lines, int numLines) {
	for(int i = 0; i < numLines; i++) {
		if (lines[i]) {
			free(lines[i]);
		}
	}
	free(lines);
}

int main(int argc, char *argv[]) {
	int freeStackCounter = 0;
	void *freeStack[FREE_STACK_MAX] = {};

	SystemInformation *sysInfo = malloc(sizeof(SystemInformation));
	if (sysInfo == NULL) return -1;
	freeStack[freeStackCounter] = sysInfo;
	freeStackCounter++;

	struct passwd *pwd = getpwuid(getuid());
	if (!pwd) {
		freeSysInfo(freeStack, freeStackCounter);
		return -2;
	}

	struct utsname OSInfo;
	if (uname(&OSInfo) != 0) {
		freeSysInfo(freeStack, freeStackCounter);
		return -3;
	}

	int lineColor[3] = LINE_COLOR;
	int mainColor[3] = MAIN_COLOR;
	int secondColor[3] = SECOND_COLOR;
	int thirdColor[3] = THIRD_COLOR;

	struct Configs config = {SHOW_USER_INFO, SHOW_OS_INFO, SHOW_CPU_INFO, SHOW_RAM_INFO, SHOW_SCREEN_INFO, SHOW_TIME};


    char **lines;
    int numLines;

	char *resource = (argv[1] != NULL) ? argv[1] : "resources/default.res";

    lines = readFileLines(resource, &numLines);
    if (lines == NULL) {
		freeSysInfo(freeStack, freeStackCounter);
        return -4;
    }
	int currentLine = 0;

	int newLineCounter = numLines;

	unsigned long lineLen = strlen(lines[currentLine]);
	char *newLine = malloc(lineLen * sizeof(char));
	if (newLine == NULL) {
		freeSysInfo(freeStack, freeStackCounter);
		freeLines(lines, numLines);
		return -5;
	}
	while(newLineCounter < MINIMAL_RESOURCE_SIZE) {		
		for(int i = 0; i < lineLen; i++) {
			newLine[i] = ' ';
		}
		lines[newLineCounter++] = newLine;
	}


	if (config.showUserInfo) {	
		fetchUser(sysInfo, pwd);
		fetchHostname(sysInfo, OSInfo);
		fetchShell(sysInfo, pwd);
		fetchTerminal(sysInfo);
		int userHostLen = strlen(sysInfo->user)+strlen(sysInfo->hostname)+1;
		int ShellTermLen = strlen(sysInfo->terminal)+strlen(sysInfo->shell)+1;
		
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(mainColor, "%s@%s", sysInfo->user, sysInfo->hostname);	
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		for(int i = 0; i < userHostLen; i++) {
			printf("-");
		}
		printf("\n");
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(secondColor, "%s|%s", sysInfo->terminal, sysInfo->shell);	
		/*
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		for(int i = 0; i < ((userHostLen > ShellTermLen) ? userHostLen : ShellTermLen); i++) {
			printf("-");
		}
		printf("\n");
		*/

		if (sysInfo->user) {
			freeStack[freeStackCounter] = sysInfo->user;
			freeStackCounter++;
		}
		if (sysInfo->hostname) {
			freeStack[freeStackCounter] = sysInfo->hostname;
			freeStackCounter++;
		}
		if (sysInfo->terminal) {
			freeStack[freeStackCounter] = sysInfo->terminal;
			freeStackCounter++;
		}
		if (sysInfo->shell) {
			freeStack[freeStackCounter] = sysInfo->shell;
			freeStackCounter++;
		}
	}

	if(config.showOSInfo) {
		fetchArhitecture(sysInfo, OSInfo);
		fetchRelease(sysInfo, OSInfo);
		fetchOS(sysInfo, OSInfo);	
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(secondColor, "OS:%s|Release:%s|Arhitecture:%s", sysInfo->OS, sysInfo->release, sysInfo->arhitecture);
		if (sysInfo->arhitecture) {
			freeStack[freeStackCounter] = sysInfo->arhitecture;
			freeStackCounter++;
		}
		if (sysInfo->release) {
			freeStack[freeStackCounter] = sysInfo->release;
			freeStackCounter++;
		}
		if (sysInfo->OS) {
			freeStack[freeStackCounter] = sysInfo->OS;
			freeStackCounter++;
		}
	}

	if (config.showCPUInfo) {
		fetchCPUInfo(sysInfo);
		void *originalCPUPtr = sysInfo->CPU;
		sysInfo->CPU+=2;
		int CPUStringLength = strlen(sysInfo->CPU);
		if (CPUStringLength > 0) {
			sysInfo->CPU[CPUStringLength-1] = '\0';
		}
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(secondColor, "CPU:%s|(%ld/%ld)", sysInfo->CPU, sysInfo->CPUCoresTotal, sysInfo->CPUCoresOnline);
		if (sysInfo->CPU) {
			freeStack[freeStackCounter] = originalCPUPtr;
			freeStackCounter++;
		}
	}

	if (config.showRAMInfo) {
		fetchRAMInfo(sysInfo);
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(thirdColor, "RAM:(%.2f/%.2f)", sysInfo->totalRAM, sysInfo->usedRAM);
	}

	if (config.showTime) {
		fetchUPTime(sysInfo);
		fetchCurrentTime(sysInfo);
		int date[3] = {sysInfo->currentSystemTime->tm_mday, sysInfo->currentSystemTime->tm_mon+1, sysInfo->currentSystemTime->tm_year + 1900};
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(thirdColor, "UPTIME:%.1f", sysInfo->uptime);
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		PRINT_COLORED_TEXT(thirdColor, "DATE:%02d-%02d-%04d", date[0], date[1], date[2]);
	}

	if (config.showScreenInfo) {
		fetchScreens(sysInfo);
		PRINT_COLORED_TEXT_NO_NEW_LINE(lineColor, "%s", lines[currentLine++]);
		for(int i = 0; i < sysInfo->screenCount; i++) {
			PRINT_COLORED_TEXT_NO_NEW_LINE(thirdColor, "Screen %d: (%dx%d)", i, sysInfo->screens[i].width, sysInfo->screens[i].height);
			if ( i > 0) PRINT_COLORED_TEXT_NO_NEW_LINE(thirdColor, "%c", '|');
		}
		printf("\n");
		if(sysInfo->screens) {
			freeStack[freeStackCounter] = sysInfo->screens;
			freeStackCounter++;
		}
	}


	while(currentLine < newLineCounter) {
		PRINT_COLORED_TEXT(lineColor, "%s", lines[currentLine++]);
	}
	
	free(newLine);
	freeSysInfo(freeStack, freeStackCounter);
	freeLines(lines, numLines);

	return 0;
}
