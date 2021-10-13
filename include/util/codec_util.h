#ifndef CODEC_UTIL_H
#define CODEC_UTIL_H

#include <stdint.h>

#include <string>

namespace qcloud_cos {

class CodecUtil {
 public:
  /**
   * @brief 将字符x转成十六进制 (x的值[0, 15])
   *
   * @param x
   *
   * @return 十六进制字符
   */
  static unsigned char ToHex(const unsigned char& x);

  /**
   * @brief 将二进制数据转成十六进制 (x的值[0, 15]),上层调用保证hex的大小足够
   *
   * @param bin
   *
   * @param binLen
   *
   * @param hex 存放结果的数据块
   *
   * @return 无
   */
  static void BinToHex(const unsigned char* bin, unsigned int binLen,
                       char* hex);

  static std::string EncodeKey(const std::string& key);

  /**
   * @brief 对字符串进行URL编码
   *
   * @param str   带编码的字符串
   *
   * @return  经过URL编码的字符串
   */
  static std::string UrlEncode(const std::string& str);

  /**
   * @brief 对字符串进行base64编码
   *
   * @param plainText  待编码的字符串
   *
   * @return 编码后的字符串
   */
  static std::string Base64Encode(const std::string& plainText);

  /**
   * @brief 获取hmacSha1值
   *
   * @param plainText  明文
   * @param key        秘钥
   *
   * @return 获取的hmacsha1值
   */
  static std::string HmacSha1(const std::string& plainText,
                              const std::string& key);

  /**
   * @brief 获取hmacSha1的16进制值
   *
   * @param plainText  明文
   * @param key        秘钥
   *
   * @return 获取的hmacsha1的16进制值
   */
  static std::string HmacSha1Hex(const std::string& plainText,
                                 const std::string& key);

  static std::string RawMd5(const std::string& plainText);

  static std::string RawMd51(const std::string& plainText);

  static std::string HexToBin(const std::string& strHex);
};

}  // namespace qcloud_cos
#endif
