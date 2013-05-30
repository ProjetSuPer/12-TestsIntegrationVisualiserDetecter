-- Nom de la bdd : bdd_super
-- Utilisateur de la bdd : user_super
-- Mot de passe de l'utilisateur : mdp_super

DROP DATABASE IF EXISTS bdd_super;

CREATE DATABASE IF NOT EXISTS bdd_super CHARACTER SET 'utf8' COLLATE utf8_bin;

USE bdd_super;

#DROP TABLE IF EXISTS lieu;

CREATE TABLE lieu
(
   num_lieu	INT UNSIGNED	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_lieu, legende)

)ENGINE=INNODB;


#DROP TABLE IF EXISTS lecteur;

CREATE TABLE lecteur
(
   num_lecteur	INT UNSIGNED		NOT NULL,
   num_lieu	INT UNSIGNED UNIQUE	NOT NULL,
   ip		VARCHAR (15) UNIQUE	NOT NULL,
   estConnecte	TINYINT UNSIGNED	NOT NULL,
   constraint estConnecte_is_bool	check(estConnecte BETWEEN 0 and 1),

   PRIMARY KEY (num_lecteur)

)ENGINE=INNODB;


#DROP TABLE IF EXISTS vue;

CREATE TABLE vue
(
   num_vue	INT UNSIGNED	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,
   image	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_vue)
   
)ENGINE=INNODB;


#DROP TABLE IF EXISTS representationLieuSurVue;

CREATE TABLE representationLieuSurVue
(
   num_vue	INT UNSIGNED 	NOT NULL,
   num_lieu	INT UNSIGNED 	NOT NULL,
   num_zone	INT UNSIGNED 	NOT NULL,
   x		INT		NOT NULL,
   y		INT		NOT NULL,
   xA		INT		NOT NULL,
   yA		INT		NOT NULL,
   xB		INT		NOT NULL,
   yB		INT		NOT NULL,

   PRIMARY KEY (num_vue, num_lieu, num_zone)

)ENGINE=INNODB;

#DROP TABLE IF EXISTS personne;

CREATE TABLE personne
(
   num_pers	INT UNSIGNED 	NOT NULL,
   nom		VARCHAR(50)	NOT NULL,
   prenom	VARCHAR(50)	NOT NULL,
   societe	VARCHAR(50)	NOT NULL,
   dateDebut	DATE		NOT NULL,
   dateFin	DATE		NOT NULL,
   photo	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_pers)

)ENGINE=INNODB;

#DROP TABLE IF EXISTS badge;

CREATE TABLE badge
(
   num_badge	INT UNSIGNED	NOT NULL,
   num_pers	INT UNSIGNED	NOT NULL,
   dateMiseEnService	DATE	NOT NULL,
   dateChangePile	DATE	NOT NULL,
   estActif	TINYINT UNSIGNED	NOT NULL,
   constraint estActif_is_bool	check(estActif BETWEEN 0 and 1),

   PRIMARY KEY (num_badge, num_pers)

)ENGINE=INNODB;

#DROP TABLE IF EXISTS zone;

CREATE TABLE zone
(
   num_zone	INT UNSIGNED	NOT NULL,
   num_lieu	INT UNSIGNED	NOT NULL,
   sensMonter	INT UNSIGNED	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (num_zone, num_lieu)

)ENGINE=INNODB;

#DROP TABLE IF EXISTS log;

CREATE TABLE log
(
   num_badge	INT UNSIGNED	NOT NULL,
   dateLog	VARCHAR(100)	NOT NULL,
   legende	VARCHAR(100)	NOT NULL,

   PRIMARY KEY (dateLog, num_badge)

)ENGINE=INNODB;

#DROP TABLE IF EXISTS super;

CREATE TABLE super
(
   config	INT UNSIGNED	NOT NULL,
   tempoM	INT UNSIGNED	NOT NULL,
   tempoR	INT UNSIGNED	NOT NULL,

   PRIMARY KEY (config)

)ENGINE=INNODB;



-- Interdiction de supprimer un lieu auquel une zone est liée
-- Un lieu modifié est aussi modifié dans zone
alter table zone add constraint fk_zone_num_lieu foreign key (num_lieu)
      references lieu (num_lieu) on delete restrict on update cascade;

-- Interdiction de supprimer une personne auquel un badge est lié
-- Une personne modifiée est aussi modifiée dans badge
alter table badge add constraint fk_badge_num_pers foreign key (num_pers)
      references personne (num_pers) on delete restrict on update cascade;

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


INSERT INTO lieu (num_lieu, legende) VALUES
  (1, "Entree tunnel zone 1"),
  (2, "Angle tunnel zone 2"),
  (3, "Dans tunnel zone 3, en face des dependances");


INSERT INTO lecteur (num_lecteur, num_lieu, ip, estConnecte) VALUES
  (1, 1, '192.168.60.101', 0),
  (2, 2, '192.168.60.102', 0),
  (3, 3, '192.168.0.3', 0);

INSERT INTO vue (num_vue, legende, image) VALUES
  (1, "Vue globale", "ressources/vue_generale.jpg"),
  (2, "Vue zoomant de l'entree aux dependances", "ressources/vue_lieu_1-2-3.jpg");

INSERT INTO representationLieuSurVue (num_vue, num_lieu, num_zone, x, y, xA, yA, xB, yB) VALUES
  (1, 1, 1, 580, 472, 570, 530, 0, 0),
  (1, 2, 2, 540, 442, 540, 510, 0, 0),
  (1, 2, 23, 540, 442, 530, 490, 0, 0),
  (1, 3, 3, 520, 432, 480, 450, 0, 0),
  (2, 1, 1, 580, 488, 550, 530, 400, 390),
  (2, 2, 2, 220, 128, 400, 390, 200, 190),
  (2, 2, 23, 220, 128, 200, 190, 40, 190),
  (2, 3, 3, 60, 28, 40, 190, 41, 10);


INSERT INTO personne (nom, prenom, societe, dateDebut, dateFin, num_pers, photo) VALUES
  ('scherer', 'nicolas', 'LAB', '0000-00-00', '0000-00-00', 1, 'ressources/supprimer.png'),
  ('dada', 'dodo', 'didi', '2013-05-01', '2013-05-29', 2, '');


INSERT INTO badge (num_badge, num_pers, dateMiseEnService, dateChangePile, estActif) VALUES
  (01, 1, 0, 0, 0);

INSERT INTO zone (num_zone, num_lieu,sensMonter, legende) VALUES
  (1, 1, 2, "reception uniquement lecteur 1"),
  (2, 2, 1, "reception uniquement lecteur 2"),
  (23, 2, 3, "reception lecteur 2 et lecteur 3"),
  (3, 3, 1, "reception uniquement lecteur 3");

INSERT INTO log (num_badge, dateLog, legende) VALUES
  (1, 'Mon May 27 16:17:19 2013', 'Nouveau badge'),
  (1, 'Mon May 27 16:17:21 2013', 'Perte de Reception Badge'),
  (1, 'Mon May 27 16:19:02 2013', 'Nouveau badge'),
  (1, 'Mon May 27 16:19:04 2013', 'Perte de Reception Badge');

INSERT INTO super (config, tempoM, tempoR) VALUES
  (1, 5000, 2500);
