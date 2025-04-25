#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

#define GPIO_ARRAY_LOOP(arr, i) for (i = 0; i < LEN(arr); i++)

typedef enum {
    OFF_STATE,
    AUTO_STATE,
    ON_STATE,
} interior_light_state_typedef;

interior_light_state_typedef get_interior_light_state(void);
void configure_interior_light_switch(void);
extern void switch_toggled(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins);
