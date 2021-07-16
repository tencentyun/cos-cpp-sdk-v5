#include "util/codec_util.h"

#include <cassert>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "Poco/SHA1Engine.h"
#include "Poco/HMACEngine.h"
#include "Poco/MD5Engine.h"

#include "util/file_util.h"

namespace qcloud_cos {

#define REVERSE_HEX(c) ( ((c) >= 'A') ? ((c) & 0xDF) - 'A' + 10 : (c) - '0' )
#define HMAC_LENGTH 20

unsigned char CodecUtil::ToHex(const unsigned char &x) {
    return x > 9 ? (x - 10 + 'A') : x + '0';
}

void CodecUtil::BinToHex(const unsigned char *bin,unsigned int binLen, char *hex) {
    for(unsigned int i = 0; i < binLen; ++i) {
        hex[i<<1] = ToHex(bin[i] >> 4);
        hex[(i<<1)+1] = ToHex(bin[i] & 15 );
    }
}

std::string CodecUtil::EncodeKey(const std::string& key) {
    std::string encodedKey = "";
    std::size_t length = key.length();
    for (size_t i = 0; i < length; ++i) {
        if (isalnum((unsigned char)key[i]) ||
            (key[i] == '-') ||
            (key[i] == '_') ||
            (key[i] == '.') ||
            (key[i] == '~') ||
            (key[i] == '/')) {
            encodedKey += key[i];
        } else {
            encodedKey += '%';
            encodedKey += ToHex((unsigned char)key[i] >> 4);
            encodedKey += ToHex((unsigned char)key[i] % 16);
        }
    }
    return encodedKey;
}

std::string CodecUtil::UrlEncode(const std::string& str) {
    std::string encodedUrl = "";
    std::size_t length = str.length();
    for (size_t i = 0; i < length; ++i) {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~')) {

            encodedUrl += str[i];
        } else {
            encodedUrl += '%';
            encodedUrl += ToHex((unsigned char)str[i] >> 4);
            encodedUrl += ToHex((unsigned char)str[i] % 16);
        }
    }
    return encodedUrl;
}

std::string CodecUtil::Base64Encode(const std::string& plain_text) {
    static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const std::size_t plain_text_len = plain_text.size();
    std::string retval((((plain_text_len + 2) / 3) * 4), '=');
    std::size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    const std::string::const_iterator plain_text_end = plain_text.end();

    for (std::string::const_iterator i = plain_text.begin(); i != plain_text_end; ++i) {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }

    if (bits_collected > 0) {
        assert(bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }
    assert(outpos >= (retval.size() - 2));
    assert(outpos <= retval.size());
    return retval;
}

std::string CodecUtil::HmacSha1Hex(const std::string& plain_text,const std::string& key) {
    Poco::HMACEngine<Poco::SHA1Engine> hmac_engine(key);
    hmac_engine.update(plain_text);
    return Poco::DigestEngine::digestToHex(hmac_engine.digest());
}

std::string CodecUtil::RawMd5(const std::string& plainText) {
    Poco::MD5Engine md5_engine;
    md5_engine.update(plainText);
    Poco::DigestEngine::Digest digest = md5_engine.digest();
    std::string raw_md5(digest.begin(), digest.end());
    return raw_md5;
}

// convert a hexadecimal string to binary value
std::string CodecUtil::HexToBin(const std::string &strHex) {
    if (strHex.size() % 2 != 0) {
        return "";
    }

    std::string strBin;
    strBin.resize(strHex.size() / 2);
    for (size_t i = 0; i < strBin.size(); i++) {
        uint8_t cTmp = 0;
        for (size_t j = 0; j < 2; j++) {
            char cCur = strHex[2 * i + j];
            if (cCur >= '0' && cCur <= '9') {
                cTmp = (cTmp << 4) + (cCur - '0');
            }
            else if (cCur >= 'a' && cCur <= 'f') {
                cTmp = (cTmp << 4) + (cCur - 'a' + 10);
            }
            else if (cCur >= 'A' && cCur <= 'F') {
                cTmp = (cTmp << 4) + (cCur - 'A' + 10);
            }
            else {
                return "";
            }
        }
        strBin[i] = cTmp;
    }
    return strBin;
}// end of HexToBin

}
