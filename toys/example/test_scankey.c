/* test_scankey.c - collate incoming ansi escape sequences.
 *
 * Copyright 2015 Rob Landley <rob@landley.net>

USE_TEST_SCANKEY(NEWTOY(test_scankey, 0, 0))

config TEST_SCANKEY
  bool "test_scankey"
  default n
  help
    usage: test_scankey

    Move a letter around the screen. Hit ESC to exit.
*/

#define FOR_test_scankey
#include "toys.h"

void test_scankey_main(void)
{
  time_t t[2];
  unsigned width, height, tick;
  char c = 'X', scratch[16];
  int key, x, y;

  t[0] = t[1] = x = tick = 0;
  memset(scratch, 0, 16);
  y = 1;

  sigatexit(tty_sigreset);  // Make ctrl-c restore tty
  tty_esc("?25l");          // hide cursor
  tty_esc("0m");            // reset color to default
  tty_esc("2J");            // Clear screen
  set_terminal(1, 1, 0);    // Raw mode

  for (;;) {
    tty_jump(x, y);
    xputc(c);
    t[1&++tick] = time(0);
    key = scan_key_getsize(scratch, !!t[0]+2*(t[0] != t[1]), &width, &height);
    tty_jump(0, 0);
    printf("ESC to exit: key=%d x=%d y=%d width=%d height=%d   ",
           key, x, y, width, height);

    if (key == -2) continue;
    if (key <= ' ') break;
    if (key>=256) {
      tty_jump(x, y);
      xputc(' ');

      key -= 256;
      if (key==KEY_UP) y--;
      else if (key==KEY_DOWN) y++;
      else if (key==KEY_RIGHT) x++;
      else if (key==KEY_LEFT) x--;
      else if (key==KEY_PGUP) y = 0;
      else if (key==KEY_PGDN) y = 999;
      else if (key==KEY_HOME) x = 0;
      else if (key==KEY_END) x = 999;
      if (y<1) y = 1;
      if (y>=height) y = height-1;
      if (x<0) x = 0;
      if (x>=width) x = width-1;
    } else c = key;
  }
  tty_reset();
}
