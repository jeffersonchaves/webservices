//******************** BIBLIOTECAS *********************
//https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;


//***************** VARIÁVEIS GLOBAIS ******************

WiFiClient client;

String base_url = "http://SEU_IP:8080/iot/rest/temperatures";

char* ssid      = "REDE";
char* password  = "SENHA";

//********************** SETUP *************************

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connectando ao WiFi...");
  }
}

//********************** LOOP *************************

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    Serial.println("\n\n[HTTP] inicio da requisicao...");

    String requisicao = "POST";

    //REQUISICAO GET
    if (http.begin(client, base_url)) {

      if (requisicao == "GET") {

        httpGETRequest(http);

      } else if(requisicao == "POST"){
        
        httpPOSTRequest(http);
      
      }


      http.end();
      
    } else {
      
      Serial.println("[HTTP] Impossível realizar conexão com o servidor.");
    
    }
  }

  delay(20000);
}


//********************** GET REQUEST *************************

void httpGETRequest(HTTPClient &http) {

  Serial.println("[HTTP] GET...");

  //REALIZA A REQUISIÇÃO
  int httpCode = http.GET();

  // Segundo a biblioteca httpCode será negativo em caso de erro
  if (httpCode > 0) {

    // O cabeçalho HTTP foi enviado e o cabeçalho de resposta do servidor foi tratado
    Serial.println("[HTTP] GET -> RESPONSE CODE: " + String(httpCode));


    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_ACCEPTED) {

      String payload = http.getString();
      Serial.println(payload);

    }

  } else {

    Serial.println("[HTTP] GET -> FALHOU. RESPONSE CODE: " + String(http.errorToString(httpCode).c_str()));

  }
}


//********************** POST REQUEST *************************

void httpPOSTRequest(HTTPClient &http) {

  Serial.println("[HTTP] POST...");

  getHeaders(http);

  // Inicia a conexao e envia os cabeçalhos HTTP
  http.addHeader("Content-Type", "application/json");
  
  //REALIZA A REQUISIÇÃO
  int httpCode = http.POST("{\"temperature\": \"19\", \"timestamp\": \"06/09/2022 14:54:20\" }");

  // Segundo a biblioteca httpCode será negativo em caso de erro
  if (httpCode > 0) {

    // O cabeçalho HTTP foi enviado e o cabeçalho de resposta do servidor foi tratado
    Serial.println("[HTTP] POST -> RESPONSE CODE: " + String(httpCode));


    if (httpCode == HTTP_CODE_CREATED) {

      String payload = http.getString();

      Serial.println("\n**** headers ******");
      String location = http.header("Location");
      Serial.println("location: " + location); 

      String date = http.header("Date");
      Serial.println("date: " + date); 

      Serial.println("\n**** payload ******");
      Serial.println(payload);

    }

  } else {

    Serial.println("[HTTP] GET -> FALHOU. RESPONSE CODE: " + String(http.errorToString(httpCode).c_str()));

  }
}

void getHeaders(HTTPClient &http){
  
  const char * headerKeys[] = {"Date", "Location"};
  const size_t numberOfHeaders = 2;

  http.collectHeaders(headerKeys, numberOfHeaders);
}
