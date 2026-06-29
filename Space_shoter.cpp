#include <raylib.h>
import std;

struct nave {
	Vector2 posicion;
	Texture2D textura;
};

struct misil {
	Vector2 posicion;
};

struct meteorito {
	Vector2 posicion;
	Texture2D tipo_textura;
	float Velocidad;
};


Texture2D CargarTexturaChica(const char* ruta, int ancho, int alto) {
	Image img = LoadImage(ruta);
	ImageResize(&img, ancho, alto);
	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

int main() {

	InitWindow(800, 600, "space");
	SetTargetFPS(60);
    

	nave jugador;
	jugador.posicion.x = 400;
	jugador.posicion.y = 500;

	
	
	jugador.textura = CargarTexturaChica("textures/spaceShips_002.png", 60, 60);
	Texture2D texture_misil = CargarTexturaChica("textures/spaceMissiles_040.png", 15, 35);

	Texture2D textures_meteoritos[4];
	textures_meteoritos[0] = CargarTexturaChica("textures/spaceMeteors_001.png", 80, 80);
	textures_meteoritos[1] = CargarTexturaChica("textures/spaceMeteors_002.png", 70, 70);
	textures_meteoritos[2] = CargarTexturaChica("textures/spaceMeteors_003.png", 45, 45);
	textures_meteoritos[3] = CargarTexturaChica("textures/spaceMeteors_004.png", 35, 35);


	std::vector<meteorito> meteoritos;
	std::vector<misil> misiles;

	meteoritos.reserve(15);
	misiles.reserve(5);

	int contador = 0;
	int cooldown = 0;
	int puntos = 0;
	int record = 0;
	int record_guardado = 0;
	float velocidad_actual = 0.0f;

	bool game_over = false;

	std::ifstream archivo_record("Save_points.txt");

	if (archivo_record.is_open()) {

		archivo_record >> record_guardado;
		archivo_record.close();
	}
	record = record_guardado;

	while (!WindowShouldClose()) {
		if (!game_over) {
			++contador;

			if (cooldown > 0) {
				--cooldown;
			}

			if (contador >= 90) {


				meteorito nuevo;

				nuevo.posicion.x = std::rand() % 680 + 50;
				nuevo.posicion.y = -60;

				int que_sera = std::rand() % 4 + 1;

				if (que_sera == 1) {
					nuevo.tipo_textura = textures_meteoritos[0];
					nuevo.Velocidad = 1.0f + velocidad_actual;
				}
				else if (que_sera == 2) {
					nuevo.tipo_textura = textures_meteoritos[1];
					nuevo.Velocidad = 1.0f + velocidad_actual;
				}
				else if (que_sera == 3) {
					nuevo.tipo_textura = textures_meteoritos[2];
					nuevo.Velocidad = 2.0f + velocidad_actual;
				}
				else {
					nuevo.tipo_textura = textures_meteoritos[3];
					nuevo.Velocidad = 2.0f + velocidad_actual;
				}

				meteoritos.emplace_back(nuevo);
				contador = 0;
				velocidad_actual += 0.03f;

			}

			for (size_t i = 0; i < meteoritos.size(); ++i) {
				meteoritos[i].posicion.y += meteoritos[i].Velocidad;
				if (meteoritos[i].posicion.y >= 650) {
					meteoritos[i].Velocidad = 0.0f;
				}
			}
			for (size_t i = 0; i < misiles.size(); ++i) {
				misiles[i].posicion.y -= 5.0f;
				if (misiles[i].posicion.y <= -50) {
					misiles[i].posicion.y = -50;
				}
			}


			std::erase_if(meteoritos, [&](const meteorito& i) {
				if (i.Velocidad == 0.0f) {
					game_over = true;
					return true;
				}
				return false;
				});


			std::erase_if(misiles, [](const misil& i) {
				return i.posicion.y == -25;
				});


			for (size_t b = 0; b < misiles.size(); ++b) {
				for (size_t m = 0; m < meteoritos.size(); ++m) {


					Rectangle rectMisil = {
						misiles[b].posicion.x,
						misiles[b].posicion.y,
						15.0f, 35.0f
					};


					Rectangle rectMeteoro = {
						meteoritos[m].posicion.x,
						meteoritos[m].posicion.y,
						(float)meteoritos[m].tipo_textura.width,
						(float)meteoritos[m].tipo_textura.height
					};




					if (CheckCollisionRecs(rectMisil, rectMeteoro) && misiles[b].posicion.y > -15) {

						misiles[b].posicion.y = -50;
						meteoritos[m].posicion.y = -200.0f;
						++puntos;
						if (puntos > record) {
							record = puntos;
						}

					}
				}
			}

			for (size_t i = 0; i < meteoritos.size(); ++i) {
				for (size_t a = i + 1; a < meteoritos.size(); ++a) {

					Rectangle rectMeteoro1 = {
						meteoritos[i].posicion.x,
						meteoritos[i].posicion.y,
						(float)meteoritos[i].tipo_textura.width,
						(float)meteoritos[i].tipo_textura.height
					};
					Rectangle rectMeteoro2 = {
						meteoritos[a].posicion.x,
						meteoritos[a].posicion.y,
						(float)meteoritos[a].tipo_textura.width,
						(float)meteoritos[a].tipo_textura.height
					};

					if (CheckCollisionRecs(rectMeteoro1, rectMeteoro2)) {
						
						if (meteoritos[i].posicion.y < meteoritos[a].posicion.y) {
							meteoritos[i].posicion.y = meteoritos[a].posicion.y - (float)meteoritos[i].tipo_textura.height;
							meteoritos[a].Velocidad = meteoritos[i].Velocidad;
						}
						
						else {
							meteoritos[a].posicion.y = meteoritos[i].posicion.y - (float)meteoritos[a].tipo_textura.height;
							meteoritos[i].Velocidad = meteoritos[a].Velocidad;
						}
					}

				}
			}

			for (size_t i = 0; i < meteoritos.size(); ++i) {
				Rectangle rectMeteoro = {
						meteoritos[i].posicion.x,
						meteoritos[i].posicion.y,
						(float)meteoritos[i].tipo_textura.width,
						(float)meteoritos[i].tipo_textura.height
				};

				Rectangle rectJugador = {
						jugador.posicion.x,
						jugador.posicion.y,
						(float)jugador.textura.width,
						(float)jugador.textura.height
				};

				if (CheckCollisionRecs(rectJugador, rectMeteoro)) {
					game_over = true;
				}

			}
			std::erase_if(meteoritos, [&](const meteorito& i) {
				if (i.posicion.y == -200.0f) {
					return true;
				}
				return false;
				});


			if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && jugador.posicion.x <= 800 - 60) {
				jugador.posicion.x += 8.0f;
			}
			if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && jugador.posicion.x >= 0) {
				jugador.posicion.x -= 8.0f;
			}
			if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && cooldown <= 0) {
				misil nuevo;
				nuevo.posicion.x = jugador.posicion.x + 23;
				nuevo.posicion.y = jugador.posicion.y - 10;
				misiles.emplace_back(nuevo);
				cooldown = 60;
			}
			
		}
		else {
			if (IsKeyPressed(KEY_ENTER)) {
				misiles.clear();
				meteoritos.clear();
				contador = 0;
				cooldown = 0;
				puntos = 0;
				jugador.posicion.x = 400;
				velocidad_actual = 0.0f;
				game_over = false;
			}
		}

		
		BeginDrawing();
		ClearBackground(BLACK);

		if (!game_over) {
		
			for (size_t i = 0; i < misiles.size(); ++i) {
				DrawTextureV(texture_misil, misiles[i].posicion, WHITE);
			}

			
			for (size_t i = 0; i < meteoritos.size(); ++i) {
				DrawTextureV(meteoritos[i].tipo_textura, meteoritos[i].posicion, WHITE);
			}

			
			DrawTextureV(jugador.textura, jugador.posicion, WHITE);
			Vector2 puntaNave = {
	        jugador.posicion.x + (jugador.textura.width / 2.0f),
	        jugador.posicion.y
			};

			
			float choqueY = 0.0f;

			
			for (size_t i = 0; i < meteoritos.size(); ++i) {
				
				if (puntaNave.x >= meteoritos[i].posicion.x &&
					puntaNave.x <= meteoritos[i].posicion.x + meteoritos[i].tipo_textura.width) {

					
					if (meteoritos[i].posicion.y < jugador.posicion.y) {
						
						float baseMeteoro = meteoritos[i].posicion.y + meteoritos[i].tipo_textura.height;
						if (baseMeteoro > choqueY) {
							choqueY = baseMeteoro;
						}
					}
				}
			}

			for (size_t i = 0; i < misiles.size(); ++i) {

				if (puntaNave.x >= misiles[i].posicion.x &&
					puntaNave.x <= misiles[i].posicion.x + texture_misil.width) {


					if (misiles[i].posicion.y < jugador.posicion.y) {

						float basemisile = misiles[i].posicion.y + texture_misil.height;
						if (basemisile > choqueY) {
							choqueY = basemisile;
						}
					}
				}
			}


			
			Vector2 finRayo = { puntaNave.x, choqueY };

			
			Color colorLaserGuia = { 255, 0, 0, 90 };
			DrawLineEx(puntaNave, finRayo, 2.0f, colorLaserGuia);
			
			DrawText(TextFormat("PUNTOS: %i", puntos), 20, 20, 20, GREEN);
			DrawText(TextFormat("RECORD: %i", record), 20, 45, 20, GOLD);

			DrawFPS(710, 10);
		}
		else {
			
			DrawText("¡GAME OVER!", 260, 230, 45, RED);
			DrawText(TextFormat("Puntuacion Final: %i", puntos), 280, 295, 22, WHITE);
			DrawText("Presiona ENTER para reiniciar", 260, 350, 20, GRAY);
		}

		EndDrawing();


	}
	
	if (record > record_guardado) {
		
		std::ofstream archivo_record2("Save_points.txt");

		if (archivo_record2.is_open()) {
			std::print(archivo_record2, "{}", record);
			archivo_record2.close();
		}
	}
	
	UnloadTexture(jugador.textura);
	UnloadTexture(texture_misil);

	for (int i = 0; i < 4; ++i) {
		UnloadTexture(textures_meteoritos[i]);
	}


	CloseWindow();
	return 0;
}