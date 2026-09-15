# 🌱 Sistema Inteligente para Prevenção de Queimadas

Protótipo de um sistema inteligente para monitoramento de condições ambientais associadas ao risco de queimadas, desenvolvido utilizando ESP32 e sensores.

## 🎯 Objetivo

Desenvolver um sistema capaz de coletar dados ambientais e identificar condições que possam indicar maior risco de queimadas, permitindo a geração de alertas preventivos.

## ⚙️ Funcionamento

O sistema realiza leituras periódicas dos sensores conectados ao ESP32:

- 🌡️ Temperatura do ar
- 💧 Umidade do ar
- 🌱 Umidade do solo

A partir desses dados, o sistema aplica critérios de risco. Quando as condições configuradas indicam risco, o ESP32 pode enviar um alerta através de uma requisição HTTP.

## 🔧 Hardware

- ESP32
- Sensor DHT22
- Sensor de umidade do solo
- Painel solar
- Bateria
- Estrutura desenvolvida para o protótipo

## 💻 Software e tecnologias

- C/C++
- ESP32
- Arduino IDE
- Wi-Fi
- HTTP
- JSON

## 🔥 Lógica de detecção

O protótipo considera uma condição de risco quando:

**Temperatura ≥ 35 °C**

e

**Umidade do ar ≤ 30% ou umidade do solo ≤ 20%**

Esses valores são parâmetros configuráveis no código e fazem parte da lógica utilizada neste protótipo.

## 📡 Comunicação

Quando uma condição de risco é identificada, o ESP32 pode enviar os dados coletados para um endpoint através de uma requisição HTTP.

As informações enviadas incluem:

- Temperatura
- Umidade do ar
- Umidade do solo
- Identificação do alerta

## 🚀 Próximos passos

- Monitoramento do nível da bateria
- Integração completa com o sistema de alimentação solar
- Testes em condições reais
- Aperfeiçoamento da lógica de identificação de risco
- Desenvolvimento de uma interface para acompanhamento dos dados

## 📷 Protótipo

Projeto desenvolvido como protótipo acadêmico, integrando conceitos de automação, eletrônica, programação e IoT para uma aplicação de monitoramento ambiental.

## 📷 Imagens do protótipo

### Protótipo físico

![Protótipo físico](prototipo%20ofc.jpeg)

### Modelo e arquitetura do hardware

![Modelo 3D do hardware](Hardware%20Prototipo%20.png)
