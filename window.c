/*!\file window.c
 * \brief Utilisation du raster DIY comme pipeline de rendu 3D. Cet
 * exemple montre les géométries disponibles et quelques
 * transformations dessus.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 16, 2021.
 * \todo exercice intéressant à faire : changer la variation de
 * l'angle de rotation pour qu'il soit dépendant du temps et non du
 * framerate
 */
#include <assert.h>
/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* inclure la bibliothèque de rendu DIY */
#include "rasterize.h"

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* protos de fonctions locales (static) */
static void init(void);
static void draw(void);
static void key(int keycode);
static void sortie(void);

/*!\brief une surface représentant un quadrilatère */
static surface_t *_brick = NULL;
/*!\brief une surface représentant un cube */
static surface_t *_wall = NULL;
/*!\brief une surface représentant une sphere */
static surface_t *_balle = NULL;

static surface_t *_raquette = NULL;

static surface_t *_sol = NULL;


/* des variable d'états pour activer/désactiver des options de rendu */
static int _use_tex = 1, _use_color = 1, _use_lighting = 1;

/*!\brief on peut bouger la caméra vers le haut et vers le bas avec cette variable */
static float _ycam = 30.0f; // 3.0 de base
/* Plateau */
static int _plateau[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

//hauteu du plateau
static int _H = 19;
//largeur du plateau
static int _W = 15;

// Position de départ de ma balle
static vec3 _ballePosition = {0, 6, 0.0f};
// Je positionne la raquette en dessous de la position de départ de ma balle
static vec3 _raquettePosition = {0, 13, 0.0f};
// Vitesse de la balle
static vec2 _vitesseBalle = {0, 0};

void game() {
  /* on va récupérer le delta-temps */
  static double t0 = 0.0; // le temps à la frame précédente
  double t, dt;
  t = gl4dGetElapsedTime();
  dt = (t - t0) / 1000.0; // diviser par mille pour avoir des secondes
  // pour le frame d'après, je mets à jour t0
  t0 = t;

  //Physique Netwon (souvenir de Godot)
  _ballePosition.x += _vitesseBalle.x * dt;
  _ballePosition.y += _vitesseBalle.y * dt;


  float md =  _W - 1.5f;
  float mg = -_W - 1.0f;
  float mb = _H - 9.0f;
  float mh = - _H - 7.0f;

  if (_ballePosition.y <= mh) {

    _vitesseBalle.y = -_vitesseBalle.y;
    _ballePosition.x += 0.2f;
  } else if(_ballePosition.x >= md) {
    _vitesseBalle.x = -_vitesseBalle.x;
    _ballePosition.y -= 0.2f;
  } else if (_ballePosition.x <= mg)
  {
    _vitesseBalle.x = -_vitesseBalle.x;
    _ballePosition.y -= 0.2f;
  }
   else if (_ballePosition.y >= mb) {
    _ballePosition.x = 0.0;
    _ballePosition.y = 6.0;
    _vitesseBalle.y = 0.0;
    _vitesseBalle.x = 0.0;
  }

  //délimitation du dessus de la raquette
  // if (_ballePosition.y >= _raquettePosition.y-6.5f)
  // {
  //   printf("Here, on touche !");
  // }

  //délimitation du coté droit de la raquette
  // if (_ballePosition.x <= _raquettePosition.x + 2.0f)
  // {
  //   printf("Here, on touche !");
  // }
  
  //délimitation du côté gauche de la raquette
  // if (_ballePosition.x >= _raquettePosition.x - 2.0f)
  // {
  //   printf("Here, on touche !");
  // }

  //J'ai décomposé au-dessus les différentes conditions pour délimiter ma raquette
  if (_ballePosition.y >= _raquettePosition.y-6.5f && _ballePosition.x <= _raquettePosition.x + 3.5f && _ballePosition.x >= _raquettePosition.x - 4.5f)
  {
    _vitesseBalle.y = -_vitesseBalle.y;
  }
  

}


/*!\brief paramètre l'application et lance la boucle infinie. */
int main(int argc, char **argv)
{
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if (!gl4duwCreateWindow(argc, argv,           /* args du programme */
                          "The DIY Rasterizer", /* titre */
                          10, 10, 800, 600,     /* x, y, largeur, heuteur */
                          GL4DW_SHOWN) /* état visible */)
  {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  init();
  /* mettre en place la fonction d'interception clavier */
  gl4duwKeyDownFunc(key);

  gl4duwIdleFunc(game);
  /* mettre en place la fonction de display */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/*!\brief init de nos données, spécialement les trois surfaces
 * utilisées dans ce code */
void init(void)
{
  GLuint id_ball;
  GLuint id_brick;
  GLuint id_wall;
  GLuint id;

  vec4 r = {1, 0, 0, 1}, g = {0, 1, 0, 1}, b = {0, 0, 1, 1};

  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre.  IMPORTANT de
   * créer le screen avant d'utiliser les fonctions liées au
   * textures */
  gl4dpInitScreen();
  /* Pour forcer la désactivation de la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);

  /* on créé nos trois type de surfaces */
  _brick = mk_cube();           /* ça fait 2x6 triangles        */
  _wall = mk_cube();           /* ça fait 2x6 triangles      */
  _balle = mk_sphere(12, 12); /* ça fait 12x12x2 trianles ! */
  _raquette = mk_cube();       /* ça fait 2x6 triangles      */
  _sol = mk_cube();

  /* on change les couleurs de surfaces */
  _brick->dcolor = b;
  _wall->dcolor = b;
  _balle->dcolor = g;  //Balle en verte
  _raquette->dcolor = r; //Raquette en rouge pour l'identifier
  _sol->dcolor = b;

  _vitesseBalle.x = 0.0f;
  _vitesseBalle.y = 0.0f;

  id_wall = get_texture_from_BMP("images/texture_wall.bmp");
  id_ball = get_texture_from_BMP("images/balle_texture.bmp");
  id_brick = get_texture_from_BMP("images/brique_Texture.bmp");

  /* on leur rajoute à toutes la même texture */
  set_texture_id(_wall, id_wall);
  set_texture_id(_brick, id_brick);
  set_texture_id(_balle, id_ball);
  set_texture_id(_raquette, id_wall);
  set_texture_id(_sol, id_wall);

  /* si _use_tex != 0, on active l'utilisation de la texture pour les
   * trois */
  if (_use_tex)
  {
    enable_surface_option(_brick, SO_USE_TEXTURE);
    enable_surface_option(_wall, SO_USE_TEXTURE);
    enable_surface_option(_balle, SO_USE_TEXTURE);
    enable_surface_option(_raquette, SO_USE_TEXTURE);
    enable_surface_option(_sol, SO_USE_TEXTURE);
  }
  /* si _use_lighting != 0, on active l'ombrage */
  if (_use_lighting)
  {
    enable_surface_option(_brick, SO_USE_LIGHTING);
    enable_surface_option(_wall, SO_USE_LIGHTING);
    enable_surface_option(_balle, SO_USE_LIGHTING);
    enable_surface_option(_raquette, SO_USE_LIGHTING);
    enable_surface_option(_sol, SO_USE_LIGHTING);
  }
  /* on désactive le back cull face pour le quadrilatère, ainsi on
   * peut voir son arrière quand le lighting est inactif */
  //disable_surface_option(_brick, SO_CULL_BACKFACES);
  /* mettre en place la fonction à appeler en cas de sortie */
  atexit(sortie);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void)
{
  vec4 r = {1, 0, 0, 1}, b = {0, 0, 1, 1}, g = {0, 1, 0, 1}, y = {1, 0, 1, 1}, gris = {1, 1, 1, 0};
  static float a = 0.0f;
  float model_view_matrix[16], projection_matrix[16], nmv[16];
  /* effacer l'écran et le buffer de profondeur */
  gl4dpClearScreen();
  clear_depth_map();
  /* des macros facilitant le travail avec des matrices et des
   * vecteurs se trouvent dans la bibliothèque GL4Dummies, dans le
   * fichier gl4dm.h */
  /* charger un frustum dans projection_matrix */
  MFRUSTUM(projection_matrix, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 1000.0f);
  /* charger la matrice identité dans model-view */
  MIDENTITY(model_view_matrix);
  /* on positionne la caméra en arrière-haut, elle regarde le centre de la scène */
  //model_view_matrix, x, y, z, etc....)
  lookAt(model_view_matrix, 0, _ycam, 25, 0, 0, 0, 0, 0, -1);
  // lookAt(model_view_matrix, 0, _ycam, 10, 0, 0, 0, 0, 1, 0);
  /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  // memcpy(nmv, model_view_matrix, sizeof nmv); /* copie model_view_matrix dans nmv */

  // translate(nmv, -3.0f, 0.0f, 0.0f);
  // rotate(nmv, a, 1.0f, 0.0f, 0.0f);
  // transform_n_rasterize(_brick, nmv, projection_matrix);
  /* le cube est mis à droite et tourne autour de son axe z */

  _brick->dcolor = gris;
  _wall->dcolor = gris;
  _balle->dcolor = g;
  _sol->dcolor = b;

  float cX = -_W * 2.0f / 2;
  float cZ = -_H * 2.0f / 2;
  for (int i = 0; i < _H; ++i)
  {
    for (int j = 0; j < _W; ++j)
    {
      if (_plateau[i * _W + j] == 1)
      {
        memcpy(nmv, model_view_matrix, sizeof nmv); /* copie model_view_matrix dans nmv */
        translate(nmv, 2 * j + cX, 0.0f, 2 * i + cZ);
        // rotate(nmv, a, 0.0f, 0.0f, 1.0f);
        transform_n_rasterize(_wall, nmv, projection_matrix);
      } else if(_plateau[i * _W + j] == 2) {
        memcpy(nmv, model_view_matrix, sizeof nmv); /* copie model_view_matrix dans nmv */
        translate(nmv, 2 * j + cX, -1.0f, 2 * i + cZ);
        // rotate(nmv, a, 0.0f, 0.0f, 1.0f);
        transform_n_rasterize(_brick, nmv, projection_matrix);
      }
    }
  }



  // balle du casse brique
  memcpy(nmv, model_view_matrix, sizeof nmv); /* copie model_view_matrix dans nmv */
  translate(nmv, _ballePosition.x, -8.0f, _ballePosition.y);
  rotate(nmv, a, 0.0f, 1.0f, 0.0f);
  transform_n_rasterize(_balle, nmv, projection_matrix);

  // raquette du casse brique (J'ai un grand rectangle décomposer en 2 petits)
  memcpy(nmv, model_view_matrix, sizeof nmv);
  translate(nmv, _raquettePosition.x -1 , 1.0f, _raquettePosition.y);
  transform_n_rasterize(_raquette ,nmv, projection_matrix);

  memcpy(nmv, model_view_matrix, sizeof nmv);
  translate(nmv, _raquettePosition.x + 1 , 1.0f, _raquettePosition.y);
  transform_n_rasterize(_raquette ,nmv, projection_matrix);

  /* déclarer qu'on a changé des pixels du screen (en bas niveau) */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  a += 0.1f;
}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode)
{
  if(keycode == GL4DK_e) {
    if (_raquettePosition.x <= _W - 6) {   
      _raquettePosition.x += 0.7;
    }
  }
  if (keycode == GL4DK_a){
    if(_raquettePosition.x >= -_W + 4) {
      _raquettePosition.x -= 0.7;
    }
  }
  if (keycode == GL4DK_SPACE) {
    _vitesseBalle.x = -15.0f;
    _vitesseBalle.y = -15.0f;
  }

  switch (keycode)
  {
  case GL4DK_UP:
    _ycam += 0.05f;
    break;
  case GL4DK_DOWN:
    _ycam -= 0.05f;
    break;
  case GL4DK_t: /* 't' la texture */
    _use_tex = !_use_tex;
    if (_use_tex)
    {
      enable_surface_option(_brick, SO_USE_TEXTURE);
      enable_surface_option(_wall, SO_USE_TEXTURE);
      enable_surface_option(_balle, SO_USE_TEXTURE);
    }
    else
    {
      disable_surface_option(_brick, SO_USE_TEXTURE);
      disable_surface_option(_wall, SO_USE_TEXTURE);
      disable_surface_option(_balle, SO_USE_TEXTURE);
    }
    break;
  case GL4DK_c: /* 'c' utiliser la couleur */
    _use_color = !_use_color;
    if (_use_color)
    {
      enable_surface_option(_brick, SO_USE_COLOR);
      enable_surface_option(_wall, SO_USE_COLOR);
      enable_surface_option(_balle, SO_USE_COLOR);
    }
    else
    {
      disable_surface_option(_brick, SO_USE_COLOR);
      disable_surface_option(_wall, SO_USE_COLOR);
      disable_surface_option(_balle, SO_USE_COLOR);
    }
    break;
  case GL4DK_l: /* 'l' utiliser l'ombrage par la méthode Gouraud */
    _use_lighting = !_use_lighting;
    if (_use_lighting)
    {
      enable_surface_option(_brick, SO_USE_LIGHTING);
      enable_surface_option(_wall, SO_USE_LIGHTING);
      enable_surface_option(_balle, SO_USE_LIGHTING);
    }
    else
    {
      disable_surface_option(_brick, SO_USE_LIGHTING);
      disable_surface_option(_wall, SO_USE_LIGHTING);
      disable_surface_option(_balle, SO_USE_LIGHTING);
    }
    break;
  default:
    break;
  }
  
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void)
{
  /* on libère nos trois surfaces */
  if (_brick)
  {
    free_surface(_brick);
    _brick = NULL;
  }
  if (_wall)
  {
    free_surface(_wall);
    _wall = NULL;
  }
  if (_balle)
  {
    free_surface(_balle);
    _balle = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
