# ✏️🎹 Drawdio MIDI

**Un lápiz común que toca música al dibujar, ahora por USB MIDI.**

![Uploading image.png…]()



Un lápiz de grafito con un cable pegado atrás, una ESP32-S3 y nada más. Dibujás sobre papel y cada trazo se convierte en notas MIDI en vivo, que suenan en cualquier DAW o VST.

> Inspirado en el [Drawdio](https://web.media.mit.edu/~silver/drawdio/) original de Jay Silver (MIT Media Lab), que usaba un circuito analógico. Esta versión usa touch capacitivo y manda MIDI.

---

## ¿Cómo funciona?


https://github.com/user-attachments/assets/be5f10d5-14a1-477d-babd-6aa1e8cbd7ef



1. La ESP32-S3 lee el **touch capacitivo** del pin GPIO4, conectado a la mina del lápiz.
2. El grafito del trazo agranda el electrodo, así que **cuanto más largo es el trazo, más sube la lectura**.
3. El sketch convierte esa lectura en una nota de una escala pentatónica de 3 octavas (15 notas).
4. La **velocity** sigue la fuerza del contacto.
5. Todo sale por **USB MIDI nativo** (TinyUSB): se enchufa y suena, sin drivers.

## Qué necesitás

| Cantidad | Componente |
|---|---|
| 1 | ESP32-S3 (probado en DevKitC-1, 44 pines) |
| 1 | Lápiz de grafito común |
| 1-2 | Cables dupont |
| 2 | Cables USB-C (uno para subir el código, otro para el MIDI) |
| — | Pintura de grafito (opcional, mejora el contacto del cable) |
| — | Cinta y papel |

No usa resistencias ni componentes extra.

## Cableado

| Conexión | Pin |
|---|---|
| Cable del lápiz | **GPIO4** (touch T4) |
| GND (opcional) | **G** |
| USB MIDI | Puerto **USB** nativo (GPIO19/20) |

**Armado del lápiz**
1. Dejá a la vista la mina en la parte de atrás del lápiz.
2. Enrollá el cable pelado, cubrilo con grafito y fijalo con cinta.
3. Conectá el otro extremo al pin 4.

> **Sobre GND:** si la lectura es ruidosa (notas que suenan solas), conectá un dupont a G y dejalo fijo o apoyado en una superficie. En las pruebas estabilizó bastante la señal.

## Instalación

**Arduino IDE** con el core de ESP32 versión 3.x:

| Opción | Valor |
|---|---|
| Placa | ESP32S3 Dev Module |
| USB Mode | **USB-OTG (TinyUSB)** |
| USB CDC On Boot | **Enabled** |

1. Abrí `drawdio_midi/drawdio_midi.ino`.
2. Subilo por el puerto **UART**.
3. Enchufá un segundo cable al puerto **USB** (el nativo).
4. Abrí un DAW o monitor MIDI y elegí el dispositivo ESP32-S3.

## Uso

1. Apretá **RESET** y **no toques el lápiz** mientras calibra.
2. Cuando el monitor serie (115200) diga `>>> Hacé ahora tu trazo MÁS LARGO`, tenés 4 segundos para hacer el trazo más largo que vayas a usar. Eso define la nota más aguda.
3. Dibujá: trazos cortos dan notas graves y trazos largos dan agudas.

## Ajustes

| Qué querés cambiar | Dónde |
|---|---|
| La escala | Los números de `escala[]` |
| Menos notas (cada escalón más ancho) | Sacá números de `escala[]` |
| Velocity fija | `VELOCITY_VARIABLE = false` |
| Menos sensible | Subí el `6.0f` de `deltaOn` en `calibrar()` |
| Tiempo para aprender el rango | `APRENDER_MS` |

## Problemas comunes

- **Suena sola o queda una nota pegada:** conectá GND, apretá RESET sin tocar el lápiz y subí el umbral.
- **No suena al dibujar:** revisá el contacto cable-mina (más grafito, más cinta) y fijate en el monitor serie si `delta` sube al apoyar.
- **No aparece el dispositivo MIDI:** usá el puerto **USB** nativo y no el UART. Cerrá otros programas que tengan abierto el mismo dispositivo.
- **No sube el código:** mantené **BOOT**, tocá **RESET**, soltá BOOT.
- **Las notas cambian con la humedad o el papel:** es normal; la base se reajusta sola, pero conviene recalibrar al cambiar de ambiente.

## Créditos

- Idea original: **Drawdio**, de Jay Silver (MIT Media Lab).
- Este proyecto es parte de **Sensitive Lab**, instrumentos electrónicos DIY y tecnología creativa.

## Licencia

[Creative Commons Atribución-NoComercial 4.0 (CC BY-NC 4.0)](https://creativecommons.org/licenses/by-nc/4.0/).
Podés usarlo, estudiarlo, modificarlo y compartirlo dando crédito, pero **no para uso comercial** sin permiso.
