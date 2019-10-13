#ifndef __SERIAL_CONSOLE_H__
#define __SERIAL_CONSOLE_H__

#ifndef MAX_COMMAND_LENGTH
    #define  MAX_COMMAND_LENGTH 64
#endif

#ifndef MAX_COMMAND_PARAMETERS
    #define MAX_COMMAND_PARAMETERS 5
#endif

#include <Arduino.h>

typedef enum {
    READ_TOK,
    READ_SPACE,
    READ_ENDL
} state;

typedef enum {
    ERR_SERIAL_MAX_LENGTH,
    ERR_SERIAL_MAX_PARAMETERS
} consoleError;

class SerialConsole {
    public:
        SerialConsole();
        SerialConsole(Stream & serial);
        
        void begin(Stream & serial);
        void poll();
        void enableEcho();
        void disableEcho();
        void addChar(char c);
        void newParam();
        void onCommand();
        
        void commandHandler(int argc, char ** argv);
        void errorHandler(consoleError err);

        char m_buf[MAX_COMMAND_LENGTH];
        int8_t m_buf_pos;
        int8_t m_param_pos;
        bool m_echo;
        char * m_params[MAX_COMMAND_PARAMETERS];
        state m_state;
        Stream * m_serial;
};

#endif