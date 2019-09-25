#include <ModbusRtu.h>

#include <Ethernet.h>

#include <SPI.h>
#include <Controllino.h>

#define TIME_DIFF(a,b)     ((int32_t)((uint32_t)(a) - (uint32_t)(b))

EthernetServer server(23);

void setup() {
  // put your setup code here, to run once:
  pinMode(CONTROLLINO_D0, OUTPUT);
  pinMode(CONTROLLINO_D1, OUTPUT);
  pinMode(CONTROLLINO_D2, OUTPUT);
  pinMode(CONTROLLINO_D3, OUTPUT);
  pinMode(CONTROLLINO_D4, OUTPUT);
  pinMode(CONTROLLINO_D5, OUTPUT);
  pinMode(CONTROLLINO_D6, OUTPUT);
  pinMode(CONTROLLINO_D7, OUTPUT);
  pinMode(CONTROLLINO_D8, OUTPUT);
  pinMode(CONTROLLINO_D9, OUTPUT);
  pinMode(CONTROLLINO_D10, OUTPUT);
  pinMode(CONTROLLINO_D11, OUTPUT);

  pinMode(CONTROLLINO_R0, OUTPUT);
  pinMode(CONTROLLINO_R1, OUTPUT);
  pinMode(CONTROLLINO_R2, OUTPUT);
  pinMode(CONTROLLINO_R3, OUTPUT);
  pinMode(CONTROLLINO_R4, OUTPUT);
  pinMode(CONTROLLINO_R5, OUTPUT);
  pinMode(CONTROLLINO_R6, OUTPUT);
  pinMode(CONTROLLINO_R7, OUTPUT);
  pinMode(CONTROLLINO_R8, OUTPUT);
  pinMode(CONTROLLINO_R9, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  static const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  static const IPAddress ip(192, 168, 0, 177);
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.println("--- Device ready");
}

//static int digRd(int pin) { return digitalRead(sig->pin) == HIGH) ? 1 : 0; }
//static int digWr(int pin, int val) { return digitalWrite(sig->pin, val? HIGH : LOW); }


struct SignalDesc {
  const char *name;
  int pin;
//  int (*rd)(int pin);
//  int (*wr)(int pin, int val);
};

#define PIN_TO_DESC(x) { #x, CONTROLLINO_ ## x }
 
const SignalDesc DIGOUT_SIGS[] = {
  PIN_TO_DESC(D0),
  PIN_TO_DESC(D1),
  PIN_TO_DESC(D2),
  PIN_TO_DESC(D3),
  PIN_TO_DESC(D4),
  PIN_TO_DESC(D5),
  PIN_TO_DESC(D6),
  PIN_TO_DESC(D7),
  PIN_TO_DESC(D8),
  PIN_TO_DESC(D9),
  PIN_TO_DESC(D10),
  PIN_TO_DESC(D11),
  { NULL, 0 }
};

const SignalDesc RLYOUT_SIGS[] = {
  PIN_TO_DESC(R0),
  PIN_TO_DESC(R1),
  PIN_TO_DESC(R2),
  PIN_TO_DESC(R3),
  PIN_TO_DESC(R4),
  PIN_TO_DESC(R5),
  PIN_TO_DESC(R6),
  PIN_TO_DESC(R7),
  PIN_TO_DESC(R8),
  PIN_TO_DESC(R9),
  { NULL, 0 }
};
 
 
const SignalDesc DIGIN_SIGS[] = {
  PIN_TO_DESC(A0),
  PIN_TO_DESC(A1),
  PIN_TO_DESC(A2),
  PIN_TO_DESC(A3),
  PIN_TO_DESC(A4),
  PIN_TO_DESC(A5),
  PIN_TO_DESC(A6),
  PIN_TO_DESC(A7),
  PIN_TO_DESC(A8),
  PIN_TO_DESC(A9),
  PIN_TO_DESC(IN0),
  PIN_TO_DESC(IN1),
  { NULL, 0 }
};

int printTable(char *buf, const struct SignalDesc *table) 
{
  char *p = buf;
  
  for (const struct SignalDesc *sig = table; sig->name; sig++) {
    p+=sprintf(p, "%4s", sig->name);
  }
  p+=sprintf(p, "\n");

  for (const struct SignalDesc *sig = table; sig->name; sig++) {
    p+=sprintf(p, "%4i", (digitalRead(sig->pin) == HIGH) ? 1 : 0);
  }
  p+=sprintf(p, "\n");

  return p-buf;
}


int printSig(char *buf, const struct SignalDesc *sig) 
{
  return sprintf(buf, "%4s\n%4i\n", sig->name, (digitalRead(sig->pin) == HIGH) ? 1 : 0);
}

const struct SignalDesc *findSig(const char *name, const struct SignalDesc *table) 
{
  for (const struct SignalDesc *sig = table; sig->name; sig++)
    if (!strcmp(name, sig->name))
      return sig;   

  return NULL; 
}

struct ShellState {
  char cmd[128];
  char reply[1024];
};

void strip(char *msg) {
  char *i, *o;
  for (i=msg, o=msg; *i; ++i) {
    if (*i<33 || *i>127) 
      continue; 
    else *o++=*i;
  }
  *o = '\0';
}

void shell(struct ShellState *s) 
{
  s->reply[0] = '\0';
  char *i = s->cmd;
  char *p = s->reply;

  enum { cmdNone, cmdRead, cmdSet, cmdReset } cmd;
 
  strip(s->cmd);
  Serial.println(s->cmd);
  
  switch (*i) {
  case '\0':
    p += printTable(p, DIGIN_SIGS);
    p += printTable(p, DIGOUT_SIGS);
    p += printTable(p, RLYOUT_SIGS);
    return;
    
  case '+': 
    cmd = cmdSet;
    ++i;
    break;

  case '-':
    cmd = cmdReset;
    ++i;
    break;

  case 'D':
  case 'R':
  case 'I':
  case 'A':
    cmd = cmdRead;
    break;

  default:
    return;    
  } 

  const struct SignalDesc *sig = NULL;
  switch (cmd) {
    case cmdNone: 
      break;

    case cmdRead:
    case cmdSet:
    case cmdReset:
      sig = findSig(i, DIGIN_SIGS);
      if (!sig) sig = findSig(i, DIGOUT_SIGS);
      if (!sig) sig = findSig(i, RLYOUT_SIGS);
      if (!sig) return;
      break;
  }
  
  switch (cmd) {
    case cmdRead: 
      p += printSig(p, sig);
      break;

    case cmdSet:
      digitalWrite(sig->pin, HIGH);
      p += printSig(p, sig);
      break;

    case cmdReset:
      digitalWrite(sig->pin, LOW);
      p += printSig(p, sig);
      break;
  }
}

struct ShellState st;

void loop() 
{
  
  
  EthernetClient client = server.available();
  if (client) {
    int msglen = client.read(st.cmd, sizeof(st.cmd));
    st.cmd[msglen] = '\0';
    Serial.print(st.cmd);

    shell(&st);
    int len = strlen(st.reply);
    if (len) {
      client.write(st.reply, len);
      Serial.print(st.reply);     
    }
  }
}
