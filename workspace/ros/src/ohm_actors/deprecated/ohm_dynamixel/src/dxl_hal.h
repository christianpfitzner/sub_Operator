#ifndef _DYNAMIXEL_HAL_HEADER
#define _DYNAMIXEL_HAL_HEADER


#ifdef __cplusplus
extern "C" {
#endif

extern int dxl_hal_open(const char* device, const float baudrate);
extern void dxl_hal_close(void);
//extern int dxl_hal_set_baud( float baudrate );
extern void dxl_hal_clear(void);
extern int dxl_hal_tx(const unsigned char *pPacket, const int numPacket);
extern int dxl_hal_rx(unsigned char *pPacket, const int numPacket);
extern void dxl_hal_set_timeout(const int NumRcvByte);
extern int dxl_hal_timeout(void);



#ifdef __cplusplus
}
#endif

#endif
