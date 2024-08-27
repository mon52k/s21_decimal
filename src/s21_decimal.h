#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_
#include <math.h>
#include <stdio.h>
#define CONVERTING_ERROR 1
#define OK 0
#define SIGN 0x80000000
#define TRUE 1
#define FALSE 0
#define INF 1
#define NINF 2
#define NAN12 3
// #define INT_MAX 2147483647
#define SC 0x00ff0000

#define SUCCESS 0

typedef struct s21_decimal {
  unsigned int bits[4];
} s21_decimal;

void s21_set_sign(s21_decimal *dec);
void s21_reset_sign(s21_decimal *dec);
int s21_check_bits(int value, int position);
int s21_set_bits(int value, int position);
int s21_reset_bit(int value, int position);
void s21_reset_decimal(s21_decimal *dec);
int s21_get_expon(float src);
void s21_set_dec_bits(s21_decimal *dec, int position);
int s21_check_dec_bits(s21_decimal dec, int position);
void s21_set_mantissa_to_decimal(s21_decimal *dec, int exp, float src);
void s21_set_scale(s21_decimal *dec, int scale);
int s21_get_scale(s21_decimal dec);
int s21_get_sign(s21_decimal dec);
int s21_get_highest_bits(s21_decimal dec);
void s21_swap_values(s21_decimal *dec1, s21_decimal *dec2);

int s21_normalization(s21_decimal *dec1, s21_decimal *dec2, int *final_scale);
int s21_shift_left(s21_decimal *dec, int shift);
s21_decimal s21_binary_div(s21_decimal dec1, s21_decimal dec2,
                           s21_decimal *reminder, int *fail);
int s21_div_by_10(s21_decimal *dec, int scale);
int s21_mul_by_10(s21_decimal *dec, int scale);
int s21_normal(s21_decimal *dec1, s21_decimal *dec2, int scale);

void s21_sub_bits(s21_decimal dec1, s21_decimal dec2, s21_decimal *end);
void s21_div_bits(s21_decimal dec1, s21_decimal dec2, s21_decimal *buf,
                  s21_decimal *end);
int s21_sub_without_scale(s21_decimal value1, s21_decimal value2,
                          s21_decimal *end);
int s21_add_without_scale(s21_decimal dec1, s21_decimal dec2, s21_decimal *end);

int s21_from_int_to_decimal(int src, s21_decimal *dec);
int s21_from_float_to_decimal(float src, s21_decimal *dec);
int s21_from_decimal_to_float(s21_decimal src, float *dec);
int s21_from_decimal_to_int(s21_decimal src, int *dec);

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_equal(s21_decimal dec1, s21_decimal dec2);
int s21_is_greater(s21_decimal dec1, s21_decimal dec2);
int s21_is_less(s21_decimal dec1, s21_decimal dec2);
int s21_is_less_or_equal(s21_decimal dec1, s21_decimal dec2);
int s21_is_greater_or_equal(s21_decimal dec1, s21_decimal dec2);
int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2);

int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_floor(s21_decimal value, s21_decimal *result);

#endif
