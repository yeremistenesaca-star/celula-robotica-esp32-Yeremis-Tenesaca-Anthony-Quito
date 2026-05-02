#define BTN1 18
#define BTN2 19
#define BTN_RESET 21

#define LED1 23
#define LED2 22
#define LED3 5

enum Estado {
  IDLE,
  AUTH,
  CLASS,
  PROCESS,
  INSPECT,
  READY
};

volatile Estado estado = IDLE;

volatile bool f_btn1 = false;
volatile bool f_btn2 = false;
volatile bool f_reset = false;

hw_timer_t *timer = NULL;
volatile int tiempo = 0;

volatile unsigned long last1 = 0;
volatile unsigned long last2 = 0;
volatile unsigned long lastR = 0;

void IRAM_ATTR isr_btn1() {
  if (millis() - last1 > 200) {
    f_btn1 = true;
    last1 = millis();
  }
}

void IRAM_ATTR isr_btn2() {
  if (millis() - last2 > 200) {
    f_btn2 = true;
    last2 = millis();
  }
}

void IRAM_ATTR isr_reset() {
  if (millis() - lastR > 200) {
    f_reset = true;
    lastR = millis();
  }
}

void IRAM_ATTR onTimer() {
  tiempo++;
}

void setup() {

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  attachInterrupt(BTN1, isr_btn1, FALLING);
  attachInterrupt(BTN2, isr_btn2, FALLING);
  attachInterrupt(BTN_RESET, isr_reset, FALLING);

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000000, true, 0);
}

void loop() {

  if (f_btn1) {
    if (estado == IDLE) estado = AUTH;
    f_btn1 = false;
  }

  if (f_btn2) {
    if (estado == AUTH) estado = CLASS;
    f_btn2 = false;
  }

  if (f_reset) {
    estado = IDLE;
    tiempo = 0;

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

    f_reset = false;
  }

  switch (estado) {

    case IDLE:
      break;

    case AUTH:
      break;

    case CLASS:
      tiempo = 0;
      estado = PROCESS;
      break;

    case PROCESS:
      digitalWrite(LED1, HIGH);

      if (tiempo >= 5) {
        digitalWrite(LED1, LOW);
        tiempo = 0;
        estado = INSPECT;
      }
      break;

    case INSPECT:
      digitalWrite(LED2, HIGH);

      if (tiempo >= 3) {
        digitalWrite(LED2, LOW);
        estado = READY;
      }
      break;

    case READY:
      digitalWrite(LED3, HIGH);
      break;
  }
}