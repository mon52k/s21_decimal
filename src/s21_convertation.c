#include "s21_decimal.h"

// int s21_from_int_to_decimal(int src, s21_decimal *dec) {
//   int res = OK;
//   if (dec) {
//     s21_reset_decimal(dec);
//     if (src < 0) {
//       src *= -1;
//       s21_set_sign(dec);
//     }
//     dec->bits[0] = src;
//   } else {
//     res = CONVERTING_ERROR;
//   }
//   return res;
// }

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = 0;
  if (dst == NULL) {
    return 1;
  }
  s21_reset_decimal(dst);
  if (src > 0) {
    dst->bits[0] = src;
  } else if (src < 0) {
    src = -src;
    dst->bits[0] = src;
    s21_set_sign(dst);
  } else {
    result = 1;
  }
  return result;
}

int s21_from_float_to_decimal(float src, s21_decimal *dec) {
  int res = 0;
  if (src == 1 / 0.0 && src == 0 / 0.0 && src == -1 / 0.0 && src == -0 / 0.0)
    res = 1;
  s21_reset_decimal(dec);
  if (src != 0) {
    s21_reset_decimal(dec);
    if (dec && !res) {
      int is_negative = 0;
      if (src < 0) {
        is_negative = 1;
        src *= -1;
      }
      double db = src;
      int exp = s21_get_expon((float)db);
      int scale = 0;
      while (scale < 28 && (int)db / (int)pow(2, 21) == 0) {
        db *= 10;
        scale++;
      }
      db = round(db);
      if (scale <= 28 && (exp > -94 && exp < 96)) {
        while (fmod(db, 10) == 0 && scale > 0) {
          db = db / 10;
          scale--;
        }
        exp = s21_get_expon((float)db);
        s21_set_dec_bits(dec, exp);
        s21_set_mantissa_to_decimal(dec, exp, (float)db);
        s21_set_scale(dec, scale);
        if (is_negative) {
          s21_set_sign(dec);
        }
      } else {
        res = 1;
      }
    }
  }
  return res;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  *dst = 0;
  int res = s21_truncate(src, &src);
  if (res != OK || src.bits[1] || src.bits[2] || src.bits[0] > 2147483647) {
    res = CONVERTING_ERROR;
  } else {
    *dst = src.bits[0];
    if (s21_get_sign(src)) *dst *= -1;
  }
  return res;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int res = OK;
  double tmp = 0;
  int scale = 0;
  if (dst) {
    for (int i = 0; i < 96; i++)
      if (s21_check_dec_bits(src, i)) tmp += pow(2, i);
    for (int i = 16; i < 24; i++)
      if (s21_check_bits(src.bits[3], i)) scale = s21_set_bits(scale, i - 16);
    if (scale > 0) {
      for (int i = scale; i > 0; i--) tmp /= 10.0;
    }
    *dst = (float)tmp;
    *dst *= src.bits[3] >> 31 ? -1 : 1;
  } else {
    res = CONVERTING_ERROR;
  }
  return res;
}