#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/matrix_leds.h"
#include <math.h>
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define ADC_PIN 28 // GPIO para o voltímetro
#define Botao_A 5  // GPIO para botão A

int R_conhecido = 10000;   // Resistor de 10k ohm
float R_x = 0.0;           // Resistor desconhecido
float ADC_VREF = 3.31;     // Tensão de referência do ADC
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
reset_usb_boot(0, 0);
}

const char *color[10]={"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo","Verde", "Azul", "Violeta", "Cinza", "Branco"};

//Vetor com as cores em formato GRB hexadecimal, a ordem das cores segue a ordem do vetor de string de cores acima
const uint32_t matrix_colors[10]={0x00000000, 0x001E1E00, 0x00330000,0x00213300,0x00333300,0x00190000,0x00000033,0x00190019,0x00191919,0x00333333};

uint8_t e24_base[24] = {10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30, 33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91};

// Função para encontrar o valor comercial mais próximo da série E24
/*
  Esta função calcula o valor padrão mais próximo da série de resistores E24
  para a resistência de entrada `R_x`. A série E24 consiste em 24 valores
  espaçados logaritmicamente por década, e esta função itera por esses valores
  em uma faixa de décadas para encontrar a melhor correspondência.
*/
float e24_closest(float R_x) {
    float more_closest = 0;
    float less_error = 1e9;
    for (int exp = -1; exp <= 6; exp++) {
        float factor = powf(10, exp);
        for (uint8_t i = 0; i < 24; i++) {
            float value = e24_base[i] * factor;
            float error = fabsf(R_x - value);
            if (error < less_error) {
                less_error = error;
                more_closest = value;
            }
        }
    }

    if(more_closest>1e5)more_closest=1e5;
    else if (more_closest<510)more_closest=510;

    return more_closest;
}

// Função para determinar as cores do código de cores de um resistor
// com base no valor comercial mais próximo e exibir essas cores na matriz de LEDs.
void color_coding(float commercial_value, const char **code1, const char **code2, const char **multi) {

    uint8_t first, second, multi_exp = 0;

    // Normaliza o valor para obter os dois primeiros dígitos significativos
    while (commercial_value >= 100) {
        commercial_value /= 10; // Reduz o valor dividindo por 10
        multi_exp++;            // Incrementa o expoente do multiplicador
    }

    // Obtém o primeiro e o segundo dígito do valor normalizado
    first = (int)commercial_value / 10;   
    second = (int)commercial_value % 10;

    // Associa os dígitos e o expoente às cores correspondentes
    *code1 = color[first];      // Cor correspondente ao primeiro dígito
    *code2 = color[second];     // Cor correspondente ao segundo dígito
    *multi = color[multi_exp];  // Cor correspondente ao multiplicador

    // Exibe as cores na matriz de LEDs
    matrix_code(colors[first], colors[second], colors[multi_exp]);
}



int main()
{
    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Aqui termina o trecho para modo BOOTSEL com botão B

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    setup_led_matrix();
    
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

    float tensao;
    char str_y[10]; // Buffer para armazenar a string
    float commercial_value;
    const char *code1, *code2, *multi;

    bool cor = true;
    while (true)
    {
        adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

        float soma = 0.0f;
        for (int i = 0; i < 500; i++)
        {
            soma += adc_read();
            sleep_ms(1);
        }
        float media = soma / 500.0f;

        // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);

        commercial_value=e24_closest(R_x);

        sprintf(str_y, "%1.0f Ohms", commercial_value);   // Converte o float em string

        color_coding(commercial_value,&code1, &code2, &multi);

        // cor = !cor;
        //  Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor);                          // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
        ssd1306_line(&ssd, 3, 13, 123, 13, cor);           // Desenha uma linha
        ssd1306_line(&ssd, 3, 24, 123, 24, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "  Ohmimetro", 8, 5);    // Desenha uma string   
        ssd1306_draw_string(&ssd, str_y, 25, 16);          // Desenha uma string
        ssd1306_draw_string(&ssd, "1:", 8, 30);    // Desenha uma string 
        ssd1306_draw_string(&ssd, code1, 25, 30);
        ssd1306_draw_string(&ssd, "2:", 8, 40);    // Desenha uma string 
        ssd1306_draw_string(&ssd, code2, 25, 40);
        ssd1306_draw_string(&ssd, "3:", 8, 50);    // Desenha uma string  
        ssd1306_draw_string(&ssd, multi, 25, 50);
        ssd1306_send_data(&ssd);                           // Atualiza o display
        draw_code_matrix();
        sleep_ms(700);
    }
}