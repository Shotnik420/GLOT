# ✈️ GLOT - Kontroler Lotu RC

**GLOT** to projekt, którego głównym celem jest stworzenie kontrolera lotu (Flight Controller) dla modeli samolotów RC, oferującego możliwie najlepszy stosunek jakości do ceny. 

Projekt powstał z pasji do lotnictwa oraz elektroniki i pełni funkcję zaawansowanej platformy edukacyjnej. Stanowi poligon doświadczalny, który pozwala:
- Zgłębić w praktyce **podstawy awioniki** oraz mechaniki lotu.
- Rozwijać umiejętności tworzenia systemów wbudowanych (Embedded Systems) w oparciu o architekturę **STM32**.
- Sprawdzać, jak zaprojektowane od zera układy elektroniczne i algorytmy (np. stabilizacja PID) radzą sobie w **trudnych i dynamicznych warunkach** fizycznych podczas lotu.

---

## Historia i Wersje Projektu

Do tej pory doczekał się dwóch głównych generacji sprzętowych (i łącznie trzech iteracji).

- 📦 **[GLOT v1.0 / 1.5](https://github.com/Shotnik420/GLOT/tree/main/Versions/GLOT%20v1.0)**  
  *Wersja prototypowa.* Pierwsze kroki i weryfikacja koncepcji (Proof of Concept). Zakończona sukcesem w postaci pierwszego, kilkusekundowego lotu. Pozwoliła zidentyfikować kluczowe problemy z zasilaniem (brownouty), zakłóceniami magistrali I2C i gabarytami płytek. Składała się z dedykowanego nadajnika na ESP32 i odbiornika na STM32.

- ⚡ **GLOT v2.0 (Obecna wersja)**  
  *Rozwój i optymalizacja.* Najnowsza generacja kontrolera, w której wyciągnięto wnioski z błędów poprzednika. Zoptymalizowano kwestie zasilania serwomechanizmów, poprawiono routing ścieżek na PCB i zmniejszono rozmiary urządzenia, aby lepiej pasowało do ciasnych kadłubów modeli RC. 
