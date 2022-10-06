// simpleserial.c

#include "simpleserial.h"
#include <stdint.h>
#include "hal.h"


#define MAX_SS_CMDS 16
static int num_commands = 0;

#define MAX_SS_LEN 256

// 0xA6 formerly 
#define CW_CRC 0x4D 
uint8_t ss_crc(uint8_t *buf, uint8_t len) {
  unsigned int k = 0;
  uint8_t crc = 0x00;
  while (len--) {
    crc ^= *buf++;
    for (k = 0; k < 8; k++) {
      crc = crc & 0x80 ? (crc << 1) ^ CW_CRC: crc << 1;
    }
  }
  return crc;
}

typedef struct ss_cmd {
  char c;
  unsigned int len;
  uint8_t (*fp)(uint8_t*, uint8_t);
  uint8_t flags;
} ss_cmd;
static ss_cmd commands[MAX_SS_CMDS];
// Callback function for "v" command.
// This can exist in v1.0 as long as we don't actually send back an ack ("z")
uint8_t check_version(uint8_t *v, uint8_t len) {
  return SS_VER;
}

uint8_t ss_num_commands(uint8_t *x, uint8_t len) {
  uint8_t ncmds = num_commands & 0xFF;
  simpleserial_put('r', 0x01, &ncmds);
  return 0x00;
}

typedef struct ss_cmd_repr {
  uint8_t c;
  uint8_t len;
  uint8_t flags;
} ss_cmd_repr;

uint8_t ss_get_commands(uint8_t *x, uint8_t len) {
  ss_cmd_repr repr_cmd_buf[MAX_SS_CMDS];
  for (uint8_t i = 0; i < (num_commands & 0xFF); i++) {
    repr_cmd_buf[i].c = commands[i].c;
    repr_cmd_buf[i].len = commands[i].len;
    repr_cmd_buf[i].flags = commands[i].flags;
  }
  
  simpleserial_put('r', num_commands * 0x03, (void *) repr_cmd_buf);
  return 0x00;
}

static char hex_lookup[16] =
  {
   '0', '1', '2', '3', '4', '5', '6', '7',
   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };

int hex_decode(int len, char* ascii_buf, uint8_t* data_buf) {
  for(int i = 0; i < len; i++) {
    char n_hi = ascii_buf[2*i];
    char n_lo = ascii_buf[2*i+1];
    
    if(n_lo >= '0' && n_lo <= '9')
      data_buf[i] = n_lo - '0';
    else if(n_lo >= 'A' && n_lo <= 'F')
      data_buf[i] = n_lo - 'A' + 10;
    else if(n_lo >= 'a' && n_lo <= 'f')
      data_buf[i] = n_lo - 'a' + 10;
    else
      return 1;
    
    if(n_hi >= '0' && n_hi <= '9')
      data_buf[i] |= (n_hi - '0') << 4;
    else if(n_hi >= 'A' && n_hi <= 'F')
      data_buf[i] |= (n_hi - 'A' + 10) << 4;
    else if(n_hi >= 'a' && n_hi <= 'f')
      data_buf[i] |= (n_hi - 'a' + 10) << 4;
    else
      return 1;
  }
  
  return 0;
}

// Set up the SimpleSerial module by preparing internal commands
// This just adds the "v" command for now...
void simpleserial_init() {
  simpleserial_addcmd('v', 0, check_version);
  simpleserial_addcmd('w', 0, ss_get_commands);
  simpleserial_addcmd('y', 0, ss_num_commands);
}

int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t)) {
  return simpleserial_addcmd_flags(c, len, fp, CMD_FLAG_NONE);
}

int simpleserial_addcmd_flags(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t), uint8_t fl) {
  if(num_commands >= MAX_SS_CMDS)
    return 1;
  
  if(len >= MAX_SS_LEN)
    return 1;
  
  commands[num_commands].c     = c;
  commands[num_commands].len   = len;
  commands[num_commands].fp    = fp;
  commands[num_commands].flags = fl;
  num_commands++;
  
  return 0;
}

void simpleserial_get(void) {
  char ascii_buf[2*MAX_SS_LEN];
  uint8_t data_buf[MAX_SS_LEN];
  char c;
  char errorcode = 0x0;
  
  // Find which command we're receiving
  c = getch();
  
  int cmd;
  for(cmd = 0; cmd < num_commands; cmd++) {
    if(commands[cmd].c == c)
      break;
  }
    
  // If we didn't find a match, give up right away
  if(cmd == num_commands) {
    errorcode = 0x1;
    goto exception;
  }
  
  // If flag CMD_FLAG_LEN is set, the next byte indicates the sent length
  if ((commands[cmd].flags & CMD_FLAG_LEN) != 0) {
    uint8_t l = 0;
    char buff[2];
    buff[0] = getch();
    buff[1] = getch();
    if (hex_decode(1, buff, &l)) {
      errorcode = 0x2;
      goto exception;
    }
    commands[cmd].len = l;
  }
  
  // Receive characters until we fill the ASCII buffer
  for(int i = 0; i < 2*commands[cmd].len; i++) {
    c = getch();
    
    // Check for early \n
    if(c == '\n' || c == '\r') {
      putch(2*commands[cmd].len);
      putch(i);
      for (int k = 0; k<i; k++)
	putch(ascii_buf[k]);
      errorcode = 0x3;
      goto exception;
    }
    
    ascii_buf[i] = c;
  }
  
  // Assert that last character is \n or \r
  c = getch();
  if(c != '\n' && c != '\r') {
    errorcode = 0x4;
    goto exception;
  }
  
  // ASCII buffer is full: convert to bytes
  // Check for illegal characters here
  if(hex_decode(commands[cmd].len, ascii_buf, data_buf)) {
    errorcode = 0x5;
    goto exception;
  }
  
  // Callback
  uint8_t ret[1];
  ret[0] = commands[cmd].fp(data_buf, commands[cmd].len);
  
  // Acknowledge (if version is 1.1)
#if SS_VER == SS_VER_1_1
  simpleserial_put('z', 1, ret);
#endif

  return;

 exception:
  putch(errorcode);
  return;

}

void simpleserial_put(char c, uint8_t size, uint8_t* output) {
  // Write first character
  putch(c);
  
  // Write each byte as two nibbles
  for(int i = 0; i < size; i++) {
      putch(hex_lookup[output[i] >> 4 ]);
      putch(hex_lookup[output[i] & 0xF]);
  }
  
  // Write trailing '\n'
  putch('\n');
}
