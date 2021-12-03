
/*
  Machine generated code designed for:
  https://realtimelogic.com/ba/doc/?url=Mako.html#acmedns

  You may integrate the "security module" with the Barracuda App
  Server build or compile into a DLL/shared-library for the Mako
  Server

  **** Barracuda App Server build:

  In your main startup code, include the following C code:

  #define EMBEDDED_ZONE_KEY
  #include "tokengen.c"

  In the main startup function, include the following C code:
  balua_tokengen(L);

  **** Mako Server:

  If you are using the pre-compiled Mako Server, compile and create a DLL/shared library as follows:

  Step 1:
  download and unzip the Mako Server's module example. See the following link for details:
  https://makoserver.net/documentation/c-modules/
  wget https://makoserver.net/download/MakoModuleExample.zip
  unzip MakoModuleExample.zip

  Step-2a:
  Compile and produce a shared library with GCC:
  gcc -IMakoModuleExample/src/lua -shared -fpic -o tokengen.so tokengen.c MakoModuleExample/src/lua/luaintf.c

  Step-2b:
  Compile and produce a DLL for Windows:
  Download the ready to use Tiny C Compiler:
  wget https://realtimelogic.com/downloads/tools/TCC-win.zip
  unzip TCC-win.zip
  tcc\tcc -IMakoModuleExample/src/lua -shared -o tokengen.dll tokengen.c MakoModuleExample/src/lua/luaintf.c

  Step-3:
  Place the DLL/shared-library in a location accessible by the Mako Server.
  Make sure you can load the module by creating a simple LSP page that runs the following code: require"tokengen"
*/

#ifndef EMBEDDED_ZONE_KEY
#include <luaintf.h>
#include <lauxlib.h>
#endif

#include <stdint.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wsequence-point"

typedef struct
{
   lua_CFunction rndbs;
} ZoneData;

