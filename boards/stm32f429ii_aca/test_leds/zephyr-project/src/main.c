#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <inttypes.h>

#define SLEEP_TIME_MS	500

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

/* Loop through a list of related GPIOs */
#define GPIO_LIST_LOOP(list_name) for (i = 0; i < LEN(list_name); i++)

/* Onboard car traffic light LEDs 1-3. */
static struct gpio_dt_spec leds_list[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led4), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led5), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led6), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led7), gpios, {0}),
};

/* Onboard buttons 1, 2, and 3. */
static struct gpio_dt_spec traffic_buttons[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw2), gpios, {0}),
};

static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

void
configure_buttons(void)
{
    int ret;
    int i;

    // Go through and configure each button.
    GPIO_LIST_LOOP(traffic_buttons) {
        if (!gpio_is_ready_dt(&traffic_buttons[i])) {
            printk("Error: button device %s is not ready\n",
                traffic_buttons[i].port->name);
            return;
        }

        ret = gpio_pin_configure_dt(&traffic_buttons[i], GPIO_INPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n",
                ret, traffic_buttons[i].port->name, traffic_buttons[i].pin);
            return;
        }

        ret = gpio_pin_interrupt_configure_dt(&traffic_buttons[i],
                        GPIO_INT_EDGE_TO_ACTIVE);
        if (ret != 0) {
            printk("Error %d: failed to configure interrupt on %s pin %d\n",
                ret, traffic_buttons[i].port->name, traffic_buttons[i].pin);
            return;
        }
    }

    unsigned int pin_mask_sum = 0;

    GPIO_LIST_LOOP(traffic_buttons) {
        pin_mask_sum |= BIT(traffic_buttons[i].pin);
    }

    gpio_init_callback(&button_cb_data, button_pressed, pin_mask_sum);
    gpio_add_callback(traffic_buttons[0].port, &button_cb_data);
}

void
configure_leds(void)
{
    int ret;
    int i;

    // Go through and check and configure each LED.
    GPIO_LIST_LOOP(leds_list) {
        if (leds_list[i].port && !gpio_is_ready_dt(&leds_list[i])) {
            printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, leds_list[i].port->name);
            leds_list[i].port = NULL;
	    }

        if (leds_list[i].port) {
            ret = gpio_pin_configure_dt(&leds_list[i], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, leds_list[i].port->name, leds_list[i].pin);
                leds_list[i].port = NULL;
            } else {
                printk("Set up LED at %s pin %d\n", leds_list[i].port->name, leds_list[i].pin);
            }
        }
    }
}

int
main(void)
{
    int i;

    configure_buttons();
    configure_leds();

    GPIO_LIST_LOOP(leds_list) {
        gpio_pin_set_dt(&leds_list[i], 0);
    }

    while (1) {
        GPIO_LIST_LOOP(leds_list) {
            gpio_pin_set_dt(&leds_list[i], 1);
            k_msleep(SLEEP_TIME_MS);
            gpio_pin_set_dt(&leds_list[i], 0);
        }
    }

    return 0;
}