/*
  Push the primary secret key material as a Lua string. 

  We utilize White-box Cryptography (WBC) to obfuscate the secret
  key material. The Lua TPM (Trusted Platform Module)
  implementation doesn't require a specific fixed secret key; the
  only requirement is that a pre-selected, randomly chosen number
  remains consistent across reboots. This ensures persistence
  without exposing the actual key.

  The transformation applied below uses an S-box substitution
  technique to obscure the key's value, adding an extra layer of
  protection.
*/
baAssert(sizeof(ENCRYPTIONKEY) > 255); /* ENCRYPTIONKEY too short */
luaL_Buffer b;
size_t i;
U8* transformedKey = (U8*)luaL_buffinitsize(L,&b,sizeof(ENCRYPTIONKEY));
for (i = 0; i < sizeof(ENCRYPTIONKEY); i++) {
   /* Apply S-box substitution; This would crash if ENCRYPTIONKEY < 256 */
   transformedKey[i] = ENCRYPTIONKEY[ENCRYPTIONKEY[i]] ^ ENCRYPTIONKEY[i];
}
luaL_addsize(&b, sizeof(ENCRYPTIONKEY));
luaL_pushresult(&b);