static const uint8_t zkASCII[] = {
'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

#ifndef EMBEDDED_ZONE_KEY


typedef struct {
   uint32_t iv[8];
   uint64_t bits_hashed;
   uint8_t leftover[64];
   size_t leftover_offset;
} SHA256;



typedef struct
{
   SHA256 sha256Ctx;
   uint8_t key[64];
} HMACCtx;


static const uint32_t k256[64] = {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
   0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
   0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
   0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
   0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
   0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
   0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
   0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
   0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t ROTR(uint32_t a, uint32_t n)
{
   return (((a) >> n) | ((a) << (32 - n)));
}

#define Sigma0(a)(ROTR((a), 2) ^ ROTR((a), 13) ^ ROTR((a), 22))
#define Sigma1(a)(ROTR((a), 6) ^ ROTR((a), 11) ^ ROTR((a), 25))
#define sigma0(a)(ROTR((a), 7) ^ ROTR((a), 18) ^ ((a) >> 3))
#define sigma1(a)(ROTR((a), 17) ^ ROTR((a), 19) ^ ((a) >> 10))

#define Ch(a, b, c)(((a) & (b)) ^ ((~(a)) & (c)))
#define Maj(a, b, c)(((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))

static uint32_t
BigEndian(const uint8_t **c)
{
   uint32_t n = 0;
   n = (((uint32_t)(*((*c)++))) << 24);
   n |= ((uint32_t)(*((*c)++)) << 16);
   n |= ((uint32_t)(*((*c)++)) << 8);
   n |= ((uint32_t)(*((*c)++)));
   return n;
}

static void
compress(uint32_t *iv, const uint8_t *data)
{
   uint32_t a, b, c, d, e, f, g, h;
   uint32_t s0, s1;
   uint32_t t1, t2;
   uint32_t work_space[16];
   uint32_t n;
   uint32_t i;
   a = iv[0]; b = iv[1]; c = iv[2]; d = iv[3];
   e = iv[4]; f = iv[5]; g = iv[6]; h = iv[7];
   for (i = 0; i < 16; ++i) 
   {
      n = BigEndian(&data);
      t1 = work_space[i] = n;
      t1 += h + Sigma1(e) + Ch(e, f, g) + k256[i];
      t2 = Sigma0(a) + Maj(a, b, c);
      h = g; g = f; f = e; e = d + t1;
      d = c; c = b; b = a; a = t1 + t2;
   }
   for ( ; i < 64; ++i)
   {
      s0 = work_space[(i+1)&0x0f];
      s0 = sigma0(s0);
      s1 = work_space[(i+14)&0x0f];
      s1 = sigma1(s1);

      t1 = work_space[i&0xf] += s0 + s1 + work_space[(i+9)&0xf];
      t1 += h + Sigma1(e) + Ch(e, f, g) + k256[i];
      t2 = Sigma0(a) + Maj(a, b, c);
      h = g; g = f; f = e; e = d + t1;
      d = c; c = b; b = a; a = t1 + t2;
   }
   iv[0] += a; iv[1] += b; iv[2] += c; iv[3] += d;
   iv[4] += e; iv[5] += f; iv[6] += g; iv[7] += h;
}



static void
SHA256_constructor(SHA256* s)
{
   memset(s, 0, sizeof(SHA256));
   s->iv[0] = 0x6a09e667;
   s->iv[1] = 0xbb67ae85;
   s->iv[2] = 0x3c6ef372;
   s->iv[3] = 0xa54ff53a;
   s->iv[4] = 0x510e527f;
   s->iv[5] = 0x9b05688c;
   s->iv[6] = 0x1f83d9ab;
   s->iv[7] = 0x5be0cd19;
}


static void
SHA256_append(SHA256* s, const uint8_t *data, size_t datalen)
{
   while (datalen-- > 0)
   {
      s->leftover[s->leftover_offset++] = *(data++);
      if (s->leftover_offset >= 64)
      {
         compress(s->iv, s->leftover);
         s->leftover_offset = 0;
         s->bits_hashed += (64 << 3);
      }
   }
}

static void
SHA256_finish(SHA256* s, uint8_t *digest)
{
   uint32_t i;
   s->bits_hashed += (s->leftover_offset << 3);
   s->leftover[s->leftover_offset++] = 0x80; /* always room for one byte */
   if (s->leftover_offset > (sizeof(s->leftover) - 8))
   {
      memset(s->leftover + s->leftover_offset, 0,
             sizeof(s->leftover) - s->leftover_offset);
      compress(s->iv, s->leftover);
      s->leftover_offset = 0;
   }
   memset(s->leftover + s->leftover_offset, 0,
          sizeof(s->leftover) - 8 - s->leftover_offset);
   s->leftover[sizeof(s->leftover) - 1] = (uint8_t)(s->bits_hashed);
   s->leftover[sizeof(s->leftover) - 2] = (uint8_t)(s->bits_hashed >> 8);
   s->leftover[sizeof(s->leftover) - 3] = (uint8_t)(s->bits_hashed >> 16);
   s->leftover[sizeof(s->leftover) - 4] = (uint8_t)(s->bits_hashed >> 24);
   s->leftover[sizeof(s->leftover) - 5] = (uint8_t)(s->bits_hashed >> 32);
   s->leftover[sizeof(s->leftover) - 6] = (uint8_t)(s->bits_hashed >> 40);
   s->leftover[sizeof(s->leftover) - 7] = (uint8_t)(s->bits_hashed >> 48);
   s->leftover[sizeof(s->leftover) - 8] = (uint8_t)(s->bits_hashed >> 56);
   compress(s->iv, s->leftover);
   for (i = 0; i < 8; ++i)
   {
      uint32_t t = *((uint32_t *) &s->iv[i]);
      *digest++ = (uint8_t)(t >> 24);
      *digest++ = (uint8_t)(t >> 16);
      *digest++ = (uint8_t)(t >> 8);
      *digest++ = (uint8_t)(t);
   }
}


static void
HMACCtx_constructor(HMACCtx *ctx, const uint8_t *key, uint16_t keyLen)
{
   uint16_t blockLen = 64;
   uint8_t *k;
   uint16_t l4 = (blockLen >> 2);
   memset(ctx->key, 0, blockLen);
   if (keyLen <= blockLen)
   {
      memcpy(ctx->key, key, keyLen);
   }
   else
   {
      SHA256 c;
      SHA256_constructor(&c);
      SHA256_append(&c, key, keyLen);
      SHA256_finish(&c, ctx->key);
      keyLen = 32;
   }
   k = ctx->key;
   while (l4--)
   {
      *(k++) ^= 0x36;
      *(k++) ^= 0x36;
      *(k++) ^= 0x36;
      *(k++) ^= 0x36;
   }
   SHA256_constructor(&(ctx->sha256Ctx));
   SHA256_append(&(ctx->sha256Ctx), (uint8_t*)&(ctx->key), blockLen);
}


static void
HMACCtx_append(HMACCtx *ctx, const uint8_t *data, uint32_t len)
{
   SHA256_append(&(ctx->sha256Ctx), data, len);
}


static void
HMACCtx_finish(HMACCtx *ctx, uint8_t *HMAC)
{
   uint16_t blockLen = 64;
   uint8_t *k;
   uint16_t l4;
   k = ctx->key;
   l4 = (blockLen >> 2);
   while (l4--)
   {
      *(k++) ^= (0x36 ^ 0x5C);  /* turn ipad into opad */
      *(k++) ^= (0x36 ^ 0x5C);
      *(k++) ^= (0x36 ^ 0x5C);
      *(k++) ^= (0x36 ^ 0x5C);
   }
   SHA256_finish(&(ctx->sha256Ctx), HMAC);
   SHA256_constructor(&(ctx->sha256Ctx));
   SHA256_append(&(ctx->sha256Ctx), (uint8_t*)&(ctx->key), blockLen);
   SHA256_append(&(ctx->sha256Ctx), HMAC, 32);
   SHA256_finish(&(ctx->sha256Ctx), HMAC);
}

static void
PBKDF2(uint8_t *dk, const char *passphrase, const char *salt,
       uint32_t saltLen, uint32_t iterations, uint16_t dkLen)
{
   HMACCtx ctx;
   uint8_t iterBE[4], hash[32];
   uint32_t i;
   uint16_t blockLen, hashLen, j;
   hashLen = 32;
   iterBE[0] = 0;
   iterBE[1] = 0;
   iterBE[2] = 0;
   iterBE[3] = 1;
   for (;;)
   {
      HMACCtx_constructor(&ctx, (const uint8_t*)passphrase, (uint16_t)strlen(passphrase));
      HMACCtx_append(&ctx, (const uint8_t*)salt, saltLen);
      HMACCtx_append(&ctx, iterBE, 4);
      HMACCtx_finish(&ctx, hash);  /* U_1 */
      blockLen = (hashLen >= dkLen) ? hashLen : dkLen;
      memcpy(dk, hash, blockLen);
      for (i = 1; i < iterations; i++)
     
      {
         HMACCtx_constructor(&ctx, (const uint8_t*)passphrase, (uint16_t)strlen(passphrase));
         HMACCtx_append(&ctx, hash, hashLen);
         HMACCtx_finish(&ctx, hash);
         for (j = 0; j < blockLen; j++)
         {
            dk[j] ^= hash[j];
         }
      }
      if (dkLen > hashLen)
      {
         dkLen -= hashLen;
         dk += hashLen;
         /* increment iterBE */
         if (0 == ++iterBE[3])
         {
            if (0 == ++iterBE[2])
            {
               if (0 == ++iterBE[1])
               {
                  iterBE[0]++;
               }
            }
         }
      }
      else
      {
         break;
      }
   }
}
#else
#define SHA256 SharkSslSha256Ctx
#define SHA256_constructor SharkSslSha256Ctx_constructor
#define SHA256_append SharkSslSha256Ctx_append
#define SHA256_finish SharkSslSha256Ctx_finish
#define PBKDF2(dk, pwd, salt, saltLen, iterations, dkLen)               \
   sharkssl_PEM_PBKDF2(dk, pwd, salt, saltLen, iterations, dkLen, SHARKSSL_HASHID_SHA256)
#endif

static const uint8_t secData[]={
	153,
	0x90,
	0xA6,
	0xDC,
	0xBC,
	0x20,
	0x6C,
	0x90,
	0x69,
	0xED,
	0xB9,
	0x93,
	0x04,
	0x71,
	0x38,
	0x87,
	0xE5,
	0x96,
	0x37,
	0x4C,
	0x67,
	0xB0,
	0xAE,
	0x0F,
	0xF8,
	0xEE,
	0x13,
	0x87,
	0xB7,
	0xF3,
	0xDE,
	0x87,
	0x30,
	0x23,
	0x16,
	0x38,
	0x0B,
	0xDB,
	0x68,
	0x13,
	0xAF,
	0x37,
	0x2A,
	0xAD,
	0xE8,
	0x7B,
	0x4B,
	0x2E,
	0x57,
	0xE4,
	0x0D,
	0xE9,
	0x28,
	0xCA,
	0x26,
	0xEC,
	0x8A,
	0x7F,
	0x85,
	0xC2,
	0x21,
	0xE5,
	0xA2,
	0x1B,
	0x37
};

	static const char zoneKey[] ={
0xff,0xd9,0x85,0x49,0xaa,0x41,0x22,0xb6,0x74,0x90,0x64,0x83,0x79,0xcf,0x8c,0x57,0x3d,0x07,0x3d,0x96,0x41,0x22,0x94,0xa8,0xa4,0x9a,0x19,0x1f,0x29,0xf3,0xbe,0xe1
};
static int calculateToken(lua_State *L)
{
	SHA256 sha256Ctx;
	uint8_t buf[65];
	uint8_t dk[32];
	const uint8_t* rnd32;
	size_t serverIdLen;
	const uint8_t* serverId;
	ZoneData* zd=(ZoneData*)lua_touserdata(L,lua_upvalueindex(1));
	const uint8_t* serverHash = (uint8_t*)luaL_checklstring(L,2,&serverIdLen);
	if(serverIdLen != 32) luaL_error(L,"invalid");
	serverId = (uint8_t*)luaL_checklstring(L,1,&serverIdLen);
	lua_settop(L,2);
	lua_pushcfunction(L, zd->rndbs); 
	lua_pushinteger(L, 32);
	lua_call(L, 1, 1);
	rnd32 = (const uint8_t*)lua_tostring(L,3);

	buf[62] = secData[ 9] ^ secData[40];
	buf[63] = buf[62] << 4;
	buf[62] = zkASCII[buf[62] >>= 4];
	buf[63] = zkASCII[buf[63] >> 4];
	buf[30] = secData[21] ^ secData[42];
	buf[31] = buf[30] << 4;
	buf[30] = zkASCII[buf[30] >>= 4];
	buf[31] = zkASCII[buf[31] >> 4];
	buf[20] = secData[61] ^ secData[34];
	buf[21] = buf[20] << 4;
	buf[20] = zkASCII[buf[20] >>= 4];
	buf[21] = zkASCII[buf[21] >> 4];
	buf[12] = secData[ 5] ^ secData[32];
	buf[13] = buf[12] << 4;
	buf[12] = zkASCII[buf[12] >>= 4];
	buf[13] = zkASCII[buf[13] >> 4];
	buf[10] = secData[29] ^ secData[20];
	buf[11] = buf[10] << 4;
	buf[10] = zkASCII[buf[10] >>= 4];
	buf[11] = zkASCII[buf[11] >> 4];
	buf[22] = secData[27] ^ secData[38];
	buf[23] = buf[22] << 4;
	buf[22] = zkASCII[buf[22] >>= 4];
	buf[23] = zkASCII[buf[23] >> 4];
	buf[18] = secData[ 7] ^ secData[60];
	buf[19] = buf[18] << 4;
	buf[18] = zkASCII[buf[18] >>= 4];
	buf[19] = zkASCII[buf[19] >> 4];
	buf[ 8] = secData[51] ^ secData[28];
	buf[ 9] = buf[ 8] << 4;
	buf[ 8] = zkASCII[buf[ 8] >>= 4];
	buf[ 9] = zkASCII[buf[ 9] >> 4];
	buf[32] = secData[47] ^ secData[36];
	buf[33] = buf[32] << 4;
	buf[32] = zkASCII[buf[32] >>= 4];
	buf[33] = zkASCII[buf[33] >> 4];
	buf[58] = secData[15] ^ secData[ 6];
	buf[59] = buf[58] << 4;
	buf[58] = zkASCII[buf[58] >>= 4];
	buf[59] = zkASCII[buf[59] >> 4];
	buf[ 6] = secData[55] ^ secData[10];
	buf[ 7] = buf[ 6] << 4;
	buf[ 6] = zkASCII[buf[ 6] >>= 4];
	buf[ 7] = zkASCII[buf[ 7] >> 4];
	buf[ 0] = secData[23] ^ secData[30];
	buf[ 1] = buf[ 0] << 4;
	buf[ 0] = zkASCII[buf[ 0] >>= 4];
	buf[ 1] = zkASCII[buf[ 1] >> 4];
	buf[56] = secData[ 1] ^ secData[50];
	buf[57] = buf[56] << 4;
	buf[56] = zkASCII[buf[56] >>= 4];
	buf[57] = zkASCII[buf[57] >> 4];
	buf[54] = secData[57] ^ secData[22];
	buf[55] = buf[54] << 4;
	buf[54] = zkASCII[buf[54] >>= 4];
	buf[55] = zkASCII[buf[55] >> 4];
	buf[52] = secData[25] ^ secData[64];
	buf[53] = buf[52] << 4;
	buf[52] = zkASCII[buf[52] >>= 4];
	buf[53] = zkASCII[buf[53] >> 4];
	buf[16] = secData[39] ^ secData[46];
	buf[17] = buf[16] << 4;
	buf[16] = zkASCII[buf[16] >>= 4];
	buf[17] = zkASCII[buf[17] >> 4];
	buf[ 2] = secData[37] ^ secData[52];
	buf[ 3] = buf[ 2] << 4;
	buf[ 2] = zkASCII[buf[ 2] >>= 4];
	buf[ 3] = zkASCII[buf[ 3] >> 4];
	buf[50] = secData[31] ^ secData[56];
	buf[51] = buf[50] << 4;
	buf[50] = zkASCII[buf[50] >>= 4];
	buf[51] = zkASCII[buf[51] >> 4];
	buf[44] = secData[49] ^ secData[44];
	buf[45] = buf[44] << 4;
	buf[44] = zkASCII[buf[44] >>= 4];
	buf[45] = zkASCII[buf[45] >> 4];
	buf[14] = secData[ 3] ^ secData[16];
	buf[15] = buf[14] << 4;
	buf[14] = zkASCII[buf[14] >>= 4];
	buf[15] = zkASCII[buf[15] >> 4];
	buf[26] = secData[45] ^ secData[26];
	buf[27] = buf[26] << 4;
	buf[26] = zkASCII[buf[26] >>= 4];
	buf[27] = zkASCII[buf[27] >> 4];
	buf[28] = secData[35] ^ secData[58];
	buf[29] = buf[28] << 4;
	buf[28] = zkASCII[buf[28] >>= 4];
	buf[29] = zkASCII[buf[29] >> 4];
	buf[48] = secData[43] ^ secData[ 4];
	buf[49] = buf[48] << 4;
	buf[48] = zkASCII[buf[48] >>= 4];
	buf[49] = zkASCII[buf[49] >> 4];
	buf[34] = secData[17] ^ secData[14];
	buf[35] = buf[34] << 4;
	buf[34] = zkASCII[buf[34] >>= 4];
	buf[35] = zkASCII[buf[35] >> 4];
	buf[38] = secData[59] ^ secData[12];
	buf[39] = buf[38] << 4;
	buf[38] = zkASCII[buf[38] >>= 4];
	buf[39] = zkASCII[buf[39] >> 4];
	buf[42] = secData[33] ^ secData[54];
	buf[43] = buf[42] << 4;
	buf[42] = zkASCII[buf[42] >>= 4];
	buf[43] = zkASCII[buf[43] >> 4];
	buf[40] = secData[41] ^ secData[ 8];
	buf[41] = buf[40] << 4;
	buf[40] = zkASCII[buf[40] >>= 4];
	buf[41] = zkASCII[buf[41] >> 4];
	buf[24] = secData[63] ^ secData[48];
	buf[25] = buf[24] << 4;
	buf[24] = zkASCII[buf[24] >>= 4];
	buf[25] = zkASCII[buf[25] >> 4];
	buf[36] = secData[53] ^ secData[24];
	buf[37] = buf[36] << 4;
	buf[36] = zkASCII[buf[36] >>= 4];
	buf[37] = zkASCII[buf[37] >> 4];
	buf[60] = secData[13] ^ secData[18];
	buf[61] = buf[60] << 4;
	buf[60] = zkASCII[buf[60] >>= 4];
	buf[61] = zkASCII[buf[61] >> 4];
	buf[ 4] = secData[11] ^ secData[ 2];
	buf[ 5] = buf[ 4] << 4;
	buf[ 4] = zkASCII[buf[ 4] >>= 4];
	buf[ 5] = zkASCII[buf[ 5] >> 4];
	buf[46] = secData[19] ^ secData[62];
	buf[47] = buf[46] << 4;
	buf[46] = zkASCII[buf[46] >>= 4];
	buf[47] = zkASCII[buf[47] >> 4];
	buf[64]=0;
	PBKDF2(dk, (char*)buf, zoneKey, sizeof(zoneKey), 1000, 32);
	SHA256_constructor(&sha256Ctx);
	SHA256_append(&sha256Ctx, rnd32, 32);
	SHA256_append(&sha256Ctx, dk, 32);
	SHA256_append(&sha256Ctx, serverId, serverIdLen);
	SHA256_append(&sha256Ctx, serverHash, 32);
	SHA256_finish(&sha256Ctx, buf);
	lua_pushlstring(L, (char*)buf, 32);
	lua_pushvalue(L,3); /* rnd */
	return 2;
}

static int zoneInfo(lua_State *L)
{

	static const char zoneName[] ={
'l','s','p','a','p','p','m','g','r','.','r','e','a','l','t','i','m','e','l','o','g','i','c','.','c','o','m'
};
   lua_pushlstring(L, zoneName, sizeof(zoneName));
   lua_pushlstring(L, zoneKey, sizeof(zoneKey));
   return 2;
}

static const luaL_Reg lfuncs[] = {
   {"token", calculateToken},
   {"info", zoneInfo},
   {NULL, NULL}
};


static void
tokenGenLib(lua_State *L)
{
   ZoneData* zd;
   luaL_newlibtable(L,lfuncs);
   zd=(ZoneData*)lua_newuserdata(L, sizeof(ZoneData));
   lua_getglobal(L, "ba");
   lua_getfield(L,-1,"rndbs");
   zd->rndbs=lua_tocfunction(L, -1);
   lua_pop(L,2);
   luaL_setfuncs(L,lfuncs,1);
}


#ifdef EMBEDDED_ZONE_KEY
static int
installTokenGenLib(lua_State *L)
{
   tokenGenLib(L);
   return 1;
}
static void
balua_tokengen(lua_State *L)
{
   luaL_requiref(L, "etokengen", installTokenGenLib, FALSE);
   lua_pop(L, 1);
}
#else
int
#ifdef _WIN32
__declspec(dllexport) __cdecl
#endif
luaopen_tokengen(lua_State *L)
{
   luaintf(L);
   tokenGenLib(L);
   return 1;
} 
#endif

