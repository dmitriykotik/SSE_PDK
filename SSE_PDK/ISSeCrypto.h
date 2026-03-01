#pragma once

enum eSSeCryptoHash
{
	eSSeCryptoHashMD5,
	eSSeCryptoHashSHA1,
	eSSeCryptoHashSHA256,
	eSSeCryptoHashSHA384,
	eSSeCryptoHashSHA512,
	eSSeCryptoChecksumCRC32
};

enum eSSeCryptoAesMode
{
	eSSeCryptoAesMode128,
	eSSeCryptoAesMode256
};


class ISSeCryptoHash
{
public:
	virtual void Release() = 0;

	virtual void Reset() = 0;
	virtual void Update(const void *input, int len) = 0;
	virtual void Final() = 0;

	// Only valid after calling Final(). Return hash in bytes, use GetHashSize() to determine hash size.
	virtual uint8* GetHash() = 0;

	// Only valid after calling Final(). Return hash in hexadecimal with null-terminated char.
	virtual const char* GetHashHex() = 0;

	// Get hash size in bytes.
	virtual uint32 GetHashSize() = 0;
};

class ISSeCryptoAES
{
public:
	virtual void Release() = 0;

	// Set up AES with the key/iv and cipher size.
	virtual void SetKey(const void* key, const void* iv, eSSeCryptoAesMode mode) = 0;

	// Encrypt a byte sequence (with a block size 16) using the AES cipher.
	virtual void EncryptCBC(const void* msg, void* out, int length) = 0;

	// Decrypt a byte sequence (with a block size 16) using the AES cipher.
	virtual void DecryptCBC(const void* in, void* out, int length) = 0;
};

class ISSeCryptoRC4
{
public:
	virtual void Release() = 0;

	// Get ready for an encrypt/decrypt operation
	virtual void Setup(const void* key, int length) = 0;

	// Perform the encrypt/decrypt operation (can use it for either since this is a stream cipher).
	virtual void Crypt(void* data, int length) = 0;
};

class ISSeCrypto
{
public:
	// Generate nRandDataBytes random bytes and write into pRandData. Set bNonZero to have non-zero individual bytes.
	virtual void GetRandom(void* pRandData, int nRandDataBytes, bool bNonZero = false) = 0;

	virtual ISSeCryptoHash* CreateHash(eSSeCryptoHash eType) = 0;
	virtual ISSeCryptoAES* CreateAES() = 0;
	virtual ISSeCryptoRC4* CreateRC4() = 0;
};
