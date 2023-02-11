# Parkirna rampa
Ovaj projekt omogućuje upravljanje parkirnom rampom pomoću keypada, senzora pokreta i servo motora.

## Funkcionalnosti
### 1. Odabir opcije za unos ili promjenu lozinke
Pri pokretanju programa na LCD-u se priakzuju opcije za A - Unos lozinke i B - Promjenu lozinke

### 2. Opcija promjene lozinke
Za promjenu lozinke, potrebno je prvo upisati master lozinku (zadana vrijednost 0000).
Ukoliko je lozinka ispravna, pokreće se postupak unosa nove lozinke.
Potrebno je upisati 4 znaka nove lozinke nakon čega se nova lozinka prikazuje na ekranu te zamijenjuje staru.
Povratak na početni ekran.

### 3. Opcija unosa lozinke za podiznanje rampe
Za podizanje rampe potebno je upisati ispravnu master lozinku ili ispravnu novu lozinku koju smo postavili u ranijem koraku.
Ispravnim unosom rampa se podiže i zatvara nakon prolaza automobila.
Povratak na početni ekran.

### 4. Senzor pokreta za podizanje rampe
Senzor pokreta na drugoj strani rampe detektira pokret (prilaz automobila), te tada podiže rampu.

### 5. Buzzer
Buzzer se koristi kao potvrdna zvučna povratna informacija pri ptitisku tipke na keypadu, te za potvrdni signal pri unosu ispravne lozinke i alarm pri unosu neispravne.

### 6. Servo motor
Servo motor se koristi za podizanje i spuštanje rampe

## Lozinke
Lozinka ima 4 znaka.
Uneseni znakovi lozinke pri unosu su zamijenjene znakom *.
Moguća su 3 pokušaja unosa ispravne lozinke, nakon 3 neuspješna unosa uključuje se zvučni alarm koji je aktivan sve dok se ne unese ispravna lozinka.
