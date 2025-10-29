/*
 * Interfaz Gráfica Simple de Control Arduino
 * 
 * IMPORTANTE: Ajustar el puerto serial en la línea 14
 */
import processing.serial.*;
Serial puerto;
String puertoArduino = "COM3"; // CAMBIAR según el puerto serial a utilizar
// Estados
boolean E1 = false;
boolean E2 = false;
boolean L1 = false;
boolean L2 = false;
// Colores simples
color colorAlto = color(0, 200, 0);      // Verde
color colorBajo = color(150);             // Gris
color colorLedOn = color(255, 220, 0);    // Amarillo
color colorLedOff = color(100);           // Gris oscuro
boolean conectado = false;
void setup() {
  size(540, 300);

  println("Puertos disponibles:");
  printArray(Serial.list());

  try {
    puerto = new Serial(this, puertoArduino, 9600);
    puerto.bufferUntil('\n');
    conectado = true;
    delay(2000);
    puerto.write("STATUS\n");
  } catch (Exception e) {
    println("Error: " + e.getMessage());
  }
}
void draw() {
  background(255,100,8);

  // Título
  fill(0);
  textSize(20);
  textAlign(CENTER);
  text("Control Arduino", width/2, 30);

  // ENTRADAS
  fill(0);
  textSize(16);
  textAlign(LEFT);
  text("Entradas", 40, 80);

  dibujarEntrada("E1", 160, 60, E1);
  dibujarEntrada("E2", 290, 60, E2);

  // SALIDAS
  fill(0);
  textSize(16);
  textAlign(LEFT);
  text("Salidas", 45, 180);

  dibujarSalida("L1", 185, 150, L1);
  dibujarSalida("L2", 305, 150, L2);

  // Estado conexión
  fill(conectado ? color(0, 0, 0) : color(0, 0, 0));
  textSize(20);
  textAlign(CENTER);
  text(conectado ? "Conectado" : "Desconectado", width/2, height - 50);
}
void dibujarEntrada(String nombre, int x, int y, boolean estado) {
  // Rectángulo
  fill(estado ? colorAlto : colorBajo);
  stroke(0);
  strokeWeight(2);
  rect(x, y, 100, 50);

  // Texto
  fill(255);
  textSize(18);
  textAlign(CENTER, CENTER);
  text(nombre, x + 50, y + 25);
}
void dibujarSalida(String nombre, int x, int y, boolean estado) {
  // Círculo LED
  fill(estado ? colorLedOn : colorLedOff);
  stroke(0);
  strokeWeight(2);
  ellipse(x + 25, y + 25, 50, 50);

  // Texto
  fill(estado ? 0 : 255);
  textSize(18);
  textAlign(CENTER, CENTER);
  text(nombre, x + 25, y + 25);
}
void mousePressed() {
  if (!conectado) return;
  
  // Verificar clic en L1 (ajustado a la nueva posición)
  if (dist(mouseX, mouseY, 210, 175) < 25) {
    L1 = !L1;
    puerto.write("L1:" + (L1 ? "1" : "0") + "\n");
  }
  
  // Verificar clic en L2 (ajustado a la nueva posición)
  if (dist(mouseX, mouseY, 330, 175) < 25) {
    L2 = !L2;
    puerto.write("L2:" + (L2 ? "1" : "0") + "\n");
  }
}
void serialEvent(Serial p) {
  String mensaje = trim(p.readStringUntil('\n'));
  if (mensaje == null) return;

  println(mensaje);

  if (mensaje.equals("READY")) {
    conectado = true;
    puerto.write("STATUS\n");
  }
  else if (mensaje.startsWith("E:")) {
    String[] valores = mensaje.substring(2).split(",");
    E1 = valores[0].equals("1");
    E2 = valores[1].equals("1");
  }
  else if (mensaje.startsWith("L:")) {
    String[] valores = mensaje.substring(2).split(",");
    L1 = valores[0].equals("1");
    L2 = valores[1].equals("1");
  }
}
