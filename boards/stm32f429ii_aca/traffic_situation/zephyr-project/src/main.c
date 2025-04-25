#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <inttypes.h>

#define SLEEP_TIME_MS        1
#define BTN_PRESS_SLEEP_TIME 4000  /* Time between traffic change after pedestrian button press. */
#define TRAFFIC_SWITCH_TIME  2000  /* Time it takes to switch between traffic light states */
#define PEDESTRIAN_WALK_TIME 10000 /* Time pedestrians have to walk before traffic light switch */

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

/* Loop through a list of GPIOs with a given integer */
#define GPIO_LIST_LOOP(list_name, i) for (i = 0; i < LEN(list_name); i++)

/* Onboard car traffic light LEDs 1-3. */
static struct gpio_dt_spec car_traffic_leds[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}), /* red */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0}), /* yellow */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0}), /* green */
};

// static struct gpio_dt_spec car_traffic_red_led    = car_traffic_leds[0];
// static struct gpio_dt_spec car_traffic_yellow_led = car_traffic_leds[1];
// static struct gpio_dt_spec car_traffic_green_led  = car_traffic_leds[2];

/* Aliases for car traffic light LEDs */
static struct gpio_dt_spec car_traffic_red_led    = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});
static struct gpio_dt_spec car_traffic_yellow_led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0});
static struct gpio_dt_spec car_traffic_green_led  = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0});

/* Onboard pedestrian traffic light LEDs 5 and 6. */
static struct gpio_dt_spec pedestrian_leds[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led4), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led5), gpios, {0}),
};

/* Aliases for pedestrian traffic light LEDs */
static struct gpio_dt_spec pedestrian_red_led   = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led4), gpios, {0});
static struct gpio_dt_spec pedestrian_green_led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led5), gpios, {0});

/* Pedstrian traffic light control button. */
static struct gpio_dt_spec pedestrian_button = GPIO_DT_SPEC_GET_OR(
              DT_ALIAS(sw2), gpios, {0});

static struct gpio_callback button_cb_data;

/* Lock for if there currently is an ongoing traffic situation */
int traffic_state = 0;

void
change_traffic_situation(void)
{
    traffic_state = 1;

    k_msleep(BTN_PRESS_SLEEP_TIME); 

    gpio_pin_set_dt(&car_traffic_green_led,    0);
    gpio_pin_set_dt(&car_traffic_yellow_led,   1);
    k_msleep(TRAFFIC_SWITCH_TIME); 

    gpio_pin_set_dt(&car_traffic_yellow_led, 0);
    gpio_pin_set_dt(&car_traffic_red_led,    1);
    k_msleep(TRAFFIC_SWITCH_TIME); 

    gpio_pin_set_dt(&pedestrian_red_led,   0);
    gpio_pin_set_dt(&pedestrian_green_led, 1);
    k_msleep(PEDESTRIAN_WALK_TIME);

    gpio_pin_set_dt(&pedestrian_red_led,   1);
    gpio_pin_set_dt(&pedestrian_green_led, 0);
    k_msleep(TRAFFIC_SWITCH_TIME); 

    gpio_pin_set_dt(&car_traffic_yellow_led, 1);
    gpio_pin_set_dt(&car_traffic_red_led,    0);
    k_msleep(TRAFFIC_SWITCH_TIME); 

    gpio_pin_set_dt(&car_traffic_yellow_led, 0);
    gpio_pin_set_dt(&car_traffic_green_led,  1);

    traffic_state = 0;
}

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	// printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());

    // if (pins & BIT(pedestrian_button.pin)) {
        // if (!traffic_state)
        //     change_traffic_situation();
    // }

    printk("Pedestrian traffic button pressed.",
                    "Changing traffic situation\n");
    
    // if (!traffic_state)
    //     change_traffic_situation();
}

void
configure_buttons(void)
{
    int ret;

    /* Configure pedestrian button. */
    if (!gpio_is_ready_dt(&pedestrian_button)) {
        printk("Error: button device %s is not ready\n",
            pedestrian_button.port->name);
        return;
    }

    ret = gpio_pin_configure_dt(&pedestrian_button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, pedestrian_button.port->name, pedestrian_button.pin);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&pedestrian_button,
                    GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, pedestrian_button.port->name, pedestrian_button.pin);
        return;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(pedestrian_button.pin));
    gpio_add_callback(pedestrian_button.port, &button_cb_data);
}

void
configure_leds(void)
{
    int ret;
    int i;

    /* Go through and check and configure each car traffic light LED. */
    GPIO_LIST_LOOP(car_traffic_leds, i) {
        if (car_traffic_leds[i].port && !gpio_is_ready_dt(&car_traffic_leds[i])) {
            printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, car_traffic_leds[i].port->name);
            car_traffic_leds[i].port = NULL;
	    }

        if (car_traffic_leds[i].port) {
            ret = gpio_pin_configure_dt(&car_traffic_leds[i], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, car_traffic_leds[i].port->name, car_traffic_leds[i].pin);
                car_traffic_leds[i].port = NULL;
            } else {
                printk("Set up LED at %s pin %d\n", car_traffic_leds[i].port->name, car_traffic_leds[i].pin);
            }
        }
    }

    /* Go through and check and configure each pedestrian traffic light LED. */
    GPIO_LIST_LOOP(pedestrian_leds, i) {
        if (pedestrian_leds[i].port && !gpio_is_ready_dt(&pedestrian_leds[i])) {
            printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, pedestrian_leds[i].port->name);
            pedestrian_leds[i].port = NULL;
	    }

        if (pedestrian_leds[i].port) {
            ret = gpio_pin_configure_dt(&pedestrian_leds[i], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, pedestrian_leds[i].port->name, pedestrian_leds[i].pin);
                pedestrian_leds[i].port = NULL;
            } else {
                printk("Set up LED at %s pin %d\n", pedestrian_leds[i].port->name, pedestrian_leds[i].pin);
            }
        }
    }
}

int
main(void)
{
    int val, i;

    configure_buttons();
    configure_leds();

    GPIO_LIST_LOOP(car_traffic_leds, i) {
        gpio_pin_set_dt(&car_traffic_leds[i], 0);
    }

    GPIO_LIST_LOOP(pedestrian_leds, i) {
        gpio_pin_set_dt(&pedestrian_leds[i], 0);
    }
    
    /* Set car traffic light to be green at start */
    gpio_pin_set_dt(&car_traffic_green_led, 1);

    /* Set pedestrian traffic light to be red at start */
    gpio_pin_set_dt(&pedestrian_red_led, 1);

    while (1) { /* If we have an LED, match its state to the button's. */
        val = gpio_pin_get_dt(&pedestrian_button);

        if (!traffic_state) {
            if (val > 0) {
                change_traffic_situation();
            }
        }
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}