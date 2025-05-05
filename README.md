# LaTeX Preprocessor (C++)

Dieses Projekt implementiert einen einfachen Präprozessor zur Umwandlung einer vereinfachten, LaTeX-ähnlichen Syntax in echtes LaTeX – z. B. zur Erzeugung von mathematischen Ausdrücken oder Quellcode-Blöcken.

## Motivation

Das Ziel des Projekts ist es, die Erstellung komplexer LaTeX-Dokumente mit mathematischen Formeln, Quellcode-Snippets und Befehlen zu vereinfachen.

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
| `##define`          | `##define AUTHOR Max`            | wird ersetzt durch `Max`             |
| `##include`         | `##include "mysection.tex"`      | fügt den Inhalt der Datei ein        |

---

## Programmiersprache

- **Sprache:** C++ (C++20 / C++23, getestet mit Visual Studio)
- **Compiler:** MSVC (via Visual Studio)
- JSON-Support mit [nlohmann/json](https://github.com/nlohmann/json) (MIT-Lizenz)
---

## Beispiel: Input / Output

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

Die Konfiguration des Tools erfolgt über eine JSON-Datei wie z. B.:

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

* pdflatex_path: Optional – ermöglicht PDF-Generierung direkt aus dem Tool (sofern installiert)

  
### Anforderungen

* Ein C++ Compiler mit C++20/23-Unterstützun

* getestet in Visual Studio 

* nlohmann/json – bereits im Projekt enthalten (json.hpp)

* Für PDF-Ausgabe: z. B. pdflatex über MiKTeX

