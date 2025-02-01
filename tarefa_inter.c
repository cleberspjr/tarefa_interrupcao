#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "tarefa_inter.pio.h"

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define LED_MATRIX_PIN 7
#define PIN_G_LED 11
#define PIN_B_LED 12
#define PIN_R_LED 13
#define QTD_LEDS 25

// Funções necessárias para o funcionamento correto
void begin_matrix(uint pin);
void button_init(uint pin);
void begin_leds(uint pin);
void led_matrix_set_color(uint index, uint8_t r, uint8_t g, uint8_t b);
void led_matrix_clear();
void led_matrix_update();
void show_all_numbers(int number_index);
static void button_irq(uint gpio, uint32_t events);
bool turn_off_led(repeating_timer_t *t);

// Variáveis globais
uint8_t led_R[QTD_LEDS] = {0};
uint8_t led_G[QTD_LEDS] = {0};
uint8_t led_B[QTD_LEDS] = {0};
PIO np_pio;
uint sm;
static volatile int current_number = 0;
static volatile uint32_t last_press_btn_A = 0;
static volatile uint32_t last_press_btn_B = 0;

//Matrix 5x5 que mostra os números
const uint32_t numbers[10][25] = {
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1}, // 0
    {1,1,1,1,1,  0,0,1,0,0,  0,0,1,0,1,  0,1,1,0,0,  0,0,1,0,0}, // 1
    {1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1}, // 2
    {1,1,1,1,1,  0,0,0,0,1,  0,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1}, // 3
    {1,0,0,0,0,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1}, // 4
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 5
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 6
    {0,0,0,1,0,  0,0,1,0,0,  0,1,0,0,0,  0,0,0,0,1,  1,1,1,1,1}, // 7
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1}, // 8
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1}, // 9
};
// cada número possui uma cor
int number_colors[10][3] = {
    {2, 0, 0}, {2, 2, 0}, {46, 28, 12}, {1, 0, 1}, {0, 2, 2},
    {2, 1, 0}, {0, 2, 1}, {0, 0, 2}, {2, 2, 0}, {0, 2, 0},
};
//Essa função inicia a matrix
void begin_matrix(uint pin) {
    uint offset = pio_add_program(pio0, &tarefa_inter_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, false);
    tarefa_inter_program_init(np_pio, sm, offset, pin, 800000.f);
    led_matrix_clear();
}
// aqui configura os botoẽs 
void button_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

// Aqui configura os leds
void begin_leds(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}
// Define a cor de um LED específico da matriz
void led_matrix_set_color(uint index, uint8_t r, uint8_t g, uint8_t b) {
    led_R[index] = r;
    led_G[index] = g;
    led_B[index] = b;
}
//aqui limpa a matrix
void led_matrix_clear() {
    memset(led_R, 0, QTD_LEDS);
    memset(led_G, 0, QTD_LEDS);
    memset(led_B, 0, QTD_LEDS);
}
//aqui atualiza a matrix
void led_matrix_update() {
    for (uint i = 0; i < QTD_LEDS; ++i) {
        pio_sm_put_blocking(np_pio, sm, led_G[i]);
        pio_sm_put_blocking(np_pio, sm, led_R[i]);
        pio_sm_put_blocking(np_pio, sm, led_B[i]);
    }
    sleep_us(100);
}

void show_all_numbers(int number_index) {
    led_matrix_clear();
    for (int i = 0; i < QTD_LEDS; i++) {
        if (numbers[number_index][i])
            led_matrix_set_color(i, number_colors[number_index][0], number_colors[number_index][1], number_colors[number_index][2]);
    }
    led_matrix_update();
}

// Função de interrupção dos botões
void button_irq(uint gpio, uint32_t events) {
    uint32_t time_now = to_us_since_boot(get_absolute_time());  //obtem o tempo atual em microsegundos
    static repeating_timer_t timer; //repetidor estático

    // Verifica se a interrupção é válida (evita múltiplas leituras)
    if ((gpio == BUTTON_A_PIN && (time_now - last_press_btn_A > 200000)) ||
        (gpio == BUTTON_B_PIN && (time_now - last_press_btn_B > 200000))) {

        volatile uint32_t *last_time = (gpio == BUTTON_A_PIN) ? &last_press_btn_A : &last_press_btn_B; //Define qual led acender
        uint led_pin = (gpio == BUTTON_A_PIN) ? PIN_G_LED : PIN_B_LED;

        *last_time = time_now;
        
        // Impede que o número passe de 9 ou fique abaixo de 0
        if (gpio == BUTTON_A_PIN && current_number < 9) {
            current_number++;
        } else if (gpio == BUTTON_B_PIN && current_number > 0) {
            current_number--;
        }

        gpio_put(led_pin, 1); //aqui acende o led correspondente ao botão pressionado
        add_repeating_timer_ms(200, turn_off_led, (void *)led_pin, &timer);
    }
}


bool turn_off_led(repeating_timer_t *t) {
    gpio_put((uint)t->user_data, 0);
    return false;
}

int main() {
    stdio_init_all();  //incialização 
    button_init(BUTTON_A_PIN);  
    button_init(BUTTON_B_PIN);

    for (uint pin = PIN_G_LED; pin <= PIN_R_LED; pin++) //aqui inicia os leds individuais 
        begin_leds(pin);

    begin_matrix(LED_MATRIX_PIN); //Inicializa a matriz de LEDs WS2812
    
    //Aqui configura as interrupções
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_irq);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_irq);

    //loop que faz o led vermelho piscar 
    while (true) {
        gpio_put(PIN_R_LED, 1);
        sleep_ms(100);
        gpio_put(PIN_R_LED, 0);
        sleep_ms(100);
        show_all_numbers(current_number); //exibe o número atual na matrix
    }
}
