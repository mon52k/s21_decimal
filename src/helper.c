#include "s21_decimal.h"

int s21_check_bits(int value, int position) {
  return (value & (1 << position)) != 0;
}

int s21_set_bits(int value, int position) { return value | (1 << position); }

int s21_reset_bit(int value, int position) {
  return value & (~(1 << position));
}

void s21_set_sign(s21_decimal *dec) {
  dec->bits[3] = s21_set_bits(dec->bits[3], 31);
}

void s21_reset_sign(s21_decimal *dec) {
  dec->bits[3] = s21_reset_bit(dec->bits[3], 31);
}

void s21_reset_decimal(s21_decimal *dec) {
  for (int i = 0; i < 4; i++) {
    dec->bits[i] = 0;
  }
}

int s21_get_expon(float src) {
  unsigned int res = 0;
  unsigned int ftoint = *((unsigned *)&src);
  for (int i = 23; i < 31; i++) {
    if (s21_check_bits(ftoint, i)) res = s21_set_bits(res, i);
  }
  return (res >> 23) - 127;
}

void s21_set_dec_bits(s21_decimal *dec, int position) {
  int byteind = position / 32;
  int bitsind = position % 32;
  dec->bits[byteind] = s21_set_bits(dec->bits[byteind], bitsind);
}

int s21_check_dec_bits(s21_decimal dec, int position) {
  int byteind = position / 32;
  int bitsind = position % 32;
  return (dec.bits[byteind] & (1 << bitsind)) != 0;
}

void s21_set_mantissa_to_decimal(s21_decimal *dec, int exp, float src) {
  unsigned tmp = *((unsigned *)&src);
  for (int i = exp - 1, j = 22; i >= 0 && j >= 0; i--, j--) {
    if (s21_check_bits(tmp, j)) {
      s21_set_dec_bits(dec, i);
    }
  }
}

void s21_set_scale(s21_decimal *dec, int scale) {
  for (int i = 16, j = 0; i < 24; i++, j++) {
    if (s21_check_bits(scale, j)) dec->bits[3] = s21_set_bits(dec->bits[3], i);
  }
}

int s21_get_sign(s21_decimal dec) { return dec.bits[3] >> 31; }

int s21_get_scale(const s21_decimal dec) { return (dec.bits[3] & SC) >> 16; }

int s21_shift_left(s21_decimal *dec, int shift) {
  int res = 0;
  int highest_bits = s21_get_highest_bits(*dec);
  int last_low_byte_bits = 0;
  int last_mid_byte_bits = 0;
  if (highest_bits + shift > 95) {
    res = 1;
  } else {
    for (int i = 0; i < shift; i++) {
      last_low_byte_bits = s21_check_dec_bits(*dec, 31);
      last_mid_byte_bits = s21_check_dec_bits(*dec, 63);
      dec->bits[0] = dec->bits[0] << 1;
      dec->bits[1] = dec->bits[1] << 1;
      dec->bits[2] = dec->bits[2] << 1;
      if (last_low_byte_bits) s21_set_dec_bits(dec, 32);
      if (last_mid_byte_bits) s21_set_dec_bits(dec, 64);
    }
  }
  if (s21_get_sign(*dec) && res) res = 2;
  return res;
}

s21_decimal s21_binary_div(s21_decimal dec1, s21_decimal dec2,
                           s21_decimal *reminder, int *fail) {
  s21_reset_decimal(reminder);
  s21_decimal end = {{0, 0, 0, 0}};
  int end_scale = 0;
  *fail = s21_normalization(&dec1, &dec2, &end_scale);
  for (int i = s21_get_highest_bits(dec1); i >= 0; i--) {
    if (s21_check_dec_bits(dec1, i)) s21_set_dec_bits(reminder, 0);
    if (s21_is_greater_or_equal(*reminder, dec2)) {
      *fail = s21_sub_without_scale(*reminder, dec2, reminder);
      if (i != 0) *fail = s21_shift_left(reminder, 1);
      if (s21_check_dec_bits(dec1, i - 1)) s21_set_dec_bits(reminder, 0);
      *fail = s21_shift_left(&end, 1);
      s21_set_dec_bits(&end, 0);
    } else {
      *fail = s21_shift_left(&end, 1);
      if (i != 0) *fail = s21_shift_left(reminder, 1);
      if (i - 1 >= 0 && s21_check_dec_bits(dec1, i - 1))
        s21_set_dec_bits(reminder, 0);
    }
  }
  s21_set_scale(reminder, end_scale);
  return end;
}

int s21_get_highest_bits(s21_decimal dec) {
  int i = 95;
  while (!s21_check_dec_bits(dec, i) && i >= 0) {
    i--;
  }
  return i;
}

