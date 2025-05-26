# LaTeX Preprocessor (C++)

Dieses Projekt implementiert einen einfachen Präprozessor zur Umwandlung einer vereinfachten, LaTeX-ähnlichen Syntax in echtes LaTeX. Ziel ist es, mathematische Ausdrücke, Quellcode-Snippets und Inhalte effizienter zu schreiben.

## Motivation

Die manuelle Erstellung komplexer LaTeX-Dokumente kann fehleranfällig und aufwendig sein. Dieses Tool vereinfacht die Nutzung typischer Konstrukte wie Brüche, Wurzeln oder Codeblöcke durch eine intuitive Makrosyntax.

---

## Features

### Unterstützte Makros

| Makro                | Beispiel                        | Ergebnis (LaTeX)                    |
|---------------------|----------------------------------|--------------------------------------|
| `\frac(x, y)`        | `\frac(1, 2)`                     | `\frac{1}{2}`                        |
| `\sqrt(x)`           | `\sqrt(9)`                        | `\sqrt{9}`                           |
| `\abs(x)`            | `\abs(-1)`                        | `\left\|-1\right\|`                  |
| `\log(x)`            | `\log(n)`                         | `\log{n}`                            |
| `\pow(x, y)`         | `\pow(x, 2)`                      | `{x}^{2}`                            |
| `#math(...)`        | `#math(frac(1,2))`               | `\(\frac{1}{2}\)`                    |
| `#blockmath(...)`   | `#blockmath(frac(1,2))`          | `\[\frac{1}{2}\]`                    |
| `#codeblock[cpp]{}` | `#codeblock[cpp]{int main() {}}`| `\begin{lstlisting}[language=cpp]...\end{lstlisting}`|
| `\define`          | `\define{AUTHOR}{Max}`            | ersetzt `AUTHOR` im gesamten Text    |
| `\include`         | `\include{"mysection.tex"}`      | fügt Dateiinhalte ein                 |

---

### Dynamisch konfigurierbare Makros

Mathematische Makros wie `frac`, `sqrt`, `log`, `pow` usw. werden beim Programmstart dynamisch aus der Datei `config/macros.json` geladen. Dadurch können neue Makros flexibel ergänzt oder bestehende geändert werden – ganz ohne Änderungen am Quellcode.

Strukturelle Elemente wie `#math(...)`, `#blockmath(...)` oder `#codeblock[...]` sind weiterhin fest in der Programmlogik definiert. Dabei wird der Name des Makros, die Anzahl der Argumente und das gewünschte Ausgabeformat (für LaTeX) angegeben. Im Formatstring müssen Platzhalter der Form __0__, __1__, ..., __n__ verwendet werden, um die jeweiligen Argumente einzufügen – beginnend bei __0__ für das erste Argument.

Beispiel (`macros.json`):

```json
{
  "frac": [2, "\\frac{__0__}{__1__}"],
  "sqrt": [1, "\\sqrt{__0__}"]
}
```

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
\define{AUTHOR}{Max}

\title{Mein Dokument}
\author{AUTHOR}

\section{Einleitung}
Hallo Welt! #math(\frac(1, 2))

#blockmath(\sqrt(\pow(n, 2)))

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

---

## Kommandozeilenargumente
| Option           | Beschreibung                                       | Standardwert               |
| ---------------- | -------------------------------------------------- | -------------------------- |
| `-o`, `--output` | Pfad zur Ausgabedatei                              | `./output/test_output.tex` |
| `-f`, `--format` | Ausgabeformat (`latex`, `markdown`, `html`)        | `latex`                    |
| `-m`, `--macros` | Pfad zur Makrodefinition (`.json`)                 | `./config/macros.json`     |
| `input`          | Eingabedatei (Pflichtparameter, z. B. `input.tex`) | —                          |
| `-h`, `--help`   | Zeigt Hilfe und Optionenbeschreibung an            | —                          |

Beispiel:
LatexPreprocessor -o output/out.tex -f latex -m config/macros.json input/example.tex

---

### Anforderungen 

* C++20-kompatibler Compiler (z. B. Visual Studio 2022)

