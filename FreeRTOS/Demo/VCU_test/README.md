# VCU Test Suite

Suite de tests complète pour les fonctions `throttle.c` et `utils.c` du projet VCU (Vehicle Control Unit).

## Structure du Projet

```
VCU_test/
├── Makefile              # Makefile pour compiler et exécuter les tests
├── README.md             # Ce fichier
├── include/              # Headers de test
│   ├── test_helpers.h    # Macros et structures pour les tests
│   ├── console.h         # Mock de console pour les tests
│   └── FreeRTOS.h        # Mock de FreeRTOS pour les tests
├── src/                  # Sources de test
│   ├── test_helpers.c    # Implémentation des helpers de test
│   ├── test_throttle.c   # Tests pour throttle.c
│   └── test_utils.c      # Tests pour utils.c
└── build/                # Répertoire de build (généré)
    ├── test_throttle     # Exécutable de test throttle
    └── test_utils        # Exécutable de test utils
```

## Fonctionnalités Testées

### Tests de throttle.c

1. **CheckAndLimitRange()**
   - Valeurs dans la plage valide
   - Valeurs en dehors de la plage (trop hautes/basses)
   - Valeurs avec POT_SLACK
   - Potentiomètres inversés

2. **NormalizeThrottle()**
   - Normalisation de 0% à 100%
   - Indices invalides
   - Division par zéro (potmin == potmax)

3. **CalcThrottle()**
   - Direction neutre
   - Freinage avec régénération (différentes vitesses)
   - Accélération normale
   - Zone morte (deadzone)
   - Calcul de couple en fonction de la vitesse

4. **RampThrottle()**
   - Rampe montante
   - Rampe descendante
   - Rampe de régénération
   - Limitation aux valeurs max/min

5. **SpeedLimitCommand()**
   - Limitation en fonction de la vitesse
   - Comportement avec throttle négatif
   - Comportement au-dessus de la limite

6. **TemperatureDerate()**
   - Température normale (pas de limitation)
   - Température élevée (limitation à 50%)
   - Surchauffe (limitation à 0%)
   - Throttle négatif avec température élevée

### Tests de utils.c

1. **GetUserThrottleCommand()**
   - Les deux throttles valides et correspondants
   - Différence entre throttles (<10%)
   - Mode "limp" (différence >10%)
   - Un throttle hors plage
   - Les deux throttles hors plage
   - Direction neutre/park

2. **calculateSpeed()**
   - Moyenne des 4 roues
   - Vitesses différentes
   - Vitesse zéro

3. **checkMessageTimeStamps()**
   - Timeouts APPS (capteurs de pédale)
   - Timeouts roues
   - Timeouts pédale de frein
   - Drapeaux de derate

4. **Fonctions inline (change, changeFloat)**
   - Mapping de plages de valeurs
   - Cas limites (min/max)
   - Plages négatives

## Utilisation

### Compilation

Pour compiler tous les tests :
```bash
make
```

Ou pour compiler un test spécifique :
```bash
make build/test_throttle
make build/test_utils
```

### Exécution des Tests

Pour exécuter tous les tests :
```bash
make test
```

Pour exécuter uniquement les tests throttle :
```bash
make test-throttle
```

Pour exécuter uniquement les tests utils :
```bash
make test-utils
```

### Nettoyage

Pour nettoyer les fichiers compilés :
```bash
make clean
```

### Aide

Pour afficher l'aide :
```bash
make help
```

## Format des Résultats

Les tests affichent des résultats colorés pour une meilleure lisibilité :
- ✓ en **vert** : Test réussi
- ✗ en **rouge** : Test échoué
- Les résumés sont affichés en **jaune**

Exemple de sortie :
```
═══════════════════════════════════════
  THROTTLE FUNCTIONS TEST SUITE
═══════════════════════════════════════

▶ Testing CheckAndLimitRange
  ✓ Value 2500 should be within range [1000, 4000]
  ✓ Value should not change when within range
  ✗ Value 700 should be out of range (expected: 0, got: 1)
  ...

═══════════════════════════════════════
SUMMARY:
  Total tests: 45
  Passed: 43
  Failed: 2
═══════════════════════════════════════
```

## Structures de Test Dummy

Le fichier `test_helpers.c` fournit des fonctions pour créer facilement des structures de test :

### Création de structures
```c
MotorControlState_t motorState = create_dummy_motor_state();
GlobalState_t globalState = create_dummy_global_state();
```

### Modification des valeurs
```c
// Définir les valeurs APPS
set_apps_values(&motorState, 2500, 2500, timestamp1, timestamp2);

// Définir les vitesses de roues
set_wheel_speeds(&motorState, 1000, 1100, 900, 1000, timestamp);

// Définir l'état de la pédale de frein
set_brake_pedal(&motorState, true, timestamp);
```

## Macros de Test

### Assertions de base
```c
TEST_ASSERT(condition, "message");
TEST_ASSERT_EQUAL(expected, actual, "message");
TEST_ASSERT_FLOAT_EQUAL(expected, actual, tolerance, "message");
```

### Organisation
```c
TEST_SECTION("Nom de la section");
TEST_SUMMARY(results);
```

## Cas de Test Couverts

### Cas Limites
- Valeurs minimales et maximales
- Division par zéro évitée
- Indices invalides
- Overflow/underflow

### Cas Nominaux
- Fonctionnement normal
- Valeurs typiques
- Séquences d'opérations courantes

### Cas d'Erreur
- Capteurs défaillants
- Messages CAN timeouts
- Différences entre throttles duaux
- Conditions de températures extrêmes

### Cas de Sécurité
- Mode "limp" (mode dégradé)
- Derate de température
- Limitation de vitesse
- Timeouts de messages

## Dépendances

- GCC (compilateur C)
- Make
- Bibliothèque mathématique standard (-lm)

## Notes de Développement

### Mocks
Les fichiers suivants sont des mocks pour permettre les tests sans FreeRTOS :
- `include/FreeRTOS.h` : Mock de FreeRTOS (définition de TickType_t)
- `include/console.h` : Mock de console_print()

### Extensions Possibles
Pour étendre les tests, vous pouvez :
1. Ajouter de nouveaux fichiers de test dans `src/`
2. Créer de nouvelles fonctions helper dans `test_helpers.c`
3. Ajouter les nouveaux exécutables dans le Makefile
4. Utiliser les macros existantes pour créer des tests cohérents

### Conventions de Code
- Préfixe `test_` pour toutes les fonctions de test
- Préfixe `create_dummy_` pour les fonctions de création de structures
- Préfixe `init_` pour les fonctions d'initialisation

## Troubleshooting

### Erreurs de compilation
Si vous rencontrez des erreurs de chemins :
- Vérifiez que `../VCU_app/src/` et `../VCU_app/include/` existent
- Ajustez les chemins dans le Makefile si nécessaire

### Tests échouant
- Vérifiez les valeurs de tolérance pour les comparaisons flottantes
- Assurez-vous que les paramètres sont initialisés correctement
- Activez `console_print()` dans `include/console.h` pour debug

### Warnings du compilateur
Les warnings `-Wall -Wextra` sont activés pour détecter les problèmes potentiels.
Ils doivent tous être résolus avant de considérer les tests comme complets.

## Contact et Contribution

Pour toute question ou amélioration, n'hésitez pas à :
- Ajouter de nouveaux cas de test
- Améliorer la couverture de code
- Optimiser les performances
- Corriger les bugs découverts

---

**Version:** 1.0  
**Date:** 2026-03-05  
**Auteur:** VCU Development Team