int s21_div_by_10(s21_decimal *dec, int scale) {
  int ret = 0;
  while (scale--) {
    s21_decimal reminder;
    s21_decimal ten = {{10, 0, 0, 0}};
    *dec = s21_binary_div(*dec, ten, &reminder, &ret);
    if (ret != 0) break;
  }
  return ret;
}

int s21_mul_by_10(s21_decimal *dec, int scale) {
  int res = 0;
  while (scale--) {
    s21_decimal buf1 = *dec, buf2 = *dec;
    res = s21_shift_left(&buf2, 3);
    if (res != 0) break;
    res = s21_shift_left(&buf1, 1);
    if (res != 0) break;
    res = s21_add(buf2, buf1, dec);
    if (res != 0) break;
  }
  return res;
}

int s21_normal(s21_decimal *dec1, s21_decimal *dec2, int scale) {
  int scale1 = s21_get_scale(*dec1);
  int scale2 = s21_get_scale(*dec2);
  int res = 0;
  int sign1 = s21_get_sign(*dec1);
  dec1->bits[3] = 0;
  int sign2 = s21_get_sign(*dec2);
  dec2->bits[3] = 0;
  if (scale1 > scale)
    res = s21_div_by_10(dec1, scale1 - scale);
  else if (scale1 < scale)
    res = s21_mul_by_10(dec1, scale - scale1);
  if (scale2 > scale)
    res = s21_div_by_10(dec2, scale2 - scale);
  else if (scale2 < scale)
    res = s21_mul_by_10(dec2, scale - scale2);
  s21_set_scale(dec1, scale);
  s21_set_scale(dec2, scale);
  if (sign1) s21_set_sign(dec1);
  if (sign2) s21_set_sign(dec2);
  return res;
}

int s21_normalization(s21_decimal *dec1, s21_decimal *dec2, int *final_scale) {
  s21_decimal tmp1 = *dec1;
  s21_decimal tmp2 = *dec2;
  int scale1 = s21_get_scale(tmp1);
  int scale2 = s21_get_scale(tmp2);
  int res = 0;
  int fscale = scale1;
  if (scale1 > scale2) {
    res = s21_normal(&tmp1, &tmp2, scale1);
    fscale = scale1;
    if (res != 0) {
      fscale = scale2;
      tmp1 = *dec1;
      tmp2 = *dec2;
      res = s21_normal(&tmp1, &tmp2, scale2);
    }
  } else if (scale1 < scale2) {
    res = s21_normal(&tmp1, &tmp2, scale2);
    fscale = scale2;
    if (res != 0) {
      fscale = scale1;
      tmp1 = *dec1;
      tmp2 = *dec2;
      res = s21_normal(&tmp1, &tmp2, scale1);
    }
  }
  *final_scale = fscale;
  *dec1 = tmp1;
  *dec2 = tmp2;
  return res;
}

void s21_sub_bits(s21_decimal dec1, s21_decimal dec2, s21_decimal *end) {
  s21_reset_decimal(end);
  if (!s21_is_equal(dec1, dec2)) {
    int dec1_last_bits = s21_get_highest_bits(dec1);
    int buffer = 0;
    int dec1_curbits = 0;
    int dec2_curbits = 0;
    for (int i = 0; i <= dec1_last_bits; i++) {
      dec1_curbits = s21_check_dec_bits(dec1, i);
      dec2_curbits = s21_check_dec_bits(dec2, i);
      if (!dec1_curbits && !dec2_curbits) {
        if (buffer) {
          buffer = 1;
          s21_set_dec_bits(end, i);
        }
      } else if (dec1_curbits && !dec2_curbits) {
        if (buffer) {
          buffer = 0;
        } else {
          s21_set_dec_bits(end, i);
        }
      } else if (!dec1_curbits && dec2_curbits) {
        if (buffer) {
          buffer = 1;
        } else {
          buffer = 1;
          s21_set_dec_bits(end, i);
        }
      } else if (dec1_curbits && dec2_curbits) {
        if (buffer) {
          buffer = 1;
          s21_set_dec_bits(end, i);
        }
      }
    }
  }
}

void s21_div_bits(s21_decimal dec1, s21_decimal dec2, s21_decimal *buf,
                  s21_decimal *end) {
  s21_reset_decimal(buf);
  s21_reset_decimal(end);
  for (int i = s21_get_highest_bits(dec1); i >= 0; i--) {
    if (s21_check_dec_bits(dec1, i)) s21_set_dec_bits(buf, 0);
    if (s21_is_greater_or_equal(*buf, dec2)) {
      s21_sub_bits(*buf, dec2, buf);
      if (i != 0) s21_shift_left(buf, 1);
      if (s21_check_dec_bits(dec1, i - 1)) s21_set_dec_bits(buf, 0);
      s21_shift_left(end, 1);
      s21_set_dec_bits(end, 0);
    } else {
      s21_shift_left(end, 1);
      if (i != 0) s21_shift_left(buf, 1);
      if ((i - 1) >= 0 && s21_check_dec_bits(dec1, i - 1))
        s21_set_dec_bits(buf, 0);
    }
  }
}

