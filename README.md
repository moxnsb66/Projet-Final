# 6GEI186 — Travail 2 : Multiplication IEEE‑754 (simple précision) en ARMv8

**Fichiers fournis**
- `float_mul.asm` — implémentation ARMv8 AArch64 avec sous‑routines et un mini banc d’essai.
- `README.md` — explication et correspondances registres ↔ variables.

## Stratégie (conforme à l’énoncé)
- On **débiaise** chaque exposant (−127), on les additionne, puis on **rebiaise** à la fin (+127).
- On extrait le **signe** (bit31) et on calcule `signe = sa XOR sb`.
- On extrait la **fraction** 23 bits et on ajoute le **1 implicite** pour obtenir des **24 bits** (1.f).
- On multiplie les fractions **24×24 → 48 bits** (`UMULL`), puis on **normalise** :  
  si le bit47 est 1 (≥ 2.0), on décale à droite de 1 et on incrémente l’exposant.
- On construit la mantisse finale avec les **23 bits** de poids fort après le 1 implicite, avec un **arrondi** basique (guard + rest).
- **Pas** de gestion d’overflow/underflow d’exposant (suivant l’énoncé).

## Sous‑routines
- `get_sign(w0)` → `w0 ∈ {0,1}`
- `get_exp_unbiased(w0)` → `w0 = exponent − 127`
- `get_frac24(w0)` → `w0 = (1<<23) | fraction`
- `fpmul_sp(w0,w1)` → `w0 = produit`

## Correspondances registres (dans `fpmul_sp`)
- `w20` = a, `w21` = b  
- `w22` = signe, `w24` = somme exposants débiaisés (+ ajustements de normalisation)  
- `w25` = frac(a) 24b, `w26` = frac(b) 24b  
- `x0` = produit 24×24 sur 64 bits (48 utiles), `x9..x15` temporaires normalisation/arrondi

## Tests (issus de l’énoncé)
- a) `0x41bc7ae1 * 0x4427fc29 = 0x46775ba0`
- b) `0xc1bc7ae1 * 0x4427fc29 = 0xc6775ba0`
- c) `0x41bc7ae1 * 0xc427fc29 = 0xc6775ba0`
- d) `0xc1bc7ae1 * 0xc427fc29 = 0x46775ba0`

## Compilation/Exécution (pistes)
- macOS/Apple Silicon (clang) : assemblez puis liez avec un point d’entrée `_start` ou intégrez dans un harness C.
- Linux aarch64 : adaptez le prologue d’entrée et la séquence de sortie (syscall `exit`).

## Points à surveiller
- Les **NaN/Inf/Zéros dénormalisés** ne sont pas gérés ici (hors périmètre).
- L’arrondi peut être affiné (round‑to‑nearest‑even).

— Bon TP!
