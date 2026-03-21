# VCU Test Framework

Framework de test complet pour les modules **throttle.c** et **utils.c** du projet VCU Linux FreeRTOS.

## 📋 Description

Ce framework permet de tester facilement les fonctions critiques de contrôle du throttle et les fonctions utilitaires avec :
- **Tests automatisés** avec assertions intelligentes
- **Affichage coloré et formaté** des résultats
- **Statistiques complètes** (tests passés/échoués, assertions)
- **Makefile optimisé** avec plusieurs modes de compilation

## 🗂️ Structure du projet

```
tests/
├── Makefile                 # Système de compilation
├── README.md                # Ce fichier
├── test_framework.h         # Framework de test (macros et utilitaires)
├── test_main.c              # Point d'entrée principal
├── test_throttle.c          # Tests du module throttle
├── test_utils.c             # Tests du module utils
└── build/                   # Dossier de compilation (créé automatiquement)
    ├── test_runner          # Exécutable généré
    └── *.o                  # Fichiers objets
```

## 🚀 Utilisation

### Installation et exécution rapide

```bash
cd tests/
make run
```

### Commandes disponibles

| Commande | Description |
|----------|-------------|
| `make run` | Compile et exécute les tests |
| `make build` | Compile uniquement l'exécutable |
| `make clean` | Nettoie les fichiers compilés |
| `make rebuild` | Recompile depuis zéro |
| `make debug` | Compile avec symboles de débogage |
| `make release` | Compile en mode optimisé |
| `make run-gdb` | Lance les tests sous GDB |
| `make help` | Affiche l'aide |
| `make list-tests` | Liste tous les tests disponibles |
| `make info` | Affiche la configuration du build |

## 📊 Test Suites

### TestsThrottle (throttle.c)

#### ✓ CheckAndLimitRange
- Teste le clamping des valeurs d'entrée throttle
- Vérifie la gestion de l'inversion throttle (potmin > potmax)
- Tests avec slack tolerance

Exemple :
```c
potmin[0] = 100;  potmax[0] = 900;
int val = 950;
CheckAndLimitRange(&val, 0);  // val sera clampé à 900
```

#### ✓ NormalizeThrottle
- Normalise les valeurs throttle de [potmin, potmax] à [0, 100%]
- Gère les cas limites (index invalides, potmin == potmax)

Exemple :
```c
potmin[0] = 0;  potmax[0] = 1000;
float pct = NormalizeThrottle(500, 0);  // Retourne 50.0%
```

#### ✓ CalcThrottle
- Calcul du throttle avec deadzone
- Gestion de la régénération au freinage
- Filtrage de la vitesse

### Tests Utils (utils.c)

#### ✓ change (conversion entière)
- Conversion linéaire d'une plage à une autre (entiers)
- Cas avec plages inversées
- Cas avec plages négatives

Exemple :
```c
int result = change(5, 0, 10, 0, 100);  // 50
```

#### ✓ changeFloat (conversion flottante)
- Conversion linéaire avec précision flottante
- Cas d'usage pratiques : conversion ADC, tensions, courants

Exemple :
```c
float pwm = changeFloat(512.0f, 0.0f, 1023.0f, 0.0f, 255.0f);  // ~127.5
```

#### ✓ Math Helpers
- Macros ABS, MIN, MAX

#### ✓ Scénarios Pratiques
- Mapping pédale throttle (ADC → %)
- Pression de frein (kPa → PWM)
- Tension moteur (RPM → Voltage)
- Surveillance batterie (V → ADC)

## 🎨 Format d'affichage

### Exemple de sortie

```
═══════════════════════════════════════════════════════════════
                    VCU TEST FRAMEWORK - TEST SUITE
                   Testing: throttle.c, utils.c
═══════════════════════════════════════════════════════════════

╔════════════════════════════════════════════════════════════╗
║ Test Suite: CheckAndLimitRange
╚════════════════════════════════════════════════════════════╝

   ▶ Value within range - should pass
   ✓ Test PASSED

   ▶ Value below minimum - should clamp to min
   ✓ Test PASSED
   
   ...

╔════════════════════════════════════════════════════════════╗
║ TEST SUMMARY
╚════════════════════════════════════════════════════════════╝

  Tests:
      Total:  25
      Passed: 25 
      Failed: 0

  Assertions:
      Total:  87
      Passed: 87
      Failed: 0

  ✓ ALL TESTS PASSED!
```

