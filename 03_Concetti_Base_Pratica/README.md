# Puntata 3 — Concetti Base in Pratica: GPIO e I/O Digitale

## Obiettivo

Controllare LED e leggere pulsanti in modo professionale.  
Imparerai il concetto di **debouncing** (fondamentale con i pulsanti fisici) e organizzerai il codice con funzioni pulite.

---

## 1. I/O Digitale — Teoria Rapida

I pin digitali di Arduino lavorano a due livelli di tensione:
- **HIGH** → 5V (logica 1)
- **LOW** → 0V (logica 0)

```cpp
pinMode(pin, OUTPUT);    // configura come uscita
pinMode(pin, INPUT);     // configura come ingresso (senza pull-up)
pinMode(pin, INPUT_PULLUP); // ingresso con resistenza pull-up interna (~50kΩ)
```

### Pull-up interno — come funziona

Con `INPUT_PULLUP`:
- Pin **non premuto** → HIGH (5V tramite pull-up)
- Pin **premuto** (collega pin a GND) → LOW

Questo è il modo standard di collegare pulsanti su Arduino: **nessuna resistenza esterna necessaria**.

---

## 2. Schema di Collegamento

```
Arduino          Pulsante
─────────        ────────
Pin 2  ──────── un terminale
GND    ──────── altro terminale

Arduino          LED
─────────        ───
Pin 9  ──[220Ω]── Anodo (+)
GND    ──────── Catodo (-)
```

> Il LED built-in (pin 13) ha già la resistenza sulla board, ma per LED esterni serve sempre una resistenza da 220Ω.

---

## 3. Il Problema del Bounce

Quando premi un pulsante fisico, i contatti metallici **rimbalzano** meccanicamente per alcuni millisecondi (5–50ms). Arduino è così veloce che registra decine di pressioni invece di una.

**Senza debouncing:**
```
Premi 1 volta → Arduino legge: LOW HIGH LOW HIGH LOW HIGH LOW → 7 eventi!
```

**Con debouncing:**
```
Premi 1 volta → Arduino aspetta che il segnale si stabilizzi → 1 evento
```

---

## 4. Debouncing Software

Tecnica del **filtro temporale**: ignora i cambi di stato troppo ravvicinati.

```cpp
const unsigned long DEBOUNCE_MS = 50;   // 50ms è sufficiente per la maggior parte dei pulsanti

unsigned long lastDebounceTime = 0;
int lastButtonState = HIGH;
int buttonState = HIGH;

void readButton(int pin) {
    int reading = digitalRead(pin);

    // Se il segnale è cambiato, resetta il timer
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    // Solo se il segnale è stabile da DEBOUNCE_MS ms, aggiorna lo stato
    if (millis() - lastDebounceTime > DEBOUNCE_MS) {
        buttonState = reading;
    }

    lastButtonState = reading;
}
```

---

## 5. Rilevare il "Fronte" (Edge Detection)

Spesso non vuoi sapere se il pulsante **è premuto** ma **quando viene premuto** (fronte di discesa) o **rilasciato** (fronte di salita).

```cpp
int prevButtonState = HIGH;
int currentButtonState = HIGH;

void loop() {
    currentButtonState = digitalRead(BUTTON_PIN);   // con debounce

    // Fronte di discesa: pulsante appena premuto
    if (prevButtonState == HIGH && currentButtonState == LOW) {
        // azione al click!
    }

    // Fronte di salita: pulsante appena rilasciato
    if (prevButtonState == LOW && currentButtonState == HIGH) {
        // azione al rilascio!
    }

    prevButtonState = currentButtonState;
}
```

---

## 6. PWM — Controllo Luminosità LED

I pin con PWM (segnati con `~` sulla board: 3, 5, 6, 9, 10, 11) permettono di variare l'intensità.

```cpp
// analogWrite su pin PWM: valore da 0 (spento) a 255 (massima luminosità)
analogWrite(9, 0);    // spento
analogWrite(9, 128);  // circa 50%
analogWrite(9, 255);  // massima luminosità
```

> Su R4 WiFi, `analogWrite()` genera un segnale PWM a 490Hz (o 980Hz per pin 5, 6). Non è vera tensione analogica — è un'onda quadra con duty cycle variabile.

---

## 📂 Listato

→ [`listati/03_button_led_debounce.ino`](./listati/03_button_led_debounce.ino)

Il listato implementa:
- Lettura pulsante con debouncing completo
- LED controllato dal pulsante (toggle al click)
- Rilevamento del fronte (edge detection)
- Contatore di click con output Serial
- PWM per effetto "fade" opzionale

---

## 🏋️ Esercizi

1. Aggiungi un secondo pulsante su pin 3. Il primo pulsante accende/spegne il LED, il secondo cambia la luminosità (3 livelli: 25%, 50%, 100%).
2. Implementa un **contatore a 3 bit**: ogni click incrementa un contatore 0–7, i 3 LED collegati a pin 9, 10, 11 mostrano il valore in binario.
3. **Sfida:** crea un sistema "hold to repeat" — se tieni premuto il pulsante per più di 1 secondo, il LED lampeggia velocemente finché non rilasci.
