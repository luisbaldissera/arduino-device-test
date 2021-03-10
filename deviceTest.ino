#include <string.h>

// Limits
#define BUFFER_SIZE 128
#define MAX_KEY 32
#define MAX_CMD 8
#define MAX_ARG 16
#define MAX_WORDS 16
#define KEY_MAX 20
#define ARG_MAX 16
#define DELIMITERS " ,.:;!?-\t\n\r"
// Commands
#define CMD_DW     "DW" // digitalWrite
#define CMD_AW     "AW" // analogWrite
#define CMD_DR     "DR" // digitalRead
#define CMD_AR     "AR" // analogRead
#define CMD_PM     "PM" // pinMode
#define CMD_STOP   "STOP" // Command for stop reading operations
// Arguments
#define ARG_HIGH    "HIGH"
#define ARG_LOW     "LOW"
#define ARG_IN      "IN"
#define ARG_OUT     "OUT"
#define ARG_TRUE    "TRUE"
#define ARG_FALSE   "FALSE"
#define ARG_A0      "A0"
#define ARG_A1      "A1"
#define ARG_A2      "A2"
#define ARG_A3      "A3"
#define ARG_A4      "A4"
#define ARG_A5      "A5"
// Command errors
#define NOERR         0
#define ERR_PIN      -1
#define ERR_VAL      -2
#define ERR_TIME     -3
#define ERR_OVFLW    -4
#define ERR_CMD      -5
#define ERR_ARG      -6

struct {
    char key[KEY_MAX];
    int (*call)(int, int);
} commands[MAX_CMD];
int cmd_i = 0;

struct {
    char arg[ARG_MAX];
    int value;
} args[MAX_ARG];
int arg_i = 0;

size_t nbytes;

int normalize(char*, char*[]);

int command_add(char const *, int (*)(int,int));
int command(int, char*[]);

int arg_add(char const *, int);
int arg(char const *);

int cmd_dw(int, int);
int cmd_aw(int, int);
int cmd_dr(int, int);
int cmd_ar(int, int);
int cmd_pm(int, int);

void setup()
{
    Serial.begin(9600);
    while (!Serial) {;}
    // Register commands
    command_add(CMD_DW, cmd_dw);
    command_add(CMD_AW, cmd_aw);
    command_add(CMD_DR, cmd_dr);
    command_add(CMD_AR, cmd_ar);
    command_add(CMD_PM, cmd_pm);
    // Register args dictionary
    arg_add(ARG_HIGH, HIGH);
    arg_add(ARG_LOW, LOW);
    arg_add(ARG_IN, INPUT);
    arg_add(ARG_OUT, OUTPUT);
    arg_add(ARG_TRUE, true);
    arg_add(ARG_FALSE, false);
    arg_add(ARG_A0, A0);
    arg_add(ARG_A1, A1);
    arg_add(ARG_A2, A2);
    arg_add(ARG_A3, A3);
    arg_add(ARG_A4, A4);
    arg_add(ARG_A5, A5);

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
            Serial.println("Executing...");
            err = command(argc, argv);
            if (err == NOERR)
                Serial.println("OK");
            else
                Serial.println("An error occurred!");
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
    int i, cmd_found = 0, pin, val;
    char *key;
    if (argc < 3)
        return ERR_CMD;
    key = argv[0];
    pin = arg(argv[1]);
    val = arg(argv[2]);
    for (i = 0; i < cmd_i; i++) {
        if (strcmp(key, commands[i].key) == 0) {
            cmd_found = 1;
            return commands[i].call(pin, val);
        }
    }
    if (!cmd_found)
        return ERR_CMD;
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

int arg_add(char const *argname, int value) {
    if (arg_i < MAX_ARG) {
        strcpy(args[arg_i].arg, argname);
        args[arg_i].value = value;
        arg_i++;
        return NOERR;
    } else {
        return ERR_OVFLW;
    }
}

int arg(char const *argname) {
    int i;
    for (i = 0; i < arg_i; i++) {
        if (strcmp(args[i].arg, argname) == 0) {
            return args[i].value;
        }
    }
    return atoi(argname);
}

int cmd_dw(int pin, int val) {
    digitalWrite(pin, val);
    return NOERR;
}

int cmd_aw(int pin, int val) {
    analogWrite(pin, val);
    return NOERR;
}

int cmd_dr(int pin, int val) { return NOERR; }
int cmd_ar(int pin, int val) { return NOERR; }

int cmd_pm(int pin, int val) {
    pinMode(pin,val);
    return NOERR;
}
