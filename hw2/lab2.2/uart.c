/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

int uprintd(UART *up, int d)
{
        int i;
        char stack[100];

        if(d == 0)
        {
                uputc(up, '0');
                return 1;
        }
        else if(d < 0)
        {
                uputc(up, '-');
        }

        for(i = 0; d != 0; i++)
        {
                stack[i] = '0' + (d % 10);
                d /= 10;
        }

        while(--i >= 0)
                uputc(up, stack[i]);
}

int uprintu(UART *up, u32 u)
{
        int i, x;
        char stack[16];

        uprints(up, "0x");
        if(u == 0)
        {
                uputc(up, '0');
                return 1;
        }

        for(i = 0; u != 0; i++)
        {
                x = u % 16;
                if(x < 10)
                        stack[i] = '0' + x;
                else
                        stack[i] = 'A' + (x - 10);

                u /= 16;
        }

        while(--i >= 0)
                uputc(up, stack[i]);
}

int uprintf(UART *up, char *fmt, ...)
{
        int i, *p = (int *)&fmt;
        p++;

        for(i = 0; fmt[i] != 0; i++)
        {
                if(fmt[i] == '%')
                {
                        i++;
                        switch(fmt[i])
                        {
                                case 's': uprints(up, (char *)*p);
                                        p++;
                                        break;
                                case 'd': uprintd(up, (int)*p);
                                        p++;
                                        break;
                                case 'u': uprintu(up, (u32)*p);
                                        p++;
                                        break;
                                case 'c': uputc(up, (char)*p);
                                        p++;
                                        break;
                                default:
                                        uputc(up, '?');
                                        break;
                        }
                }
                else
                        uputc(up, fmt[i]);
        }
}
