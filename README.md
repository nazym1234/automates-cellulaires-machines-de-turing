✅ Résumé de mon projet de métasimulation
🧠 Objectif général
Ce projet consistait à simuler des automates cellulaires unidimensionnels puis à démontrer leur capacité à simuler une machine de Turing, conformément au théorème d’universalité.

📌 Étapes réalisées
1. Automate cellulaire
J’ai conçu une structure de données pour représenter un automate cellulaire avec des états personnalisables.

J’ai modélisé les configurations comme des listes d’états à un instant donné.

Lecture automatique d’un fichier décrivant un automate + mot d’entrée → initialisation des structures.

Implémentation de la fonction de transition pour faire évoluer une configuration en un pas.

Simulation complète avec plusieurs modes d’arrêt :

Nombre fixe d’itérations

Stabilisation (plus de changements)

Transition cible atteinte

Affichage lisible des configurations à chaque pas (console).

2. Automates cellulaires personnalisés
J’ai créé 4 automates aux comportements intéressants :

Un qui fait croître la configuration infiniment.

Un autre avec valeurs cycliques sur toutes les cases.

Deux automates inventés aux comportements visuellement dynamiques, testés via la simulation.

3. Machine de Turing
Définition d’une structure de données pour représenter une machine de Turing.

Représentation des configurations de la bande et de la tête de lecture.

Lecture d’un fichier de transitions + mot d’entrée → initialisation complète.

Fonction de calcul pas-à-pas.

Simulation complète jusqu’à acceptation ou rejet.

4. Simulation d’une machine de Turing par un automate cellulaire
Implémentation du simulateur d’automate cellulaire universel.

Chaque cellule encode un couple (état, symbole).

Codage des transitions de la machine de Turing sous forme de règles locales pour l’automate.

Vérification sur plusieurs exemples que le comportement est fidèle.

5. Réflexion théorique
Réponse à la question : le problème HALTING-CELLULAR-AUTOMATON est indécidable, par analogie avec l’arrêt d’une machine de Turing.

⚙️ Aspects pratiques
Le projet est exécutable avec make, qui lance l’ensemble des simulations ou les tests séparément.

Un README décrit clairement l’usage des fichiers et les commandes.

Code réalisé en C, bien structuré, modulaire, et documenté.


//reponse a une auestions theorique: 
Le problème HALTING-CELLULAR-AUTOMATON est indécidable. 

En effet, il consiste à décider si, un moment dans un automate cellulaire A sur une entrée w, une cellule prendra un jour la valeur s ∈ S. 

On a montre ( en cours) que les automates cellulaires simulent les machines Turing : ils peuvent simuler n’importe quelle machine de Turing. On peut donc coder une machine de Turing M dans un automate cellulaire A de manière à ce que l’apparition d’un certain symbole s corresponde à l'arrêt ou à un certain état de M. 

Ce problème est equivalent à détecter si une machine de Turing atteint un certain état ou s’arrête (problème de l’arrêt) ce qui est un problème connu pour être indécidable ( cours)



Conclusion : HALTING-CELLULAR-AUTOMATON est un problème indécidable.