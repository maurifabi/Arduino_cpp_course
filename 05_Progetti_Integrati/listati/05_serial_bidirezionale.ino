/*
 * Listato 05 — Comunicazione Serial Bidirezionale con Protocollo
 * ---------------------------------------------------------------
 * Dimostra:
 *  - lettura Serial non bloccante (accumulo caratteri)
 *  - parsing comandi con e senza parametri (formato CMD:param)
 *  - risposta strutturata con prefissi [OK], [ERR], [STATUS]
 *  - controllo LED tramite comandi Serial
 *  - blink configurabile via Serial (non bloccante)
 *
 * Hardware: Arduino Uno R4 WiFi
 * Connessioni:
 *   - LED built-in (pin 13) — nessun componente esterno necessario
 *   - Oppure: LED esterno su pin 9 con resistenza 220Ω
 *
 * Come usare:
 *   1. Carica lo sketch su Arduino
 *   2. Apri Serial Monitor a 115200 baud
 *   3. Imposta terminatore "Newline" (o "Both NL & CR")
 *   4. Digita un comando e premi Invio
 *
 * Comandi disponibili:
 *   LED:ON      → accende il LED
 *   LED:OFF     → spegne il LED
 *   LED:TOGGLE  → inverte lo stato del LED
 *   BLINK:500   → fa lampeggiare il LED ogni 500ms
 *   BLINK:OFF   → ferma il blink
 *   STATUS      → stampa lo stato corrente
 *   HELP        → mostra i comandi disponibili
 *
 * Autore: Corso C/C++ Arduino
 */

// ─────────────────────────────────────────
// COSTANTI
// ─────────────────────────────────────────
const int LED_PIN         = 13;     // LED built-in
const int SERIAL_BAUD     = 115200;
const int MAX_CMD_LENGTH  = 64;     // lunghezza massima comando in caratteri

// Prefissi per le risposte (stile log strutturato)
#define LOG_OK     "[OK]     "
#define LOG_ERR    "[ERR]    "
#define LOG_STATUS "[STATUS] "
#define LOG_INFO   "[INFO]   "

// ─────────────────────────────────────────
// VARIABILI GLOBALI
// ─────────────────────────────────────────

// ── Stato LED ──
bool          ledState       = false;
bool          blinkEnabled   = false;
unsigned long blinkIntervalMs = 500;
unsigned long lastBlinkMs    = 0;

// ── Buffer Serial ──
String inputBuffer  = "";      // accumula caratteri in arrivo
bool   cmdReady     = false;   // segnala che un comando completo è pronto

// ─────────────────────────────────────────
// PROTOTIPI FUNZIONI
// ─────────────────────────────────────────
void   initHardware();
void   checkSerial();
void   processCommand(const String &raw);
String parseCommandName(const String &raw, String &param);
void   executeCommand(const String &cmd, const String &param);
void   handleBlink();
void   setLed(bool state);
void   printHelp();
void   printStatus();

// ─────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────
void setup() {
    Serial.begin(SERIAL_BAUD);
    while (!Serial) { ; }

    initHardware();

    Serial.println("╔══════════════════════════════════════════╗");
    Serial.println("║  Sketch 05 - Serial Bidirezionale        ║");
    Serial.println("║  Arduino Uno R4 WiFi                     ║");
    Serial.println("╚══════════════════════════════════════════╝");
    Serial.println(LOG_INFO "Sistema pronto.");
    Serial.println(LOG_INFO "Digita HELP per vedere i comandi disponibili.");
    Serial.println("──────────────────────────────────────────");
}

// ─────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────
void loop() {
    // 1. Controlla se sono arrivati dati dalla Serial
    checkSerial();

    // 2. Se c'è un comando completo, processalo
    if (cmdReady) {
        processCommand(inputBuffer);
        inputBuffer = "";    // svuota il buffer per il prossimo comando
        cmdReady    = false;
    }

    // 3. Gestisci il blink LED (non bloccante)
    if (blinkEnabled) {
        handleBlink();
    }
}

// ─────────────────────────────────────────
// IMPLEMENTAZIONE FUNZIONI
// ─────────────────────────────────────────

// Configura i pin
void initHardware() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    inputBuffer.reserve(MAX_CMD_LENGTH);   // pre-alloca memoria per il buffer
}

// Legge i byte disponibili dalla Serial e li accumula nel buffer
// È NON BLOCCANTE: processa solo i byte già arrivati, poi restituisce il controllo
void checkSerial() {
    while (Serial.available() > 0) {
        char c = (char)Serial.read();

        if (c == '\n' || c == '\r') {
            // Fine del comando: segnalalo solo se il buffer non è vuoto
            if (inputBuffer.length() > 0) {
                cmdReady = true;
            }
        } else if (inputBuffer.length() < MAX_CMD_LENGTH) {
            inputBuffer += c;    // accumula il carattere
        }
        // Se il buffer è pieno, i caratteri in eccesso vengono ignorati
    }
}

