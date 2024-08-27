#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *end) {
  int res = 0;
  if (!end) {
    res = 1;
  } else {
    s21_reset_decimal(end);
    int fscale = 0;
    res = s21_normalization(&value_1, &value_2, &fscale);
    if (res == 0) {
      res = s21_add_without_scale(value_1, value_2, end);
      s21_set_scale(end, fscale);
    }
  }
  return res;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *end) {
  int fscale = 0;
  int res = s21_normalization(&value_1, &value_2, &fscale);
  if (res == 0) {
    s21_reset_decimal(end);
    res = s21_sub_without_scale(value_1, value_2, end);
    s21_set_scale(end, fscale);
  }
  return res;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *end) {
  int res = 0;
  int scale_1 = s21_get_scale(value_1);
  int scale_2 = s21_get_scale(value_2);
  int sign = 0;
  if ((s21_get_sign(value_1) && !s21_get_sign(value_2)) ||
      (!s21_get_sign(value_1) && s21_get_sign(value_2)))
    sign = 1;
  value_1.bits[3] = 0;
  value_2.bits[3] = 0;
  s21_reset_decimal(end);
  for (int i = 0; i <= s21_get_highest_bits(value_2); i++) {
    int bits_2 = s21_check_dec_bits(value_2, i);
    if (bits_2) {
      s21_decimal tmp = value_1;
      res = s21_shift_left(&tmp, i);
      if (res) break;
      res = s21_add_without_scale(*end, tmp, end);
      if (res) break;
    }
  }
  int scale_res = scale_1 + scale_2;
  if (scale_res > 28)
    res = 2;
  else
    s21_set_scale(end, scale_res);
  if (sign && !res) s21_set_sign(end);
  if (res == 1 && sign) {
    res = 2;
  }
  return res;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *end) {
  int ret_val = 0;
  s21_reset_decimal(end);
  s21_decimal zero = {{0, 0, 0, 0}};
  if (s21_is_equal(value_2, zero)) {
    ret_val = NAN12;
  } else {
    int sign_1 = s21_get_sign(value_1);
    int sign_2 = s21_get_sign(value_2);
    int scale_1 = s21_get_scale(value_1);
    int scale_2 = s21_get_scale(value_2);
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    s21_decimal reminder = {{0, 0, 0, 0}}, dec_int_part = {{0, 0, 0, 0}};
    int division_counter = 0;
    dec_int_part = s21_binary_div(value_1, value_2, &reminder, &ret_val);
    *end = dec_int_part;
    while (!s21_is_equal(reminder, zero) && division_counter <= 10) {
      s21_mul_by_10(&reminder, 1);
      division_counter++;
      dec_int_part = s21_binary_div(reminder, value_2, &reminder, &ret_val);
      s21_set_scale(&dec_int_part, division_counter);
      ret_val = s21_add(*end, dec_int_part, end);
      if (ret_val != 0) break;
    }
    if (sign_1 != sign_2) s21_set_dec_bits(end, 127);
    int scale_end = scale_1 - scale_2;
    if (scale_end < 0 && !ret_val) {
      ret_val = s21_mul_by_10(end, 0 - scale_end);
    }
    if (s21_get_sign(value_1) != s21_get_sign(value_2)) s21_set_sign(end);
    if (s21_is_equal(*end, zero) && !s21_is_equal(value_1, zero)) ret_val = 2;
  }
  return ret_val;
}
