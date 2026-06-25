/*
 * Listato 01 — Ripasso Base C/C++ su Arduino Uno R4 WiFi
 * -------------------------------------------------------
 * Dimostra i concetti fondamentali di C/C++ applicati ad Arduino:
 *  - tipi di dato e loro dimensioni
 *  - variabili globali, locali e static
 *  - funzioni custom con passaggio per valore e per riferimento
 *  - struct per raggruppare dati
 *  - timing non bloccante con millis()
 *
 * Hardware: Arduino Uno R4 WiFi
 * Connessioni: nessuna (usa solo il LED built-in e la Serial)
 *
 * Autore: Corso C/C++ Arduino
 */

// ─────────────────────────────────────────
// COSTANTI (preferire const a #define)
// ─────────────────────────────────────────
const int    LED_PIN        = 13;    // LED built-in Arduino Uno R4
const long   BLINK_INTERVAL = 1000; // ms tra un toggle e l'altro
const int    BUFFER_SIZE    = 8;    // dimensione buffer per la media

// ─────────────────────────────────────────
// STRUTTURA — raggruppa dati correlati
// ─────────────────────────────────────────
struct SystemInfo {
    unsigned long uptimeMs;   // millisecondi dall'avvio
    int           loopCount;  // quante volte è stato eseguito loop()
    bool          ledState;   // stato attuale del LED
};

// ─────────────────────────────────────────
// VARIABILI GLOBALI
// ─────────────────────────────────────────
SystemInfo sysInfo = {0, 0, false};          // inizializzazione struct
unsigned long lastBlinkMs = 0;               // timestamp ultimo toggle LED
int readings[BUFFER_SIZE] = {0};             // buffer per la media mobile
int bufferIndex = 0;                         // indice corrente nel buffer

// ─────────────────────────────────────────
// PROTOTIPI FUNZIONI (buona pratica C++)
// ─────────────────────────────────────────
void    toggleLed(bool &state);
void    addReading(int value, int *buffer, int size, int &index);
float   computeAverage(int *buffer, int size);
void    printTypeInfo();
void    updateSystemInfo(SystemInfo &info);

// ─────────────────────────────────────────
// SETUP — eseguito una sola volta all'avvio
// ─────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }  // attendi che la porta Serial sia pronta (R4 WiFi)

    pinMode(LED_PIN, OUTPUT);

    // Stampa dimensioni dei tipi su questo hardware
    printTypeInfo();

    Serial.println("─────────────────────────────");
    Serial.println("Setup completato. Avvio loop.");
    Serial.println("─────────────────────────────");
}

// ─────────────────────────────────────────
// LOOP — eseguito continuamente
// ─────────────────────────────────────────
void loop() {
    // Aggiorna le informazioni di sistema
    updateSystemInfo(sysInfo);

    // Timing non bloccante: toggle LED ogni BLINK_INTERVAL ms
    unsigned long now = millis();
    if (now - lastBlinkMs >= (unsigned long)BLINK_INTERVAL) {
        lastBlinkMs = now;
        toggleLed(sysInfo.ledState);
    }

    // Simulazione lettura sensore: valore che oscilla
    // (in un progetto reale: analogRead(A0))
    int simulatedReading = 500 + (sysInfo.loopCount % 100);
    addReading(simulatedReading, readings, BUFFER_SIZE, bufferIndex);

    // Stampa informazioni ogni 5 secondi (usa variabile static per il timer)
    static unsigned long lastPrintMs = 0;
    if (now - lastPrintMs >= 5000UL) {
        lastPrintMs = now;

        float avg = computeAverage(readings, BUFFER_SIZE);

        Serial.print("Uptime: ");
        Serial.print(sysInfo.uptimeMs / 1000);
        Serial.print("s | Loop count: ");
        Serial.print(sysInfo.loopCount);
        Serial.print(" | LED: ");
        Serial.print(sysInfo.ledState ? "ON" : "OFF");
        Serial.print(" | Media letture: ");
        Serial.println(avg, 1);
    }
}

// ─────────────────────────────────────────
// IMPLEMENTAZIONE FUNZIONI
// ─────────────────────────────────────────

// Inverte lo stato del LED e aggiorna la variabile passata per riferimento
void toggleLed(bool &state) {
    state = !state;
    digitalWrite(LED_PIN, state ? HIGH : LOW);
}

// Aggiunge un valore al buffer circolare (ring buffer)
// Usa un puntatore per l'array e un riferimento per l'indice
void addReading(int value, int *buffer, int size, int &index) {
    buffer[index] = value;
    index = (index + 1) % size;  // avanza l'indice in modo circolare
}

// Calcola la media aritmetica di un array di interi
// Restituisce float per precisione
float computeAverage(int *buffer, int size) {
    long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return (float)sum / size;
}

// Stampa le dimensioni dei tipi fondamentali (utile per capire l'hardware)
void printTypeInfo() {
    Serial.println("=== Dimensioni tipi su questo hardware ===");
    Serial.print("bool:          "); Serial.print(sizeof(bool));          Serial.println(" byte");
    Serial.print("char:          "); Serial.print(sizeof(char));          Serial.println(" byte");
    Serial.print("int:           "); Serial.print(sizeof(int));           Serial.println(" byte");
    Serial.print("long:          "); Serial.print(sizeof(long));          Serial.println(" byte");
    Serial.print("unsigned long: "); Serial.print(sizeof(unsigned long)); Serial.println(" byte");
    Serial.print("float:         "); Serial.print(sizeof(float));         Serial.println(" byte");
    Serial.print("double:        "); Serial.print(sizeof(double));        Serial.println(" byte");
    Serial.println("==========================================");
}

// Aggiorna i campi della struttura SystemInfo
void updateSystemInfo(SystemInfo &info) {
    info.uptimeMs  = millis();
    info.loopCount++;
}
