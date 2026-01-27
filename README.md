# LaTeX Preprocessor (C++)

Ein modularer und erweiterbarer Präprozessor für LaTeX-artige Eingabesyntax.
Ziel des Projekts ist es, Dokumente in einer vertrauten LaTeX-Syntax zu verfassen
und diese vor der eigentlichen Weiterverarbeitung (z. B. LaTeX-Kompilierung oder
Markdown-Export) strukturiert zu transformieren.


---

## Motivation

Autoren arbeiten im Quelltext mit LaTeX-ähnlicher Syntax und profitieren dabei von
Syntax-Highlighting, Autovervollständigung und Editor-Unterstützung.
Der Präprozessor ermöglicht es, diese Eingabe leichtgewichtig zu erweitern,
zu vereinfachen und perspektivisch für alternative Ausgabeformate aufzubereiten.

---
## Features

### Unterstützte Makroarten

| Makrotyp                               | Beschreibung                                              |
|----------------------------------------|-----------------------------------------------------------|
| `\define{KEY}` / `\define{KEY}{VALUE}` | Textersetzungen und Schalter                              |
| `\ifdef{KEY}...\else...\endif`         | Bedingte Verarbeitung                                     |
| `\include{...}`                        | Rekursive Einbindung externer Dateien                     |
| `Formatmakros`                         | Frei konfigurierbare Transformationen                     |


---

### Formatmakros (dynamisch konfigurierbar)

Formatmakros werden vollständig über eine externe JSON-Datei definiert und
nicht fest im Quellcode implementiert. Jedes Formatmakro besitzt:

- einen Makronamen,
- eine feste Argumentanzahl,
- ein Ersetzungsmuster mit Platzhaltern (`__0__`, `__1__`, …).

Die Platzhalter werden während der Vorverarbeitung durch die tatsächlichen
Argumente des Makroaufrufs ersetzt.

---

Beispiel: mathematische Makros:

```json
{
    "\\frac": {
      "type": "format",
      "arg_count": 2,
      "replacement": "\\frac{__0__}{__1__}"
    },
    "\\sqrt": {
      "type": "format",
      "arg_count": 1,
      "replacement": "\\sqrt{__0__}"
    }
}
```
Ein Aufruf wie

`\frac{1, 2}`

wird während der Vorverarbeitung automatisch in die definierte LaTeX-Struktur überführt.

---

## Beispiel für ein benutzerdefiniertes Formatmakro

Formatmakros sind nicht auf mathematische Ausdrücke beschränkt.
Über das Ersetzungsmuster lassen sich beliebige Text- oder
LaTeX-Strukturen definieren.

Das folgende Makro erzeugt beispielsweise eine hervorgehobene Hinweisstruktur:


```json
{
  "\\note": {
    "type": "format",
    "arg_count": 1,
    "replacement": "\\textbf{Hinweis:} __0__"
  }
}
```
Verwendung im Quelltext:

`\note{Dies ist ein wichtiger Hinweis}`

Ausgabe:

`\textbf{Hinweis:} Dies ist ein wichtiger Hinweis
`

---

## Beispiel: Eingabe -> Ausgabe

### Eingabe

```latex
\define{AUTHOR}{Max}
\define{DEBUG}

\title{Beispieldokument}
\author{AUTHOR}

\ifdef{DEBUG}
Nur im Debug sichtbar.
\else
Alternative
\endif

\section{Mathematik}
\frac{1}{2}

\include{footer.tex}



```
### Ausgabe

```latex
\title{Beispieldokument}
\author{Max}

Nur im Debug sichtbar.

\section{Mathematik}
\frac{1}{2}

% Inhalt von footer.tex wurde hier eingefügt


```

---
## Fehlerdiagnose
* Fehler und Warnungen werden zentral gesammelt
* Ausgabe mit Dateipfad + Zeilennummer
* Verarbeitung bricht nicht beim ersten Fehler ab

Beispiel:

```
Fehler in main.tex: Zeile 12
Syntaxfehler: \\define ohne korrekt geschlossenen KEY

```


---
## Kommandozeilenargumente

| Option           | Beschreibung                         | Standard                        |
|------------------|--------------------------------------|----------------------------------|
| `input`          | Eingabedatei (Pflichtparameter)      | —                                |
| `-o`, `--output` | Ausgabedatei                         | `./output/test_output.tex`       |
| `-m`, `--macros` | JSON-Makrodefinition                 | `./config/dynamic_macro.json`    |
| `-h`, `--help`   | Zeigt Hilfe an                       | —                                |


Beispiel:
latexprepro -o out.tex -m config/dynamic_macro.json input.tex

--- 

## Build & Tests
### Voraussetzung
* C++20-Compiler
* Git
* CMake ≥ 3.16

### Build

- `git clone https://github.com/Fatih-Arik/LatexPreprocessor.git`
- `cd LatexPreprocessor`
- `cmake -S . -B build`
- `cmake --build build`



--- 

## Tests

* Framework: Catch2
* Ausführung:

### Ausführung (nach Build)
`cd build`

`ctest --output-on-failure`

---

## Plattformen

Getestet unter:
* Windows 11
* Debian (VM)

---

