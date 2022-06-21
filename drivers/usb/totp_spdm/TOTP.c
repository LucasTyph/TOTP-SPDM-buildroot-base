/*
* Adapted from https://github.com/Netthaw/TOTP-MCU
*/

#include <linux/module.h>
#include "TOTP.h"
#include "sha1.h"

uint8_t* _hmacKey;
uint8_t _keyLength;
uint8_t _timeZoneOffset;
uint32_t _timeStep;

// Init the library with the private key, its length and the timeStep duration
void TOTP(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep) {
    _hmacKey = hmacKey;
    _keyLength = keyLength;
    _timeStep = timeStep;
}
EXPORT_SYMBOL(TOTP);

void setTimezone(uint8_t timezone){
    _timeZoneOffset = timezone;
}
EXPORT_SYMBOL(setTimezone);

uint32_t TimeStruct2Timestamp(struct tm time){
    //time.tm_mon -= 1;
    //time.tm_year -= 1900;
    return mktime(time.tm_year, time.tm_mon, time.tm_mday,
        time.tm_hour, time.tm_min, time.tm_sec) -
        (_timeZoneOffset * 3600) - 2208988800;
}
EXPORT_SYMBOL(TimeStruct2Timestamp);

// Generate a code, using the timestamp provided
uint32_t getCodeFromTimestamp(uint32_t timeStamp) {
    uint32_t steps = timeStamp / _timeStep;
    return getCodeFromSteps(steps);
}
EXPORT_SYMBOL(getCodeFromTimestamp);

// Generate a code, using the timestamp provided
uint32_t getCodeFromTimeStruct(struct tm time) {
    return getCodeFromTimestamp(TimeStruct2Timestamp(time));
}
EXPORT_SYMBOL(getCodeFromTimeStruct);

// Generate a code, using the number of steps provided
uint32_t getCodeFromSteps(uint32_t steps) {
    uint8_t* _hash;
    uint32_t _truncatedHash;
    uint8_t _offset;
    uint8_t j;

    // STEP 0, map the number of steps in a 8-bytes array (counter value)
    uint8_t _byteArray[8];
    _byteArray[0] = 0x00;
    _byteArray[1] = 0x00;
    _byteArray[2] = 0x00;
    _byteArray[3] = 0x00;
    _byteArray[4] = (uint8_t)((steps >> 24) & 0xFF);
    _byteArray[5] = (uint8_t)((steps >> 16) & 0xFF);
    _byteArray[6] = (uint8_t)((steps >> 8) & 0XFF);
    _byteArray[7] = (uint8_t)((steps & 0XFF));

    // STEP 1, get the HMAC-SHA1 hash from counter and key
    initHmac(_hmacKey, _keyLength);
    writeArray(_byteArray, 8);
    _hash = resultHmac();

    // STEP 2, apply dynamic truncation to obtain a 4-bytes string
    _truncatedHash = 0;
    _offset = _hash[20 - 1] & 0xF;
    for (j = 0; j < 4; ++j) {
        _truncatedHash <<= 8;
        _truncatedHash  |= _hash[_offset + j];
    }

    // STEP 3, compute the OTP value
    _truncatedHash &= 0x7FFFFFFF;    //Disabled
    _truncatedHash %= 1000000;

    return _truncatedHash;
}
EXPORT_SYMBOL(getCodeFromSteps);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("TOTP Implementation");
MODULE_VERSION("1.0");