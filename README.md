# ft_irc

**ft_irc** est un projet de l'école **42** visant à implémenter un **serveur IRC** conforme à la **RFC 1459**.  
L'objectif est d'apprendre la programmation réseau bas niveau, la gestion de clients multiples et l'implémentation d'un protocole de communication en temps réel.

## Fonctionnalités

- Connexion de clients via TCP
- Commandes principales : `PASS`, `NICK`, `USER`, `JOIN`, `PART`, `PRIVMSG`
- Gestion des channels, des modes et des erreurs
- Communication en temps réel entre utilisateurs

## Usage

```bash
make
./ircserv <port> <password>