### Éléments du rapport

- 🟢 **✓ Test PASSED** - Tous les assertions ont réussi
- 🔴 **✗ Test FAILED** - Au moins un assertion a échoué
- ✗ **Failed assertion** - Détails de l'erreur avec valeurs attendues/obtenues
- ℹ **Info** - Messages informatifs sur le test
- ▶ **Test** - Début d'un test

## 🛠️ Écrire vos propres tests

### Structure de base

```c
#include "test_framework.h"

void test_my_function(void)
{
    TEST_SUITE_START("My Test Suite");

    // Test 1
    {
        TEST_START("Description du test");
        
        // Setup
        int value = 42;
        
        // Action
        int result = my_function(value);
        
        // Vérification
        ASSERT_EQUAL_INT(result, expected_value);
        TEST_INFO_INT("Result was", result);
        
        TEST_END;
    }
}
```

### Macros disponibles

#### Assertions

```c
ASSERT_EQUAL_INT(actual, expected)           // Comparer des entiers
ASSERT_EQUAL_FLOAT(actual, expected, tol)    // Comparer des flottants
ASSERT_TRUE(condition)                       // Vérifier vrai
ASSERT_FALSE(condition)                      // Vérifier faux
ASSERT_IN_RANGE(value, min, max)            // Vérifier dans une plage
```

#### Infos

```c
TEST_INFO(message)                          // Message générique
TEST_INFO_INT(label, value)                 // Afficher un entier
TEST_INFO_FLOAT(label, value)               // Afficher un flottant
```

## 🔍 Débogage

### Compiler avec debug

```bash
make debug       # Compile avec -g et -O0
make run-gdb     # Lance sous GDB
```

### Dans GDB

```gdb
(gdb) break test_CheckAndLimitRange
(gdb) run
(gdb) print potval
(gdb) next
```

## 📈 Statistiques

Le framework suivi automatiquement :
- Nombre total de tests
- Nombre de tests réussis/échoués
- Nombre total d'assertions
- Nombre d'assertions réussies/échouées

## 🎯 Bonnes pratiques

1. **Tests unitaires** : Chaque test doit tester une seule chose
2. **Assertions claires** : Utiliser les assertions appropriées
3. **Messages informatifs** : Ajouter des messages pour comprendre les données
4. **Cas limites** : Toujours tester min, max, cas invalides
5. **Tolerance flottante** : Utiliser `ASSERT_EQUAL_FLOAT` avec tolerance appropriée

## 📝 Exemples de tests complets

### Test simple

```c
TEST_START("Normalize 50% of range");
float result = NormalizeThrottle(500, 0);
ASSERT_EQUAL_FLOAT(result, 50.0f, 0.01f);
TEST_END;
```

### Test avec setup

```c
TEST_START("Clamping above maximum");
potmin[0] = 100;
potmax[0] = 900;
int val = 950;
bool success = CheckAndLimitRange(&val, 0);
ASSERT_FALSE(success);
ASSERT_EQUAL_INT(val, 900);
TEST_INFO_INT("Clamped to", val);
TEST_END;
```

## 🐛 Troubleshooting

### GCC introuvable

```bash
sudo apt-get install build-essential
```

### Erreur de compilation

```bash
make clean
make build
```

### Tests échouent de manière intermittente

- Vérifier les variables globales
- Réinitialiser l'état dans chaque test
- Chercher les race conditions

## 📚 Référence rapide

```bash
# Utilisation rapide
cd tests/
make                  # Défaut : build + run
make run              # Compile et exécute
make clean            # Nettoie
make rebuild          # Recompile tout
```

## 📄 License

Partie du projet VCU Linux FreeRTOS v202107.00

---

**Auteur** : Framework de test personnalisé  
**Date** : 2024  
**Test Framework** : Version 1.0
