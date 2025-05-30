# Proiect C++ - Aplicatie de Management Hotelier

## Descriere generală
Această aplicație C++ rulează în consolă și oferă funcționalități pentru gestionarea completă a unui hotel: de la clienți și camere, până la rezervări. Proiectul este bazat pe Programare Orientată pe Obiect și are două categorii principale de utilizatori: **Receptioner** și **Administrator**.

## Categorii de utilizatori

### Receptioner
- Adaugă clienți noi
- Vizualizează lista clienților
- Vizualizează camere libere / ocupate
- Gestionează rezervări:
  - Adăugare rezervare
  - Confirmare rezervare
  - Check-in
  - Check-out
  - Anulare rezervare
  - Modificare rezervare
  - Vizualizare rezervări

### Administrator
- Adaugă camere noi
- Vizualizează toate camerele existente în sistem

## Structura fișierelor

| Fișier               | Descriere                                                              |
| -------------------- | ---------------------------------------------------------------------- |
| `main.cpp`           | Punctul de intrare în aplicație                                        |
| `Hotel.h / .cpp`     | Clasa principală care gestionează clienții, camerele și rezervările    |
| `Client.h / .cpp`    | Clasă pentru reprezentarea și manipularea clienților                   |
| `Camera.h / .cpp`    | Clasă care definește o cameră de hotel                                 |
| `Rezervare.h / .cpp` | Clasă pentru rezervări, cu stări și metode de gestiune                 |
| `clienti.txt`        | Fișier text cu date despre clienți (ID, nume, prenume, CNP, telefon)   |
| `camere.txt`         | Fișier text cu informații despre camere (număr, tip, preț, facilități) |
| `rezervari.txt`      | Fișier text care păstrează toate rezervările și starea lor curentă     |


## Cum rulezi aplicația

1. Asigură-te că ai un compilator C++ instalat (ex: `g++`).
2. Compilează proiectul:

   ```bash
   g++ main.cpp Hotel.cpp Client.cpp Camera.cpp Rezervare.cpp -o hotel_app
