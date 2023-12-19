
String cmd_return = "";
const String MSG_CR_LF = "\r\n";


class DualPrint 
{
public:
    DualPrint() : send_serial(true), buffer_cmd(true) {}

    virtual size_t print(const String &c)                    { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(char c)                             { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned char c, int i = DEC)       { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(int c, int i = DEC)                 { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned int c, int i = DEC)        { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(long c, int i = DEC)                { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned long c, int i = DEC)       { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(double c, int = 2)                  { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }

    virtual size_t println(const String &c)                  { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(char c)                           { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned char c, int i = DEC)     { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(int c, int i = DEC)               { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned int c, int i = DEC)      { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(long c, int i = DEC)              { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned long c, int i = DEC)     { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(double c, int i = 2)              { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(void)                             { if (send_serial) { Serial.println(""); }  if (buffer_cmd) { cmd_return += MSG_CR_LF; }    return 1; }

    bool send_serial, buffer_cmd;
} out;



void setup() {

  // put your setup code here, to run once:
  Serial.begin(57600);           // set up Serial library at 57600 bps
  out.println("POLARGRAPH ON!");
  //DualPrint.print("Hardware: ");
}

void loop() {
  // put your main code here, to run repeatedly:

}
