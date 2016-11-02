#ifndef TINYAES128C_WRAPPER_H
#define TINYAES128C_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);





void AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

#ifdef __cplusplus
}
#endif

#endif // TINYAES128C_WRAPPER_H

