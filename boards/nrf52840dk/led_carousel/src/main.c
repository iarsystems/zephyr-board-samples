#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <inttypes.h>

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

#define BLINK_TIME        500 /* LED blink time */

#define CLOCKWISE         1
#define COUNTERCLOCKWISE  0

// Define buttons 1 and 2 as LEDs direction control buttons.
static const struct gpio_dt_spec direction_control_buttons[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios, {0}),
};

/* Onboard LEDs 1-4.
* Ordered as 1-2, 4-3 so that the LEDs go in a circle.
*/
static struct gpio_dt_spec gpio_leds[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0}),
};

static struct gpio_callback button_cb_data;

int led_direction = CLOCKWISE;

void
button_pressed(const struct device *dev, struct gpio_callback *cb,
              uint32_t pins)
{
    // Set the direction the LEDs are going in based on the button that is pressed.
    if (pins & BIT(direction_control_buttons[0].pin)) {
        printk("Button 1 pressed\n");
        led_direction = COUNTERCLOCKWISE;
    }
    else {
        printk("Button 2 pressed\n");
        led_direction = CLOCKWISE;
    }
}

void
configure_buttons(void)
{
    int ret;
    int i;

    // Go through and configure each button.
    for(i = 0; i < LEN(direction_control_buttons); i++) {
        if (!gpio_is_ready_dt(&direction_control_buttons[i])) {
            printk("Error: button device %s is not ready\n",
                direction_control_buttons[i].port->name);
            return;
        }

        ret = gpio_pin_configure_dt(&direction_control_buttons[i], GPIO_INPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n",
                ret, direction_control_buttons[i].port->name, direction_control_buttons[i].pin);
            return;
        }

        ret = gpio_pin_interrupt_configure_dt(&direction_control_buttons[i],
                        GPIO_INT_EDGE_TO_ACTIVE);
        if (ret != 0) {
            printk("Error %d: failed to configure interrupt on %s pin %d\n",
                ret, direction_control_buttons[i].port->name, direction_control_buttons[i].pin);
            return;
        }
    }

    gpio_init_callback(&button_cb_data, button_pressed,
               BIT(direction_control_buttons[0].pin) | BIT(direction_control_buttons[1].pin));
    gpio_add_callback(direction_control_buttons[0].port, &button_cb_data);
}

void
configure_leds(void)
{
    int ret;
    int i;

    // Go through and configure each LED.
    for(i = 0; i < LEN(gpio_leds); i++) {
        if (gpio_leds[i].port && !gpio_is_ready_dt(&gpio_leds[i])) {
            printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, gpio_leds[i].port->name);
            gpio_leds[i].port = NULL;
	    }

        if (gpio_leds[i].port) {
            ret = gpio_pin_configure_dt(&gpio_leds[i], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, gpio_leds[i].port->name, gpio_leds[i].pin);
                gpio_leds[i].port = NULL;
            } else {
                printk("Set up LED at %s pin %d\n", gpio_leds[i].port->name, gpio_leds[i].pin);
            }
        }
    }
}

int
main(void)
{
    int i = 0;

    configure_buttons();
    configure_leds();

    // Continuously cycle through and toggle the LEDs one by one.
    while (1) {
        gpio_pin_set_dt(&gpio_leds[i], 1);
        k_msleep(BLINK_TIME);
        gpio_pin_set_dt(&gpio_leds[i], 0);

        if (led_direction == CLOCKWISE) {
            i++;
            if (i > LEN(gpio_leds)-1)
                i = 0;
        }
        else if (led_direction == COUNTERCLOCKWISE) {
            i--;
            if (i < 0)
                i = LEN(gpio_leds) - 1;
        }
    }

    return 0;
}