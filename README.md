# LaTeX Preprocessor (C++)

Dieses Projekt implementiert einen einfachen Präprozessor zur Umwandlung einer vereinfachten, LaTeX-ähnlichen Syntax in echtes LaTeX – z. B. zur Erzeugung von mathematischen Ausdrücken oder Quellcode-Blöcken.

## Motivation

Das Ziel des Projekts ist es, die Erstellung komplexer LaTeX-Dokumente mit mathematischen Formeln, Quellcode-Snippets und Befehlen zu vereinfachen.

---

## Features

$E = mc^2$

### Unterstützte Makros

| Makro                | Beispiel                        | Ergebnis (LaTeX)                    |
|---------------------|----------------------------------|--------------------------------------|
| `frac(x, y)`        | `frac(1, 2)`                     | `\frac{1}{2}`                        |
| `sqrt(x)`           | `sqrt(9)`                        | `\sqrt{9}`                           |
| `abs(x)`            | `abs(-1)`                        | `\left\|-1\right\|`                  |
| `log(x)`            | `log(n)`                         | `\log{n}`                            |
| `pow(x, y)`         | `pow(x, 2)`                      | `{x}^{2}`                            |
| `#math(...)`        | `#math(frac(1,2))`               | `\( \frac{1}{2} \)`                  |
| `#blockmath(...)`   | `#blockmath(frac(1,2))`          | `\[ \frac{1}{2} \]`                  |
| `#codeblock[cpp]{}` | `#codeblock[cpp]{int main() {}}`| `\begin{lstlisting}[language=cpp]...\end{lstlisting}`|

---

## Programmiersprache

- **Sprache:** C++23 preview in VS (Visual Studio)

---

## Beispiel: Input 

### Eingabe (`input.tex`)
```latex
\title{Mein Dokument}
\author{Max Mustermann}

\section{Einleitung}
Hallo Welt! #math(frac(1, 2))

#blockmath(sqrt(pow(n, 2)))

```

### Ausgabe (`input.tex`)
```latex
\title{Mein Dokument}
\author{Max Mustermann}

\section{Einleitung}
Hallo Welt! \( \frac{1}{2} \)

\[ \sqrt{n^2} \]
```

### Anforderungen

* C++20-kompatibler Compiler (z. B. Visual Studio)

* nlohmann/json – bereits im Projekt enthalten (json.hpp)

* Für PDF-Ausgabe: pdflatex

### Kompilierung

Visual Studio
   
    .sln-Datei öffnen → Projekt ausführen