// Normalizza il comando (maiuscolo, rimuove spazi) e lo instrada
void processCommand(const String &raw) {
    String cleaned = raw;
    cleaned.trim();         // rimuovi spazi iniziali/finali
    cleaned.toUpperCase();  // converti in maiuscolo per case-insensitive

    if (cleaned.length() == 0) return;

    Serial.print(">>> ");
    Serial.println(cleaned);    // echo del comando ricevuto

    String param;
    String cmd = parseCommandName(cleaned, param);
    executeCommand(cmd, param);
}

// Separa il nome del comando dal parametro (divisore: ':')
// Modifica 'param' per riferimento
String parseCommandName(const String &raw, String &param) {
    int colonPos = raw.indexOf(':');

    if (colonPos == -1) {
        param = "";
        return raw;
    }

    param = raw.substring(colonPos + 1);
    return raw.substring(0, colonPos);
}

// Esegue il comando con il parametro corrispondente
void executeCommand(const String &cmd, const String &param) {

    if (cmd == "LED") {
        if (param == "ON") {
            blinkEnabled = false;
            setLed(true);
            Serial.println(LOG_OK "LED acceso");

        } else if (param == "OFF") {
            blinkEnabled = false;
            setLed(false);
            Serial.println(LOG_OK "LED spento");

        } else if (param == "TOGGLE") {
            blinkEnabled = false;
            setLed(!ledState);
            Serial.print(LOG_OK "LED → ");
            Serial.println(ledState ? "acceso" : "spento");

        } else {
            Serial.print(LOG_ERR "Parametro LED non valido: ");
            Serial.println(param);
            Serial.println(LOG_INFO "Valori accettati: ON, OFF, TOGGLE");
        }

    } else if (cmd == "BLINK") {
        if (param == "OFF") {
            blinkEnabled = false;
            setLed(false);
            Serial.println(LOG_OK "Blink fermato");

        } else {
            // Prova a convertire il parametro in intero (millisecondi)
            long ms = param.toInt();
            if (ms > 0) {
                blinkIntervalMs = (unsigned long)ms;
                blinkEnabled    = true;
                lastBlinkMs     = millis();
                Serial.print(LOG_OK "Blink ogni ");
                Serial.print(ms);
                Serial.println("ms");
            } else {
                Serial.print(LOG_ERR "Intervallo non valido: ");
                Serial.println(param);
                Serial.println(LOG_INFO "Esempio: BLINK:500");
            }
        }

    } else if (cmd == "STATUS") {
        printStatus();

    } else if (cmd == "HELP") {
        printHelp();

    } else {
        Serial.print(LOG_ERR "Comando sconosciuto: ");
        Serial.println(cmd);
        Serial.println(LOG_INFO "Digita HELP per la lista comandi");
    }
}

// Gestisce il blink non bloccante
void handleBlink() {
    unsigned long now = millis();
    if (now - lastBlinkMs >= blinkIntervalMs) {
        lastBlinkMs = now;
        setLed(!ledState);
    }
}

// Imposta lo stato del LED e aggiorna la variabile di stato
void setLed(bool state) {
    ledState = state;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// Stampa lo stato corrente del sistema
void printStatus() {
    Serial.print(LOG_STATUS "LED=");
    Serial.print(ledState ? "ON" : "OFF");
    Serial.print("  blink=");
    Serial.print(blinkEnabled ? "ON" : "OFF");
    if (blinkEnabled) {
        Serial.print(" (");
        Serial.print(blinkIntervalMs);
        Serial.print("ms)");
    }
    Serial.print("  uptime=");
    Serial.print(millis() / 1000.0, 1);
    Serial.println("s");
}

// Stampa l'elenco dei comandi disponibili
void printHelp() {
    Serial.println("──────────────── COMANDI ────────────────");
    Serial.println("  LED:ON       accende il LED");
    Serial.println("  LED:OFF      spegne il LED");
    Serial.println("  LED:TOGGLE   inverte lo stato del LED");
    Serial.println("  BLINK:ms     lampeggia ogni <ms> millisecondi");
    Serial.println("  BLINK:OFF    ferma il lampeggio");
    Serial.println("  STATUS       mostra lo stato corrente");
    Serial.println("  HELP         mostra questo messaggio");
    Serial.println("─────────────────────────────────────────");
}
