-- Nom de la bdd : bdd_super
-- Utilisateur de la bdd : user_super
-- Mot de passe de l'utilisateur : mdp_super

DROP DATABASE IF EXISTS bdd_super;

CREATE DATABASE IF NOT EXISTS bdd_super CHARACTER SET 'utf8' COLLATE utf8_bin;

USE bdd_super;

DROP TABLE IF EXISTS lieu;

CREATE TABLE lieu
(
   num_lieu	INT UNSIGNED	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_lieu, legende)

)ENGINE=INNODB;


DROP TABLE IF EXISTS lecteur;

CREATE TABLE lecteur
(
   num_lecteur	INT UNSIGNED		NOT NULL,
   num_lieu	INT UNSIGNED UNIQUE	NOT NULL,
   ip		VARCHAR (15) UNIQUE	NOT NULL,
   estConnecte	TINYINT UNSIGNED	NOT NULL,
   constraint estConnecte_is_bool	check(estConnecte BETWEEN 0 and 1),

   PRIMARY KEY (num_lecteur)

)ENGINE=INNODB;


DROP TABLE IF EXISTS vue;

CREATE TABLE vue
(
   num_vue	INT UNSIGNED	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_vue)
   
)ENGINE=INNODB;


DROP TABLE IF EXISTS representationLieuSurVue;

CREATE TABLE representationLieuSurVue
(
   num_vue	INT UNSIGNED	NOT NULL,
   num_lieu	INT UNSIGNED	NOT NULL,
   x		INT		NOT NULL,
   y		INT		NOT NULL,
   
   PRIMARY KEY (num_vue, num_lieu)
   
)ENGINE=INNODB;

-- Interdiction de supprimer un lieu auquel un lecteur est lié
-- Un lieu modifié est aussi modifié dans lecteur
alter table lecteur add constraint fk_lecteur_num_lieu foreign key (num_lieu)
      references lieu (num_lieu) on delete restrict on update cascade;

-- La suppression d'une vue supprime toutes les lignes concernant cette vue
alter table representationLieuSurVue add constraint fk_representationLieuSurVue_num_vue foreign key (num_vue)
      references vue (num_vue) on delete cascade on update cascade;

-- La suppression d'un lieu entraine la perte d'un lieu dans une vue qui l'affichait
alter table representationLieuSurVue add constraint fk_representationLieuSurVue_num_lieu foreign key (num_lieu)
      references lieu (num_lieu) on delete cascade on update cascade;


INSERT INTO lieu (num_lieu, legende) VALUE
  (1, "Entrée tunnel zone 1"),
  (2, "Angle tunnel zone 2"),
  (3, "Dans tunnel zone 3, en face des dépendances");


INSERT INTO lecteur (num_lecteur, num_lieu, ip, estConnecte) VALUE
  (1, 1, '127.0.0.1', 0),
  (2, 2, '127.0.0.2', 0),
  (3, 3, '127.0.0.3', 0);


INSERT INTO vue (num_vue, legende) VALUE
  (1, "Vue globale du site représentant tous les lieux"),
  (2, "Vue zoomant sur les secteurs 1, 2 et 3, de l'entrée aux dépendances.");


INSERT INTO representationLieuSurVue (num_vue, num_lieu, x, y) VALUE
  (1, 1, 0, 0),
  (1, 2, 0, 0),
  (1, 3, 0, 0),
  (2, 1, 0, 0),
  (2, 2, 0, 0),
  (2, 3, 0, 0);

