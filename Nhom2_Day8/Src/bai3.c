#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

// LED định nghĩa
#define LED_GREEN_PORT         GPIOA
#define LED_GREEN_PIN          GPIO_Pin_5
#define LED_GREEN_CLOCK        RCC_AHB1Periph_GPIOA

#define RGB_GREEN_PORT         GPIOA     //RGB_GREEN PA0
#define RGB_GREEN_PIN          GPIO_Pin_0
#define RGB_GREEN_CLOCK        RCC_AHB1Periph_GPIOA

#define RGB_GREEN2_PORT        GPIOA    // RGB_RED PA11
#define RGB_GREEN2_PIN         GPIO_Pin_11
#define RGB_GREEN2_CLOCK       RCC_AHB1Periph_GPIOA

#define LED_RED1_PORT          GPIOA    //RGB_RED pa1
#define LED_RED1_PIN           GPIO_Pin_1
#define LED_RED1_CLOCK         RCC_AHB1Periph_GPIOA

#define LED_RED2_PORT          GPIOB
#define LED_RED2_PIN           GPIO_Pin_13
#define LED_RED2_CLOCK         RCC_AHB1Periph_GPIOB

#define BUZZER_PORT            GPIOC
#define BUZZER_PIN             GPIO_Pin_9
#define BUZZER_CLOCK           RCC_AHB1Periph_GPIOC

#define LED3_GREEN_PORT        GPIOA
#define LED3_GREEN_PIN         GPIO_Pin_5
#define LED3_GREEN_CLOCK       RCC_AHB1Periph_GPIOA

// Nút nhấn
#define BUTTON_B3_PORT         GPIOA
#define BUTTON_B3_PIN          GPIO_Pin_4
#define BUTTON_B3_CLOCK        RCC_AHB1Periph_GPIOA

#define BUTTON_B5_PORT         GPIOB
#define BUTTON_B5_PIN          GPIO_Pin_4
#define BUTTON_B5_CLOCK        RCC_AHB1Periph_GPIOB

#define BTN_PRESSED            Bit_RESET   // Vì dùng GPIO_PuPd_UP
#define BTN_RELEASED           Bit_SET

// ---------- HÀM TRỢ GIÚP ----------
void delay_ms(uint32_t time) {
    for (uint32_t i = 0; i < (time * 800); i++);
}

void init_output(GPIO_TypeDef* port, uint16_t pin, uint32_t clock) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(clock, ENABLE);

    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);
}

void init_input(GPIO_TypeDef* port, uint16_t pin, uint32_t clock) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(clock, ENABLE);

    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);
}

void set_pin(GPIO_TypeDef* port, uint16_t pin, uint8_t state) {
    if (state)
        GPIO_SetBits(port, pin);
    else
        GPIO_ResetBits(port, pin);
}

uint8_t read_button(GPIO_TypeDef* port, uint16_t pin) {
    return GPIO_ReadInputDataBit(port, pin);
}

void blink_all_leds_and_buzzer(uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        set_pin(RGB_GREEN_PORT, RGB_GREEN_PIN, 1);
        set_pin(LED_RED1_PORT, LED_RED1_PIN, 1);
        set_pin(LED_RED2_PORT, LED_RED2_PIN, 1);
        set_pin(BUZZER_PORT, BUZZER_PIN, 1);
        delay_ms(300);

        set_pin(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
        set_pin(LED_RED1_PORT, LED_RED1_PIN, 0);
        set_pin(LED_RED2_PORT, LED_RED2_PIN, 0);
        set_pin(BUZZER_PORT, BUZZER_PIN, 0);
        delay_ms(300);
    }
}

void blink_rgb_green2(uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        set_pin(RGB_GREEN2_PORT, RGB_GREEN2_PIN, 1);
        delay_ms(300);
        set_pin(RGB_GREEN2_PORT, RGB_GREEN2_PIN, 0);
        delay_ms(300);
    }
}

// ---------- HÀM CHÍNH ----------
int main(void) {
    uint8_t press_count_b3 = 0;
    uint16_t hold_counter_b5 = 0;
    uint8_t led_b5_on = 0;

    // Khởi tạo LED và Buzzer
    init_output(LED_GREEN_PORT, LED_GREEN_PIN, LED_GREEN_CLOCK);
    init_output(RGB_GREEN_PORT, RGB_GREEN_PIN, RGB_GREEN_CLOCK);
    init_output(RGB_GREEN2_PORT, RGB_GREEN2_PIN, RGB_GREEN2_CLOCK);  // Khởi tạo LED RGB_GREEN PA11
    init_output(LED_RED1_PORT, LED_RED1_PIN, LED_RED1_CLOCK);
    init_output(LED_RED2_PORT, LED_RED2_PIN, LED_RED2_CLOCK);
    init_output(BUZZER_PORT, BUZZER_PIN, BUZZER_CLOCK);
    init_output(LED3_GREEN_PORT, LED3_GREEN_PIN, LED3_GREEN_CLOCK);  // Thêm LED3

    // Khởi tạo nút nhấn
    init_input(BUTTON_B3_PORT, BUTTON_B3_PIN, BUTTON_B3_CLOCK);
    init_input(BUTTON_B5_PORT, BUTTON_B5_PIN, BUTTON_B5_CLOCK);

    // Nháy LED GREEN 3 lần khi khởi động
    for (int i = 0; i < 3; i++) {
        set_pin(LED_GREEN_PORT, LED_GREEN_PIN, 1);
        delay_ms(300);
        set_pin(LED_GREEN_PORT, LED_GREEN_PIN, 0);
        delay_ms(300);
    }

    // ---------- VÒNG LẶP CHÍNH ----------
    while (1) {
        // Xử lý nút B3 nhấn 3 lần
        if (read_button(BUTTON_B3_PORT, BUTTON_B3_PIN) == BTN_PRESSED) {
            delay_ms(20);  // debounce
            if (read_button(BUTTON_B3_PORT, BUTTON_B3_PIN) == BTN_PRESSED) {
                while (read_button(BUTTON_B3_PORT, BUTTON_B3_PIN) == BTN_PRESSED);  // đợi nhả
                press_count_b3++;
                if (press_count_b3 >= 3) {
                    blink_all_leds_and_buzzer(3);
                    blink_rgb_green2(3);   // Nháy LED RGB_GREEN PA11 3 lần
                    press_count_b3 = 0;
                }
            }
        }

        // Xử lý giữ nút B5 > 500ms để bật cả LED RGB_GREEN (PA0) và RGB_GREEN2 (PA11)
        if (read_button(BUTTON_B5_PORT, BUTTON_B5_PIN) == BTN_PRESSED) {
            delay_ms(10);
            hold_counter_b5 += 100;
            if (hold_counter_b5 >= 500 && led_b5_on == 0) {
                set_pin(RGB_GREEN_PORT, RGB_GREEN_PIN, 1);  // Bật LED PA0
                set_pin(RGB_GREEN2_PORT, RGB_GREEN2_PIN, 1); // Bật LED PA11
                led_b5_on = 1;
            }
        } else {
            // Khi nhả nút, tắt cả hai LED xanh
            hold_counter_b5 = 0;
            led_b5_on = 0;
            set_pin(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);  // Tắt LED PA0
            set_pin(RGB_GREEN2_PORT, RGB_GREEN2_PIN, 0); // Tắt LED PA11
        }

}
}
