/***************************************************************************//**
  @file     front-pc.c
  @brief    Frontend del TPF Tetris, para PC
  @author   Augusto Milani
 ******************************************************************************/

#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

typedef struct {
		ALLEGRO_EVENT event;			// Evento que ocurrió (ver switch).
		ALLEGRO_KEYBOARD_STATE ks;		// Registro del teclado.
		ALLEGRO_EVENT_QUEUE *queue;		// Registro de eventos.
		bool flag;						// Bandera para terminar programa.
		bool redraw;					// Bandera para dibujar.
		bool stop;						// Bandera para menú principal y pausa.
	} argument_t;

static int TITLE_WIDTH, TITLE_HEIGHT, BACK_WIDTH, BACK_HEIGHT, TILE_WIDTH, TILE_HEIGHT;
static int dx, dy, move;	//Centrar imagen en pantalla.
static float scale;

#define FILS 20
#define COLS 10
int matrix[FILS][COLS];

static void must_init (bool test, const char *description);
static void stop(argument_t *argument, ALLEGRO_BITMAP *title);
static void play(argument_t *argument, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *blocka);

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

	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ALLEGRO_DISPLAY *display = al_create_display(100, 100);
	if(!display) {
		printf("couldn't load display\n");
		return 1;
	}
	must_init(display, "display");

	if(!al_init_image_addon()) {
		printf("couldn't initialize image addon\n");
		return 1;
	}



	ALLEGRO_BITMAP *background = al_load_bitmap(BACKGROUND_FILE);
	if(!background) {
		fprintf(stderr, "Failed to load image: %d\n", al_get_errno());
		return -1;
	}
	ALLEGRO_BITMAP *title = al_load_bitmap(TITLE_FILE);
	if(!title) {
		fprintf(stderr, "Failed to load image: %d\n", al_get_errno());
		return -1;
	}
	ALLEGRO_BITMAP *blocks = al_load_bitmap(BLOCKS_FILE);
		if(!title) {
			fprintf(stderr, "Failed to load image: %d\n", al_get_errno());
			return -1;
		}

	ALLEGRO_TIMER *timer = al_create_timer(1.0);
	must_init(timer, "timer");

	argument_t argument;	//Estructura a pasar como argumento.

	argument.queue = al_create_event_queue();
	must_init(argument.queue, "queue");
    al_register_event_source(argument.queue, al_get_keyboard_event_source());
	al_register_event_source(argument.queue, al_get_display_event_source(display));
	al_register_event_source(argument.queue, al_get_mouse_event_source());

	int SCREEN_WIDTH = al_get_display_width(display);
	int SCREEN_HEIGHT = al_get_display_height(display);

	TITLE_WIDTH = al_get_bitmap_width(title);
	TITLE_HEIGHT = al_get_bitmap_height(title);

	BACK_WIDTH = al_get_bitmap_width(background);
	BACK_HEIGHT = al_get_bitmap_height(background);

	TILE_WIDTH = al_get_bitmap_width(blocks)/4;
	TILE_HEIGHT = al_get_bitmap_height(blocks)/10;

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

	al_start_timer(timer);

	int i, j;
	for(i=0 ; i<FILS ; i++) {
		for(j=0 ; j<COLS ; j++) {
			matrix[i][j] = 1;
		}
	}

	stop(&argument, title);
	play(&argument, background, blocks);

	/* Finalización del Programa */
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(argument.queue);

	return 0;
}

/* Reviso si Allegro se inicializó correctamente */
static void must_init(bool test, const char *description) {
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}



void stop(argument_t *argument, ALLEGRO_BITMAP *title) {
	/* Imprimir el Menú del Juego */
	al_clear_to_color(al_map_rgb(0, 0, 0));		// Limpio pantalla con negro.
	printf("%f", scale);
	al_draw_scaled_bitmap(title, 0, 0, TITLE_WIDTH, TITLE_HEIGHT,
						  dx, dy,
						  TITLE_WIDTH * scale, TITLE_HEIGHT * scale, 0);
	al_flip_display();

	argument->stop = true;
	while(argument->stop) {	//Mientras esté en pausa, no empieza.

		al_wait_for_event(argument->queue, &(argument->event));
		switch ((argument->event).type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				argument->flag = true;	// Al cerrar el display, termina el programa.
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				argument->stop = false;
				break;

			case ALLEGRO_EVENT_KEY_CHAR:
				al_get_keyboard_state(&(argument->ks));
				argument->stop = false;

				if(al_key_down(&(argument->ks), ALLEGRO_KEY_ESCAPE)) {
					argument->flag = true;	//Al presionar Esc, se detiene el programa.
				}
				break;

			default:
				break;
		}
	}
}

void play(argument_t *argument, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *blocks) {
	while(!(argument->flag)) {

		al_wait_for_event(argument->queue, &(argument->event));
		switch ((argument->event).type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				argument->flag = true;	// Al cerrar el display, termina el programa.
				break;

			case ALLEGRO_EVENT_TIMER:
				if(!(argument->stop)) {
					argument->redraw = true;
					//TODO backend acá (pieza cae)**************
				}
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if((argument->event).mouse.button == 1) {	// Click izquierdo.
					//TODO backend (pieza gira izq)***************
				}
				else if((argument->event).mouse.button == 2) {	//Click derecho.
					//TODO backend (pieza gira derecha)***************
				}
				break;

			case ALLEGRO_EVENT_KEY_CHAR:
				al_get_keyboard_state(&(argument->ks));

				if(al_key_down(&(argument->ks), ALLEGRO_KEY_ESCAPE)) {
					argument->flag = true;
				}

				else {
					argument->redraw = true;
					if(al_key_down(&(argument->ks), ALLEGRO_KEY_LEFT)) {
						//TODO agregar backend acá*****************
					}
					if(al_key_down(&(argument->ks), ALLEGRO_KEY_RIGHT)) {
						//TODO agregar backend acá*****************
					}
					if(al_key_down(&(argument->ks), ALLEGRO_KEY_DOWN)) {
						//TODO agregar backend acá*****************
					}
				}
				break;

			default:
				break;
		}

		/* Rutina de Impresión */
		if(argument->redraw && al_is_event_queue_empty(argument->queue)) {
			al_clear_to_color(al_map_rgb(0, 0, 0));		// Limpio pantalla con negro.

			al_draw_scaled_bitmap(background, 0, 0, BACK_WIDTH, BACK_HEIGHT,
										 dx, dy, BACK_WIDTH * scale, BACK_HEIGHT * scale,
										 0);
			int i, j;
			for(i=0 ; i<FILS ; i++) {
				for(j=0 ; j<COLS ; j++) {
					if(matrix[i][j]) {
						al_draw_scaled_bitmap(blocks,	0, 0,	TILE_WIDTH, TILE_HEIGHT,
											 dx + move*(12+j), dy + move*(5+i),	TILE_WIDTH * scale, TILE_HEIGHT * scale,		0);
					}
				}
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
			al_flip_display();
			argument->redraw = false;
		}
	}
}
