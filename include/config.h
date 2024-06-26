// INFO CONFIGS
#define SHOW_USER_INFO true // user | hostname | shell | terminal
#define SHOW_OS_INFO true // arhitecture | release | OS
#define SHOW_CPU_INFO true // CPU model | Total number of cores | Online cores
#define SHOW_RAM_INFO true // Total RAM | RAM in use
#define SHOW_IN_MB false // show RAM usage default in GB
#define SHOW_SCREEN_INFO true // Total screens | Resolution
#define SHOW_TIME true // DATE | UPTIME


// COLOR CONFIGS
#define ANSI_COLORS_RGB(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define RESET_COLOR "\x1b[0m"
#define LINE_COLOR {255, 0, 255}
#define MAIN_COLOR {255, 0, 0}
#define SECOND_COLOR {0, 255, 0}
#define THIRD_COLOR {0, 0, 255}

#define MAX_USER_LINE_SIZE 256

#define PRINT_COLORED_TEXT(rgb, format, ...) \
	printf(ANSI_COLORS_RGB(%d, %d, %d) format RESET_COLOR "\n", rgb[0], rgb[1], rgb[2], __VA_ARGS__)

#define PRINT_COLORED_TEXT_NO_NEW_LINE(rgb, format, ...) \
	printf(ANSI_COLORS_RGB(%d, %d, %d) format RESET_COLOR ,rgb[0], rgb[1], rgb[2], __VA_ARGS__)


struct Configs {
	bool showUserInfo;
	bool showOSInfo;
	bool showCPUInfo;
	bool showRAMInfo;
	bool showScreenInfo;
	bool showTime;
};
