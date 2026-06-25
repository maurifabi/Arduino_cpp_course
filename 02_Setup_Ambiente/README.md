# Puntata 2 — Setup Ambiente e Arduino Uno R4 WiFi

## Obiettivo

Configurare l'ambiente di sviluppo, capire la struttura di uno sketch Arduino e scrivere il primo programma reale con **funzioni custom** e **LED blink non bloccante**.

---

## 1. Setup IDE Arduino

### Arduino IDE 2.x (consigliato)

1. Scarica [Arduino IDE 2](https://www.arduino.cc/en/software)
2. Installa la board **Arduino UNO R4** tramite Board Manager:
   - Vai su **Tools → Board → Boards Manager**
   - Cerca `Arduino UNO R4`
   - Installa il pacchetto **Arduino UNO R4 Boards** (by Arduino)
3. Collega la board con cavo USB-C
4. Seleziona la porta: **Tools → Port → (la porta COM/ttyACM)**
5. Seleziona la board: **Tools → Board → Arduino UNO R4 WiFi**

### Verifica Installazione

Apri **File → Examples → 01.Basics → Blink** e carica. Il LED sulla board dovrebbe lampeggiare. ✅

---

## 2. Struttura di uno Sketch Arduino

Un file `.ino` è in realtà C++ con alcune macro aggiunte dall'IDE:

```
NomeProgetto/
└── NomeProgetto.ino     ← file principale
```

Lo sketch ha sempre due funzioni obbligatorie:

```cpp
void setup() {
    // Eseguita UNA SOLA VOLTA all'avvio (o dopo reset)
    // → Inizializza pin, Serial, librerie
}

void loop() {
    // Eseguita CONTINUAMENTE in un ciclo infinito
    // → Logica del programma, letture, aggiornamenti
}
```

> **Dietro le quinte:** l'IDE genera automaticamente la funzione `main()` che chiama `setup()` e poi `loop()` in un `while(true)`. Non devi scrivere `main()`.

---

## 3. Arduino Uno R4 WiFi — Caratteristiche Chiave

| Caratteristica | Valore |
|---------------|--------|
| Microcontrollore | Renesas RA4M1 (ARM Cortex-M4, 48 MHz) |
| RAM | 32 KB SRAM |
| Flash | 256 KB |
| Pin digitali I/O | 14 (di cui 6 PWM) |
| Pin analogici input | 6 (A0–A5) |
| Risoluzione ADC | 14 bit (0–16383) |
| Tensione operativa | 5V |
| WiFi | ESP32-S3 integrato |
| LED built-in | Pin 13 |
| LED Matrix | 12×8 LED RGB integrata |

> ⚠️ **Differenza importante rispetto all'Uno classico:**  
> L'R4 usa ARM a 32 bit. `int` è **2 byte** (come AVR), ma `double` è **8 byte** (diverso da AVR dove era 4). L'ADC ha risoluzione **14 bit** (vs 10 bit dell'Uno classico), quindi `analogRead()` ritorna 0–16383.

---

## 4. Pin e I/O Digitale — Basi

```cpp
// Configura un pin come OUTPUT o INPUT
pinMode(13, OUTPUT);           // pin 13 come uscita
pinMode(2, INPUT_PULLUP);      // pin 2 come ingresso con resistenza pull-up interna

// Scrittura digitale
digitalWrite(13, HIGH);        // porta il pin a 5V (ON)
digitalWrite(13, LOW);         // porta il pin a 0V (OFF)

// Lettura digitale
int state = digitalRead(2);    // legge HIGH o LOW
```

---

## 5. Timing: `delay()` vs `millis()`

### `delay(ms)` — bloccante ❌ (da evitare nei progetti reali)

```cpp
void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);   // BLOCCA tutto per 1 secondo
    digitalWrite(LED_PIN, LOW);
    delay(1000);   // BLOCCA ancora
    // In questo secondo non puoi leggere pulsanti, sensori, ecc.
}
```

### `millis()` — non bloccante ✅ (approccio corretto)

```cpp
unsigned long lastToggle = 0;
bool ledState = false;

void loop() {
    unsigned long now = millis();
    if (now - lastToggle >= 1000UL) {
        lastToggle = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
    // Qui puoi fare ALTRO mentre aspetti il secondo!
}
```

---

## 6. Serial Monitor

Strumento fondamentale per il debug:

```cpp
void setup() {
    Serial.begin(115200);    // velocità in baud
    while (!Serial) { ; }   // attendi connessione (solo su R4/Leonardo/ecc)
}

void loop() {
    Serial.println("Messaggio");        // stampa con newline
    Serial.print("Valore: ");
    Serial.println(42);
    Serial.print("Float: ");
    Serial.println(3.14, 2);            // 2 decimali
}
```

Per vedere l'output: **Tools → Serial Monitor** (o `Ctrl+Shift+M`). Imposta il baud a **115200**.

---

## 📂 Listato

→ [`listati/02_blink_custom.ino`](./listati/02_blink_custom.ino)

Primo sketch completo con:
- LED blink non bloccante
- Funzioni custom per organizzare il codice
- Contatore di blink con output Serial
- Commenti didattici

---

## 🏋️ Esercizi

1. Modifica `BLINK_INTERVAL` a 500ms. Poi prova 200ms e 2000ms. Osserva la differenza.
2. Aggiungi un secondo LED su pin 12 (con resistenza 220Ω) che lampeggi a **frequenza doppia** rispetto al primo.
3. Aggiungi alla Serial la stampa del **tempo esatto in secondi** ad ogni blink (es: `"Blink #3 alle 3.0s"`).
4. **Sfida:** fai lampeggiare il LED in pattern SOS (3 corti, 3 lunghi, 3 corti) usando solo `millis()`.
