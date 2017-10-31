#define MAX_SHOW_LINE_LEN  7040

char *find_line(char *string);
char *read_show_line(void);
int  read_show_line0(char *buffer, int buffer_len);
char *skip_passed(char *ptr0, char *string);
void crash(char *fmt, ...);
void sys_crash(char *fmt, ...);

