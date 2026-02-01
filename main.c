#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

#define WIDTH (512+256)
#define HEIGHT 512

#define PLAYER_SIZE 16

#define LEVEL_COUNT 8

#define GRAVITY 0.15


typedef enum GameState {
	GAME,
	MENU,
	END
} GameState;

GameState game_state = MENU;

typedef struct Base {
	float x;
	float y;
} Base;

typedef struct Object {
	Base coord;
	Base speed;
} Object;

typedef struct Player {
	Object object;
	float coyote_time;
	float jump_buff;
	float time_since_spawn;
	float time_since_start;
	bool has_jumped;
	bool on_ground;
} Player;


typedef struct Platform {
	Object object;
	Base size;
} Platform;

typedef enum Shape {
	RECT,
	CIRCLE
} Shape;


typedef struct MaskRect {
	Base coord;
	Shape shape;
	Base size;
	bool movable;
} MaskRect;

typedef struct MaskCirc {
	Base coord;
	Shape shape;
	float radius;
	bool movable;
} MaskCirc;

typedef union Mask {
	MaskRect rect;
	MaskCirc circ;
} Mask;

typedef struct Enemy{
	Base coord;
} Enemy;

typedef struct Level {
	Platform* platforms;
	int plat_count;
	Mask* masks;
	int mask_count;
	Enemy* enemies;
	int enemy_count;
	Base start;
	Base end;
	char* text;
	Base text_coord;
} Level;

bool is_point_inside_mask(Mask mask, Base coord);

void degen_levels(void);
int gen_levels(void);

int mask_index = -1;

char timer_buff[64];

Player player = (Player){
	(Object){
		(Base){
			19,
			0
		},
		(Base){
			0,
			0
		}
	},
	1,
	1,
	0.5,
	false,
	false
};

Level** levels = NULL;

int current_level = 0;

void reset_enemies(void){
	player.time_since_spawn = 0.5;
	if(current_level < 5) return;

	switch(current_level){
		case 5:
			levels[5]->enemies[0] = (Enemy){
				(Base){
					WIDTH/2-PLAYER_SIZE/2,
					HEIGHT-50
				}
			};
			break;
		case 6:
			levels[6]->enemies[0] = (Enemy){
				(Base){
					WIDTH/2+WIDTH/4,
					HEIGHT/2-PLAYER_SIZE
				}
			};
			break;
		case 7:
			levels[7]->enemies[0] = (Enemy){
				(Base){
					WIDTH/2-100,
					HEIGHT-80
				}
			};
			levels[7]->enemies[1] = (Enemy){
				(Base){
					WIDTH/2+75,
					HEIGHT-80
				}
			};
			break;
		default:
			game_state = END;
			break;
	}
}

