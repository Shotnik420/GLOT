# GLOT 1.0/1.5

Wersja startowa, pierwsze kroki położone w kierunku stworzenia prototypu. Pomimo wielu błędów udało się go uruchomić w wersji fizycznej i dokonać jedyny udany lot trwający 7-8 sekund.

Połączyłem dwie wersje w jedną sekcję gdyż jedyna różnica między nimi do płytka która dostała pare poprawek względem poprzedniczki. Kod między nimi jest taki sam.

Postanowiłem stworzyć równocześnie nadajnik jak i odbiornik w tej wersji gdyż wizja kupowania drogiego jak na studenta kontrolera za setki złoty.

Projekt więc podzielił się na dwie części:

- Nadajnik - stworzony z ESP32-S3, który poprzez bluetooth i bibliotekę Bluepad32 pozwalał się łączyć z kontrolerem do konsoli, a za pomocą modułu nRF24L01+PA+LNA posyłać jego wejścia w świat. Decyzja o tak dużym mikrokontrolerze i tak dużych kom

- Odbiornik - Stworzony na STM32 Blackpill z STM32F411ceu6 na pokładzie, który również posiadał moduł nRF24L01+PA+LNA, który ściągał informacje i je przekładał na zmiany w położeniach dla 4 silników serwo i prędkości jednego silnika bezszczotkowego. Na pokładzie był również żyroskop i prosty system PID, go obsługujący.

### Co poszło dobrze

- Udało się dokonać połączenia radiowego między urządzeniami.
- Wylanie masy poprawiło bardzo design płytek.
- Nauczenie się obsługi lutownicy Hot Air

#### Odbiornik

- Pisanie kodu w .C dla STM'a okazało się zaskakująco proste i przyjemne. Stworzyłem całkiem dobry system, który posiadał odbieranie danych radiowych i przekazywanie dostarczonych wartości na zakres serw. Jest prowizoryczny PID (Prioporcjonalno-całkująco-różniczkujący), który jednak jest tak skomplikowany że nie przyznam się do jego napisania, a zawdzięczam go Gemini od Google'a, lecz jest to jeden z moich celów na następne wersje by go zrozumieć i napisać raz jeszcze.

### Co poszło ŹLE

#### Odbiornik

- Połączenie prądu serw z prądem układu sterowniczego. STM32 brownoutował za każdym mocniejszym pociągnieciem serwa. Wymagało przecięcia ścieżek i dolutowania osobnej baterii.
- Pomylone ścieżki SDA i SCL dla modułu GY-91. Wymagało przeciecia ścieżek i zrobienia zworek.
- Zbyt słaby prąd dla serw. Uruchomienie wszystkich naraz było niemożliwe.
- Rozmiar płytki był wyraźnie niekorzystny dla budowy samolotu. Za duży.
- Niezabezpieczenie układów przed wsadzeniem ich do ciasnego kokpitu pojazdu spowodowało fizyczne zwarcie i spalenie mikrokontrolera i przetwornicy

#### Nadajnik

- Użycie komponentów będących tzw. "pod ręką" przez co płytka nadajnika jest niepotrzebnie duża. Duże przyciski, duży moduł przetwornicy, duży mikrokontroler.
- Dodanie pól na przyciski, których nie musiało tam być bo kontroler realizował wszystkie potrzeby wejścia.
- Pomimo faktu że zrobienie nadajnika na podstawie kontrolera do Xboxa wydawało się fajnym pomysłem to faktyczna realizacja trochę zniechęciła mnie do tej wizji. Częste zerwania połączeń, zmiany bibliotek, zacinający się cały esp32. Zastanowie się nad kupnem faktycznego kontrolera w przyszłości.
