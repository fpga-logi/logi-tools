#ifndef LOGILIB_H
#define LOGILIB_H


#ifdef __cplusplus
extern "C" {
#endif
int logi_open(void);
void logi_close(void);

int logi_write(unsigned char * buffer, unsigned int length, unsigned int address);
int logi_read(unsigned char * buffer, unsigned int length, unsigned int address);
#ifdef __cplusplus
}
#endif


#endif

