#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


int printf(const char *fmt, ...) {
  va_list args;
  int i,len;
  char out[200];
  va_start(args,fmt);
  vsprintf(out,fmt,args);
  va_end(args);
  len = strlen(out);
  for(i = 0;i < len;i ++)
  {
    _putc(out[i]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char c;
  char *str = out;
  const char *tmp;
  char num_s[100];
  int i,j,len,num;
  int flag,field_width;

  for(;*fmt; fmt++)
  {
      if(*fmt != '%')
      {
	  *str++ = *fmt;
	  continue;
      }

      flag = 0;
      fmt++;
      while(*fmt == ' ' || *fmt == '0')
      {
	if(*fmt == ' ')  flag |= 8;
	else if(*fmt == '0') flag |= 1;
	fmt++;
      }

      field_width = 0;
      if(*fmt >= '0' && *fmt <= '9')
      {
	      while(*fmt >= '0' && *fmt <= '9')
	      {
		      field_width = field_width*10 + *fmt -'0';
		      fmt++;
	      }
      }
      else if(*fmt == '*')
      {
	      fmt++;
	      field_width = va_arg(ap,int);
      }
      //base = 10;

      switch(*fmt)
      {
	  case 's':
	      tmp = va_arg(ap,char *);
	      len = strlen(tmp);
	      for(i = 0;i < len;i ++)
	      {
		   *str++ = *tmp++;
	      }
	      continue;
	  case 'd': break;
      }

      num = va_arg(ap,int);
      j = 0;
      if(num == 0)
      {
	  num_s[j++] = '0';
      }
      else
      {
	  if(num < 0)
	  {
	      *str++ = '-';
	      num = -num;
	  }
	  //j = 0;
	  while(num)
	  {
	      num_s[j++] = num%10 + '0';
	      num /= 10;
	  }
      }
      if(j < field_width)
      {
	      num = field_width - j;
	      c = flag & 1 ? '0' : ' ';
	      while(num--)
	      {
		      *str++ = c;
	      }
      }
      while(j--)
      {
	  *str++ = num_s[j];
      }
  }
  *str = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int  val;
  va_start(args,fmt);
  val = vsprintf(out,fmt,args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif

// int vsprintf(char*, const char*, va_list);

// // Maximum buffer: 65536
// char printf_buf[65536];

// int printf(const char *fmt, ...) {
//   va_list args;
//   va_start(args, fmt);
//   vsprintf(printf_buf, fmt, args);
//   va_end(args);
//   char *p = printf_buf;
//   while (*p) {
//     putch(*p);
//     p++;
//   }
//   return 0;
// }

// static struct {
//   int lpad;
//   char pad_char;
// } pref;

// void sprint_basic_format(char** pout, char** pin, va_list* args) {
//   if (**pin == 's') {
//     char *p = va_arg(*args, char*);
//     while (*p) *(*pout)++ = *p++;
//   } else if (**pin == 'd') {
//     int val = va_arg(*args, int);
//     int f = 1;
//     if (val < 0) f = -1;

//     int buf[24] = {0};
//     int i = 0;
//     for (; i < 10 && val; i++) {
//       buf[i] = (val % 10) * f;
//       val /= 10;
//     }

//     if (i == 0) i++;
//     if (f == -1 && pref.pad_char == '0') *(*pout)++ = '-';
//     for (int j = 0; j < pref.lpad - i - (f == -1); j++)
//       *(*pout)++ = pref.pad_char;
//     if (f == -1 && pref.pad_char == ' ') *(*pout)++ = '-';
    
//     for (i--; i >= 0; i--) {
//       *(*pout)++ = buf[i] + '0';
//     }
//   } else if (**pin == 'u') {
//     unsigned int val = va_arg(*args, unsigned int);
//     int buf[24] = {0};
//     int i = 0;
//     for (; i < 10 && val; i++) {
//       buf[i] = (val % 10);
//       val /= 10;
//     }
//     if (i == 0) i++;
//     for (int j = 0; j < pref.lpad - i; j++)
//       *(*pout)++ = pref.pad_char;
//     for (i--; i >= 0; i--) {
//       *(*pout)++ = buf[i] + '0';
//     }
//   } else if (**pin == 'x') {
//     const char hex_char[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
//     unsigned int val = va_arg(*args, unsigned int);
//     int buf[24] = {0};
//     int i = 0;
//     for (; i < 10 && val; i++) {
//       buf[i] = (val % 16);
//       val /= 16;
//     }

//     if (i == 0) i++;
//     for (int j = 0; j < pref.lpad - i; j++)
//       *(*pout)++ = pref.pad_char;
//     for (i--; i >= 0; i--) {
//       *(*pout)++ = hex_char[buf[i]];
//     }
//   } else if (**pin == 'c') {
//     char val = va_arg(*args, int);
//     *(*pout)++ = val;
//   } else if (**pin == 'p') {
//     *(*pout)++ = '0';
//     *(*pout)++ = 'x';
//     const char hex_char[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
//     unsigned int val = va_arg(*args, unsigned int);
//     int buf[24] = {0};
//     int i = 0;
//     for (; i < 10 && val; i++) {
//       buf[i] = (val % 16);
//       val /= 16;
//     }

//     if (i == 0) i++;
//     for (int j = 0; j < pref.lpad - i; j++)
//       *(*pout)++ = pref.pad_char;
//     for (i--; i >= 0; i--) {
//       *(*pout)++ = hex_char[buf[i]];
//     }
//   } else {
//     putch(**pin);
//     assert(false);
//   }
//   (*pin)++;
// }

// int sprint_read_pad(char** pout, char** pin) {
//   int sum = **pin - '0';
//   if (sum < 0 || sum > 9) return -1;
//   (*pin)++;
//   int ans = sprint_read_pad(pout, pin);
//   if (ans != -1) return sum * 10 + ans;
//   return sum;
// }

// void sprint_format(char** pout, char** pin, va_list* args) {
//   switch (**pin) {
//     case '%':
//       *(*pout)++ = '%';
//       break;

//     case '0':
//       (*pin)++;
//       pref.pad_char = '0';
//       sprint_format(pout, pin, args);
//       break;

//     case '1' ... '9':
//       pref.lpad = sprint_read_pad(pout, pin);
//       sprint_format(pout, pin, args);
//       break;

//     case 'l':
//       (*pin)++;
//       sprint_format(pout, pin, args);
//       break;

//     default:
//       sprint_basic_format(pout, pin, args);
//   }
// }

// int vsprintf(char *out, const char *fmt, va_list args) {
//   char *pout = out;
//   char *pin = (void*)fmt;
//   while (*pin) {
//     pref.lpad = 0;
//     pref.pad_char = ' ';
//     switch (*pin) {
//       case '%':
//         pin++;
//         sprint_format(&pout, &pin, &args);
//       default:
//         *pout = *pin;
//         pin++;
//         pout++;
//     }
//   }
  
//   *pout = 0;
//   return pout - out;
// }

// int sprintf(char *out, const char *fmt, ...) {
//   va_list args;
//   va_start(args, fmt);
//   int len = vsprintf(out, fmt, args);
//   va_end(args);
//   return len;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   // TODO: Limit output size
//   va_list args;
//   va_start(args, fmt);
//   int len = vsprintf(out, fmt, args);
//   va_end(args);
//   return len;
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
//   return 0;
// }

// #endif
// // #include <am.h>
// // #include <klib.h>
// // #include <klib-macros.h>
// // #include <stdarg.h>

// // #if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// // int printf(const char *fmt, ...) {
// //   panic("Not implemented");
// // }

// // int vsprintf(char *out, const char *fmt, va_list ap) {
// //   panic("Not implemented");
// // }

// // int sprintf(char *out, const char *fmt, ...) {
// //   panic("Not implemented");
// // }

// // int snprintf(char *out, size_t n, const char *fmt, ...) {
// //   panic("Not implemented");
// // }

// // int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
// //   panic("Not implemented");
// // }

// // #endif
