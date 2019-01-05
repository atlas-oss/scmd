#ifndef CRYPT_H
#define CRYPT_H

int aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
		int aad_len, unsigned char *key, unsigned char *iv, int iv_len,
		unsigned char *ciphertext, unsigned char *tag);

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
		unsigned char *aad, int aad_len, unsigned char *tag,
		unsigned char *key, unsigned char *iv, int iv_len,
		unsigned char *plaintext);

unsigned char *getAESKey();

#endif
