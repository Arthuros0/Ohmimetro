#include "matrix_leds.h"

#define NUM_PIXELS 25

#define OUT_PIN 7

PIO pio=pio0;
uint sm=0;

// Função para obter o índice correto de um LED na matriz
// A matriz é organizada em zigue-zague, então esta função
// ajusta o índice com base na linha e coluna.
uint8_t obter_index(uint8_t i) {
  uint8_t x = i % 5;  // Calcula a coluna (resto da divisão por 5)
  uint8_t y = i / 5;  // Calcula a linha (divisão inteira por 5)
  // Se a linha for par, o índice é calculado normalmente.
  // Se a linha for ímpar, a ordem das colunas é invertida.
  return (y % 2 == 0) ? (y * 5 + x) : (y * 5 + (4 - x));
}

// Função para configurar os códigos de cores em colunas específicas da matriz
// code1: Cor para a coluna 1
// code2: Cor para a coluna 3
// multi: Cor para a coluna 5
void matrix_code(const uint32_t code1, const uint32_t code2, const uint32_t multi) {
  for (uint8_t i = 0; i < 5; i++) { // Itera sobre as 5 linhas da matriz
    status[i * 5 + 0] = code1; // Define a cor da coluna 1
    status[i * 5 + 2] = code2; // Define a cor da coluna 3
    status[i * 5 + 4] = multi; // Define a cor da coluna 5
  }
}

// Função para desenhar a matriz de LEDs com base no array de status
void draw_code_matrix() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) { // Itera sobre todos os LEDs
    uint8_t pos = obter_index(i); // Obtém o índice correto do LED
    // Envia o valor de cor correspondente ao LED para o PIO
    pio_sm_put_blocking(pio0, sm, status[24 - pos]);
  }
  sleep_us(100); // Aguarda 100 microsegundos para atualizar a matriz
}


void setup_led_matrix() {
    bool ok;
    // Configura o clock para 133 MHz
    ok = set_sys_clock_khz(133000, false);
    stdio_init_all();

    printf("Iniciando a transmissão PIO\n");
    if (ok) printf("Clock configurado para %ld Hz\n", clock_get_hz(clk_sys));

    // Configuração do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);
}


uint32_t status[25] = {
    0x00000000, 0x01010100, 0x00000100, 0x01010100, 0x00000000, 
    0x00000000, 0x01010100, 0x00000100, 0x01010100, 0x00000000, 
    0x00000000, 0x01010100, 0x00000100, 0x01010100, 0x00000000, 
    0x00000000, 0x01010100, 0x00000100, 0x01010100, 0x00000000, 
    0x00000000, 0x01010100, 0x00000100, 0x01010100, 0x00000000
    };

const uint32_t colors[10] = {
  0x00000000, // Preto
  0x06120000, // Marrom
  0x00190000, // Vermelho
  0x020D0000, // Laranja
  0x19190000, // Amarelo
  0x19000000, // Verde
  0x00001900, // Azul
  0x000C0C00, // Violeta
  0x01010100, // Cinza
  0x19191900  // Branco
};