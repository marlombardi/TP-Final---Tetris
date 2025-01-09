/***************************************************************************//**
  @file     front-pc.c
  @brief    Frontend del TPF Tetris, para PC
  @author   Augusto Milani
 ******************************************************************************/

#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

typedef struct {
		ALLEGRO_EVENT event;			// Evento que ocurrió (ver switch).
		ALLEGRO_KEYBOARD_STATE ks;		// Registro del teclado.
		ALLEGRO_EVENT_QUEUE *queue;		// Registro de eventos.
		ALLEGRO_TIMER *timer;
		ALLEGRO_FONT *font;
		ALLEGRO_BITMAP *title;
		ALLEGRO_BITMAP *background;
		ALLEGRO_BITMAP *blocks;
		bool flagMenu;						// Bandera para menú principal y pausa.
		bool flagPlay;						// Bandera para terminar programa.
		bool redraw;					// Bandera para dibujar.
	} argument_t;

static int TITLE_WIDTH, TITLE_HEIGHT, BACK_WIDTH, BACK_HEIGHT, TILE_WIDTH, TILE_HEIGHT;
static int dx, dy, move;	//Centrar imagen en pantalla.
static float scale;
static argument_t argument;

#define FILS 20
#define COLS 10
int matrix[FILS][COLS];

static void check_init(void *pointer, const char *name);
static void must_init (bool test, const char *description);

static void TetrisMenu();
static void TetrisPlay();
static void TetrisPause();

static void menuDraw();
static void playDraw();
static void pauseDraw(int index);

/*
typedef struct {
	char matrix[16][10];	// Posición de las piezas.
	char next;		// Siguiente pieza.
	char level;		// Nivel de avance en el juego, cambia el color de las piezas.
	int score; 	//Puntaje acumulado por nivel y líneas completadas.
} info_t; */



#define TITLE_FILE 		"title.png"
#define BACKGROUND_FILE "background.png"
#define BLOCKS_FILE 	"blocks.png"

int main() {
	srand(time(NULL));	//Semilla para función rand().

	/* Inicializo Allegro */
	must_init(al_init(), "allegro");
	must_init(al_install_keyboard(), "keyboard");
	must_init(al_install_mouse(), "mouse");
	must_init(al_init_font_addon(), "font");
	must_init(al_init_ttf_addon(), "ttf");
	must_init(al_init_image_addon(), "image addon");



	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ALLEGRO_DISPLAY *display = al_create_display(100, 100);
	check_init(display, "display");


	argument.font = al_load_font("nintendo-nes-font.ttf", 24, 0);
	check_init(argument.font, "tetris font TTF");

	argument.background = al_load_bitmap(BACKGROUND_FILE);
	check_init(argument.background, "background image");
	argument.title = al_load_bitmap(TITLE_FILE);
	check_init(argument.title, "title image");
	argument.blocks = al_load_bitmap(BLOCKS_FILE);
	check_init(argument.blocks, "blocks image");

	argument.timer = al_create_timer(1.0);
	check_init(argument.timer, "timer");
	argument.queue = al_create_event_queue();
	check_init(argument.queue, "queue");


	al_register_event_source(argument.queue, al_get_keyboard_event_source());
	al_register_event_source(argument.queue, al_get_display_event_source(display));
	al_register_event_source(argument.queue, al_get_mouse_event_source());

	int SCREEN_WIDTH = al_get_display_width(display);
	int SCREEN_HEIGHT = al_get_display_height(display);

	TITLE_WIDTH = al_get_bitmap_width(argument.title);
	TITLE_HEIGHT = al_get_bitmap_height(argument.title);

	BACK_WIDTH = al_get_bitmap_width(argument.background);
	BACK_HEIGHT = al_get_bitmap_height(argument.background);

	TILE_WIDTH = al_get_bitmap_width(argument.blocks)/4;
	TILE_HEIGHT = al_get_bitmap_height(argument.blocks)/10;

	float scale_x = (float)SCREEN_WIDTH / BACK_HEIGHT;
	float scale_y = (float)SCREEN_HEIGHT / BACK_WIDTH;
	if(scale_x < scale_y) {
		scale = scale_x;
	}
	else {
		scale = scale_y;
	}

	dy = (SCREEN_HEIGHT - BACK_HEIGHT * scale) / 2;	//Centro la imagen verticalmente
	dx = (SCREEN_WIDTH - BACK_WIDTH * scale) / 2;		//Centro la imagen horizontalmente

	move = TILE_WIDTH*scale;

	al_start_timer(argument.timer);

	int i, j;
	for(i=0 ; i<FILS ; i++) {
		for(j=0 ; j<COLS ; j++) {
			matrix[i][j] = 1;
		}
	}

	/* INICIA JUEGO */
	TetrisMenu();

	/* Finalización del Programa */
	al_destroy_font(argument.font);
	al_destroy_display(display);
	al_destroy_timer(argument.timer);
	al_destroy_event_queue(argument.queue);

	return 0;
}