int s21_sub_without_scale(s21_decimal dec1, s21_decimal dec2,
                          s21_decimal *end) {
  s21_reset_decimal(end);
  int ret = 0;
  int reminder = 0;
  if (!s21_get_sign(dec1) && s21_get_sign(dec2)) {
    s21_negate(dec2, &dec2);
    ret = s21_add_without_scale(dec1, dec2, end);
  } else if (s21_get_sign(dec1) && !s21_get_sign(dec2)) {
    s21_set_sign(&dec2);
    ret = s21_add_without_scale(dec1, dec2, end);
  } else {
    if (s21_get_sign(dec1) && s21_get_sign(dec2)) {
      s21_negate(dec2, &dec2);
      s21_negate(dec1, &dec1);
      s21_swap_values(&dec1, &dec2);
    }
    if (!s21_is_greater_or_equal(dec1, dec2)) {
      s21_set_sign(end);
      s21_swap_values(&dec1, &dec2);
    }
    for (int i = 0; i < 96; i++) {
      if (s21_check_dec_bits(dec1, i) - s21_check_dec_bits(dec2, i) == 0) {
        if (reminder == 1) {
          s21_set_dec_bits(end, i);
        }
      } else if (s21_check_dec_bits(dec1, i) - s21_check_dec_bits(dec2, i) ==
                 1) {
        if (reminder == 0)
          s21_set_dec_bits(end, i);
        else
          reminder = 0;
      } else if (s21_check_dec_bits(dec1, i) - s21_check_dec_bits(dec2, i) ==
                 -1) {
        if (reminder != 1) {
          s21_set_dec_bits(end, i);
          reminder = 1;
        } else {
          reminder = 1;
        }
      }
      if (i == 95 && reminder) {
        if (s21_get_sign(*end))
          ret = 2;
        else
          ret = 1;
      }
    }
  }
  return ret;
}

int s21_add_without_scale(s21_decimal dec1, s21_decimal dec2,
                          s21_decimal *end) {
  int res = 0;
  if (s21_get_sign(dec1) != s21_get_sign(dec2)) {
    s21_decimal dec1tmp = dec1;
    s21_decimal dec2tmp = dec2;
    if (s21_get_sign(dec1))
      s21_negate(dec1, &dec1tmp);
    else
      s21_negate(dec2, &dec2tmp);
    res = s21_sub_without_scale(dec1tmp, dec2tmp, end);
    if (s21_get_sign(dec1) && s21_is_greater(dec1, dec2))
      s21_negate(*end, end);
    else if (s21_is_greater(dec2, dec1))
      s21_negate(*end, end);
  } else {
    int check_minus;
    int buffer = 0;
    s21_decimal temp = {{0, 0, 0, 0}};
    if (s21_get_sign(dec1)) s21_set_sign(&temp);
    for (int i = 0; i < 96; i++) {
      if (s21_check_dec_bits(dec1, i) && s21_check_dec_bits(dec2, i)) {
        if (buffer) s21_set_dec_bits(&temp, i);
        buffer = 1;
      } else if (s21_check_dec_bits(dec1, i) || s21_check_dec_bits(dec2, i)) {
        if (!buffer) s21_set_dec_bits(&temp, i);
      } else if (buffer) {
        s21_set_dec_bits(&temp, i);
        buffer = 0;
      }
      if (i == 95 && buffer) {
        check_minus = s21_get_sign(temp);
        if (check_minus == 1)
          res = 2;
        else
          res = 1;
      }
    }
    *end = temp;
  }
  return res;
}

void s21_swap_values(s21_decimal *dec1, s21_decimal *dec2) {
  s21_decimal tmp = *dec2;
  *dec2 = *dec1;
  *dec1 = tmp;
}
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_reset_decimal(result);
  int res = OK;
  s21_decimal zero = {{0, 0, 0, 0}};
  if (s21_is_equal(value_2, zero)) {
    res = NAN12;
  } else {
    if (!s21_get_sign(value_1) && !s21_get_sign(value_2)) {
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
    } else if (!s21_get_sign(value_1) && s21_get_sign(value_2)) {
      s21_reset_sign(&value_2);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
    } else if (s21_get_sign(value_1) && !s21_get_sign(value_2)) {
      s21_reset_sign(&value_1);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
      s21_set_sign(&value_1);
    } else if (s21_get_sign(value_1) && s21_get_sign(value_2)) {
      s21_reset_sign(&value_1);
      s21_reset_sign(&value_2);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
      s21_set_sign(&value_1);
    }
    *result = value_1;
  }
  return res;
}
