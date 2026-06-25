# Puntata 1 — Fondamenti Teorici: Ripasso C/C++ per Arduino

## Obiettivo

Rinfrescare i concetti chiave di C/C++ e capire **come si trasformano in pratica** su Arduino.  
Chi conosce già questi concetti può scorrere velocemente e concentrarsi sulla sezione "Come si usa su Arduino".

---

## 1. Tipi di Dato

In C/C++ i tipi hanno dimensioni fisse. Su Arduino (AVR/ARM) è importante scegliere il tipo giusto per:
- risparmiare RAM (limitata: 32 KB su R4)
- evitare overflow

| Tipo | Dimensione | Range | Uso tipico su Arduino |
|------|-----------|-------|----------------------|
| `bool` | 1 byte | true/false | flag, stati logici |
| `byte` / `uint8_t` | 1 byte | 0–255 | pin, registri |
| `int` / `int16_t` | 2 byte | -32768–32767 | valori ADC, contatori |
| `unsigned int` / `uint16_t` | 2 byte | 0–65535 | timer, contatori |
| `long` / `int32_t` | 4 byte | ±2 miliardi | `millis()`, timestamp |
| `unsigned long` | 4 byte | 0–4 miliardi | `millis()` (sempre positivo!) |
| `float` | 4 byte | ±3.4×10³⁸ | calcoli sensori (lento!) |
| `char` | 1 byte | -128–127 | caratteri, comunicazione serial |

> ⚠️ **Attenzione:** `millis()` restituisce `unsigned long`. Usa sempre `unsigned long` per le variabili di tempo!

---

## 2. Variabili: Scope e Durata

```cpp
// Variabile globale — vive per tutta l'esecuzione, visibile ovunque
int globalCounter = 0;

void setup() {
    // Variabile locale — esiste solo dentro questa funzione
    int localValue = 10;
}

void loop() {
    // Variabile static — locale ma NON viene reinizializzata a ogni chiamata
    static int callCount = 0;
    callCount++;  // mantiene il valore tra una chiamata e l'altra
}
```

**Regola pratica:** usa variabili globali solo per dati condivisi tra funzioni (es. stato del sistema). Tutto il resto: locali.

---

## 3. Funzioni

In Arduino lo sketch ha due funzioni obbligatorie: `setup()` e `loop()`.  
Ma puoi (e devi!) creare le tue funzioni per organizzare il codice.

```cpp
// Funzione che restituisce un valore
float celsiusToFahrenheit(float celsius) {
    return celsius * 9.0 / 5.0 + 32.0;
}

// Funzione void che modifica una variabile tramite riferimento
void readSensor(int pin, float &result) {
    int raw = analogRead(pin);
    result = raw * (5.0 / 1023.0);  // converti in volt
}
```

---

## 4. Puntatori — Applicazione Pratica su Arduino

I puntatori su Arduino si usano principalmente per:
1. **Passare array a funzioni** (es. buffer di dati)
2. **Modificare variabili in funzioni** (passaggio per riferimento)
3. **Lavorare con la memoria PROGMEM** (salvare costanti in Flash)

```cpp
// Passaggio di array a funzione (l'array decade in puntatore)
float average(int *data, int length) {
    long sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    return (float)sum / length;
}

// Uso:
int readings[5] = {100, 102, 98, 101, 99};
float avg = average(readings, 5);
```

---

## 5. `const` e `#define` — Costanti su Arduino

Preferisci `const` a `#define` quando possibile: il compilatore può verificare i tipi.

```cpp
// Preferito: il compilatore conosce il tipo
const int LED_PIN = 13;
const float VOLTAGE_REFERENCE = 5.0;

// Usato spesso in Arduino legacy — evita per le costanti numeriche
#define LED_OLD 13  // nessun tipo, nessun debug
```

---

## 6. Array e Strutture (struct)

```cpp
// Array di letture per la media mobile
const int BUFFER_SIZE = 10;
int sensorBuffer[BUFFER_SIZE];

// Struttura per raggruppare dati correlati
struct SensorData {
    int rawValue;
    float voltage;
    float celsius;
};

SensorData tempSensor;
tempSensor.rawValue = analogRead(A0);
tempSensor.voltage  = tempSensor.rawValue * (5.0 / 1023.0);
```

---

## 7. Come Questi Concetti si Mappano su Arduino

| Concetto C/C++ | Dove lo trovi su Arduino |
|----------------|--------------------------|
| Variabili globali | stato dei pin, flag, contatori |
| Variabili `static` | debouncing, timer non bloccanti |
| Funzioni | `setup()`, `loop()`, funzioni custom |
| Puntatori/riferimenti | buffer seriali, array di dati |
| `struct` | raggruppare dati di un sensore |
| `const` | numero di pin, parametri fisici |

---

## 📂 Listato

→ [`listati/01_ripasso_base.ino`](./listati/01_ripasso_base.ino)

Il listato dimostra tutti i concetti sopra in un unico sketch funzionante:
- variabili di diversi tipi
- funzioni custom
- uso di struct
- timing non bloccante con `millis()`

---

## 🏋️ Esercizi

1. Aggiungi al listato una funzione `mapVoltage(int raw)` che converta un valore ADC (0–1023) in tensione (0.0–5.0V) e la stampi sulla Serial.
2. Crea una `struct` `LedState` con i campi `pin`, `isOn` (bool), `lastToggleMs` (unsigned long). Usala per controllare il LED nel loop.
3. Modifica la funzione `average()` per usare un puntatore invece di un riferimento, e verifica che il risultato sia identico.