/* Reviso si Allegro se inicializó correctamente */
static void must_init(bool test, const char *description) {
    if(test) return;

    printf("couldn't initialize %s.\n", description);
    exit(1);
}

/* Chequeo si los punteros son NULL */
static void check_init(void *pointer, const char *name) {
    if (!pointer) {
        fprintf(stderr, "couldn't iniatialize %s.\n", name);
        exit(-1);
    }
}

static void TetrisMenu() {
	menuDraw();
	argument.flagMenu = false;

	while( !(argument.flagMenu) ) {	//Mientras esté en pausa, no empieza.

		al_wait_for_event(argument.queue, &(argument.event));
		switch ((argument.event).type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				argument.flagMenu = true;	// Al cerrar el display, termina el programa.
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				TetrisPlay();
				break;

			case ALLEGRO_EVENT_KEY_CHAR:
				al_get_keyboard_state(&(argument.ks));

				if(al_key_down(&(argument.ks), ALLEGRO_KEY_ESCAPE)) {
					argument.flagMenu = true;	//Al presionar Esc, se detiene el programa.
				}
				else {
					TetrisPlay();
				}
				break;

			default:
				break;
		}
		/* Rutina de Impresión */
		if(argument.redraw && !(argument.flagMenu) && al_is_event_queue_empty(argument.queue)) {
			argument.redraw = false;
			menuDraw();
		}
	}
}

static void TetrisPlay() {
	playDraw();
	argument.flagPlay = false;

	while( !(argument.flagPlay) ) {

		al_wait_for_event(argument.queue, &(argument.event));
		switch ((argument.event).type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				argument.flagPlay = true;	// Al cerrar el display, termina el programa.
				break;

			case ALLEGRO_EVENT_TIMER:
				argument.redraw = true;
				//TODO backend acá (pieza cae)**************
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if((argument.event).mouse.button == 1) {	// Click izquierdo.
					//TODO backend (pieza gira izq)***************
				}
				else if((argument.event).mouse.button == 2) {	//Click derecho.
					//TODO backend (pieza gira derecha)***************
				}
				break;

			case ALLEGRO_EVENT_KEY_CHAR:
				al_get_keyboard_state(&(argument.ks));

				if(al_key_down(&(argument.ks), ALLEGRO_KEY_ESCAPE)) {
					TetrisPause();
				}

				else {
					if(al_key_down(&(argument.ks), ALLEGRO_KEY_LEFT)) {
						argument.redraw = true;
						//TODO agregar backend acá*****************
					}
					else if(al_key_down(&(argument.ks), ALLEGRO_KEY_RIGHT)) {
						argument.redraw = true;
						//TODO agregar backend acá*****************
					}
					else if(al_key_down(&(argument.ks), ALLEGRO_KEY_DOWN)) {
						argument.redraw = true;
						//TODO agregar backend acá*****************
					}
				}
				break;

			default:
				break;
		}

		/* Rutina de Impresión */
		if(argument.redraw && !(argument.flagPlay) && al_is_event_queue_empty(argument.queue)) {
			argument.redraw = false;
			playDraw();
		}
	}
}

