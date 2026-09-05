# GLOT 1.0 / 1.5

Wersja startowa i pierwsze kroki postawione w kierunku stworzenia w pełni funkcjonalnego prototypu. Pomimo błędów wieku dziecięcego, platformę udało się uruchomić fizycznie, co zaowocowało udanym lotem trwającym 7-8 sekund. Połączyłem wersje 1.0 i 1.5 w jedną sekcję, ponieważ jedyną różnicą między nimi jest płytka PCB, która otrzymała kilka poprawek, kod pozostaje identyczny.

Zdecydowałem się na stworzenie autorskiego nadajnika i odbiornika, aby ominąć koszty drogiej aparatury RC (co dla budżetu studenta jest kluczowe). Projekt podzielił się na dwa moduły:

- **Nadajnik:** Oparty na ESP32-S3. Wykorzystuje bibliotekę Bluepad32 do łączenia się z kontrolerem od konsoli przez Bluetooth, a wejścia przesyła w świat za pomocą modułu nRF24L01+PA+LNA. Decyzja o użyciu tak potężnego mikrokontrolera i sporych komponentów z tzw. "szuflady" sprawiła, że układ stał się dość masywny.
- **Odbiornik:** Zbudowany na bazie płytki STM32 Blackpill z układem STM32F411CEU6. Moduł nRF24L01+PA+LNA odbiera dane i tłumaczy je na wysterowanie 4 serwomechanizmów oraz prędkość silnika bezszczotkowego (ESC). Na pokładzie znajduje się moduł GY-91 (MPU9250), obsługiwany przez prowizoryczny system PID stabilizujący lot.

![Nadajnik](https://github.com/Shotnik420/GLOT/blob/main/Versions/GLOT%20v1.0/Nadajnik%20dzialajacy.jpg?raw=true)

---

## Użyte technologie

| Technologia / Komponent | Opis i zastosowanie w projekcie                                                                 |
| :---------------------- | :---------------------------------------------------------------------------------------------- |
| **C / STM32 HAL**       | Główny język i biblioteka sprzętowa użyta do zaprogramowania mikrokontrolera STM32.             |
| **I2C**                 | Protokół komunikacyjny użyty do odczytu danych z modułu GY-91 (akcelerometr/żyroskop).          |
| **SPI**                 | Szybka magistrala użyta do obsługi modułu radiowego nRF24L01+PA+LNA.                            |
| **PWM (Timery)**        | Sprzętowe generowanie sygnałów (TIM2, TIM3) sterujących serwami i silnikiem bezszczotkowym.     |
| **USB CDC**             | Wirtualny port COM (USB) wykorzystany do wygodnego przesyłania logów i debugowania.             |
| **Algorytm PID**        | Regulator (Proporcjonalno-Całkująco-Różniczkujący) wyliczający korekty lotek i steru wysokości. |

---

## Podsumowanie testów

### Odbiornik

### Co poszło dobrze

- Pisanie kodu w .C dla STM'a okazało się zaskakująco proste i przyjemne. Stworzyłem całkiem dobry system, który posiadał odbieranie danych radiowych i przekazywanie dostarczonych wartości na zakres serw. Jest prowizoryczny PID (Prioporcjonalno-całkująco-różniczkujący), który jednak jest tak skomplikowany że nie przyznam się do jego napisania, a zawdzięczam go Gemini od Google'a, lecz jest to jeden z moich celów na następne wersje by go zrozumieć i napisać raz jeszcze.

### Co poszło ŹLE

- Połączenie prądu serw z prądem układu sterowniczego. STM32 brownoutował za każdym mocniejszym pociągnieciem serwa. Wymagało przecięcia ścieżek i dolutowania osobnej baterii.

- Pomylone ścieżki SDA i SCL dla modułu GY-91. Wymagało przeciecia ścieżek i zrobienia zworek.

- Zbyt słaby prąd dla serw. Uruchomienie wszystkich naraz było niemożliwe.

- Rozmiar płytki był wyraźnie niekorzystny dla budowy samolotu. Za duży.

- Niezabezpieczenie układów przed wsadzeniem ich do ciasnego kokpitu pojazdu spowodowało fizyczne zwarcie i spalenie mikrokontrolera i przetwornicy

### Nadajnik

### Co poszło ŹLE

- Użycie komponentów będących tzw. "pod ręką" przez co płytka nadajnika jest niepotrzebnie duża. Duże przyciski, duży moduł przetwornicy, duży mikrokontroler.

- Dodanie pól na przyciski, których nie musiało tam być bo kontroler realizował wszystkie potrzeby wejścia.

- Pomimo faktu że zrobienie nadajnika na podstawie kontrolera do Xboxa wydawało się fajnym pomysłem to faktyczna realizacja trochę zniechęciła mnie do tej wizji. Częste zerwania połączeń, zmiany bibliotek, zacinający się cały esp32. Zastanowie się nad kupnem faktycznego kontrolera w przyszłości.
