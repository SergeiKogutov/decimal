#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error = 0;
  double temp = (double)*dst;
  for (int i = 0; i < 96; i++) {
    if (i <= 31) {
      temp += s21_get_bit(src, i) * pow(2, i);
    }
    if (i >= 32 && i <= 63) {
      temp += s21_get_bit(src, i) * pow(2, i);
    }
    if (i >= 64 && i <= 95) {
      temp += s21_get_bit(src, i) * pow(2, i);
    }
  }
  temp = temp * pow(10, -s21_get_scale(src));
  if (s21_get_sign(src)) temp = temp * (-1);
  *dst = temp;
  return error;
}

long double s21_fabs(double x) {
  if (x < 0) {
    x = -x;
  }
  return (long double)x;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_zero_decimal(dst);
  int return_value = 0;
  if (isinf(src) || isnan(src)) {
    return_value = 1;
  } else {
    if (src != 0) {
      if (src < 0) {
        s21_set_sign(dst);
      }
      int *ptr = (int *)&src;
      int exp = ((*ptr >> 23) & 0xFF) - 127;
      double temp = (double)s21_fabs(src);
      int scale = 0;
      while (scale < 28 && (int)temp / (int)pow(2, 21) == 0) {
        scale++;
        temp *= 10;
      }
      temp = round(temp);
      if (scale <= 28 && (exp > -94 && exp < 96)) {
        floatbits mant = {0};
        temp = (float)temp;
        while (fmod(temp, 10) == 0 && scale > 0) {
          scale--;
          temp /= 10;
        }
        mant.fl = temp;
        exp = ((*(int *)&mant.fl & ~0x80000000) >> 23) - 127;
        dst->bits[exp / 32] = dst->bits[exp / 32] | 1 << exp % 32;
        for (int i = exp - 1, j = 22; j >= 0; i--, j--)
          if ((mant.ui & (1 << j)) != 0) dst->bits[i / 32] |= 1 << i % 32;
        dst->bits[3] = dst->bits[3] | (scale << 16);
      }
    }
  }
  return return_value;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int error = 0;
  s21_zero_decimal(dst);
  if (src < 0) {
    s21_set_sign(dst);
    src = (-1) * src;
  }
  if (src > INT_MAX)
    error = 1;
  else
    dst->bits[0] = src;

  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = 0;
  int scale = s21_get_scale(src);
  if (src.bits[1] || src.bits[2]) {
    error = 1;
  } else {
    *dst = src.bits[0];
    if (scale > 0 && scale <= 28) {
      *dst /= pow(10, scale);
    }
  }
  if (s21_get_bit(src, 127)) *dst = *dst * (-1);
  return error;
}

int s21_from_decimal_to_double(s21_decimal src, long double *dst) {
  long double temp = (double)*dst;
  for (int i = 0; i < 96; i++) {
    temp += s21_get_bit(src, i) * pow(2, i);
  }
  temp = temp * pow(10, -s21_get_scale(src));
  if (s21_get_bit(src, 127)) temp = temp * (-1);
  *dst = temp;
  return 0;
}
