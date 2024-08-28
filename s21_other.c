#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  s21_reset_decimal(result);
  int res = OK;
  s21_truncate(value, result);
  s21_decimal one = {{1, 0, 0, 0}};
  if (s21_get_sign(value)) {
    res = s21_sub(*result, one, result);
  }
  return res;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_reset_decimal(result);
  int res = OK;
  int sign = s21_get_sign(value);
  s21_reset_sign(&value);
  s21_decimal tmp = {{0}};
  s21_truncate(value, &tmp);
  s21_decimal tmp_copy = tmp;
  s21_sub(value, tmp, &tmp);
  s21_decimal five = {{5, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_set_scale(&five, 1);
  if (s21_is_greater_or_equal(tmp, five)) {
    res = s21_add(tmp_copy, one, result);
  } else {
    *result = tmp_copy;
  }
  if (sign) s21_set_sign(result);
  return res;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_reset_decimal(result);
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal tmp = {{0, 0, 0, 0}};
  int sign_dec = s21_get_sign(value);
  int scale = s21_get_scale(value);
  if (!scale) {
    *result = value;
  } else {
    for (int i = scale; i > 0; i--) {
      s21_div_bits(value, ten, &tmp, result);
      value = *result;
    }
  }
  if (sign_dec) s21_set_sign(result);
  return OK;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  int sign = s21_get_sign(*result);
  if (sign) {
    s21_reset_sign(result);
  } else {
    s21_set_sign(result);
  }
  return OK;
}
