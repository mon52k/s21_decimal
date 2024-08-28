#include "s21_decimal.h"

int s21_is_equal(s21_decimal dec1, s21_decimal dec2) {
  int scale = 0;
  int end = 1;
  s21_normalization(&dec1, &dec2, &scale);
  if (dec1.bits[0] == 0 && dec1.bits[1] == 0 && dec1.bits[2] == 0 &&
      dec2.bits[0] == 0 && dec2.bits[1] == 0 && dec2.bits[2] == 0) {
    end = 1;
  } else if (s21_get_sign(dec1) && !s21_get_sign(dec2)) {
    end = 0;
  } else if (!s21_get_sign(dec1) && s21_get_sign(dec2)) {
    end = 0;
  } else if (s21_get_sign(dec1) == s21_get_sign(dec2)) {
    for (int i = 2; i >= 0; i--) {
      if (dec1.bits[i] > dec2.bits[i]) {
        end = 0;
        break;
      } else if (dec1.bits[i] < dec2.bits[i]) {
        end = 0;
        break;
      } else if (dec1.bits[i] == dec2.bits[i]) {
        continue;
      }
    }
  }
  return end;
}

int s21_is_greater(s21_decimal dec1, s21_decimal dec2) {
  int scale = 0;
  int end = 1;
  s21_normalization(&dec1, &dec2, &scale);
  if (s21_get_sign(dec1) && !s21_get_sign(dec2)) {
    end = 0;
  } else if (!s21_get_sign(dec1) && s21_get_sign(dec2)) {
    end = 1;
  } else if (s21_get_sign(dec1) == s21_get_sign(dec2)) {
    for (int i = 2; i >= 0; i--) {
      if (dec1.bits[i] > dec2.bits[i]) {
        end = 1;
        break;
      } else if (dec1.bits[i] < dec2.bits[i]) {
        end = 0;
        break;
      } else if (dec1.bits[i] == dec2.bits[i]) {
        end = 0;
        continue;
      }
    }
    if (s21_get_sign(dec1)) {
      if (end == 1)
        end = 0;
      else
        end = 1;
    }
  }
  return end;
}

int s21_is_less(s21_decimal dec1, s21_decimal dec2) {
  return (s21_is_greater(dec2, dec1));
}

int s21_is_less_or_equal(s21_decimal dec1, s21_decimal dec2) {
  return (s21_is_less(dec1, dec2) || s21_is_equal(dec1, dec2));
}

int s21_is_greater_or_equal(s21_decimal dec1, s21_decimal dec2) {
  return (s21_is_greater(dec1, dec2) || s21_is_equal(dec1, dec2));
}

int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2) {
  return (!s21_is_equal(dec1, dec2));
}