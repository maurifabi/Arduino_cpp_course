/*
 * Listato 02 — Primo Sketch: LED Blink con Funzioni Custom
 * ---------------------------------------------------------
 * Primo sketch funzionante su Arduino Uno R4 WiFi.
 * Dimostra:
 *  - struttura setup() / loop()
 *  - timing NON bloccante con millis()
 *  - funzioni custom per organizzare il codice
 *  - output su Serial Monitor per il debug
 *
 * Hardware: Arduino Uno R4 WiFi
 * Connessioni: nessuna aggiuntiva — usa il LED built-in (pin 13)
 *
 * Autore: Corso C/C++ Arduino
 */

// ─────────────────────────────────────────
// COSTANTI
// ─────────────────────────────────────────
const int          LED_PIN        = 13;     // LED built-in
const unsigned int BLINK_INTERVAL = 1000;   // intervallo blink in ms

// ─────────────────────────────────────────
// VARIABILI GLOBALI DI STATO
// ─────────────────────────────────────────
bool          ledState    = false;   // stato attuale del LED (true = acceso)
unsigned long lastBlinkMs = 0;       // timestamp dell'ultimo cambio di stato
int           blinkCount  = 0;       // contatore totale dei blink

// ─────────────────────────────────────────
// PROTOTIPI FUNZIONI
// ─────────────────────────────────────────
void initializeLed(int pin);
void handleBlink(unsigned long currentMs);
void toggleLed();
void printBlinkInfo();

// ─────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────
void setup() {
    // Inizializza la comunicazione Serial a 115200 baud
    Serial.begin(115200);

    // Attendi che la porta Serial sia pronta
    // (necessario su R4 WiFi per non perdere i primi messaggi)
    while (!Serial) { ; }

    // Inizializza il LED tramite funzione custom
    initializeLed(LED_PIN);

    Serial.println("╔════════════════════════════════╗");
    Serial.println("║  Sketch 02 - LED Blink Custom  ║");
    Serial.println("║  Arduino Uno R4 WiFi           ║");
    Serial.println("╚════════════════════════════════╝");
    Serial.print("Intervallo blink: ");
    Serial.print(BLINK_INTERVAL);
    Serial.println(" ms");
    Serial.println("─────────────────────────────────");
}

// ─────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────
void loop() {
    // Leggi il tempo corrente UNA SOLA VOLTA per ogni iterazione del loop
    unsigned long now = millis();

    // Gestisci il blink passando il tempo corrente alla funzione
    handleBlink(now);

    // Qui potresti aggiungere altra logica (lettura sensori, ecc.)
    // senza bloccare il programma, perché non usiamo delay()!
}

// ─────────────────────────────────────────
// IMPLEMENTAZIONE FUNZIONI
// ─────────────────────────────────────────

// Configura il pin del LED come OUTPUT e lo spegne
void initializeLed(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    ledState = false;
    Serial.print("LED inizializzato su pin ");
    Serial.println(pin);
}

// Controlla se è il momento di fare il blink e lo esegue
// Riceve il timestamp corrente per evitare chiamate multiple a millis()
void handleBlink(unsigned long currentMs) {
    // Calcola il tempo trascorso dall'ultimo toggle
    // IMPORTANTE: la sottrazione unsigned gestisce correttamente
    // il rollover di millis() dopo circa 49 giorni
    if (currentMs - lastBlinkMs >= BLINK_INTERVAL) {
        lastBlinkMs = currentMs;   // aggiorna il riferimento temporale
        toggleLed();               // cambia stato del LED
        printBlinkInfo();          // stampa info sulla Serial
    }
}

// Inverte lo stato del LED (se era ON diventa OFF e viceversa)
void toggleLed() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);

    // Ogni volta che il LED si accende, incrementa il contatore
    if (ledState) {
        blinkCount++;
    }
}

// Stampa le informazioni del blink sulla Serial
void printBlinkInfo() {
    Serial.print("Blink #");
    Serial.print(blinkCount);
    Serial.print("  |  LED: ");
    Serial.print(ledState ? "ON " : "OFF");
    Serial.print("  |  Uptime: ");
    Serial.print(millis() / 1000.0, 1);
    Serial.println("s");
}
