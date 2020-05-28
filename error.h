#define ANSI_BOLD "\x1b[1m"
#define ANSI_BLACK "\x1b[30m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"

void errHead () ;
void errSemantic () ;
void errWarning () ;
void errLex () ;
void errFatal () ;
void errParse () ;
void errCodegen () ;
void errArrow () ;
void errLine (int line) ;