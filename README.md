# LaTeX Preprocessor (C++)

Das Ziel dieses Projekts war es, einen erweiterbaren Präprozessor für vereinfachtes LaTeX zu erstellen, um flexible Textformate effizienter zu schreiben und konvertieren zu können.

## Motivation

Die manuelle Erstellung komplexer LaTeX-Dokumente kann fehleranfällig und aufwendig sein. Dieses Tool vereinfacht die Nutzung typischer Konstrukte wie Brüche, Wurzeln oder Codeblöcke durch eine intuitive Makrosyntax.

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
| `\define{...}{...}`  | `\define{AUTHOR}{Max}`                                     | definiert ein Flag ohne Wert                               |
| `\define{...}`       | `\define{DEBUG}`                                           | ersetzt `AUTHOR` im gesamten Text                          |
| `\ifdef{X}...\endif` | `\ifdef{DEBUG} Text \endif`                                | wird nur ersetzt, wenn `DEBUG` definiert ist               |
| `\ifdef{X}...\else...\endif` | `\ifdef{DEBUG} Text \else Verzweigung \endif`      | Verzweigung wird nur ersetzt, wenn `DEBUG` nicht definiert |
| `\include{...}`      | `\include{"mysection.tex"}`                                | Inhalt der Datei wird eingefügt                            |

---

### Dynamisch konfigurierbare Makros

Makros wie \frac, \sqrt, \log, \pow etc. werden zur Laufzeit aus einer JSON-Datei geladen:

Hier können beliebige einfache Makros ( in der Form \name(0, 1, ...) ) definieren werden, ohne den Code zu ändern, Platzhalter wie __0__, __1__, etc. stehen für die Argumente und die Zahl für die Anzahl an Argumenten. 

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
Dies ist nur sichtbar im Debug-Modus.
\else
Das ist eine Verzweigung
\endif

\section{Mathematik}
Inline: #math(\frac(1, 2))

Block:
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

Dies ist nur sichtbar im Debug-Modus.

\section{Mathematik}
Inline: \(\frac{1}{2}\)

Block:
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
| Option           | Beschreibung                                       | Standardwert               |
| ---------------- | -------------------------------------------------- | -------------------------- |
| `-o`, `--output` | Pfad zur Ausgabedatei                              | `./output/test_output.tex` |
| `-m`, `--macros` | Pfad zur Makrodefinition (`.json`)                 | `./config/macros.json`     |
|(Planung) `-f`, `--format` | Ausgabeformat (`latex`, `markdown`, `html`)| `latex`                   |
| `input`          | Eingabedatei (Pflichtparameter, z. B. `input.tex`) | —                          |
| `-h`, `--help`   | Zeigt Hilfe und Optionenbeschreibung an            | —                          |



Beispiel:
LatexPreprocessor -o output/out.tex -m config/macros.json input/example.tex

--- 

## Technologien

- **Sprache:** C++ (C++20 / C++23)
- **Compiler:** MSVC (via Visual Studio)
- **Bibliotheken:**
  - [`nlohmann/json`](https://github.com/nlohmann/json) 
  - [`jarro2783/cxxopts`](https://github.com/jarro2783/cxxopts) 
  - Standardbibliothek: `<regex>`, `<fstream>`, `<filesystem>`, etc.


---

### Anforderungen 

* C++20-kompatibler Compiler (z. B. Visual Studio 2022)

