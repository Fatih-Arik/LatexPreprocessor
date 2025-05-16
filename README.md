# LaTeX Preprocessor (C++)

Dieses Projekt implementiert einen einfachen Präprozessor zur Umwandlung einer vereinfachten, LaTeX-ähnlichen Syntax in echtes LaTeX. Ziel ist es, mathematische Ausdrücke, Quellcode-Snippets und Inhalte effizienter zu schreiben.

## Motivation

Die manuelle Erstellung komplexer LaTeX-Dokumente kann fehleranfällig und aufwendig sein. Dieses Tool vereinfacht die Nutzung typischer Konstrukte wie Brüche, Wurzeln oder Codeblöcke durch eine intuitive Makrosyntax.

---

## Features

### Unterstützte Makros

| Makro                | Beispiel                        | Ergebnis (LaTeX)                    |
|---------------------|----------------------------------|--------------------------------------|
| `frac(x, y)`        | `frac(1, 2)`                     | `\frac{1}{2}`                        |
| `sqrt(x)`           | `sqrt(9)`                        | `\sqrt{9}`                           |
| `abs(x)`            | `abs(-1)`                        | `\left\|-1\right\|`                  |
| `log(x)`            | `log(n)`                         | `\log{n}`                            |
| `pow(x, y)`         | `pow(x, 2)`                      | `{x}^{2}`                            |
| `#math(...)`        | `#math(frac(1,2))`               | `\(\frac{1}{2}\)`                    |
| `#blockmath(...)`   | `#blockmath(frac(1,2))`          | `\[\frac{1}{2}\]`                    |
| `#codeblock[cpp]{}` | `#codeblock[cpp]{int main() {}}`| `\begin{lstlisting}[language=cpp]...\end{lstlisting}`|
| `##define`          | `##define AUTHOR Max`            | ersetzt `AUTHOR` im gesamten Text    |
| `##include`         | `##include "mysection.tex"`      | fügt Dateiinhalte ein                 |

---

### Dynamisch konfigurierbare Makros

Mathematische Makros wie `frac`, `sqrt`, `log`, `pow` usw. werden beim Programmstart dynamisch aus der Datei `config/macros.json` geladen. Dadurch können neue Makros flexibel ergänzt oder bestehende geändert werden – ganz ohne Änderungen am Quellcode.

Strukturelle Elemente wie `#math(...)`, `#blockmath(...)` oder `#codeblock[...]` sind weiterhin fest in der Programmlogik definiert.

Beispiel (`macros.json`):

```json
{
  "frac": [2, "\\frac{__0__}{__1__}"],
  "sqrt": [1, "\\sqrt{__0__}"]
}
---
## Technologien

- **Sprache:** C++ (C++20 / C++23)
- **Compiler:** MSVC (via Visual Studio)
- **Bibliotheken:**
  - [`nlohmann/json`](https://github.com/nlohmann/json) (für Konfigurationsdateien)
  - Standardbibliothek: `<regex>`, `<fstream>`, `<filesystem>`, etc.

---

## Beispiel

### Eingabe

```latex
##define AUTHOR Max

\title{Mein Dokument}
\author{AUTHOR}

\section{Einleitung}
Hallo Welt! #math(frac(1, 2))

#blockmath(sqrt(pow(n, 2)))

#codeblock[cpp]{
#include <iostream>
int main() {
    std::cout << "Hallo Welt!";
}
}

```

### Ausgabe 
```latex
\title{Mein Dokument}
\author{Max}

\section{Einleitung}
Hallo Welt! \(\frac{1}{2}\)

\[\sqrt{{n}^{2}}\]

\begin{lstlisting}[language=cpp]
#include <iostream>
int main() {
    std::cout << "Hallo Welt!";
}
\end{lstlisting}
```

### Konfiguration(config.json)

```json
{
  "input_file": "input.tex",
  "output_file": "output.tex",
  "output_folder": "output/",
  "pdflatex_path": "C:/Program Files/MiKTeX/miktex/bin/x64/pdflatex.exe"
}
```

Hinweise:

* input_file: Pfad zur Eingabedatei

* output_file: Zielname für den konvertierten LaTeX-Code

* output_folder: Zielverzeichnis

* pdflatex_path: Optional – Optional – erlaubt automatische PDF-Erzeugung


### Anforderungen 

* C++20-kompatibler Compiler (z. B. Visual Studio 2022)

* Optional: pdflatex (für direkte PDF-Ausgabe)