static void TetrisPause() {
	pauseDraw(0);

	int index = 0;
	pauseDraw(index);

	al_flip_display();

	bool resume = false;
	while(!resume  ) {

		al_wait_for_event(argument.queue, &(argument.event));
		switch ((argument.event).type) {

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				argument.flagPlay = true;	// Al cerrar el display, termina el programa.
				argument.flagMenu = true;
				break;

			case ALLEGRO_EVENT_KEY_CHAR:
				al_get_keyboard_state(&(argument.ks));

				if(al_key_down(&(argument.ks), ALLEGRO_KEY_ESCAPE)) {
					resume = true;
				}
				else if(al_key_down(&(argument.ks), ALLEGRO_KEY_UP)) {
					argument.redraw = true;
					index--;
				}
				else if(al_key_down(&(argument.ks), ALLEGRO_KEY_DOWN)) {
					argument.redraw = true;
					index++;
				}
				else if(al_key_down(&(argument.ks), ALLEGRO_KEY_SPACE)) {
					resume = true;
					switch(index) {

						case 0:	//CONTINUE
							argument.redraw = true;
							break;					//Continúa el juego.

						case 1:	//RESTART
							argument.redraw = true;
							//TODO backend acá?? ni idea	//Reinicia el juego
							break;

						case 2:	//EXIT
							argument.flagPlay = true;	//Vuelve al menú.
							argument.redraw = true;
							break;

						case 3:	//CLOSE
							argument.flagPlay = true;
							argument.flagMenu = true;
							break;

						default:
							break;
					}
				}
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				argument.flagPlay = true;
				break;

			default:
				break;
		}
		if(argument.redraw && !resume && al_is_event_queue_empty(argument.queue)) {
			if(index < 0) {
				index = 3;
			}
			else if(index > 3) {
				index = 0;
			}

			pauseDraw(index);
			argument.redraw = false;
		}
	}
}







/* Imprimir el Menú del Juego */
static void menuDraw() {
	al_clear_to_color(al_map_rgb(0, 0, 0));		// Limpio pantalla con negro.
	al_draw_scaled_bitmap(argument.title, 0, 0, TITLE_WIDTH, TITLE_HEIGHT,
						  dx, dy,
						  TITLE_WIDTH * scale, TITLE_HEIGHT * scale, 0);
	al_flip_display();
}

/* Imprimir el Juego con la Matriz */
static void playDraw() {
	al_clear_to_color(al_map_rgb(0, 0, 0));		// Limpio pantalla con negro.

	al_draw_scaled_bitmap(argument.background, 0, 0, BACK_WIDTH, BACK_HEIGHT,
								 dx, dy, BACK_WIDTH * scale, BACK_HEIGHT * scale,
								 0);
	int i, j;
	for(i=0 ; i<FILS ; i++) {
		for(j=0 ; j<COLS ; j++) {
			if(matrix[i][j]) {
				al_draw_scaled_bitmap(argument.blocks,	0, 0,	TILE_WIDTH, TILE_HEIGHT,
									 dx + move*(12+j), dy + move*(5+i),	TILE_WIDTH * scale, TILE_HEIGHT * scale,		0);
			}
		}
	}
	al_flip_display();
}


static void pauseDraw(int index) {
	int pauseSelect[4] = {255, 255, 255, 255};
	pauseSelect[index] = 0;

	al_draw_scaled_bitmap(argument.background, 0, 0, BACK_WIDTH, BACK_HEIGHT,
									 dx, dy, BACK_WIDTH * scale, BACK_HEIGHT * scale, 0);
	al_draw_text(argument.font, al_map_rgb(255, 255, 255), dx + move*17, dy +move*10, ALLEGRO_ALIGN_CENTRE, "PAUSE");
	al_draw_text(argument.font, al_map_rgb(255, 255, pauseSelect[0]), dx + move*12, dy +move*21, 0, "CONTINUE");
	al_draw_text(argument.font, al_map_rgb(255, 255, pauseSelect[1]), dx + move*12, dy +move*22, 0, "RESTART");
	al_draw_text(argument.font, al_map_rgb(255, 255, pauseSelect[2]), dx + move*12, dy +move*23, 0, "EXIT");
	al_draw_text(argument.font, al_map_rgb(255, 255, pauseSelect[3]), dx + move*12, dy +move*24, 0, "CLOSE");

	al_flip_display();
}


/* NOTAS
			 *
			 * dx y dy indican la esquina izquierda del background, por lo que me desplazo
			 * por cantidad de píxeles (26.4 aproximadamente) para cada bloque.
			 *
			 * Divido por 4.5 y 11 para ajustar con exactitud en base a la paleta de
			 * bloques en el PNG usado.
			 *
			 * La escala se ajusta de manera global una vez al principio, TODO ver
			 * con distintas resoluciones.
			 *
			 * */
