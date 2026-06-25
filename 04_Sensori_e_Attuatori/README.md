# Puntata 4 — Sensori e Letture Analogiche

## Obiettivo

Leggere sensori analogici in modo professionale: capire l'ADC, convertire i valori raw in grandezze fisiche e **filtrare il rumore** con la media mobile.

---

## 1. L'ADC di Arduino Uno R4 WiFi

L'**ADC** (Analog-to-Digital Converter) converte una tensione analogica (0–5V) in un numero digitale.

Su Arduino Uno R4 WiFi:
- **Risoluzione: 14 bit** → valori da 0 a 16383
- `analogRead(pin)` → ritorna un valore tra 0 e 16383
- Tensione di riferimento: 5V (default)

> ⚠️ **Differenza dall'Uno classico:** l'Uno standard ha 10 bit (0–1023). Se usi codice vecchio, devi adattare le conversioni!

### Formula di Conversione

```
Tensione = (valore_ADC / 16383.0) × 5.0
```

```cpp
const float ADC_MAX     = 16383.0;  // R4 WiFi: 14 bit
const float VREF        = 5.0;      // tensione di riferimento

float readVoltage(int pin) {
    int raw = analogRead(pin);
    return (raw / ADC_MAX) * VREF;
}
```

---

## 2. Sensori Analogici Comuni

### Sensore NTC (termistore) — Temperatura

Un **NTC** è una resistenza che diminuisce con la temperatura. Si usa in un **partitore di tensione** con una resistenza fissa R0.

```
5V ──[R0=10kΩ]──┬── A0
                │
              [NTC]
                │
               GND
```

**Formula di Steinhart-Hart (semplificata):**

```cpp
const float R0         = 10000.0;   // resistenza fissa del partitore (10kΩ)
const float NTC_NOMINAL = 10000.0;  // resistenza NTC a 25°C (10kΩ)
const float TEMP_NOMINAL = 25.0;    // temperatura nominale (°C)
const float B_COEFF    = 3950.0;    // coefficiente B dell'NTC (dal datasheet)

float rawToTemperature(int raw) {
    float voltage = (raw / 16383.0) * 5.0;
    float resistance = R0 * voltage / (5.0 - voltage);

    // Equazione di Steinhart-Hart semplificata
    float steinhart = resistance / NTC_NOMINAL;
    steinhart = log(steinhart);
    steinhart /= B_COEFF;
    steinhart += 1.0 / (TEMP_NOMINAL + 273.15);
    return (1.0 / steinhart) - 273.15;
}
```

### Fotoresistenza (LDR) — Luce

Stessa configurazione del partitore. Il valore ADC cresce con la luminosità (o diminuisce, dipende da come orienti il partitore).

```cpp
// Mappa il valore ADC su una scala 0–100 (percentuale di luce)
int rawToLightPercent(int raw) {
    return map(raw, 0, 16383, 0, 100);
}
```

---

## 3. Il Rumore del Segnale

I segnali analogici sono **sempre rumorosi**: variazioni di ±5–50 conteggi ADC sono normali, causate da:
- Rumore elettrico del circuito
- Variazioni di alimentazione
- Disturbi elettromagnetici

Se stampi il valore raw vedrai qualcosa come:
```
8234, 8241, 8229, 8237, 8243, 8231, 8239...
```

Questo rumore può essere problematico se usi il valore per prendere decisioni (es. attivare un relè).

---

## 4. Filtraggio: Media Mobile (Moving Average)

La **media mobile** calcola la media degli ultimi N campioni. Più N è grande, più il segnale è stabile (ma risponde più lentamente ai cambiamenti reali).

```
Segnale raw:    100, 150, 90, 110, 130, 80, 120
Media mobile 4: ─── ─── ─── 112  120  102 110
```

```cpp
const int FILTER_SIZE = 16;        // numero di campioni per la media
int filterBuffer[FILTER_SIZE];     // buffer circolare
int filterIndex = 0;               // indice corrente

void addSample(int value) {
    filterBuffer[filterIndex] = value;
    filterIndex = (filterIndex + 1) % FILTER_SIZE;
}

float getAverage() {
    long sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += filterBuffer[i];
    }
    return (float)sum / FILTER_SIZE;
}
```

---

## 5. `map()` — Rimappare i Valori

La funzione `map()` di Arduino è molto utile per scalare intervalli:

```cpp
// map(valore, fromMin, fromMax, toMin, toMax)
int percent = map(rawADC, 0, 16383, 0, 100);    // ADC → percentuale
int pwmVal  = map(rawADC, 0, 16383, 0, 255);    // ADC → PWM
```

> ⚠️ `map()` usa interi. Per valori float, scrivi la conversione manualmente.

---

## 📂 Listato

→ [`listati/04_sensore_analogico.ino`](./listati/04_sensore_analogico.ino)

Il listato implementa:
- Lettura sensore analogico su A0
- Conversione in tensione e percentuale
- Media mobile su 16 campioni (buffer circolare)
- Output formattato sulla Serial ogni secondo
- Struttura `SensorReading` per organizzare i dati

---

## 🏋️ Esercizi

1. Modifica `FILTER_SIZE` a 4, 8, 32. Osserva come cambia la risposta agitando il sensore. Trova il valore che bilancia stabilità e reattività.
2. Aggiungi una soglia: se il valore medio supera il 70%, accendi il LED su pin 9.
3. Implementa il calcolo del **valore minimo e massimo** degli ultimi 16 campioni e stampali insieme alla media.
4. **Sfida:** crea una funzione `printBar(int percent)` che stampa una barra ASCII proporzionale al valore (es: `[████████░░] 80%`).
