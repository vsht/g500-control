#ifndef G500_CONTROL_H
#define G500_CONTROL_H



#define VERSION "0.0.2"

#define VENDOR 0x046d

/* means that we are not interested what G500 replies to the information recieved */
/* for debugging purposes change this to 1 */
#define DEBUG_INFOS 0

/* G500's signature*/
#define MOUSE_G500 0xc068

static int send_report20(int fd, const unsigned char *buf, size_t size);

static int send_report(int fd, const unsigned char *buf, size_t size);
void send_msg(int fd, const unsigned char c0,
                      const unsigned char c1,
                      const unsigned char c2,
                      const unsigned char c3,
                      const unsigned char c4,
                      const unsigned char c5);

void send_msg20(int fd, const unsigned char c0,
                        const unsigned char c1,
                        const unsigned char c2,
                        const unsigned char c3,
                        const unsigned char c4,
                        const unsigned char c5,
                        const unsigned char c6,
                        const unsigned char c7,
                        const unsigned char c8,
                        const unsigned char c9,
                        const unsigned char c10,
                        const unsigned char c11,
                        const unsigned char c12,
                        const unsigned char c13,
                        const unsigned char c14,
                        const unsigned char c15,
                        const unsigned char c16,
                        const unsigned char c17,
                        const unsigned char c18 );

static int query_report(int fd, int id, unsigned int *buf, size_t size);
void hid_reply(int fd);
void g500_reply (int fd, const int debug_infos);
int dpi_convert (const int dpi_value);
void display_help();


#endif /* G500_CONTROL */
