#include "input.h"
#include "intq.h"
#include "serial.h"

/* Stores keys from the keyboard and serial port. */
static struct intq buffer;

/* Initializes the input buffer. */
void
input_init (void)
{
  intq_init (&buffer);
  strcpy((char*)buffer.name, "buffer");
}

/* Adds a key to the input buffer.
   Interrupts must be off and the buffer must not be full. */
void
input_putc (uint8_t key)
{
  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (!intq_full (&buffer));

  intq_putc (&buffer, key);
  serial_notify ();
}

/* Retrieves a key from the input buffer.
   If the buffer is empty, waits for a key to be pressed. */
uint8_t
input_getc (void)
{
  uint8_t key;

  key = intq_getc (&buffer);
  serial_notify ();

  if (key == '\r')
    key = '\n';

  if (key == '\177' || key == '\b') {
    serial_putc('\b');
    serial_putc(' ');
    serial_putc('\b');
    return key;
  }

  // echo
  serial_putc(key);

  return key;
}

/* Returns true if the input buffer is full,
   false otherwise.
   Interrupts must be off. */
bool
input_full (void)
{
  ASSERT (intr_get_level () == INTR_OFF);
  return intq_full (&buffer);
}
