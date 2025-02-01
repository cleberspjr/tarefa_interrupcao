## Autor: Cleber Santos Pinto Júnior - https://github.com/cleberspjr


## Projeto de Controle de LEDs e Botões com RP2040
Este projeto foi desenvolvido para a placa BitDogLab, utilizando o microcontrolador RP2040. O objetivo principal é consolidar o entendimento de interrupções, debouncing de botões e controle de LEDs, incluindo LEDs comuns e LEDs endereçáveis WS2812.

## Objetivos do Projeto
Compreender o uso de interrupções: Utilizar interrupções para detectar o pressionamento de botões e realizar ações específicas.

Implementar debouncing via software: Corrigir o fenômeno de bouncing nos botões para garantir leituras confiáveis.

Controlar LEDs comuns e endereçáveis: Demonstrar o controle de um LED RGB comum e uma matriz de LEDs WS2812.

Fixar o uso de resistores de pull-up internos: Configurar os botões com resistores de pull-up internos para garantir um funcionamento adequado.

Desenvolver um projeto funcional: Integrar hardware e software para criar um sistema que atenda aos requisitos propostos.

## Descrição do Projeto
O projeto utiliza os seguintes componentes conectados à placa BitDogLab:

Matriz 5x5 de LEDs WS2812: Conectada à GPIO 7, exibe números de 0 a 9 com padrões fixos e cores específicas.

LED RGB: Conectado às GPIOs 11 (verde), 12 (azul) e 13 (vermelho), com o LED vermelho piscando continuamente.

Botão A: Conectado à GPIO 5, incrementa o número exibido na matriz de LEDs.

Botão B: Conectado à GPIO 6, decrementa o número exibido na matriz de LEDs.

## Funcionalidades
1. O LED vermelho pisca 5 vezes por segundo, implementado com um loop no main e controle direto do GPIO.

2. Matriz de LEDs WS2812: Exibe números de 0 a 9 em um formato fixo, com cores específicas para cada número.
   O padrão de cada número é definido em uma matriz 5x5, onde cada LED é controlado individualmente.

3. Controle por Botões:
   Botão A: Incrementa o número exibido na matriz (de 0 a 9).
   Botão B: Decrementa o número exibido na matriz (de 9 a 0).

   O tratamento dos botões é feito com interrupções (IRQ) e debouncing via software para evitar leituras múltiplas.

## Funcionamento
# Estrutura do Código
- Interrupções: As interrupções são configuradas para detectar o pressionamento dos botões A e B. Quando um botão é pressionado, uma rotina de interrupção é acionada para atualizar o número exibido na matriz.

- Debouncing: O debouncing é implementado verificando o tempo entre as interrupções. Se o intervalo for menor que 200 ms, a interrupção é ignorada, evitando leituras falsas.

- Controle dos LEDs:

  O LED RGB é controlado diretamente pelas GPIOs, com o LED vermelho piscando em um loop.
  A matriz de LEDs WS2812 é controlada via PIO (Programmable I/O) do RP2040, permitindo o envio de dados para cada LED individualmente.

- Exibição dos Números: Cada número é representado por um padrão fixo na matriz 5x5, com cores específicas definidas no código.


## MODO DE EXECUÇÃO

1. Clonar o Repositório:

```bash
git clone https://github.com/cleberspjr/tarefa_interrupcao.git
```

2. Configure o ambiente de desenvolvimento seguindo as instruções do Pico SDK

3. Abra o projeto no VS Code

4. Importe o projeto através da extensão Raspberry Pi Tools

5. Execute a simulação através do Wokwi ou da placa Bitdoglab


## LINK PARA O VÍDEO DA TAREFA: 
https://drive.google.com/file/d/1DBetwwtJXTJU91W0-O-w4PqCiT9lrC_s/view?usp=sharing
