/***********************************************************************

   haconv.c -- A hex to ascii converter (and vice versa)
   Copyright (C) 2014 DIban Hakistron.

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

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define EOF 0xFF

const char hex_digits[16] = "0123456789ABCDEF";

/* Function prototypes */
void hex_digits_from_number (unsigned char b, char *ch1, char *ch2);
unsigned char number_from_hex_digits (char ch1, char ch2);
int parse_hex_digits (FILE *input, char *buffer);
void print_usage (char *argv0);


int
main (int argc, char *argv[])
{
  /* -- Prologue -- */
  int hex;
  char buffer[3] = "\0\0";
  char ch1, ch2;
  char option_char;
  int check_parser = 0;
  unsigned char b = '\0';
  FILE *input = stdin;

  /* Program executable name */
  char *prog = argv[0];

  /* Command flags */
  int hex_to_ascii_flag = 1;
  int ascii_to_hex_flag = 0;

  /* Option flags */
  int nlomitted_flag = 0;
  int spaces_flag = 0;
  int prefix_flag = 0;
  int interactive_flag = 0;


  /* Parse the command line arguments */
  while ((--argc > 0) && ((*++argv)[0] == '-'))
    while ((option_char = *++argv[0]) != '\0')
      switch (option_char)
	{
	case 'x':
	  hex_to_ascii_flag = 1;
	  ascii_to_hex_flag = 0;
	  break;
	case 'a':
	  ascii_to_hex_flag = 1;
	  hex_to_ascii_flag = 0;
	  break;
	case 'n':
	  nlomitted_flag = 1;  /* Omit the newline */
	  break;
	case 's':
	  spaces_flag = 1;  /* Print spaces between characters */
	  break;
	case 'p':
	  prefix_flag = 1;  /* Prefix output with '0x' */
	  break;
	case 'i':
	  interactive_flag = 1;  /* Interactive mode */
	  break;
	case 'h':
	default:
	  print_usage (prog);
	  break;
	}
  
  /* Check and open file */
  if (argc > 0)
    {
      input = fopen (*argv, "r");
      
      if (input == NULL)
	{
	  perror ("Error while opening file");
	  exit (EXIT_FAILURE);
	}
    }

  /* -- Main part -- */
  if (hex_to_ascii_flag == 1)  /* Convert hex to ascii */
    {
      /* Parses input for hexdecimal digits */
      while ((check_parser = parse_hex_digits (input, buffer)) > -1)
	{
	  if (check_parser == 0)
	    {
	      /* Convert the buffer to a number */
	      hex = number_from_hex_digits (buffer[0], buffer[1]);
	      
	      /* Print the number as character (loop of life) */
	      printf ((spaces_flag == 1) ? "%c " : "%c", hex);
	    }
	  else if ((check_parser == 1) && (interactive_flag == 1))
	    /* Print newline after output */
	    printf ("\n");
	}
    }
  else if (ascii_to_hex_flag == 1)  /* Convert ascii to hex */
    {
      while ((b = getc (input)) != EOF)  /* Read next character */
	{
	  /* Convert number to hex digits */
	  hex_digits_from_number (b, &ch1, &ch2);
	  
	  /* Print newline after output */
	  if ((interactive_flag == 1) && (b == '\n'))
	    {
	      printf ("\n");
	      continue;  /* Don't print 0x0A */
	    }
	  
	  if (prefix_flag == 1)
	    printf ((spaces_flag == 1) ? "0x%c%c " : "0x%c%c", ch1, ch2);
	  else
	    printf ((spaces_flag == 1) ? "%c%c " : "%c%c", ch1, ch2);
	}
    }
  else
    print_usage (prog);

  /* -- Epilogue -- */
  if (nlomitted_flag != 1)
    printf ("\n");

  return EXIT_SUCCESS;
}


/* Write the first (ch1) and second (ch2) digit of a
   hexdecimal number with value b */
void
hex_digits_from_number (unsigned char b, char *ch1, char *ch2)
{
  *ch1 = hex_digits[((b >> 4) & 0x0F)];
  *ch2 = hex_digits[(b & 0x0F)];
}

/* Return the index of hex_digits, which equals ch */
int
hex_digits_index (char ch)
{
  int i;
  
  for (i = 0; i < 16; i++)
    if ((ch == hex_digits[i]) || (ch-' ' == hex_digits[i]))
      return i;
  
  return EOF; /* If ch is no hex digit return EOF */
}

/* Return the value of a hexdecimal number with the
   digits ch1 and ch2 */
unsigned char
number_from_hex_digits (char ch1, char ch2)
{
  unsigned char b;

  b = hex_digits_index (ch1) << 4;
  b += hex_digits_index (ch2);

  return b;
}

/* Parses input for hexdecimal digits,
   return 0 on success */
int
parse_hex_digits (FILE *input, char *buffer)
{
  int count;
  unsigned char b;
  char tmp_buffer[3] = "\0\0";

  /* Write two hex digits to buffer */
  for (count = 0; count < 2; ++count)
    {
      b = getc(input);
      
      switch (b)   /* Filter input */
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case 'a': case 'b': case 'c': case 'd': case 'e': 
	case 'f': case 'A': case 'B': case 'C': case 'D': 
	case 'E': case 'F':
	  tmp_buffer[count] = b;
	  break;
	case EOF:
	  return -1;
	  break;
	case '\n':
	  return 1;
	  break;
	default:
	  count = -1;
	  break;
	}
    }

  /* Copy the result in buffer */
  buffer[0] = tmp_buffer[0];
  buffer[1] = tmp_buffer[1];

  return 0;
}

/* Print the usage message */
void
print_usage (char *prog)
{
  printf ("\
Usage: %s [OPTIONS] [FILE]\n\
\n\
Options: \n\
  -x      convert hex digits to ascii chars (default) \n\
  -a      convert ascii chars to hex digits \n\
  -n      omit the newline after output \n\
  -s      put spaces between the output characters \n\
  -p      prefix output hex digits with '0x' \n\
  -i      interactive mode (stdin of and stdout to console)\n\
  -h      print this help information \n\
\n", prog);
  printf ("\
Examples: \n\
  echo 'Hello, World!' | %s -as \n\
  echo -n '48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21 0A' | %s -xn \n\
  %s -a some_file.txt | %s -n > the_same_file.txt \n\
"
	  , prog, prog, prog, prog);

  exit (EXIT_FAILURE);
}
