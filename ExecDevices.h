
// ver.02 2026-02-15 — оптимизированная версия для ESP8266
#ifndef ACCURACY
    #define ACCURACY 0.49f
#endif

// ========= Класс "Термостат и Кондиционер"
class Thermostat {
    public:
        // Конструкторы с дефолтными значениями (C++11)
        Thermostat(uint8_t pin, bool active = true, float min = 5.0f, float max = 10.0f, float hyst = 2.0f, bool cooler = false);

        // Параметры
        float min;
        float max;
        float hyst;
        float paramCurrent = 0.0f;
        bool cmd = false;
        bool state = false;
        bool cooler;

        // Методы проверки изменений
        bool paramChanged();
        bool stateChanged();
        bool cmdChanged();
        bool minChanged();
        bool maxChanged();
        bool hystChanged();

        // Управление выходом
        void on();
        void off();
        bool getState();

        // Логика работы
        void runMin();
        void runMax();
        void runMinMax();
        
    private:
        uint8_t _pin;
        bool _active;
        float _paramOld = 0.0f;
        bool _stateOld = false;
        bool _cmdOld = false;
        float _minOld = 0.0f;
        float _maxOld = 0.0f;
        float _hystOld = 0.0f;
};

// Единый конструктор с инициализацией всех параметров
Thermostat::Thermostat(uint8_t pin, bool active, float min, float max, float hyst, bool cooler) :
    _pin(pin),
    _active(active),
    min(min),
    max(max),
    hyst(hyst),
    cooler(cooler)
{
    pinMode(_pin, OUTPUT);
    off();
    _stateOld = getState();
    cmdChanged();
}

void Thermostat::on() {
    digitalWrite(_pin, _active);
}

void Thermostat::off() {
    digitalWrite(_pin, !_active);
}

bool Thermostat::getState() {
    bool readValue = digitalRead(_pin);
    state = (_active) ? readValue : !readValue;
    return state;
}

bool Thermostat::paramChanged() {
    if (fabsf(paramCurrent - _paramOld) >= ACCURACY) {
        _paramOld = paramCurrent;
        return true;
    }
    return false;
}

bool Thermostat::minChanged() {
    if (fabsf(min - _minOld) > ACCURACY) {
        //Serial.println("Параметр Min изменился! Min = " + String(min) + " _minOld = " + String(_minOld) + " --> разница = " + String(fabsf(min - _minOld)));
        _minOld = min;
        return true;
    }
    return false;
}

bool Thermostat::maxChanged() {
    if (fabsf(max - _maxOld) >= ACCURACY) {
        _maxOld = max;
        return true;
    }
    return false;
}

bool Thermostat::hystChanged() {
    if (fabsf(hyst - _hystOld) >= ACCURACY) {
        _hystOld = hyst;
        return true;
    }
    return false;
}

bool Thermostat::cmdChanged() {
    if (cmd != _cmdOld) {
        _cmdOld = cmd;
        return true;
    }
    return false;
}

bool Thermostat::stateChanged() {
    bool currentState = getState();
    if (currentState != _stateOld) {
        _stateOld = currentState;
        return true;
    }
    return false;
}

void Thermostat::runMin() {
    if (!cmd) {
        if (paramCurrent <= min) on();
        if (paramCurrent >= (min + hyst)) off();
    }
}

void Thermostat::runMax() {
    if (cmd) {
        if (!cooler) {
            if (paramCurrent <= (max - hyst)) on();
            if (paramCurrent >= max) off();
        } else {
            if (paramCurrent <= (max - hyst)) off();
            if (paramCurrent >= max) on();
        }
    }
}

void Thermostat::runMinMax() {
    if (cmd) {
        if (!cooler) {
            if (paramCurrent >= max) off();
            if (paramCurrent <= min) on();
        } else {
            if (paramCurrent >= max) on();
            if (paramCurrent <= min) off();
        }
    } else {
        if (!cooler) runMin();
        else off();
    }
}

// ========= Класс "Реле" (Relay)
class Relay {
    public:
        // Конструкторы с дефолтными значениями (C++11)
        Relay(uint8_t pin, bool active = true);

        // Параметры
        bool cmd = false;
        bool state = false;
        
        // Методы проверки изменений
        bool stateChanged();
        bool cmdChanged();
        
        // Управление выходом
        void on();
        void off();
        bool getState();

        // Логика работы
        void run();
        

    private:
        uint8_t _pin;
        bool _active;
        bool _stateOld = false;
        bool _cmdOld = false;
};

// Единый конструктор с инициализацией всех параметров
Relay::Relay(uint8_t pin, bool active) :
    _pin(pin),
    _active(active)
{
    pinMode(_pin, OUTPUT);
    off();
    _stateOld = getState();
    cmdChanged();
}

void Relay::on() {
    digitalWrite(_pin, _active);
}

void Relay::off() {
    digitalWrite(_pin, !_active);
}

bool Relay::getState() {
    bool readValue = digitalRead(_pin);
    state = (_active) ? readValue : !readValue;
    return state;
}

bool Relay::cmdChanged() {
    if (cmd != _cmdOld) {
        _cmdOld = cmd;
        return true;
    }
    return false;
}

bool Relay::stateChanged() {
    bool currentState = getState();
    if (currentState != _stateOld) {
        _stateOld = currentState;
        return true;
    }
    return false;
}

void Relay::run(){
    if (cmd) on();
    else off();
}


// Класс "Виртуальное Реле"
class RelayVirt {
public:
    // Конструктор 
    RelayVirt();

    // Методы управления и проверки
    bool stateChanged();
    bool cmdChanged();

    // Установка команды
    bool cmd = false;
    bool state = false;

private:
    bool _stateOld = false;
    bool _cmdOld = false;
};

// Конструктор: инициализируем пин и начальные состояния
RelayVirt::RelayVirt()
    : _stateOld(state), _cmdOld(cmd) {}

// Проверка изменения состояния реле
bool RelayVirt::stateChanged()  {
    if (state != _stateOld) {
        _stateOld = state;
        return true;
    }
    return false;
}

// Проверка изменения команды
bool RelayVirt::cmdChanged()  {
    if (cmd != _cmdOld) {
        _cmdOld = cmd;
        return true;
    }
    return false;
}

