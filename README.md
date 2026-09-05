# ✈️ GLOT - Kontroler Lotu RC

GLOT to projekt, którego celem jest stworzenie kontrolera lotu dla samolotów RC z możliwie jak najlepszym stosunkiem jakości do ceny.

![GLOT 2.0 PCB](https://raw.githubusercontent.com/Shotnik420/GLOT/refs/heads/main/Versions/GLOT%20v2.0/GLOTv2PCB.png)

Jest to projekt głównie do naukii podstaw awioniki jak i tworzenia systemów opartych na STM32 i jak radzą sobie w trudnych warunkach.

![Zlozony zestaw](https://raw.githubusercontent.com/Shotnik420/GLOT/refs/heads/main/Versions/GLOT%20v1.0/Stery.gif)

---

## Wersje

- ⚡ **[GLOT v2.0 (Obecna wersja)](https://github.com/Shotnik420/GLOT/tree/main/Versions/GLOT%20v1.0)**  
  _Rozwój i optymalizacja._ Najnowsza generacja kontrolera, w której wyciągnięto wnioski z błędów poprzednika. Zoptymalizowano kwestie zasilania serwomechanizmów, poprawiono routing ścieżek na PCB i zmniejszono rozmiary urządzenia, aby lepiej pasowało do ciasnych kadłubów modeli RC.

- 📦 **[GLOT v1.0 / 1.5](https://github.com/Shotnik420/GLOT/tree/main/Versions/GLOT%20v1.0)**  
  _Wersja prototypowa._ Pierwsze kroki i weryfikacja koncepcji. Zakończona sukcesem w postaci pierwszego, kilkusekundowego lotu. Pozwoliła zidentyfikować kluczowe problemy z zasilaniem (brownouty), zakłóceniami magistrali I2C i gabarytami płytek. Składała się z dedykowanego nadajnika na ESP32 i odbiornika na STM32.
