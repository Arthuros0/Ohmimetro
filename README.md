# Ohmímetro com Reconhecimento Automático de Código de Cores

## Objetivo do Projeto

Desenvolver um sistema embarcado utilizando a placa BitDogLab (RP2040) capaz de:
- Medir o valor de resistores através de um circuito divisor de tensão.
- Calcular o valor da resistência utilizando o ADC.
- Encontrar o valor comercial mais próximo da série E24 (tolerância de 5%).
- Determinar as cores correspondentes para as três primeiras faixas (1ª, 2ª, multiplicador).
- Exibir no display OLED o valor do resistor e as cores das faixas.
- Exibir as cores também na matriz de LEDs.

## Funcionamento

O sistema realiza uma média de várias leituras do ADC para garantir estabilidade, calcula o valor do resistor desconhecido com base na fórmula:
```
R_x = (R_conhecido * ADC_medido) / (ADC_RESOLUÇÃO - ADC_medido)
```

Em seguida:
1. Procura o valor E24 mais próximo ao medido.
2. Calcula as três faixas de cores correspondentes.
3. Exibe no display OLED:
   - O valor numérico da resistência.
   - A identificação das faixas de cor.

Se o resistor não estiver dentro da faixa permitida (510Ω a 100kΩ), o sistema ignora valores inválidos.

## Hardware Utilizado

- Placa BitDogLab (RP2040)
- Display OLED SSD1306 128x64 via I2C
- Matriz de LEDs RGB 5x5
- Resistor conhecido de 10kΩ para divisor de tensão
- Resistores de teste (560Ω, 1kΩ até 10kΩ)
- Protoboard
- 3x Jumpers macho-fêmea

## Bibliotecas Utilizadas

- [pico-sdk](https://github.com/raspberrypi/pico-sdk)
- Biblioteca SSD1306 customizada para BitDogLab
- Biblioteca matrix_leds
- Math.h para operações matemáticas (powf, fabsf)

## Organização do Código

- `e24_closest(float R_x)`: aproxima o valor medido para o mais próximo na série E24.
- `color_coding(float commercial_value, const char **code1, const char **code2, const char **multi)`: determina as cores das faixas.
- `main()`: inicialização dos periféricos, leitura ADC, processamento e exibição no display.
- `void matrix_code(const uint32_t code1, const uint32_t code2, const uint32_t multi)`: preenche o vetor da matriz de LEDs com as cores do resistor.
- `void draw_code_matrix()`: exibe as cores do resistor na matriz de LEDs.


## Arquivos no Repositório

- `main.c`: Código principal do projeto.
- `lib/ssd1306.h` e `lib/ssd1306.c`: Controle do display OLED.
- `lib/font.h`: Fonte para desenhar textos no OLED.
- `lib/matrix_leds.c` e `lib/matrix_leds.h`: Controle da matriz de LEDs RGB.

## Observações

- O sistema aceita resistores na faixa de 510Ω até 100kΩ, conforme especificado na atividade.
- Valores fora dessa faixa não são reconhecidas.
- As cores exibidas correspondem ao padrão de codificação de cores de resistores 5%.

---

