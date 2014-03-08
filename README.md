The Call of Duty extension *libcod* is adding new server-side functions to:

 - Call Of Duty 2 1.2
 - Call Of Duty 2 1.3
 - Call Of Duty 4 1.7

Requirements:
```
dpkg --add-architecture i386
apt-get update
apt-get install gcc-multilib
apt-get install libmysqlclient-dev:i386
apt-get install g++-multilib
```

Precompiled shared libraries: http://killtube.org/downloads/libcod/

Starting the server:

```LD_PRELOAD=libcod2_1_3_nomysql.so ./cod2_lnxded +set fs_game ...```

Some enviroments need LD_LIBRARY_PATH also:

```LD_LIBRARY_PATH=. LD_PRELOAD=libcod2_1_3_nomysql.so ./cod2_lnxded +set fs_game ...```
	
Working with the source / Compiling:
```
./doit.sh tar
./doit.sh base # compiles object files needed by every .so
./doit.sh cod1_1_5 # compiles object files for CoD 1 1.5 and linking them against base to the actual bin/libcod1_1_5.so
./doit.sh cod2_1_2
./doit.sh cod2_1_3
./doit.sh cod4_1_7
./doit.sh wrapper
```

Mods depending on *libcod*:
- http://killtube.org/showthread.php?1533-DOWNLOAD-CoD2-Surf-Mod
- http://killtube.org/showthread.php?1527-DOWNLOAD-CoD2-Basetdm-Mod
- http://killtube.org/showthread.php?1593-DOWNLOAD-CoD2-Portal-Mod
- Jump-Mod by IzNoGod
	
Little overview of added functions:
- MySQL
- setVelocity, getVelocity, addVelocity (needed for the mods: portal, surf)
- keyPressed-functions for left, right, forward, backward, leanleft, leanright, jump etc., (needed for: surf, doublejump made by IzNoGod)
- blazing fast astar-implementation (needed for: zombots)
- setAlive-function (needed for: zombots, so xmodels are damagable without damage-trigger... zombots in stockmaps)
- disableGlobalPlayerCollision() 
	
Community / Help: http://killtube.org/forum.php

Porting *libcod* to a new Call Of Duty:
```find . -name '*.c*' | xargs grep -n _VERSION```