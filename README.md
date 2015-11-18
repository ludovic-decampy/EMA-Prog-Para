Programmation parallèle [github](https://github.com/ludovic-decampy/EMA-Prog-Para)
===================
Groupe de travail
-------------
* Julien Chavance
* Ludovic Decampy
* Camille Lescalliez 

Compilation
-------------
Pour compiler et lancer le projet à la volée : 
```
# Un bash suffit pour un petit projet 
compile.sh
```
Execution
-------------
Le programme envoit sur **stdout** le code HTML de sorties.  
Pour voir le rendu de la sortie, pensez à rediriger la sortie vers un fichier **.html**
```
mpirun -np 1 ./Master > out.html
```

