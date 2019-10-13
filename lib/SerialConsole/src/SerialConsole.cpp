#include "SerialConsole.h"

SerialConsole::SerialConsole() {
    disableEcho();
}

SerialConsole::SerialConsole(Stream & serial) : SerialConsole() {
    begin(serial);
}

void SerialConsole::begin(Stream & serial) {
    m_buf_pos = 0;
    m_param_pos = 0;
    m_state = READ_TOK;
    m_params[0] = m_buf;
    m_serial = & serial;
}

void SerialConsole::poll() {
    if (!m_serial->available()) return;
    
    char r = (char) m_serial->read();

    if (m_echo) m_serial->print(r);

    switch (m_state) {
        case READ_TOK:
            if (r == ' ') {
                addChar('\0');
                m_state = READ_SPACE;
            } else if ((r == '\r') || (r == '\n')) {
                m_buf[m_buf_pos] = '\0';
                m_state = READ_ENDL;
                if (r == '\n') onCommand();
            } else {
                addChar(r);
            }
            break;
        
        case READ_SPACE:
            if ((r == '\r') || (r == '\n')) {
                if (r == '\n') onCommand();
                m_state = READ_ENDL;
            } else if (r != ' ') {
                newParam();
                addChar(r);
                m_state = READ_TOK;
            }
            break;

        case READ_ENDL:
            if (r == ' ') {
                m_state = READ_SPACE;
            } else if (r == '\n') {
                onCommand();
            } else if ((r != '\r') && (r != '\n')) {
                addChar(r);
                m_state = READ_TOK;
            }
            break;
    }
}

void SerialConsole::onCommand() {
    if (!m_buf_pos) return;

    commandHandler(m_param_pos + 1, m_params);

    m_buf_pos = 0;
    m_param_pos = 0;
}

void SerialConsole::addChar(char c) {
    if (m_buf_pos != MAX_COMMAND_LENGTH) {
        m_buf[m_buf_pos++] = (char) c;
    } else {
      m_buf_pos = 0;
      m_param_pos = 0;
      errorHandler(ERR_SERIAL_MAX_LENGTH);
    }
}

void SerialConsole::newParam() {
    m_param_pos++;

    if (m_param_pos == MAX_COMMAND_PARAMETERS) {
      m_buf_pos = 0;
      m_param_pos = 0;
      errorHandler(ERR_SERIAL_MAX_PARAMETERS);
    } else {
        m_params[m_param_pos] = &m_buf[m_buf_pos];
    }
}

void SerialConsole::enableEcho() { m_echo = true; }
void SerialConsole::disableEcho() { m_echo = false; }
