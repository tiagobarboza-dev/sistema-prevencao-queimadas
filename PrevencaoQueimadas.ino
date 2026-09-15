/*
  ============================================================
  PROJETO DE PREVENÇÃO DE QUEIMADAS - PROTÓTIPO INTELIGENTE
  ============================================================
  Placa: ESP32
  Sensores: DHT22 (temperatura e umidade do ar)
            Sensor resistivo/capacitivo de umidade do solo (analógico)
  Alimentação: painel solar + bateria
  Saída: alerta preventivo via requisição HTTP (pode ser um webhook
         do IFTTT, Blynk, um backend próprio, etc.) quando as
         condições indicarem risco de incêndio.

  Bibliotecas necessárias (instalar pela Library Manager da Arduino IDE):
    - "DHT sensor library" (Adafruit)
    - "Adafruit Unified Sensor"
  ============================================================
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ----------------- CONFIGURAÇÃO DE REDE -----------------
const char* WIFI_SSID = "NOME_DA_SUA_REDE";
const char* WIFI_SENHA = "SENHA_DA_SUA_REDE";

// URL de um webhook/endpoint que recebe o alerta (ex: IFTTT, backend próprio)
const char* URL_ALERTA = "https://SEU_ENDPOINT_DE_ALERTA.com/api/alerta";

// ----------------- PINOS -----------------
#define PINO_DHT 4              // Pino digital ligado ao DHT22
#define TIPO_DHT DHT22
#define PINO_UMIDADE_SOLO 34    // Pino analógico (ADC) ligado ao sensor de solo

DHT dht(PINO_DHT, TIPO_DHT);

// ----------------- CALIBRAÇÃO DO SENSOR DE SOLO -----------------
// Valores brutos lidos pelo ADC (0-4095 no ESP32).
// Calibrar com o sensor no ar (seco) e submerso em água (úmido).
const int SOLO_SECO = 3000;   // leitura aproximada com o sensor seco
const int SOLO_UMIDO = 1200;  // leitura aproximada com o sensor em água

// ----------------- LIMIARES DE RISCO -----------------
const float TEMP_LIMITE = 35.0;       // °C - acima disso, risco aumenta
const float UMIDADE_AR_LIMITE = 30.0; // % - abaixo disso, risco aumenta
const float UMIDADE_SOLO_LIMITE = 20.0; // % - abaixo disso, risco aumenta

// Intervalo entre leituras (ms). Pode ser ajustado para economizar bateria.
const unsigned long INTERVALO_LEITURA = 60000; // 1 minuto

// Evita ficar enviando alerta repetido a cada leitura
bool alertaJaEnviado = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(PINO_UMIDADE_SOLO, INPUT);

  conectarWiFi();
}

void loop() {
  float temperatura = dht.readTemperature();
  float umidadeAr = dht.readHumidity();
  float umidadeSolo = lerUmidadeSolo();

  if (isnan(temperatura) || isnan(umidadeAr)) {
    Serial.println("Falha ao ler o sensor DHT22. Tentando novamente...");
    delay(2000);
    return;
  }

  Serial.println("----- Leitura dos sensores -----");
  Serial.print("Temperatura do ar: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Umidade do ar: ");
  Serial.print(umidadeAr);
  Serial.println(" %");

  Serial.print("Umidade do solo: ");
  Serial.print(umidadeSolo);
  Serial.println(" %");

  bool risco = calcularRisco(temperatura, umidadeAr, umidadeSolo);

  if (risco) {
    Serial.println("!!! RISCO DE QUEIMADA DETECTADO !!!");
    if (!alertaJaEnviado) {
      enviarAlerta(temperatura, umidadeAr, umidadeSolo);
      alertaJaEnviado = true;
    }
  } else {
    Serial.println("Condições dentro da normalidade.");
    alertaJaEnviado = false; // permite alertar de novo se o risco voltar a ocorrer
  }

  Serial.println("---------------------------------\n");

  delay(INTERVALO_LEITURA);
}

/**
 * Lê o sensor de umidade do solo e converte a leitura bruta do ADC
 * (0-4095) para uma porcentagem de umidade (0% seco - 100% úmido),
 * usando os valores de calibração SOLO_SECO e SOLO_UMIDO.
 */
float lerUmidadeSolo() {
  int leituraBruta = analogRead(PINO_UMIDADE_SOLO);

  float porcentagem = map(leituraBruta, SOLO_SECO, SOLO_UMIDO, 0, 100);

  // Garante que o valor fique entre 0% e 100%, mesmo fora da calibração
  if (porcentagem < 0) porcentagem = 0;
  if (porcentagem > 100) porcentagem = 100;

  return porcentagem;
}

/**
 * Avalia as três leituras e decide se as condições atuais representam
 * risco de queimada. A regra usada aqui é: alta temperatura E
 * (baixa umidade do ar OU baixa umidade do solo).
 */
bool calcularRisco(float temperatura, float umidadeAr, float umidadeSolo) {
  bool temperaturaAlta = temperatura >= TEMP_LIMITE;
  bool arSeco = umidadeAr <= UMIDADE_AR_LIMITE;
  bool soloSeco = umidadeSolo <= UMIDADE_SOLO_LIMITE;

  return temperaturaAlta && (arSeco || soloSeco);
}

/**
 * Conecta o ESP32 à rede Wi-Fi configurada.
 */
void conectarWiFi() {
  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNão foi possível conectar ao Wi-Fi. Alertas via rede não serão enviados.");
  }
}

/**
 * Envia um alerta em formato JSON para o endpoint configurado
 * (webhook, backend, app, etc.) informando as leituras que geraram o risco.
 */
void enviarAlerta(float temperatura, float umidadeAr, float umidadeSolo) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem conexão Wi-Fi: não foi possível enviar o alerta.");
    return;
  }

  HTTPClient http;
  http.begin(URL_ALERTA);
  http.addHeader("Content-Type", "application/json");

  String corpoJson = "{";
  corpoJson += "\"alerta\":\"risco_de_queimada\",";
  corpoJson += "\"temperatura\":" + String(temperatura) + ",";
  corpoJson += "\"umidade_ar\":" + String(umidadeAr) + ",";
  corpoJson += "\"umidade_solo\":" + String(umidadeSolo);
  corpoJson += "}";

  int codigoResposta = http.POST(corpoJson);

  if (codigoResposta > 0) {
    Serial.print("Alerta enviado! Código de resposta: ");
    Serial.println(codigoResposta);
  } else {
    Serial.print("Erro ao enviar alerta. Código: ");
    Serial.println(codigoResposta);
  }

  http.end();
}
