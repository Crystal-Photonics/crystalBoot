#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
	
typedef enum {rpcLEDStatus_none,rpcLEDStatus_off,rpcLEDStatus_on} rpcLEDStatus_t;

typedef enum {fvr_ERROR,fvr_SUCCESS} firmwareVerifyResult_t;

typedef struct{
	uint32_t githash;
	uint32_t gitDate_unix;
	char name[8];
	char version[8];
} firmware_descriptor_t;

typedef struct{
	uint32_t guid;
	uint32_t flashsize;
	uint32_t availFlashSize;
	uint16_t devID;
	uint16_t revision;
} mcu_descriptor_t;
//#pragma RPC noanswer mcuSendDataNoAnswer
	//void mcuSendDataNoAnswer(uint8_t data);

void mcuResetReadWritePointer(void);
void mcuWriteFirmwareBlock(uint8_t data_in[128]);
void mcuReadFirmwareBlock(uint8_t data_out[128]);
uint8_t mcuEraseFlash();

firmwareVerifyResult_t mcuVerifyFirmware();
firmware_descriptor_t mcuGetFirmwareDescriptor( );
mcu_descriptor_t mcuGetMCUDescriptor( );

void mcuReboot(void);

void mcuEnterProgrammerMode(void);
void mcuRunApplication(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
