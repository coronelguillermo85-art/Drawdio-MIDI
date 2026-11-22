// Lápiz de grafito -> notas MIDI (ESP32-S3 DevKitC-1, GPIO4)
// Placa: ESP32S3 Dev Module | USB Mode: USB-OTG (TinyUSB) | USB CDC On Boot: Enabled
// Core ESP32 3.x

#include "USB.h"
#include "USBMIDI.h"

USBMIDI midi;

const uint8_t PIN_LAPIZ = 4;
const uint8_t CANAL = 1;
const bool VELOCITY_VARIABLE = true;
const uint32_t APRENDER_MS = 4000;      // tiempo para hacer tu trazo más largo al arrancar
const float RANGO_MAX_DEFAULT = 2500;   // si no tocás al arrancar, usa este

// Pentatónica de Do, 3 octavas (15 notas). Para menos notas, sacá números de acá.
const uint8_t escala[] = {48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74, 76, 79, 81};
const int N_NOTAS = sizeof(escala);

float base = 0, deltaOn = 100, deltaOff = 50, rangoMin = 150, rangoMax = RANGO_MAX_DEFAULT;
bool tocando = false;
int idxActual = -1;
int notaActual = -1;
int candidata = -1;
int cuenta = 0;

float leer() {
  uint32_t suma = 0;
  for (int i = 0; i < 8; i++) suma += touchRead(PIN_LAPIZ);
  return suma / 8.0f;
}

void calibrar() {
  float suma = 0, mn = 1e9, mx = 0;
  for (int i = 0; i < 100; i++) {
    float v = leer();
    suma += v;
    if (v < mn) mn = v;
    if (v > mx) mx = v;
    delay(10);
  }
  base = suma / 100.0f;
  float ruido = mx - mn;
  deltaOn  = constrain(ruido * 6.0f, 60.0f, 300.0f);
  deltaOff = deltaOn * 0.5f;
  rangoMin = deltaOn * 1.5f;
}

void aprenderRango() {
  Serial.println(">>> Hacé ahora tu trazo MÁS LARGO (4 s)...");
  unsigned long t0 = millis();
  float maxd = 0;
  while (millis() - t0 < APRENDER_MS) {
    float d = leer() - base;
    if (d > maxd) maxd = d;
    delay(10);
  }
  if (maxd > deltaOn * 3.0f) rangoMax = maxd * 0.9f;
  else rangoMax = RANGO_MAX_DEFAULT;
}

void setup() {
  Serial.begin(115200);
  midi.begin();
  USB.begin();
  delay(1500);
  calibrar();          // NO toques el lápiz hasta que diga ">>>"
  aprenderRango();
  Serial.printf("base=%.0f on=%.0f off=%.0f min=%.0f max=%.0f\n",
                base, deltaOn, deltaOff, rangoMin, rangoMax);
}

void loop() {
  float v = leer();
  float d = v - base;   // en la S3 sube al tocar

  if (!tocando && d > deltaOn)  tocando = true;
  if (tocando  && d < deltaOff) tocando = false;

  // La base sigue la deriva: rápido en reposo, muy lento mientras suena
  base += (v - base) * (tocando ? 0.0005f : 0.005f);

  int nota = -1;
  uint8_t vel = 100;

  if (tocando) {
    float paso = (rangoMax - rangoMin) / N_NOTAS;
    float pos = (d - rangoMin) / paso;
    int nuevo = (int)floor(pos);
    nuevo = constrain(nuevo, 0, N_NOTAS - 1);

    if (idxActual < 0) idxActual = nuevo;
    else if (nuevo != idxActual) {
      // histéresis: solo cambia de nota si te pasás un 20% del escalón
      if (pos > idxActual + 1.2f || pos < idxActual - 0.2f) idxActual = nuevo;
    }
    nota = escala[idxActual];

    if (VELOCITY_VARIABLE) {
      vel = (uint8_t)constrain(map((long)d, (long)deltaOn, (long)rangoMax, 50, 127), 50, 127);
    }
  } else {
    idxActual = -1;
  }

  // Antirrebote: la nota nueva tiene que repetirse 3 lecturas seguidas
  if (nota == candidata) cuenta++;
  else { candidata = nota; cuenta = 1; }

  if (cuenta >= 3 && nota != notaActual) {
    if (notaActual >= 0) midi.noteOff(notaActual, 0, CANAL);
    if (nota >= 0) midi.noteOn(nota, vel, CANAL);
    else           midi.controlChange(123, 0, CANAL);   // All Notes Off
    notaActual = nota;
  }

  Serial.printf("raw=%.0f base=%.0f delta=%.0f nota=%d\n", v, base, d, notaActual);
  delay(10);
}
