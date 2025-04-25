#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <zephyr/irq.h>

#include <inttypes.h>
#include <stdint.h>

#define SLEEP_TIME_MS	1

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

/* Loop through a list of related GPIOs */
#define GPIO_LIST_LOOP(list_name) for (i = 0; i < LEN(list_name); i++)

/* Onboard car traffic light LEDs 1-3. */
static struct gpio_dt_spec traffic_leds[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}),
    // GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0}),
};

/* Onboard capacitive buttons. */
static struct gpio_dt_spec cap_buttons[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw7), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw8), gpios, {0}),
    // GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw2), gpios, {0}),
};

uint32_t current_pins;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	// printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
    if (pins & BIT(cap_buttons[0].pin)) {
        // timer1_int = k_timer_status_get(&cap_buttons_timer);
        printk("Cap_button 1 pressed lolllllllllllllll");
        gpio_pin_set_dt(&traffic_leds[1], 1);
        gpio_pin_set_dt(&traffic_leds[0], 0);
        // timer1_int = k_uptime_get();
    }
    // }

    // if (timer2_int == 0) {
    if (pins & BIT(cap_buttons[1].pin)) {
        printk("Cap_button 2 pressed lolllllllllllllll");
        // gpio_pin_set_dt(&traffic_leds[i], val);
        gpio_pin_set_dt(&traffic_leds[0], 1);
        gpio_pin_set_dt(&traffic_leds[1], 0);
        // timer2_int = k_uptime_get();
    }
    // current_pins = pins;
}

// static bool button_state[2] = {false, false}; // Track button states

// void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
// {
//     int i;
//     for (i = 0; i < 2; i++) {
//         if (pins & BIT(cap_buttons[i].pin)) {
//             button_state[i] = !button_state[i]; // Toggle button state
//             if (button_state[i]) {
//                 printk("Cap_button %d pressed", i + 1);
//                 gpio_pin_set_dt(&traffic_leds[i], 1);
//                 gpio_pin_set_dt(&traffic_leds[!i], 0); // Turn off the other LED
//             } else {
//                 printk("Cap_button %d released", i + 1);
//                 gpio_pin_set_dt(&traffic_leds[i], 0); // Turn off LED when released
//             }
//         }
//     }
// }

void
configure_buttons(void)
{
    int ret;
    int i;
    static struct gpio_callback button_cb_data;

    // Go through and configure each button.
    GPIO_LIST_LOOP(cap_buttons) {
        if (!gpio_is_ready_dt(&cap_buttons[i])) {
            printk("Error: button device %s is not ready\n",
                cap_buttons[i].port->name);
            return;
        }

        ret = gpio_pin_configure_dt(&cap_buttons[i], GPIO_INPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n",
                ret, cap_buttons[i].port->name, cap_buttons[i].pin);
            return;
        }

        ret = gpio_pin_interrupt_configure_dt(&cap_buttons[i],
                        GPIO_INT_EDGE_RISING);
        if (ret != 0) {
            printk("Error %d: failed to configure interrupt on %s pin %d\n",
                ret, cap_buttons[i].port->name, cap_buttons[i].pin);
            return;
        }
    }

    unsigned int pin_mask_sum = 0;

    GPIO_LIST_LOOP(cap_buttons) {
        pin_mask_sum |= BIT(cap_buttons[i].pin);
    }

    gpio_init_callback(&button_cb_data, button_pressed, pin_mask_sum);
    gpio_add_callback(cap_buttons[0].port, &button_cb_data);
}

void
configure_leds(void)
{
    int ret;
    int i;

    // Go through and check and configure each LED.
    GPIO_LIST_LOOP(traffic_leds) {
        if (traffic_leds[i].port && !gpio_is_ready_dt(&traffic_leds[i])) {
            printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, traffic_leds[i].port->name);
            traffic_leds[i].port = NULL;
	    }

        if (traffic_leds[i].port) {
            ret = gpio_pin_configure_dt(&traffic_leds[i], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, traffic_leds[i].port->name, traffic_leds[i].pin);
                traffic_leds[i].port = NULL;
            } else {
                printk("Set up LED at %s pin %d\n", traffic_leds[i].port->name, traffic_leds[i].pin);
            }
        }
    }
}

void my_isr(void *arg)
{
    return;
}

int
main(void)
{
    int val, i;

    // irq_connect_dynamic(6, 0x0F, NULL, NULL, 0);
    // irq_connect_dynamic(7, 0x0F, NULL, NULL, 0);
    // IRQ_DIRECT_CONNECT(6, 1, button_pressed, 0);
    // IRQ_DIRECT_CONNECT(7, 1, button_pressed, 0);
    // IRQ_DIRECT_CONNECT(irq_p, priority_p, isr_p, flags_p)

    configure_buttons();
    configure_leds();

    gpio_pin_set_dt(&traffic_leds[0], 0);
    gpio_pin_set_dt(&traffic_leds[1], 0);

    while (1) {
        // GPIO_LIST_LOOP(cap_buttons) {
        //     // val = gpio_pin_get_dt(&cap_buttons[i]);
        //     val = gpio_pin_get(cap_buttons[i].port, cap_buttons[i].pin);

        //     if (val >= 0) {
        //         gpio_pin_set_dt(&traffic_leds[i], val);
        //     }
        // }


        // GPIO_LIST_LOOP(cap_buttons) {
        //     val = gpio_pin_get_dt(&cap_buttons[i]);

        //     if ((current_pins & BIT(cap_buttons[0].pin)) && val > 0) {
        //         gpio_pin_set_dt(&traffic_leds[1], 1);
        //     }
        //     else {
        //         gpio_pin_set_dt(&traffic_leds[1], 0);
        //     }

        //     if ((current_pins & BIT(cap_buttons[1].pin)) && val > 0) {
        //     // if (current_pins & BIT(cap_buttons[1].pin)) {
        //         // printk("Cap_button 2 pressed lolllllllllllllll");
        //         gpio_pin_set_dt(&traffic_leds[0], 1);
        //     }
        //     else {
        //         gpio_pin_set_dt(&traffic_leds[0], 0);
        //     }
        // }
        // k_msleep(10);
    }

    return 0;
}
