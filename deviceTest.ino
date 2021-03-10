#include <string.h>

#define BUFFER_SIZE 128
#define MAX_KEY 32
#define MAX_CMD 8
#define MAX_WORDS 16
#define KEY_MAX 20
#define DELIMITERS " ,.:;!?-\t\n\r"

// Commands
#define CMD_DW     "DW" // digitalWrite
#define CMD_AW     "AW" // analogWrite
#define CMD_DR     "DR" // digitalRead
#define CMD_AR     "AR" // analogRead
#define CMD_PM     "PM" // pinMode
#define CMD_STOP   "STOP" // Command for stop reading operations
// Arguments
#define ARG_HIGH   "HIGH"
#define ARG_LOW    "LOW"
#define ARG_IN     "IN"
#define ARG_OUT    "OUT"
#define ARG_TRUE   "TRUE"
#define ARG_FALSE  "FALSE"
// Command errors
#define NOERR       0
#define ERR_PIN    -1
#define ERR_VAL    -2
#define ERR_TIME   -3
#define ERR_OVFLW  -4
#define ERR_CMD_NF -5
#define ERR_ARG    -6

struct {
    char key[KEY_MAX];
    int (*call)(int, int);
} commands[MAX_CMD];
int cmd_i = 0;
size_t nbytes;

int normalize(char*, char*[]);

int command_add(char const *, int (*)(int,int));
int command(int, char*[]);

int cmd_dw(int, int);
int cmd_aw(int, int);
int cmd_dr(int, int);
int cmd_ar(int, int);
int cmd_pm(int, int);

void setup()
{
    Serial.begin(9600);
    while (!Serial) {;}
    command_add(CMD_DW, cmd_dw);
    command_add(CMD_AW, cmd_aw);
    command_add(CMD_DR, cmd_dr);
    command_add(CMD_AR, cmd_ar);
    command_add(CMD_PM, cmd_pm);
}

void loop()
{
    static char buffer[BUFFER_SIZE];
    static char *argv[MAX_WORDS];
    static int argc, err;
    while (!(nbytes = Serial.available())) ;
    if (nbytes >= BUFFER_SIZE)
        Serial.println("Command Error: That's a very long command! My buffer cannot support it! Help!");
    else {
        Serial.readBytes(buffer, nbytes);
        buffer[nbytes] = '\0';
        argc = normalize(buffer, argv);
        if (argc == -1)
            Serial.println("Command Error: Too many words! I'm not a polyglot!");
        else {
            err = command(argc, argv);
            if (err != NOERR) {
                Serial.println("An error occurred!");
            }
        }
    }
}

int normalize(char *buffer, char *output[]) {
    int i = 0, e = -1;
    char *p = buffer, *start, *end;
    while (p != NULL) {
        p = strtok(p, DELIMITERS);
        if (p != NULL) {
            output[i++] = p;
            if (i > MAX_WORDS) return -1;
            start = p;
            e = strcspn(p, DELIMITERS);
            p = p + e;
            end = p;
            *p = '\0';
            p = p + 1;
            while (start != end) {
                *start = toupper(*start);
                start++;
            }
        }
    }
    return i;
}

int command(int argc, char *argv[]) {
    int i, cmd_found = 0;
    char *key;
    if (argc < 1)
        return ERR_CMD_NF;
    key = argv[0];
    Serial.println("Command:");
    for (i = 0; i < argc; i++) {
        Serial.println(argv[i]);
    }
    for (i = 0; i < cmd_i; i++) {
        if (strcmp(key, commands[i].key) == 0) {
            cmd_found = 1;
            Serial.println("Command found!");
        }
    }
    if (!cmd_found)
        return ERR_CMD_NF;
    return NOERR;
}

int command_add(char const *key, int (*call)(int, int)) {
    if (cmd_i < MAX_CMD) {
        strcpy(commands[cmd_i].key, key);
        commands[cmd_i].call = call;
        cmd_i++;
        return NOERR;
    } else {
        return ERR_OVFLW;
    }
}


int cmd_dw(int pin, int val) {
    if (pin == -1) return ERR_PIN;
    if (val == -1) return ERR_VAL;
    digitalWrite(pin, val);
    return NOERR;
}

int cmd_aw(int pin, int val) { return NOERR; }
int cmd_dr(int pin, int val) { return NOERR; }
int cmd_ar(int pin, int val) { return NOERR; }
int cmd_pm(int pin, int val) { return NOERR; }
