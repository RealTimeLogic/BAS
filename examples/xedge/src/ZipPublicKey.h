/* When macro USE_ZIPSIGNATURE is defined.

   Enable signed ZIP file enforcement. Create 'zipPubKey; below using
   SharkSSLParseKey.exe public_key.pem

   Details:
    https://realtimelogic.com/ba/examples/xedge/readme.html#security
    https://realtimelogic.com/ba/doc/en/C/reference/html/SignEncZip.html
*/

/* Create the embedded key as follows: SharkSSLParseKey public_key.pem
   Keep the name 'zipPubKey', but insert produced data below.
*/
static const U8 zipPubKey[] =
{
   INSERT DATA HERE
};
