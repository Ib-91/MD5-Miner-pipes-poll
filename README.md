# MD5-Miner-pipes-poll

Le programme étend le mineur MD5 multi-processus avec la communication inter-processus. Les processus enfants recherchent en parallèles des nonces valides et transmettent leurs résultats au processus parent via des pipes. Le parent surveille simultanément l’ensemble des pipes à l’aide de poll, affiche les cinq premiers nonces reçus, puis interrompt l’exécution de tous les processus enfants.
