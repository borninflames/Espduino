# Espduino
ESPDuino fun project: 8 relay kapcsolgatása telefonról

## Célom
- A microcontroller képes magát bekonfigurálni a megadott wifi adatokkal, hogy csatlakozzon a helyi hálózathoz. (WiFiManager)
- Amint a hálón vagyok, képes kell legyek telefonnal a megadott requestek segítségével szabályozni a kimeneteket
- Első körben a telefonon kellene 8 darab toggle button.
- Tehát tudnom kell, hogy mi az állapota a 8 relének 

## Szükséges dolgok
### EspDuino oldal:
- WifiManager
- HttpServer, ami képes a requestekre reagálni valamint adatokat visszaszolgáltatni az állapotról
- El kell tudni menteni az időzítésekkel kapcsolatos dolgokat
- Vissza kell tudni tölteni az időzítéssel kapcsolatos dolgokat

### Mobil oldal
- Mobil appot kell csinálni
- Ami képes valahogy megtalálni a microcontroller ip címét
- 8 toggle button a relékhez.
- localstorageben eltárolom a microcontroller ip címét.
- authentikációra nincs szükség
