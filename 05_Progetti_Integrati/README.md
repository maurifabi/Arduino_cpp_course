# Puntata 5 — Comunicazione Serial Strutturata

## Obiettivo

Usare la Serial non solo per il debug, ma come **canale di comunicazione bidirezionale** con un protocollo semplice e robusto.  
Imparerai a parsare comandi da PC, rispondere in modo strutturato e organizzare il codice per la comunicazione.

---

## 1. Serial su Arduino — Fondamenti

Arduino Uno R4 WiFi ha più porte Serial:
- **Serial** → USB (via UART integrata, quella del Serial Monitor)
- **Serial1** → pin 0 (RX), 1 (TX) → per moduli esterni

```cpp
Serial.begin(115200);       // inizializza a 115200 baud
Serial.print("testo");      // stampa senza newline
Serial.println("testo");    // stampa con newline (\r\n)
Serial.available();         // bytes disponibili da leggere
Serial.read();              // legge 1 byte
Serial.readStringUntil('\n'); // legge fino al newline
```

---

## 2. Velocità Baud

Il baud rate deve essere lo stesso su Arduino e Serial Monitor.

| Baud rate | Uso tipico |
|-----------|-----------|
| 9600 | debug semplice (lento) |
| 115200 | debug rapido (consigliato) |
| 250000+ | streaming dati ad alta frequenza |

---

## 3. Progettare un Protocollo Semplice

Un protocollo definisce il **formato dei messaggi** per evitare ambiguità.

**Protocollo di questo listato:**
- I comandi dal PC sono in testo, terminati con `\n` (Enter)
- Formato: `COMANDO[:parametro]`
- Esempi:
  - `LED:ON` → accende il LED
  - `LED:OFF` → spegne il LED
  - `STATUS` → risponde con lo stato corrente
  - `BLINK:500` → fa lampeggiare il LED ogni 500ms

**Le risposte di Arduino:**
- Formato: `[OK] messaggio` oppure `[ERR] descrizione errore`
- Esempi:
  - `[OK] LED acceso`
  - `[ERR] Comando sconosciuto: XYZ`
  - `[STATUS] LED=ON uptime=12.3s`

---

## 4. Parsing dei Comandi

```cpp
String parseCommand(const String &input, String &param) {
    int colonPos = input.indexOf(':');

    if (colonPos == -1) {
        // Nessun parametro
        param = "";
        return input;
    }

    param = input.substring(colonPos + 1);
    return input.substring(0, colonPos);
}
```

> **Nota su `String`:** la classe `String` di Arduino usa allocazione dinamica (heap). In progetti complessi con poca RAM può causare **heap fragmentation**. Per questo corso va bene; in produzione considera array `char[]` con `strtok()`.

---

## 5. Lettura Non Bloccante della Serial

Non usare `Serial.readStringUntil()` direttamente nel loop senza timeout: potrebbe bloccare il programma. L'approccio corretto è **accumulare i caratteri** man mano che arrivano.

```cpp
String inputBuffer = "";
bool commandReady  = false;

void checkSerial() {
    while (Serial.available() > 0) {
        char c = (char)Serial.read();

        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                commandReady = true;   // segnala che c'è un comando completo
            }
        } else {
            inputBuffer += c;          // accumula carattere
        }
    }
}
```

---

## 6. Debug Strutturato

Con un canale Serial ben organizzato, il debug diventa molto più efficace.

```cpp
// Livelli di log
#define LOG_INFO  "[INFO] "
#define LOG_OK    "[OK]   "
#define LOG_ERR   "[ERR]  "
#define LOG_DATA  "[DATA] "

// Uso:
Serial.println(LOG_OK "LED acceso");
Serial.println(LOG_ERR "Parametro non valido");
```

---

## 📂 Listato

→ [`listati/05_serial_bidirezionale.ino`](./listati/05_serial_bidirezionale.ino)

Il listato implementa:
- Ricezione comandi dal Serial Monitor (non bloccante)
- Parsing di comandi con e senza parametri
- Comandi: `LED:ON`, `LED:OFF`, `LED:TOGGLE`, `BLINK:ms`, `BLINK:OFF`, `STATUS`, `HELP`
- Risposte strutturate con prefissi `[OK]`, `[ERR]`, `[STATUS]`
- Blink LED non bloccante controllabile via Serial

---

## 🏋️ Esercizi

1. Aggiungi il comando `READ` che risponde con il valore letto da `analogRead(A0)` in formato `[DATA] A0=8192 (2.50V)`.
2. Aggiungi il comando `UPTIME` che risponde con il tempo in secondi dall'avvio.
3. Implementa un comando `BLINK:n:ms` (es `BLINK:5:200`) che fa lampeggiare il LED esattamente N volte a intervalli di ms millisecondi, poi si ferma.
4. **Sfida:** crea un sistema di **log con timestamp** — ogni risposta include il tempo in secondi (es: `[12.3s][OK] LED acceso`).
