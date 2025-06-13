# LaTeX Preprocessor (C++)

Ein modularer und erweiterbarer Präprozessor für vereinfachte LaTeX-Syntax. Ziel ist die effiziente Erstellung technischer Dokumente mit kürzerer und intuitiverer Syntax.


---

## Features

### Unterstützte Makros

| Makro                | Beispiel                        | Ergebnis (LaTeX)                                                                                        |
|---------------------|----------------------------------|---------------------------------------------------------------------------------------|
| `\frac(x, y)`        | `\frac(1, 2)`                                              | `\frac{1}{2}`                                              |
| `\sqrt(x)`           | `\sqrt(9)`                                                 | `\sqrt{9}`                                                 |
| `\abs(x)`            | `\abs(-1)`                                                 | `\left\|-1\right\|`                                        | 
| `\log(x)`            | `\log(n)`                                                  | `\log{n}`                                                  |
| `\pow(x, y)`         | `\pow(x, 2)`                                               | `{x}^{2}`                                                  |
| `#math(...)`         | `#math(frac(1,2))`                                         | `\(\frac{1}{2}\)`                                          |
| `#blockmath(...)`    | `#blockmath(frac(1,2))`                                    | `\[\frac{1}{2}\]`                                          |
| `#codeblock[cpp]{}`  | `#codeblock[cpp]{int main() {}}`                           | `\begin{lstlisting}[language=cpp]...\end{lstlisting}`      |
| `\define{...}{...}`  | `\define{AUTHOR}{Max}`                                     | Definiert Makro ohne Wert                                  |
| `\define{...}`       | `\define{DEBUG}`                                           | Makro mit Ersetzungswert                                   |
| `\ifdef{X}...\endif` | `\ifdef{DEBUG} Text \endif`                                | Nur sichtbar, wenn `DEBUG` definiert ist                   |
| `\ifdef{X}...\else...\endif` | `\ifdef{DEBUG} Text \else Verzweigung \endif`      | Alternative bei fehlender Definition                       |
| `\include{...}`      | `\include{"mysection.tex"}`                                | Inhalt der Datei wird eingefügt                            |

---

### Dynamisch konfigurierbare Makros

Makros wie \frac, \sqrt, \log, \pow etc. werden zur Laufzeit aus einer JSON-Datei geladen:

* Erste Zahl = Anzahl der Argumente

* \__0__, \__1__, … = Platzhalter für Argumente

---

Beispiel (`macros.json`):

```json
{
  "\frac": [2, "\\frac{__0__}{__1__}"],
  "\sqrt": [1, "\\sqrt{__0__}"]
}
```

---

## Beispiel

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
Inline: #math(\frac(1, 2))

#blockmath(\sqrt(\pow(n, 2)))

#codeblock[cpp]{
#include <iostream>
int main() {
    std::cout << "Hallo Welt!";
}
}

\include{"footer.tex"}


```

### Ausgabe 
```latex
\title{Beispieldokument}
\author{Max}

Nur im Debug sichtbar.

\section{Mathematik}
Inline: \(\frac{1}{2}\)

\[\sqrt{{n}^{2}}\]

\begin{lstlisting}[language=cpp]
#include <iostream>
int main() {
    std::cout << "Hallo Welt!";
}
\end{lstlisting}

% Inhalt von footer.tex wird hier eingefügt
```

---

## Kommandozeilenargumente
| Option           | Beschreibung                                          | Standardwert               |
| ---------------- | ------------------------------------------------------|--------------------------- |
| `-o`, `--output` | Pfad zur Ausgabedatei                                 | `./output/test_output.tex` |
| `-m`, `--macros` | JSON-Datei mit Makrodefinitionen                      | `./config/macros.json`     |
| `-f`, `--format` | (geplant)Ausgabeformat (`latex`, `markdown`, `html`)  | `latex`                    |
| `input`          | Pfad zur Eingabedatei  (Pflichtparameter `input.tex`) | —                          |
| `-h`, `--help`   | Zeigt Hilfe an                                        | —                          |



Beispiel:
latexprepro -o output/out.tex -m config/macros.json latex_docs/komplex.tex

--- 

## Unit-Tests mit Catch2 (CMake integriert)

Das Projekt verwendet Catch2 als modernes C++-Test-Framework.

* Testquelle: `tests/test_runner.cpp`
* Getestet: Makroverarbeitung, Bedingungsersetzung (\ifdef)

### Ausführung (nach Build)
`./build/Debug/test_runner.exe`

--- 

## Build & Tests (über CMake)
### Voraussetzung
* C++20-kompatibler Compiler
* Git (für Catch2 via FetchContent)
* CMake (getestet unter der Version 4.0.2)

### Schritte

- `git clone <repo-url>`
- `cd LatexPreprocessor`
- `cmake -S . -B build`
- `cmake --build build`



### Starten:
`./build/Debug/latexprepro.exe pfad/zur/eingabe.tex`

---
## Technologien

- **Sprache:** C++20
- **Buildsystem:** [`CMake`](https://cmake.org/download/)
- **Bibliotheken:**
  - [`nlohmann/json`](https://github.com/nlohmann/json) 
  - [`jarro2783/cxxopts`](https://github.com/jarro2783/cxxopts)
  - [`catchorg/Catch2`](https://github.com/catchorg/Catch2) 

---


## Getestet unter 
* Windows 11

* Unterstützung für Linux/macOS: **in Arbeit**

