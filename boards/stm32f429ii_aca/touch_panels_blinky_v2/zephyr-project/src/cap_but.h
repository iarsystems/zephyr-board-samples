#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

// #define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

/* Loop through a list of related GPIOs */
// #define GPIO_LIST_LOOP(list_name, i) for (i = 0; i < LEN(list_name); i++)
extern struct gpio_dt_spec cap_buttons[];

typedef enum
{
  CAP_BUT1 = 0,
  CAP_BUT2
} cap_button_typedef;

typedef enum {
  cap_but1_discharge,
  cap_but1_prepare_and_start,
  cap_but1_charging,
  cap_but2_discharge,
  cap_but2_prepare_and_start,
  cap_but2_charging
} cap_buttons_state_typedef;

void cap_but_discharge(cap_button_typedef cap_but);
void cap_but_prepare(cap_button_typedef cap_but);
void cap_but_charge(cap_button_typedef cap_but);
// void cap_but_setup(cap_button_typedef cap_but);
void cap_but_setup_interrupt(cap_button_typedef cap_but);
void cap_buttons_setup(void);
