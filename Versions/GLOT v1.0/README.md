# GLOT 2.0

Po wyciągnięciu trudnych lekcji z błędów wieku dziecięcego wersji 1.0/1.5, nadszedł czas na całkowite przeprojektowanie sprzętu. Największą zmianą w wersji 2.0 jest rezygnacja z gotowych modułów typu "Blackpill" na rzecz w pełni autorskiej, zintegrowanej płytki PCB (Custom FC). Skupiłem się na miniaturyzacji, poprawie niezawodności zasilania oraz zmianie linku radiowego na profesjonalny.

W tej wersji całkowicie porzuciłem pomysł budowy własnego, masywnego nadajnika na bazie ESP32 i pada od Xboxa. Zamiast tego przeszedłem na sprawdzony i niezawodny system, co pozwoliło skupić się w 100% na rozwoju samego kontrolera lotu.
![GLOT 2.0 PCB](https://raw.githubusercontent.com/Shotnik420/GLOT/refs/heads/main/Versions/GLOT%20v2.0/GLOTv2PCB.png)

### Kluczowe zmiany sprzętowe (Odbiornik / FC):

- **Autorskie PCB:** Zintegrowanie mikrokontrolera STM32F411 bezpośrednio na płycie. Znacznie zmniejszyło to gabaryty układu, co pozwala na bezproblemowe i bezpieczne umieszczenie go w ciasnym kadłubie samolotu (koniec ze spięciami od upychania kabli na siłę). Płytka zyskała też złącze **USB-C**.
- **Rozdzielone zasilanie (Koniec z brownoutami!):** Zastosowałem zewnętrzne zasilanie dla serw (wejście z UBEC 5V) oraz dedykowany stabilizator LDO (AP7361C 3.3V) zasilający wyłącznie mikrokontroler i czujniki. Prąd serw nie zakłóca już pracy "mózgu" układu.
- **Nowy link radiowy (ELRS):** Niezawodność to podstawa. Zrezygnowałem z modułów nRF24 na rzecz odbiornika **BetaFPV (ExpressLRS)** komunikującego się po protokole UART. Gwarantuje to ogromny zasięg i brak problemów ze zrywaniem połączenia.
- **Blackbox (Pamięć Flash):** Na schemacie pojawił się układ W25Q128 (16MB pamięci Flash po SPI). To fundament pod przyszły system logowania parametrów lotu, co drastycznie ułatwi strojenie algorytmu PID w kolejnych iteracjach.
- **Poprawione błędy:** Ścieżki I2C (SDA/SCL) dla modułu GY-91 zostały poprawnie poprowadzone.

---

## Użyte technologie

| Technologia / Komponent | Opis i zastosowanie w projekcie                                                                                                  |
| :---------------------- | :------------------------------------------------------------------------------------------------------------------------------- |
| **C / STM32 HAL**       | Główny język i biblioteka sprzętowa. Kod rozwija się w stronę bardziej modularnej i przemyślanej architektury.                   |
| **I2C**                 | Protokół do komunikacji z modułem IMU (GY-91 / MPU9250). Tym razem bez zworek i cięcia ścieżek na PCB!                           |
| **UART (Serial)**       | Użyty do odbierania sygnału ze standardowego odbiornika RC (BetaFPV ELRS) oraz do debugowania.                                   |
| **SPI**                 | Szybka magistrala zaprzęgnięta do obsługi nowej pamięci Flash (W25Q128) z myślą o zapisywaniu logów z lotu (Blackbox).           |
| **PWM (Timery)**        | Sprzętowe sterowanie 4 serwami (Lotki L/P, Ster wysokości, Ster kierunku) zasilanymi z dedykowanego obwodu 5V (UBEC).            |
| **Algorytm PID**        | Wciąż obecny system stabilizacji. Główny cel na przyszłość: pełne zrozumienie, optymalizacja i samodzielne napisanie go od nowa. |

---
