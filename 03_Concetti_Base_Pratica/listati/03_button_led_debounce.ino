/*
 * Listato 03 — Sistema LED controllato da Pulsante con Debouncing
 * ----------------------------------------------------------------
 * Dimostra:
 *  - lettura pulsante con debouncing software (filtro temporale)
 *  - edge detection (rilevamento fronte di discesa/salita)
 *  - toggle LED al click
 *  - contatore di click sulla Serial
 *
 * Hardware: Arduino Uno R4 WiFi
 * Connessioni:
 *   - Pulsante: tra pin 2 e GND (usa INPUT_PULLUP, resistenza esterna NON necessaria)
 *   - LED esterno: pin 9 → resistenza 220Ω → Anodo LED → Catodo → GND
 *     (oppure usa il LED built-in su pin 13, senza componenti esterni)
 *
 * Autore: Corso C/C++ Arduino
 */

// ─────────────────────────────────────────
// COSTANTI
// ─────────────────────────────────────────
const int          BUTTON_PIN    = 2;    // pin pulsante (INPUT_PULLUP: HIGH = rilasciato)
const int          LED_PIN       = 9;    // pin LED esterno (PWM capace)
const unsigned long DEBOUNCE_MS  = 50;   // tempo minimo per considerare stabile il segnale

// ─────────────────────────────────────────
// STRUTTURA — stato del pulsante
// ─────────────────────────────────────────
struct Button {
    int           pin;
    int           stableState;       // stato stabile dopo debounce
    int           lastReading;       // ultima lettura grezza
    unsigned long lastDebounceMs;    // timestamp dell'ultimo cambiamento grezzo
};

// ─────────────────────────────────────────
// VARIABILI GLOBALI
// ─────────────────────────────────────────
Button button = {BUTTON_PIN, HIGH, HIGH, 0};

bool          ledState   = false;   // stato corrente del LED
int           clickCount = 0;       // numero di click totali
int           prevStableState = HIGH;  // stato precedente (per edge detection)

// ─────────────────────────────────────────
// PROTOTIPI FUNZIONI
// ─────────────────────────────────────────
void   initHardware();
void   updateButton(Button &btn);
bool   buttonJustPressed(const Button &btn, int &prevState);
void   handleButtonClick();
void   setLed(int pin, bool state);
void   printStatus();

// ─────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }

    initHardware();

    Serial.println("╔══════════════════════════════════════╗");
    Serial.println("║  Sketch 03 - Button + LED Debounce  ║");
    Serial.println("╚══════════════════════════════════════╝");
    Serial.println("Premi il pulsante per accendere/spegnere il LED.");
    Serial.println("──────────────────────────────────────");
}

// ─────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────
void loop() {
    // 1. Aggiorna lo stato del pulsante con debouncing
    updateButton(button);

    // 2. Controlla se il pulsante è stato appena premuto (fronte di discesa)
    if (buttonJustPressed(button, prevStableState)) {
        handleButtonClick();
    }
}

// ─────────────────────────────────────────
// IMPLEMENTAZIONE FUNZIONI
// ─────────────────────────────────────────

// Configura i pin e lo stato iniziale
void initHardware() {
    // INPUT_PULLUP: la resistenza pull-up interna porta il pin a HIGH quando il pulsante è aperto
    // Quando il pulsante viene premuto collega il pin a GND → LOW
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

// Aggiorna lo stato stabile del pulsante applicando il filtro di debouncing
// La struttura viene modificata tramite riferimento
void updateButton(Button &btn) {
    int reading = digitalRead(btn.pin);

    // Se la lettura è diversa dall'ultima, il segnale sta cambiando → resetta il timer
    if (reading != btn.lastReading) {
        btn.lastDebounceMs = millis();
    }

    // Se il segnale è rimasto stabile per almeno DEBOUNCE_MS, lo consideriamo valido
    if (millis() - btn.lastDebounceMs > DEBOUNCE_MS) {
        btn.stableState = reading;
    }

    // Salva sempre l'ultima lettura grezza per il prossimo ciclo
    btn.lastReading = reading;
}

// Restituisce true SOLO nel momento in cui il pulsante viene premuto (fronte di discesa)
// Modifica prevState per tracciare la transizione
bool buttonJustPressed(const Button &btn, int &prevState) {
    bool justPressed = false;

    // Fronte di discesa: il pulsante è passato da HIGH (rilasciato) a LOW (premuto)
    if (prevState == HIGH && btn.stableState == LOW) {
        justPressed = true;
    }

    // Aggiorna lo stato precedente
    prevState = btn.stableState;
    return justPressed;
}

// Gestisce l'azione al click del pulsante
void handleButtonClick() {
    clickCount++;
    ledState = !ledState;          // toggle dello stato LED
    setLed(LED_PIN, ledState);
    printStatus();
}

// Imposta lo stato del LED (astrazione per eventuale futura gestione PWM)
void setLed(int pin, bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
}

// Stampa lo stato corrente sulla Serial
void printStatus() {
    Serial.print("Click #");
    Serial.print(clickCount);
    Serial.print("  |  LED: ");
    Serial.print(ledState ? "ACCESO" : "SPENTO");
    Serial.print("  |  Uptime: ");
    Serial.print(millis() / 1000.0, 1);
    Serial.println("s");
}
