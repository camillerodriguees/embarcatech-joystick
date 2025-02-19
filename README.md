# Projeto: Controle de Joystick com Interrupção e Debounce

## Descrição
Este projeto implementa a leitura de um joystick analógico e botões em uma placa RP2040 (Raspberry Pi Pico). O código controla a exibição de gráficos em um display OLED SSD1306 via I2C e a intensidade de LEDs RGB via PWM. Além disso, ele utiliza interrupções e debounce para melhorar a precisão na detecção dos botões.

## Funcionalidades
- Leitura de valores do joystick analógico (eixo X e Y) através do ADC.
- Controle de um quadrado na tela OLED baseado nos movimentos do joystick.
- Alternância do estado do LED verde a cada pressionamento do botão do joystick.
- Alternância entre diferentes bordas no display a cada pressionamento do botão do joystick.
- Ativação e desativação dos LEDs ao pressionar o Botão A.
- Implementação de debounce via interrupção para evitar acionamentos múltiplos acidentais.

## Hardware Utilizado
- Placa BitDogLab

## Pinos Utilizados
| Componente  | Pino RP2040 |
|------------|------------|
| Joystick X | GPIO 26 (ADC0) |
| Joystick Y | GPIO 27 (ADC1) |
| Botão Joystick | GPIO 22 |
| Botão A | GPIO 5 |
| LED Vermelho | GPIO 13 (PWM) |
| LED Verde | GPIO 11 (PWM) |
| LED Azul | GPIO 12 (PWM) |
| I2C SDA (Display) | GPIO 14 |
| I2C SCL (Display) | GPIO 15 |

## Bibliotecas Necessárias
- `pico/stdlib.h`
- `hardware/adc.h`
- `hardware/i2c.h`
- `hardware/pwm.h`
- `lib/ssd1306.h`
- `lib/font.h`

## Como Compilar e Executar
1. Instale o SDK do Raspberry Pi Pico e configure o ambiente de desenvolvimento.
2. Compile o código-fonte utilizando `cmake` e `make`:
3. Transfira o arquivo `.uf2` gerado para a placa Pico conectada via USB.



