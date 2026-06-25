/*
 * Listato 04 — Lettura Sensore Analogico con Media Mobile
 * --------------------------------------------------------
 * Dimostra:
 *  - lettura ADC (14 bit su R4 WiFi: valori 0–16383)
 *  - conversione raw → tensione e percentuale
 *  - filtro media mobile con buffer circolare
 *  - struct per organizzare i dati del sensore
 *  - output formattato sulla Serial
 *
 * Hardware: Arduino Uno R4 WiFi
 * Connessioni:
 *   - Sensore analogico (fotoresistenza, potenziometro, ecc.) su pin A0
 *     Schema partitore: 5V → [R=10kΩ] → A0 → [Sensore] → GND
 *   - In alternativa: collega un potenziometro tra 5V e GND, cursore su A0
 *
 * Autore: Corso C/C++ Arduino
 */

// ─────────────────────────────────────────
// COSTANTI
// ─────────────────────────────────────────
const int   SENSOR_PIN   = A0;       // pin ingresso analogico
const float ADC_MAX      = 16383.0;  // R4 WiFi: ADC a 14 bit (0–16383)
const float VREF         = 5.0;      // tensione di riferimento in Volt
const int   FILTER_SIZE  = 16;       // numero campioni per la media mobile
const unsigned long PRINT_INTERVAL = 1000; // stampa ogni 1000ms

// ─────────────────────────────────────────
// STRUTTURA — dati del sensore
// ─────────────────────────────────────────
struct SensorReading {
    int   rawValue;      // valore grezzo dall'ADC (0–16383)
    float voltage;       // tensione corrispondente (0.0–5.0 V)
    float percentage;    // percentuale (0.0–100.0 %)
    float filteredPct;   // percentuale filtrata con media mobile
};

// ─────────────────────────────────────────
// VARIABILI GLOBALI
// ─────────────────────────────────────────
SensorReading sensor = {0, 0.0, 0.0, 0.0};

// Buffer circolare per la media mobile
int   filterBuffer[FILTER_SIZE] = {0};
int   filterIndex  = 0;
bool  bufferFilled = false;   // diventa true dopo aver riempito il buffer la prima volta

// ─────────────────────────────────────────
// PROTOTIPI FUNZIONI
// ─────────────────────────────────────────
void          initSensor();
void          readSensor(SensorReading &reading, int pin);
void          addToFilter(int value);
float         getFilteredAverage();
void          printSensorData(const SensorReading &reading);
void          printBar(float percent, int width);

// ─────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }

    initSensor();

    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║  Sketch 04 - Sensore Analogico + FIR  ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.print("ADC risoluzione: 14 bit (0–");
    Serial.print((int)ADC_MAX);
    Serial.println(")");
    Serial.print("Campioni filtro: ");
    Serial.println(FILTER_SIZE);
    Serial.println("────────────────────────────────────────");
    Serial.println("RAW    | Tensione | %Raw  | %Filt | Barra");
    Serial.println("────────────────────────────────────────");
}

// ─────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────
void loop() {
    // 1. Leggi il sensore e converti i valori
    readSensor(sensor, SENSOR_PIN);

    // 2. Aggiungi la lettura al filtro
    addToFilter(sensor.rawValue);

    // 3. Aggiorna il valore filtrato nella struttura
    sensor.filteredPct = getFilteredAverage();

    // 4. Stampa i dati a intervalli regolari (non bloccante)
    static unsigned long lastPrintMs = 0;
    unsigned long now = millis();
    if (now - lastPrintMs >= PRINT_INTERVAL) {
        lastPrintMs = now;
        printSensorData(sensor);
    }
}

// ─────────────────────────────────────────
// IMPLEMENTAZIONE FUNZIONI
// ─────────────────────────────────────────

// Inizializza il sensore: preriempie il buffer con una lettura iniziale
void initSensor() {
    int initialReading = analogRead(SENSOR_PIN);
    for (int i = 0; i < FILTER_SIZE; i++) {
        filterBuffer[i] = initialReading;
    }
    Serial.println("Buffer filtro inizializzato.");
}

// Legge il sensore e popola la struttura SensorReading
void readSensor(SensorReading &reading, int pin) {
    reading.rawValue   = analogRead(pin);
    reading.voltage    = (reading.rawValue / ADC_MAX) * VREF;
    reading.percentage = (reading.rawValue / ADC_MAX) * 100.0;
}

// Aggiunge un campione al buffer circolare del filtro
void addToFilter(int value) {
    filterBuffer[filterIndex] = value;
    filterIndex = (filterIndex + 1) % FILTER_SIZE;

    // Dopo il primo giro completo, il buffer è pieno
    if (filterIndex == 0) {
        bufferFilled = true;
    }
}

// Calcola la media dei campioni nel buffer
// Usa long per la somma per evitare overflow (16 × 16383 = 262128 > int16 max)
float getFilteredAverage() {
    long sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += filterBuffer[i];
    }
    float avgRaw = (float)sum / FILTER_SIZE;
    return (avgRaw / ADC_MAX) * 100.0;
}

// Stampa i dati del sensore in formato tabellare
void printSensorData(const SensorReading &reading) {
    // RAW
    Serial.print(reading.rawValue);
    // Allinea a colonna (6 caratteri)
    if (reading.rawValue < 10)    Serial.print("     ");
    else if (reading.rawValue < 100)   Serial.print("    ");
    else if (reading.rawValue < 1000)  Serial.print("   ");
    else if (reading.rawValue < 10000) Serial.print("  ");
    else                               Serial.print(" ");

    Serial.print("| ");

    // Tensione
    Serial.print(reading.voltage, 3);
    Serial.print("V   | ");

    // Percentuale raw
    Serial.print(reading.percentage, 1);
    Serial.print("% | ");

    // Percentuale filtrata
    Serial.print(reading.filteredPct, 1);
    Serial.print("% | ");

    // Barra ASCII proporzionale
    printBar(reading.filteredPct, 20);
    Serial.println();
}

// Stampa una barra ASCII lunga 'width' caratteri, proporzionale a percent (0–100)
void printBar(float percent, int width) {
    int filled = (int)((percent / 100.0) * width);
    filled = constrain(filled, 0, width);  // limita tra 0 e width

    Serial.print("[");
    for (int i = 0; i < width; i++) {
        Serial.print(i < filled ? "#" : ".");
    }
    Serial.print("]");
}