int gen_levels(void){
	levels = malloc(LEVEL_COUNT*sizeof(Level*));
	
	if(levels == NULL) return 1;

	for(int i = 0; i < LEVEL_COUNT; i++){
		levels[i] = malloc(sizeof(Level));
		if(levels[i] == NULL) return 2;
	}
	
	//level0 init

	levels[0]->start = (Base) { 100, 183 };
	levels[0]->end = (Base) { 522, 300 };
	levels[0]->platforms = malloc(3*sizeof(Platform));
	levels[0]->plat_count = 3;

//	levels[0]->text = "Micaj se sa strelicama.\nSkaci s razmakom.";
	levels[0]->text = "Move with arrows.";
	levels[0]->text_coord = (Base) { 100, 350 };

	player.object.coord = levels[0]->start;

	levels[0]->masks = malloc(5*sizeof(Mask));
	levels[0]->mask_count = 5;

	levels[0]->enemies = NULL;
	levels[0]->enemy_count = 0;



	//level1 init

	levels[1]->start = (Base) { 100, 258 };
	levels[1]->end = (Base) { WIDTH-100, 225 };
	levels[1]->platforms = malloc(3*sizeof(Platform));
	levels[1]->plat_count = 3;

//	levels[1]->text = "Micaj povrsine misom.\nJedino mozes micati povrsine s plavim rubom.";
	levels[1]->text = "Move the surfaces with your mouse.\nYou can only move surfaces with blue edges.";
	levels[1]->text_coord = (Base) { 300, 350 };

	levels[1]->masks = malloc(3*sizeof(Mask));
	levels[1]->mask_count = 3;

	levels[1]->enemies = NULL;
	levels[1]->enemy_count = 0;

	//level2 init

	levels[2]->start = (Base) { 100, 258 };
	levels[2]->end = (Base) { WIDTH-200, 225 };
	levels[2]->platforms = malloc(4*sizeof(Platform));
	levels[2]->plat_count = 4;

//	levels[2]->text = "Cini se da je ova povrsina prekratka.";
	levels[2]->text = "It appears this surface is a bit short.";
	levels[2]->text_coord = (Base) { 350, 40 };

	levels[2]->masks = malloc(2*sizeof(Mask));
	levels[2]->mask_count = 2;

	levels[2]->enemies = NULL;
	levels[2]->enemy_count = 0;

	//level3 init

	levels[3]->start = (Base) { 75, HEIGHT/2-PLAYER_SIZE };
	levels[3]->end = (Base) { 658, 225 };
	levels[3]->platforms = malloc(5*sizeof(Platform));
	levels[3]->plat_count = 5;

//	levels[3]->text = "Malo je ovaj put teze...";
	levels[3]->text = "This one is a bit harder...";
	levels[3]->text_coord = (Base){ WIDTH/2-150, HEIGHT/2 };

	levels[3]->masks = malloc(2*sizeof(Mask));
	levels[3]->mask_count = 2;

	levels[3]->enemies = NULL;
	levels[3]->enemy_count = 0;

	//level4 init

	levels[4]->start = (Base) { WIDTH/2-PLAYER_SIZE/2, 25 };
	levels[4]->end = (Base) { WIDTH/2-PLAYER_SIZE/2, HEIGHT-100 };
	levels[4]->platforms = malloc(1*sizeof(Platform));
	levels[4]->plat_count = 1;

//	levels[4]->text = "Izbjegavaj neprijatelje.";
//	levels[4]->text = "Mali pad.";
	levels[4]->text = "A small fall.";
	levels[4]->text_coord = (Base){ WIDTH/2-30, HEIGHT/2 };

	levels[4]->masks = malloc(2*sizeof(Mask));
	levels[4]->mask_count = 2;


	levels[4]->enemies = NULL;
	levels[4]->enemy_count = 0;

	//level5 init

	levels[5]->start = (Base) { 50, 174-PLAYER_SIZE };
	levels[5]->end = (Base) { WIDTH-50-PLAYER_SIZE/2, 400 };
	levels[5]->platforms = malloc(3*sizeof(Platform));
	levels[5]->plat_count = 3;

//	levels[5]->text = "Izbjegavaj neprijatelje.";
	levels[5]->text = "Dodge the enemies.";
	levels[5]->text_coord = (Base){ WIDTH/2-30, HEIGHT/2 };

	levels[5]->masks = malloc(3*sizeof(Mask));
	levels[5]->mask_count = 3;


	levels[5]->enemies = malloc(1*sizeof(Enemy));
	levels[5]->enemy_count = 1;

	//level6 init

	levels[6]->start = (Base) { 50, HEIGHT/2 };
	levels[6]->end = (Base) { WIDTH-50-PLAYER_SIZE/2, HEIGHT/2 };
	levels[6]->platforms = malloc(7*sizeof(Platform));
	levels[6]->plat_count = 7;

	levels[6]->text = "";
	levels[6]->text_coord = (Base){ WIDTH/2-30, HEIGHT/2 };

	levels[6]->masks = malloc(2*sizeof(Mask));
	levels[6]->mask_count = 2;


	levels[6]->enemies = malloc(1*sizeof(Enemy));
	levels[6]->enemy_count = 1;

	//level7 init

	levels[7]->start = (Base) { WIDTH/2-PLAYER_SIZE/2, HEIGHT-80 };
	levels[7]->end = (Base) { WIDTH/2-PLAYER_SIZE/2, HEIGHT-80-75-75-75-75-50 };
	levels[7]->platforms = malloc(9*sizeof(Platform));
	levels[7]->plat_count = 9;

	levels[7]->text = "";
	levels[7]->text_coord = (Base){ WIDTH/2-30, HEIGHT/2 };

	levels[7]->masks = malloc(2*sizeof(Mask));
	levels[7]->mask_count = 2;


	levels[7]->enemies = malloc(2*sizeof(Enemy));
	levels[7]->enemy_count = 2;

	//memory checks

	for(int i = 0; i < LEVEL_COUNT; i++){
		if(levels[i]->platforms == NULL){ return 3;}
	}

	for(int i = 0; i < LEVEL_COUNT; i++){
		if(levels[i]->masks == NULL){ return 4;}
	}

	for(int i = 0; i < LEVEL_COUNT; i++){
		if(levels[i]->enemy_count < 1) continue;
		if(levels[i]->masks == NULL){ return 5;}
	};


	//level0 level

	levels[0]->platforms[0] = (Platform){
		(Object){
			(Base){
				380,
				200
			},
			(Base){
				0,
				0
			}

		},
		(Base){
			120,
			25
		}
	};

	levels[0]->platforms[1] = (Platform){
		(Object){
			(Base){
				70,
				200
			},
			(Base){
				0,
				0
			}

		},
		(Base){
			190,
			25
		}
	};

	levels[0]->platforms[2] = (Platform){
		(Object){
			(Base){
				505,
				327
			},
			(Base){
				0,
				0
			}

		},
		(Base){
			50,
			20
		}
	};


	levels[0]->masks[0].rect = (MaskRect){
		(Base){
			60,
			80
		},
		RECT,
		(Base){
			250,
			200
		},
		false
	};

	levels[0]->masks[1].circ = (MaskCirc){
		(Base){
			301,
			107
		},
		CIRCLE,
		60,
		false
	};

	levels[0]->masks[2].circ = (MaskCirc){
		(Base){
			365,
			157
		},
		CIRCLE,
		45,
		false
	};

	levels[0]->masks[3].rect = (MaskRect){
		(Base){
			355,
			157
		},
		RECT,
		(Base){
			200,
			90
		},
		false
	};

	levels[0]->masks[4].rect = (MaskRect){
		(Base){
			505,
			157
		},
		RECT,
		(Base){
			50,
			190
		},
		false
	};

	//level1

	levels[1]->platforms[0] = (Platform){
		(Object){
			(Base){
				50,
				275
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[1]->platforms[1] = (Platform){
		(Object){
			(Base){
				225,
				250
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			250,
			25
		}
	};

	levels[1]->platforms[2] = (Platform){
		(Object){
			(Base){
				WIDTH-150,
				250
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[1]->masks[0].rect = (MaskRect){
		(Base){
			0,
			150
		},
		RECT,
		(Base){
			250,
			150
		},
		false
	}; 
	
	levels[1]->masks[1].rect = (MaskRect){
		(Base){
			240,
			50
		},
		RECT,
		(Base){
			200,
			50
		},
		true
	}; 

	levels[1]->masks[2].rect = (MaskRect){
		(Base){
			WIDTH-350,
			150
		},
		RECT,
		(Base){
			300,
			150
		},
		false
	}; 

	//level2

	levels[2]->platforms[0] = (Platform){
		(Object){
			(Base){
				25,
				300
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			250,
			25
		}
	};

	levels[2]->platforms[1] = (Platform){
		(Object){
			(Base){
				325,
				375
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			50,
			25
		}
	};

	levels[2]->platforms[2] = (Platform){
		(Object){
			(Base){
				450,
				300
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			50,
			25
		}
	};

	levels[2]->platforms[3] = (Platform){
		(Object){
			(Base){
				550,
				250
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			50,
			25
		}
	};

	levels[2]->masks[0].rect = (MaskRect){
		(Base){
			0,
			200
		},
		RECT,
		(Base){
			300,
			125
		},
		false

	};

	levels[2]->masks[1].circ = (MaskCirc){
		(Base){
			WIDTH-200+PLAYER_SIZE/2,
			225+PLAYER_SIZE/2
		},
		CIRCLE,
		75.0,
		true
	};

	//level3

	levels[3]->platforms[0] = (Platform){
		(Object){
			(Base){
				25,
				275
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[3]->platforms[1] = (Platform){
		(Object){
			(Base){
				175,
				350
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			75,
			25
		}
	};
	
	levels[3]->platforms[2] = (Platform){
		(Object){
			(Base){
				325,
				425
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			75,
			25
		}
	};

	levels[3]->platforms[3] = (Platform){
		(Object){
			(Base){
				475,
				350
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			75,
			25
		}
	};

	levels[3]->platforms[4] = (Platform){
		(Object){
			(Base){
				625,
				275
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			75,
			25
		}
	};



	levels[3]->masks[0].rect = (MaskRect){
		(Base){
			25,
			200
		},
		RECT,
		(Base){
			100,
			100
		},
		false
	};

	levels[3]->masks[1].rect = (MaskRect){
		(Base){
			WIDTH/2-32,
			250
		},
		RECT,
		(Base){
			65,
			65
		},
		true
	};

	//level4

	levels[4]->platforms[0] = (Platform){
		(Object){
			(Base){
				WIDTH/2-50,
				50
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[4]->masks[0].rect = (MaskRect){
		(Base){
			WIDTH/2-50,
			0
		},
		RECT,
		(Base){
			100,
			75
		},
		false
	};

	levels[4]->masks[1].circ = (MaskCirc){
		(Base){
			WIDTH/2,
			100
		},
		CIRCLE,
		19,
		true
	};

	//level5

	levels[5]->platforms[0] = (Platform){
		(Object){
			(Base){
				0,
				175
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[5]->platforms[1] = (Platform){
		(Object){
			(Base){
				200,
				300
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			200,
			25
		}
	};

	levels[5]->platforms[2] = (Platform){
		(Object){
			(Base){
				450,
				225
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			150,
			25
		}
	};

	levels[5]->masks[0].rect = (MaskRect){
		(Base){
			0,
			100
		},
		RECT,
		(Base){
			100,
			100
		},
		false
	};

	levels[5]->masks[1].circ = (MaskCirc){
		(Base){
			WIDTH/2,
			HEIGHT/2
		},
		CIRCLE,
		50,
		true
	};

	//level6

	levels[6]->platforms[0] = (Platform){
		(Object){
			(Base){
				50,
				HEIGHT/2+PLAYER_SIZE+1
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};

	levels[6]->platforms[1] = (Platform){
		(Object){
			(Base){
				250,
				HEIGHT/2+PLAYER_SIZE
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			200,
			25
		}
	};

	levels[6]->platforms[2] = (Platform){
		(Object){
			(Base){
				180,
				HEIGHT/2+PLAYER_SIZE-150
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			100
		}
	};

	levels[6]->platforms[3] = (Platform){
		(Object){
			(Base){
				180,
				HEIGHT/2+PLAYER_SIZE-75
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			75,
			25
		}
	};

	levels[6]->platforms[4] = (Platform){
		(Object){
			(Base){
				320,
				HEIGHT/2+PLAYER_SIZE-150
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			150
		}
	};

	levels[6]->platforms[5] = (Platform){
		(Object){
			(Base){
				370,
				HEIGHT/2+PLAYER_SIZE-260
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			150
		}
	};

	levels[6]->platforms[6] = (Platform){
		(Object){
			(Base){
				450,
				HEIGHT/2+PLAYER_SIZE-75
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			200,
			25
		}
	};

	levels[6]->masks[0].rect = (MaskRect){
		(Base){
			50,
			HEIGHT/2+PLAYER_SIZE-99
		},
		RECT,
		(Base){
			100,
			125
		},
		false
	};

	levels[6]->masks[1].circ = (MaskCirc){
		(Base){
			WIDTH-110,
			HEIGHT/2
		},
		CIRCLE,
		60,
		true
	};

	//level7

	levels[7]->platforms[0] = (Platform){
		(Object){
			(Base){
				WIDTH/2-100,
				HEIGHT-80+PLAYER_SIZE+1
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			200,
			25
		}
	};

	levels[7]->platforms[1] = (Platform){
		(Object){
			(Base){
				WIDTH/2-100-25,
				HEIGHT-80-75-75-75-75-50+12
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			80+75+75+75+75
		}
	};

	levels[7]->platforms[2] = (Platform){
		(Object){
			(Base){
				WIDTH/2+100,
				HEIGHT-80-75-75-75-75-50+12
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			80+75+75+75+75
		}
	};

	levels[7]->platforms[3] = (Platform){
		(Object){
			(Base){
				WIDTH/2-100,
				HEIGHT-80-74+30
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			50,
			25
		}
	};
	levels[7]->platforms[4] = (Platform){
		(Object){
			(Base){
				WIDTH/2+100-50,
				HEIGHT-80-74+30
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			50,
			25
		}
	};

	levels[7]->platforms[5] = (Platform){
		(Object){
			(Base){
				WIDTH/2-50,
				HEIGHT-80-75-74+45
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};
	levels[7]->platforms[6] = (Platform){
		(Object){
			(Base){
				WIDTH/2-50-25,
				HEIGHT-80-75-74+45-80
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			50
		}
	};
	levels[7]->platforms[7] = (Platform){
		(Object){
			(Base){
				WIDTH/2+50,
				HEIGHT-80-75-74+45-80
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			25,
			50
		}
	};

	levels[7]->platforms[8] = (Platform){
		(Object){
			(Base){
				WIDTH/2-50,
				HEIGHT-80-75-74+45-80-80
			},
			(Base){
				0,
				0
			}
		},
		(Base){
			100,
			25
		}
	};


	levels[7]->masks[0].circ = (MaskCirc){
		(Base){
			WIDTH/2,
			HEIGHT-80
		},
		CIRCLE,
		35,
		false
	};

	levels[7]->masks[1].circ = (MaskCirc){
		(Base){
			WIDTH/2,
			HEIGHT-130
		},
		CIRCLE,
		50,
		true
	};


	return 0;
}

void draw_rect(Base coord, Base size, Color color){
	DrawRectangle(coord.x+GetScreenWidth()/2-WIDTH/2,coord.y+GetScreenHeight()/2-HEIGHT/2,size.x,size.y,color);
}

void restart(void){
	player.object.coord = levels[current_level]->start;
	player.coyote_time = 1;
	player.jump_buff = 1;
	player.object.speed = (Base){0,0};
	reset_enemies();
}

void draw_rect_outline(Base coord, Base size, Color color){
	DrawRectangleLines(coord.x+GetScreenWidth()/2-WIDTH/2,coord.y+GetScreenHeight()/2-HEIGHT/2,size.x+1,size.y+1,color);
}

void draw_circ(Base coord, float radius, Color color){
	DrawCircle(coord.x+GetScreenWidth()/2-WIDTH/2,coord.y+GetScreenHeight()/2-HEIGHT/2,radius,color);
}

void draw_circ_outline(Base coord, float radius, Color color){
	DrawCircleLines(coord.x+GetScreenWidth()/2-WIDTH/2,coord.y+GetScreenHeight()/2-HEIGHT/2,radius+0.5f,color);
}

#define PLAYABLE_COLOR (Color){0,0,128,255}
#define PLAYABLE_COLOR_MOVABLE (Color){0,0,128,255}
#define END_COLOR (Color){255,255,0,255}

int draw(void){
	BeginDrawing();
	ClearBackground(BLACK);

	for(int i = 0; i < levels[current_level]->mask_count; i++){
		switch (levels[current_level]->masks[i].rect.shape){
			case RECT:
				if(levels[current_level]->masks[i].rect.movable){
					draw_rect_outline(levels[current_level]->masks[i].rect.coord,levels[current_level]->masks[i].rect.size,BLUE);
				}
				else{
					draw_rect_outline(levels[current_level]->masks[i].rect.coord,levels[current_level]->masks[i].rect.size,RED);
				}
				break;
			case CIRCLE:
				if(levels[current_level]->masks[i].circ.movable){
						draw_circ_outline(levels[current_level]->masks[i].circ.coord,levels[current_level]->masks[i].circ.radius,BLUE);
				}
				else{
						draw_circ_outline(levels[current_level]->masks[i].circ.coord,levels[current_level]->masks[i].circ.radius,RED);
				}
				break;
		}
	}

	for(int i = 0; i < levels[current_level]->mask_count; i++){
		if(levels[current_level]->masks[i].rect.shape == RECT){
			draw_rect(levels[current_level]->masks[i].rect.coord,levels[current_level]->masks[i].rect.size,PLAYABLE_COLOR);
		}
		else{
			draw_circ(levels[current_level]->masks[i].circ.coord,levels[current_level]->masks[i].circ.radius,PLAYABLE_COLOR);
		}
/*		switch (levels[current_level]->masks[i].rect.shape){
			case RECT:
				if(levels[current_level]->masks[i].rect.movable){
					draw_rect(levels[current_level]->masks[i].rect.coord,levels[current_level]->masks[i].rect.size,PLAYABLE_COLOR_MOVABLE);
				}
				else{
					draw_rect(levels[current_level]->masks[i].rect.coord,levels[current_level]->masks[i].rect.size,PLAYABLE_COLOR);
				}
				break;
			case CIRCLE:
				if(levels[current_level]->masks[i].circ.movable){
						draw_circ(levels[current_level]->masks[i].circ.coord,levels[current_level]->masks[i].circ.radius,PLAYABLE_COLOR_MOVABLE);
				}
				else{
						draw_circ(levels[current_level]->masks[i].circ.coord,levels[current_level]->masks[i].circ.radius,PLAYABLE_COLOR);
				}
				break;
		}*/
	}

	for(int i = 0; i < levels[current_level]->plat_count; i++){
		draw_rect(levels[current_level]->platforms[i].object.coord,levels[current_level]->platforms[i].size,WHITE);
	}

	DrawText(levels[current_level]->text,levels[current_level]->text_coord.x+GetScreenWidth()/2-WIDTH/2,levels[current_level]->text_coord.y+GetScreenHeight()/2-HEIGHT/2, 19, WHITE);

	DrawText(timer_buff,0,0,19,WHITE);

	draw_rect(levels[current_level]->end,(Base) { PLAYER_SIZE, PLAYER_SIZE }, END_COLOR);

	for(int i = 0; i < levels[current_level]->enemy_count; i++){
		draw_rect(levels[current_level]->enemies[i].coord,(Base) { PLAYER_SIZE, PLAYER_SIZE }, RED);
	}

	draw_rect(player.object.coord,(Base) { PLAYER_SIZE, PLAYER_SIZE },GRAY);

//	for(int i = 0; i < GetScreenWidth(); i++){
//		for(int j = 0; j < GetScreenHeight(); j++){
//			bool is_inside = false;
//			for(int k = 0; k < levels[current_level]->mask_count; k++){
//				if(is_point_inside_mask(levels[current_level]->masks[k],(Base) {i,j})) {is_inside = true; break;}
//			}
//			if(!is_inside) DrawPixel(i,j,BLACK);
//		}
//	}


	EndDrawing();
	return 0;
}

bool check_collision(Base coord1, Base size1, Base coord2, Base size2){
	if(coord1.x + size1.x >= coord2.x && coord1.x <= coord2.x + size2.x && coord1.y+size1.y >= coord2.y && coord1.y <= coord2.y + size2.y) return true;
	return false;
}

Base check_collisions(Base coord, Base size, Base* speed){
	Base output = {0,0};

	for(int i = 0; i < levels[current_level]->plat_count; i++){
		Platform plat = levels[current_level]->platforms[i];
		//if(coord.x+size.x+speed->x >= plat.object.coord.x && coord.x+speed->x <= plat.object.coord.x + plat.size.x && coord.y+size.y+speed->y >= plat.object.coord.y && coord.y+speed->y <= plat.object.coord.y+plat.size.y){

		Base coord_and_speed = (Base){ coord.x+speed->x, coord.y+speed->y};
		//Base center_coord = (Base) { coord.x+size.x,coord.y+size.y };

		//Base center_plat = (Base) { plat.object.coord.x+plat.size.x,plat.object.coord.y+plat.size.y };

		if(check_collision(coord_and_speed,size,plat.object.coord,plat.size)){

			if(coord.y+size.y <= plat.object.coord.y){
//				output.y = 0;
//				speed->y = -GRAVITY;
				player.on_ground = true;
				player.has_jumped = false;
			}
//			else{
//				player.on_ground = false;
//			}



			if(coord.x+size.x <= plat.object.coord.x){
				output.x = 0;
				speed->x = -GRAVITY*2;
				continue;
			}
			if(coord.x >= plat.object.coord.x+plat.size.x){
				output.x = 0;
				speed->x = GRAVITY*2;
				continue;
			}

			if(coord.y+size.y <= plat.object.coord.y){
				output.y = 0;
				speed->y = -GRAVITY*0.5;
				player.on_ground = true;
			}
//			else{
//				player.on_ground = false;
//			}

			if(coord.y+size.y >= plat.object.coord.y){
				output.y = 0;
				speed->y = GRAVITY*2;
//				player.on_ground = false;
			}
//			if(coord.x <= plat.object.coord.x && coord.x + 15 <= plat.object.coord.x){
//				 output.x = size.x + (coord.x - plat.object.coord.x);
//			}
//			if(coord.x + size.x >=plat.object.coord.x+plat.size.x) output.x = size.x;
		}
	}
	return output;
}

bool is_point_inside_mask(Mask mask, Base coord){
	switch(mask.rect.shape)
	{
		case RECT:
			return check_collision(mask.rect.coord,mask.rect.size,coord,(Base){1,1});
		case CIRCLE:
			return (coord.x-mask.circ.coord.x)*(coord.x-mask.circ.coord.x) + (coord.y-mask.circ.coord.y)*(coord.y-mask.circ.coord.y) <= mask.circ.radius*mask.circ.radius;
	}
	return false;
}

void change_level(int level){
	if(level < 0){
		return;
	}
	if(level >= LEVEL_COUNT){
		mask_index = -1;
		player.object.speed = (Base){0,0};
		player.coyote_time = 1;
		player.jump_buff = 1;
		reset_enemies();
		game_state = END;
		return;
	}
	current_level = level;
	mask_index = -1;
	player.object.coord = levels[current_level]->start;
	player.object.speed = (Base){0,0};
	player.coyote_time = 1;
	player.jump_buff = 1;
	reset_enemies();
}


void player_physics(void){
	if(player.jump_buff < 0.2) player.jump_buff += GetFrameTime();  

	if(player.time_since_spawn > 0) player.time_since_spawn -= GetFrameTime();

	player.time_since_start += GetFrameTime();

	if(player.coyote_time < 0.2 && !player.on_ground) player.coyote_time += GetFrameTime();

	if(player.on_ground) player.coyote_time = 0;

	if(player.object.speed.y < GRAVITY*60) player.object.speed.y += GRAVITY*GetFrameTime()*60;

	if(IsKeyDown(KEY_LEFT)) player.object.speed.x -= 0.3;
	if(IsKeyDown(KEY_RIGHT)) player.object.speed.x += 0.3;

	if(player.object.speed.y > GRAVITY*2.5f || player.object.speed.y < -GRAVITY*2.5f) player.on_ground = false;
	if(IsKeyDown(KEY_UP)) player.jump_buff = 0;

	if(player.time_since_spawn > 0){ player.jump_buff = 1; player.coyote_time = 1;};

	if(((player.jump_buff < 0.2 || IsKeyDown(KEY_UP)) && player.on_ground) || (IsKeyPressed(KEY_UP) && player.coyote_time < 0.2 && !player.has_jumped)){ player.object.speed.y = -8; player.has_jumped = true;}

	if(player.object.speed.x < 0.1f && player.object.speed.x > -0.1f) player.object.speed.x = 0;
	if(player.object.speed.y < 0.1f && player.object.speed.y > -0.1f) player.object.speed.y = 0;

	//Base physics_output;

	/*physics_output = */check_collisions(player.object.coord,(Base) { PLAYER_SIZE,PLAYER_SIZE }, &player.object.speed);

	player.object.coord.y += player.object.speed.y;
	player.object.coord.x += player.object.speed.x;
//	printf("%f,%f\n", player.object.coord.x, player.object.coord.y);

	player.object.speed.x = player.object.speed.x * 0.9f;
	player.object.speed.y = player.object.speed.y * 0.95f;

	bool is_colliding = false;
	for(int i = 0; i < levels[current_level]->mask_count; i++){
		if(is_point_inside_mask(levels[current_level]->masks[i],(Base) { player.object.coord.x + PLAYER_SIZE/2, player.object.coord.y + PLAYER_SIZE/2 })){ is_colliding = true; break; }
	}

	if(!is_colliding) restart();

	if(check_collision(player.object.coord,(Base) { PLAYER_SIZE, PLAYER_SIZE }, levels[current_level]->end, (Base) { PLAYER_SIZE, PLAYER_SIZE })) change_level(current_level+1);
//	printf("%d\n",is_colliding);
}


void mask_movement(void){
	static Base mouse;
	static Base current_mouse;
	static Base object_coord;
	if(!IsMouseButtonDown(0)){ mask_index = -1; return;}
//	if(IsMouseButtonPressed(0)) mouse = (Base) { GetMouseX()+GetScreenWidth()/2-WIDTH/2, GetMouseY()+GetScreenHeight()/2-HEIGHT/2 };
//	current_mouse = (Base) { GetMouseX()+GetScreenWidth()/2-WIDTH/2, GetMouseY()+GetScreenHeight()/2-HEIGHT/2 };
	if(IsMouseButtonPressed(0)) mouse = (Base) { GetMouseX(), GetMouseY() };
	current_mouse = (Base) { GetMouseX(), GetMouseY() };
//	if(IsMouseButtonPressed(0)) mouse = (Base) { GetMouseX()-GetScreenWidth()/2+WIDTH/2, GetMouseY()+GetScreenHeight()/2+HEIGHT/2 };
//	current_mouse = (Base) { GetMouseX()-GetScreenWidth()/2+WIDTH/2, GetMouseY()-GetScreenHeight()/2+HEIGHT/2 };

	for(int i = 0; i < levels[current_level]->mask_count && mask_index == -1; i++){
		Mask mask_with_shift = levels[current_level]->masks[i];
		switch(mask_with_shift.rect.shape){
			case CIRCLE:
				if(!mask_with_shift.circ.movable) continue;
				break;
			case RECT:
				if(!mask_with_shift.rect.movable) continue;
				break;
		}
		mask_with_shift.rect.coord.x += GetScreenWidth()/2-WIDTH/2;
		mask_with_shift.rect.coord.y += GetScreenHeight()/2-HEIGHT/2;
		if(is_point_inside_mask(mask_with_shift,mouse)){
			mask_index = i;
			object_coord = levels[current_level]->masks[i].rect.coord;
		}
	}
	if(mask_index != -1){
		levels[current_level]->masks[mask_index].rect.coord.x = object_coord.x + (current_mouse.x-mouse.x);
		levels[current_level]->masks[mask_index].rect.coord.y = object_coord.y + (current_mouse.y-mouse.y);
	}
}

void enemy_movement(void){
	if(levels[current_level]->enemy_count < 1) return;

	for(int i = 0; i < levels[current_level]->enemy_count; i++){
		Base movement = {0};

		if(levels[current_level]->enemies[i].coord.y > player.object.coord.y){
			movement.y -= 0.8;
		}
		else if(levels[current_level]->enemies[i].coord.y < player.object.coord.y){
			movement.y += 0.8;
		}
		if(levels[current_level]->enemies[i].coord.x > player.object.coord.x){
			movement.x -= 0.8;
		}
		else if(levels[current_level]->enemies[i].coord.x < player.object.coord.x){
			movement.x += 0.8;
		}

		levels[current_level]->enemies[i].coord.x += movement.x;
		levels[current_level]->enemies[i].coord.y += movement.y;

		if(check_collision(player.object.coord,(Base) {PLAYER_SIZE,PLAYER_SIZE},levels[current_level]->enemies[i].coord,(Base) {PLAYER_SIZE,PLAYER_SIZE})) restart();

		//distance.x = (*((int*)&(distance.x)) << 1);
		//distance.x = (*((int*)&(distance.x)) >> 1);
		//distance.y = (*((int*)&(distance.y)) << 1);
		//distance.y = (*((int*)&(distance.y)) >> 1);
	}
}

int draw_menu(void){
	BeginDrawing();
	ClearBackground(BLACK);
	draw_rect((Base){ WIDTH/2-50,HEIGHT/2},(Base){ 100, 50 }, WHITE);
	DrawText("PLAY",WIDTH/2-25+GetScreenWidth()/2-WIDTH/2,HEIGHT/2+GetScreenHeight()/2-HEIGHT/2,19,BLUE);
	Base current_mouse = (Base) { GetMouseX(), GetMouseY() };
//	current_mouse.x += GetScreenWidth()/2-WIDTH/2;
//	current_mouse.y += GetScreenHeight()/2-HEIGHT/2;
	EndDrawing();

	if(check_collision(current_mouse,(Base){1,1},(Base) {WIDTH/2-50+GetScreenWidth()/2-WIDTH/2,HEIGHT/2+GetScreenHeight()/2-HEIGHT/2}, (Base) {100, 50})){
		if(IsMouseButtonDown(0)){
			game_state = GAME;
			player.time_since_start = 0;
			current_level = 0;
			degen_levels();
			return gen_levels();
		}
	}
	return 0;
}

void draw_end(void){
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText(timer_buff,20,20,19,WHITE);
	draw_rect((Base){ WIDTH/2-50,HEIGHT/2+100},(Base){ 100, 50 }, WHITE);
	DrawText("MENU",WIDTH/2-25+GetScreenWidth()/2-WIDTH/2,HEIGHT/2+100+GetScreenHeight()/2-HEIGHT/2,19,BLACK);
	Base current_mouse = (Base) { GetMouseX(), GetMouseY() };
//	current_mouse.x += GetScreenWidth()/2-WIDTH/2;
//	current_mouse.y += GetScreenHeight()/2-HEIGHT/2;
	EndDrawing();
	if(check_collision(current_mouse,(Base){1,1},(Base) {WIDTH/2-50+GetScreenWidth()/2-WIDTH/2,HEIGHT/2+100+GetScreenHeight()/2-HEIGHT/2}, (Base) {100, 50})){
		if(IsMouseButtonDown(0)){
			game_state = MENU;
		}
	}
}

int game(void){
	while(!WindowShouldClose()){
		switch(game_state){
			case GAME:
				mask_movement();
				player_physics();
				enemy_movement();

				sprintf(timer_buff,"%.2f",player.time_since_start);

				draw();
				break;
			case MENU:
				if(draw_menu() != 0) return 1;
				break;
			case END:
				draw_end();
				break;
		}
	}

	return 0;
}

void degen_levels(void){
	for(int i = 0; i < LEVEL_COUNT; i++){
		if(levels[i]->platforms != NULL){
			free(levels[i]->platforms);
		}
		levels[i]->platforms = NULL;
		if(levels[i]->masks != NULL){
			free(levels[i]->masks);
		}
		levels[i]->masks = NULL;
		if(levels[i]->enemies != NULL) free(levels[i]->enemies);
		levels[i]->enemies = NULL;
		if(levels != NULL) free(levels[i]);
		levels[i] = NULL;
	}
	if(levels != NULL) free(levels);
	levels = NULL;
}

int main(void){
	//SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH,HEIGHT,"Kocka");

	if(!IsWindowReady()) return 1;

	SetWindowMinSize(WIDTH,HEIGHT);
	SetWindowMaxSize(WIDTH,HEIGHT);
	SetTargetFPS(60);

//	ToggleBorderlessWindowed();

	if(gen_levels() != 0) return 1;

	if(game() != 0) return 2;
	degen_levels();
	return 0;
}
