# GLOT 2.0

Druga wersja kontrolera.

Projekt podzielił się na dwa moduły:

- **Nadajnik:** Został zamieniony z własnego systemu na transmiter LiteRadio 2 SE.
- **Odbiornik:** Zamiast gotowej płytki deweloperskiej (jak w v1.0), została zaprojektowana i wykonana **własna autorska płytka PCB**.

Płytka była zainspirowana projektem [DroneController](https://github.com/FPV-Drone-STM32F411/DroneController) i połączenia z niego zostały zapożyczone na pierwsze wersje płytki. Z czasem zostały one zmione lecz autorom tego szablonu, [Evan Bhogel](https://github.com/esb8) oraz [Ammar Mahmood](https://github.com/ammarjmahmood), należy się moje uznanie iż bazowałem na ich projekcie.


![Polutowany GLOT 2.0](https://github.com/Shotnik420/GLOT/blob/main/Versions/GLOT%20v2.0/GLOT_v2_polutowane.jpg?raw=true | width=100))
![Odbiornik GLOT 2.0 PCB](https://raw.githubusercontent.com/Shotnik420/GLOT/refs/heads/main/Versions/GLOT%20v2.0/GLOTv2PCB.png)

## Co mamy na pokładzie? (Hardware)

Przejście na dedykowane PCB pozwoliło pozbyć się zbędnego balastu z gotowych modułów i zoptymalizować projekt pod kątem miejsca w ciasnym kadłubie samolotu.

Ta wersja musiała już wznieść samolot na pierwszy porządny lot.

### Kluczowe elementy nowej płytki to m.in.:

| Komponenty                   |
| :--------------------------- |
| STM32F411CEU6                |
| BETAFPV Nano Receiver 2.4GHz |
| GY-91                        |
| IS25LP128 FLASH              |

| Technologie   |
| :------------ |
| C / STM32 HAL |
| I2C           |
| UART          |
| PWM           |

# Podsumowanie wersji

## Co poszło dobrze

- Wgrywanie kodu na mikroprocesor STM32 działało i bezproblemowo wykonywało wszelkie programy.
- Zmiana na 4 warstwową płytkę było niebem a ziemią w kompaktywności układu. Cały układ zajmował 1/3 miejsca która zajmowała wersja 1.0

## Co poszło źle

Czyli czemu im więcej wiesz, tym wiesz ile jeszcze nie wiesz.

- Brak pinu dla gazu silnika bezszczotkowego o, którym zwyczajnie zapomniano. Trzeba było poświęcić pin TX dla UART.
- Słabe rozmieszczenie komponentów na płytce, kondensatory poustawiane niemal losowo co utrudniało lutowanie.
- Kupno LDO o pinoucie innym od planowanego.
- Zbyt niecierpliwe kupno płytki bez wcześniejszej dokładnej inspekcji.
