#include "crypt.h"

#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

int aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
		int aad_len, unsigned char *key, unsigned char *iv, int iv_len,
		unsigned char *ciphertext, unsigned char *tag)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	/* Initialise the encryption operation. */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		return -1;

	/* Set IV length if default 12 bytes (96 bits) is not appropriate */
	if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
		return -1;

	/* Initialise key and IV */
	if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
		return -1;

	/* Provide any AAD data. This can be called zero or more times as
	 * required
	 */
	if (1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
		return -1;

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 !=
	    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		return -1;

	ciphertext_len = len;

	/* Finalise the encryption. Normally ciphertext bytes may be written at
	 * this stage, but this does not occur in GCM mode
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		return -1;
	ciphertext_len += len;

	/* Get the tag */
	if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		return -1;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
		unsigned char *aad, int aad_len, unsigned char *tag,
		unsigned char *key, unsigned char *iv, int iv_len,
		unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	int ret;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	/* Initialise the decryption operation. */
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		return -1;

	/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
		return -1;

	/* Initialise key and IV */
	if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
		return -1;
	/* Provide any AAD data. This can be called zero or more times as
	 * required
	 */
	if (!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
		return -1;

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext,
			       ciphertext_len))
		return -1;
	plaintext_len = len;

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		return -1;

	/* Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	if (ret > 0) {
		/* Success */
		plaintext_len += len;
		return plaintext_len;
	} else {
		/* Verify failed */
		return -1;
	}
}

unsigned char *getAESKey()
{
	char *home = getenv("HOME"),
	     path[strlen(home) + 9]; // /home/<name>/.scmdkey

	static unsigned char result[1024];
	FILE *f;

	strncpy(path, home, strlen(home) + 1);
	strcat(path, "/.scmdkey");

	f = fopen(path, "r");

	fgets((char *)result, sizeof(result), f);

	return result;
}
