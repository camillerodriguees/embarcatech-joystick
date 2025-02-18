#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PB 22
#define BOTAO_A 5

#define LED_R 13
#define LED_G 11
#define LED_B 12

#define WIDTH 128
#define HEIGHT 64
#define QUADRADO_TAMANHO 8
#define DEAD_ZONE 100

volatile bool botao_joystick_pressionado = false;
volatile bool botao_a_pressionado = false;

void gpio_callback(uint gpio, uint32_t events) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    if (current_time - last_interrupt_time > 200) { // Debounce de 200ms
        if (gpio == JOYSTICK_PB) {
            botao_joystick_pressionado = true;
        } else if (gpio == BOTAO_A) {
            botao_a_pressionado = true;
        }
    }
    last_interrupt_time = current_time;
}

void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, 4095);
    pwm_set_enabled(slice, true);
}

void set_pwm_duty(uint pin, uint16_t value) {
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, value);
}

int apply_dead_zone(uint16_t adc_value, int center) {
    if (adc_value > center + DEAD_ZONE) {
        return adc_value - (center + DEAD_ZONE);
    }
    if (adc_value < center - DEAD_ZONE) {
        return adc_value - (center - DEAD_ZONE);
    }
    return 0; // Dentro da zona morta
}

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    setup_pwm(LED_R);
    setup_pwm(LED_G);
    setup_pwm(LED_B);

    uint16_t adc_x, adc_y;
    int square_y = (WIDTH - QUADRADO_TAMANHO) / 2;
    int square_x = (HEIGHT - QUADRADO_TAMANHO) / 2;
    int border_status = 0;
    bool leds_ativos = true;

    while (true) {
        adc_select_input(0);
        adc_x = adc_read();
        adc_select_input(1);
        adc_y = adc_read();

        int delta_x = -apply_dead_zone(adc_x, 2048);
        int delta_y = apply_dead_zone(adc_y, 2048);

        square_x += delta_x / 400;
        square_y += delta_y / 400;

        if (square_x < 0) square_x = 0;
        if (square_y < 0) square_y = 0;
        if (square_y > WIDTH - QUADRADO_TAMANHO) square_y = WIDTH - QUADRADO_TAMANHO;
        if (square_x > HEIGHT - QUADRADO_TAMANHO) square_x = HEIGHT - QUADRADO_TAMANHO;

        if (leds_ativos) {
            set_pwm_duty(LED_R, abs(delta_x) * 2);
            set_pwm_duty(LED_B, abs(delta_y) * 2);
        } else {
            set_pwm_duty(LED_R, 0);
            set_pwm_duty(LED_B, 0);
            set_pwm_duty(LED_G, 0);
        }

        if (botao_joystick_pressionado) {
            botao_joystick_pressionado = false;
            border_status = (border_status + 1) % 3;
            if (border_status == 0) {
                set_pwm_duty(LED_G, 0);
            } else {
                set_pwm_duty(LED_G, 4095);
            }
        }

        if (botao_a_pressionado) {
            botao_a_pressionado = false;
            leds_ativos = !leds_ativos;
        }

        ssd1306_fill(&ssd, false);
        if (border_status == 1) {
            ssd1306_rect(&ssd, 1, 1, WIDTH - 2, HEIGHT - 2, true, false);
        } else if (border_status == 2) {
            ssd1306_rect(&ssd, 3, 3, WIDTH - 6, HEIGHT - 6, true, false);
        }
        ssd1306_rect(&ssd, square_x, square_y, QUADRADO_TAMANHO, QUADRADO_TAMANHO, true, true);
        ssd1306_send_data(&ssd);

        sleep_ms(10);
    }
}
