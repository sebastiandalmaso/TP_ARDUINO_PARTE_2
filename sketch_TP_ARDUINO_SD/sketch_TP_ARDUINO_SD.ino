/*
 * Sistema de Control de Entradas/Salidas Arduino
 * Comunicación bidireccional con Processing
 * 
 * Conexiones:
 * - Entradas (Pulsadores): E1=Pin 2, E2=Pin 3 (con pull-up interno)
 * - Salidas (LEDs): L1=Pin 8, L2=Pin 9
 */

// Definición de pines
const int ENTRADA1 = 2;
const int ENTRADA2 = 3;
const int LED1 = 8;
const int LED2 = 9;

// Variables para almacenar estados
bool estadoE1 = false;
bool estadoE2 = false;
bool estadoL1 = false;
bool estadoL2 = false;

// Variables para antirrebote
bool estadoE1_prev = false;
bool estadoE2_prev = false;
unsigned long ultimoCambioE1 = 0;
unsigned long ultimoCambioE2 = 0;
const unsigned long DEBOUNCE_DELAY = 50;

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  
  // Configurar pines de entrada con pull-up interno
  pinMode(ENTRADA1, INPUT_PULLUP);
  pinMode(ENTRADA2, INPUT_PULLUP);
  
  // Configurar pines de salida
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  // Inicializar salidas en LOW
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  
  // Mensaje de inicio
  delay(1000);
  Serial.println("READY");
}

void loop() {
  // Leer estados de las entradas (invertido por pull-up)
  leerEntradas();
  
  // Procesar comandos desde Processing
  procesarComandosSerial();
  
  // Control local: activar LED correspondiente cuando se presiona el pulsador
  controlLocal();
  
  // Enviar estado de entradas a Processing si hay cambios
  enviarEstadoEntradas();
  
  delay(10);
}

void leerEntradas() {
  unsigned long tiempoActual = millis();
  
  // Leer E1 con antirrebote
  bool lecturaE1 = !digitalRead(ENTRADA1); // Invertir por pull-up
  if (lecturaE1 != estadoE1_prev && (tiempoActual - ultimoCambioE1) > DEBOUNCE_DELAY) {
    estadoE1 = lecturaE1;
    estadoE1_prev = lecturaE1;
    ultimoCambioE1 = tiempoActual;
  }
  
  // Leer E2 con antirrebote
  bool lecturaE2 = !digitalRead(ENTRADA2); // Invertir por pull-up
  if (lecturaE2 != estadoE2_prev && (tiempoActual - ultimoCambioE2) > DEBOUNCE_DELAY) {
    estadoE2 = lecturaE2;
    estadoE2_prev = lecturaE2;
    ultimoCambioE2 = tiempoActual;
  }
}

void controlLocal() {
  // Si se presiona E1, activar/desactivar L1
  static bool estadoE1_anterior = false;
  if (estadoE1 && !estadoE1_anterior) {
    estadoL1 = !estadoL1;
    digitalWrite(LED1, estadoL1);
    enviarEstadoSalidas();
  }
  estadoE1_anterior = estadoE1;
  
  // Si se presiona E2, activar/desactivar L2
  static bool estadoE2_anterior = false;
  if (estadoE2 && !estadoE2_anterior) {
    estadoL2 = !estadoL2;
    digitalWrite(LED2, estadoL2);
    enviarEstadoSalidas();
  }
  estadoE2_anterior = estadoE2;
}

void procesarComandosSerial() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    // Protocolo de comunicación:
    // L1:1 = Encender LED1
    // L1:0 = Apagar LED1
    // L2:1 = Encender LED2
    // L2:0 = Apagar LED2
    // STATUS = Solicitar estado completo
    
    if (comando == "STATUS") {
      enviarEstadoCompleto();
    }
    else if (comando.startsWith("L1:")) {
      int valor = comando.substring(3).toInt();
      estadoL1 = (valor == 1);
      digitalWrite(LED1, estadoL1);
      enviarEstadoSalidas();
    }
    else if (comando.startsWith("L2:")) {
      int valor = comando.substring(3).toInt();
      estadoL2 = (valor == 1);
      digitalWrite(LED2, estadoL2);
      enviarEstadoSalidas();
    }
  }
}

void enviarEstadoEntradas() {
  static bool estadoE1_enviado = false;
  static bool estadoE2_enviado = false;
  
  if (estadoE1 != estadoE1_enviado || estadoE2 != estadoE2_enviado) {
    Serial.print("E:");
    Serial.print(estadoE1 ? "1" : "0");
    Serial.print(",");
    Serial.println(estadoE2 ? "1" : "0");
    
    estadoE1_enviado = estadoE1;
    estadoE2_enviado = estadoE2;
  }
}

void enviarEstadoSalidas() {
  Serial.print("L:");
  Serial.print(estadoL1 ? "1" : "0");
  Serial.print(",");
  Serial.println(estadoL2 ? "1" : "0");
}

void enviarEstadoCompleto() {
  // Enviar estado de entradas
  Serial.print("E:");
  Serial.print(estadoE1 ? "1" : "0");
  Serial.print(",");
  Serial.println(estadoE2 ? "1" : "0");
  
  // Enviar estado de salidas
  Serial.print("L:");
  Serial.print(estadoL1 ? "1" : "0");
  Serial.print(",");
  Serial.println(estadoL2 ? "1" : "0");
}
