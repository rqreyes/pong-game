#include <iostream>
#include <raylib.h>

// global declaractions
// --------------------------------------------------------------------------------
typedef enum
{
	GAMEPLAY_2_CPU,
	GAMEPLAY_1_PLAYER,
	GAMEPLAY_2_PLAYER,
} screen_gameplay_t;
typedef enum
{
	TITLE,
	PAUSE,
	CONTROLS,
} screen_overlay_t;

screen_gameplay_t screen_gameplay_current = GAMEPLAY_2_CPU;
screen_overlay_t screen_overlay_current = TITLE;

bool is_screen_overlay_on = true;
bool is_game_new = true;
bool is_game_update_on = true;

const int font_size_h1 = 80;
const int font_size_h2 = 40;
const int font_size_p = 20;

Color overlay = Color{0, 0, 0, 180};
Color green = Color{38, 185, 154, 255};
Color green_dark = Color{20, 160, 133, 255};
Color green_light = Color{129, 204, 184, 255};
Color yellow = Color{243, 213, 91, 255};
Color yellow_shadow_1 = Color{243, 213, 91, 191};
Color yellow_shadow_2 = Color{243, 213, 91, 128};

int cpu_1_score = 0;
int cpu_2_score = 0;
int player_1_score = 0;
int player_2_score = 0;

void ScoreReset()
{
	cpu_1_score = 0;
	cpu_2_score = 0;
	player_1_score = 0;
	player_2_score = 0;
}

// class declarations
// --------------------------------------------------------------------------------
class Ball
{
public:
	int radius;
	float x_1, x_2, x_3, y_1, y_2, y_3;
	int speed_x, speed_y;

	void Draw()
	{
		DrawCircle(x_1, y_1, radius, yellow);
		DrawCircle(x_2, y_2, radius, yellow_shadow_1);
		DrawCircle(x_3, y_3, radius, yellow_shadow_2);
	}
	void Update(float speed_total)
	{
		x_3 = x_2;
		x_2 = x_1;
		x_1 = x_1 + (speed_x * speed_total);
		y_3 = y_2;
		y_2 = y_1;
		y_1 = y_1 + (speed_y * speed_total);

		if (y_1 + radius >= GetScreenHeight() || y_1 - radius <= 0)
		{
			speed_y *= -1;
		}
	}
	void Reset()
	{
		x_1 = GetScreenWidth() / 2;
		y_1 = GetScreenHeight() / 2;

		int speed_choices[2] = {-1, 1};
		speed_x *= speed_choices[GetRandomValue(0, 1)];
		speed_y *= speed_choices[GetRandomValue(0, 1)];
	}
};
class Paddle
{
protected:
	void LimitMovement()
	{
		if (y <= 0)
		{
			y = 0;
		}
		if (y + height >= GetScreenHeight())
		{
			y = GetScreenHeight() - height;
			;
		}
	}

public:
	float x, y;
	float width, height;
	int speed;

	void Draw()
	{
		DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
	}
	void Reset(int paddle_height)
	{
		y = GetScreenHeight() / 2 - paddle_height / 2;
	}
};
class PaddlePlayer1 : public Paddle
{
public:
	void Update(float speed_total)
	{
		if (IsKeyDown(KEY_W))
		{
			y = y - speed * speed_total;
		}
		if (IsKeyDown(KEY_S))
		{
			y = y + speed * speed_total;
		}
		LimitMovement();
	}
};
class PaddlePlayer2 : public Paddle
{
public:
	void Update(float speed_total)
	{
		if (IsKeyDown(KEY_I))
		{
			y = y - speed * speed_total;
		}
		if (IsKeyDown(KEY_K))
		{
			y = y + speed * speed_total;
		}
		LimitMovement();
	}
};
class PaddleCpu : public Paddle
{
public:
	void Update(int ball_y, float speed_total)
	{
		if (y + height / 2 > ball_y)
		{
			y = y - speed * speed_total;
		}
		if (y + height / 2 <= ball_y)
		{
			y = y + speed * speed_total;
		}
		LimitMovement();
	}
};
class Timer
{
public:
	double timestamp_start;
	double timestamp_end;
	double time_total;
	double time_old;
	float speed_total;
	float speed_old;

	void Update()
	{
		timestamp_end = GetTime();
		time_total = time_old + timestamp_end - timestamp_start;
		speed_total = speed_old + (timestamp_end - timestamp_start) / 60;
	}
	void Pause()
	{
		time_old = time_total;
		speed_old = speed_total;
	}
	void Resume()
	{
		timestamp_start = GetTime();
	}
	void Reset()
	{
		timestamp_start = GetTime();
		time_total = 0;
		time_old = 0;
		speed_total = 1;
		speed_old = 1;
	}
};

Ball ball;
PaddleCpu cpu_1_paddle;
PaddleCpu cpu_2_paddle;
PaddlePlayer1 player_1_paddle;
PaddlePlayer2 player_2_paddle;
Timer timer;

// --------------------------------------------------------------------------------
// entry point
// --------------------------------------------------------------------------------
int main()
{
	// initialization
	// --------------------------------------------------------------------------------
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "King Pong");
	SetTargetFPS(60);

	ball.radius = 20;
	ball.x_1 = SCREEN_WIDTH / 2;
	ball.x_2 = SCREEN_WIDTH / 2;
	ball.x_3 = SCREEN_WIDTH / 2;
	ball.y_1 = SCREEN_HEIGHT / 2;
	ball.y_2 = SCREEN_HEIGHT / 2;
	ball.y_3 = SCREEN_HEIGHT / 2;
	ball.speed_x = 7;
	ball.speed_y = 7;

	cpu_1_paddle.width = 24;
	cpu_1_paddle.height = 120;
	cpu_1_paddle.x = 10;
	cpu_1_paddle.y = SCREEN_HEIGHT / 2 - cpu_1_paddle.height / 2;
	cpu_1_paddle.speed = 6;

	cpu_2_paddle.width = 24;
	cpu_2_paddle.height = 120;
	cpu_2_paddle.x = SCREEN_WIDTH - cpu_2_paddle.width - 10;
	cpu_2_paddle.y = SCREEN_HEIGHT / 2 - cpu_2_paddle.height / 2;
	cpu_2_paddle.speed = 6;

	player_1_paddle.width = 24;
	player_1_paddle.height = 120;
	player_1_paddle.x = 10;
	player_1_paddle.y = SCREEN_HEIGHT / 2 - player_1_paddle.height / 2;
	player_1_paddle.speed = 6;

	player_2_paddle.width = 24;
	player_2_paddle.height = 120;
	player_2_paddle.x = SCREEN_WIDTH - player_2_paddle.width - 10;
	player_2_paddle.y = SCREEN_HEIGHT / 2 - player_2_paddle.height / 2;
	player_2_paddle.speed = 6;

	timer.timestamp_start = GetTime();
	timer.timestamp_end = GetTime();
	timer.time_total = 0;
	timer.time_old = 0;
	timer.speed_total = 1;
	timer.speed_old = 1;

	// game loop
	// --------------------------------------------------------------------------------
	while (!WindowShouldClose())
	{
		// update objects
		// --------------------------------------------------------------------------------
		auto CheckCollisionGoal = []()
		{
			if (ball.x_1 + ball.radius >= GetScreenWidth())
			{
				cpu_1_score += 1;
				player_1_score += 1;
				ball.Reset();
				timer.Reset();
			}
			else if (ball.x_1 - ball.radius <= 0)
			{
				cpu_2_score += 1;
				player_2_score += 1;
				ball.Reset();
				timer.Reset();
			}
		};
		auto CheckCollisionPaddle = [](Paddle player_paddle)
		{
			if (CheckCollisionCircleRec(Vector2{ball.x_1, ball.y_1}, ball.radius, Rectangle{player_paddle.x, player_paddle.y, player_paddle.width, player_paddle.height}))
			{
				ball.speed_x *= -1;
			}
		};
		auto ResetGame = []()
		{
			ball.Reset();
			cpu_1_paddle.Reset(cpu_1_paddle.height);
			cpu_2_paddle.Reset(cpu_2_paddle.height);
			player_1_paddle.Reset(player_1_paddle.height);
			player_2_paddle.Reset(player_2_paddle.height);
			ScoreReset();
			timer.Reset();
			is_game_new = false;
		};
		auto EndGame = [&ResetGame]()
		{
			is_screen_overlay_on = true;
			screen_overlay_current = TITLE;
			screen_gameplay_current = GAMEPLAY_2_CPU;
			ResetGame();
			is_game_new = true;
		};

		// handle gameplay
		switch (screen_gameplay_current)
		{
		case GAMEPLAY_2_CPU:
			ball.Update(timer.speed_total);
			cpu_1_paddle.Update(ball.y_1, timer.speed_total);
			cpu_2_paddle.Update(ball.y_1, timer.speed_total);
			CheckCollisionGoal();
			CheckCollisionPaddle(cpu_1_paddle);
			CheckCollisionPaddle(cpu_2_paddle);
			timer.Update();

			break;
		case GAMEPLAY_1_PLAYER:
			if (IsKeyPressed(KEY_P))
			{
				is_screen_overlay_on = true;
				screen_overlay_current = PAUSE;
				is_game_update_on = false;
				timer.Pause();
			}
			if (is_game_update_on)
			{
				ball.Update(timer.speed_total);
				player_1_paddle.Update(timer.speed_total);
				cpu_2_paddle.Update(ball.y_1, timer.speed_total);
				CheckCollisionGoal();
				CheckCollisionPaddle(player_1_paddle);
				CheckCollisionPaddle(cpu_2_paddle);
				timer.Update();
			}

			break;
		case GAMEPLAY_2_PLAYER:
			if (IsKeyPressed(KEY_P))
			{
				is_screen_overlay_on = true;
				screen_overlay_current = PAUSE;
				is_game_update_on = false;
			}
			if (is_game_update_on)
			{
				ball.Update(timer.speed_total);
				player_1_paddle.Update(timer.speed_total);
				player_2_paddle.Update(timer.speed_total);
				CheckCollisionGoal();
				CheckCollisionPaddle(player_1_paddle);
				CheckCollisionPaddle(player_2_paddle);
				timer.Update();
			}

			break;
		default:
			break;
		}

		// handle overlay
		if (is_screen_overlay_on)
		{
			switch (screen_overlay_current)
			{
			case TITLE:
				if (IsKeyPressed(KEY_ONE))
				{
					is_screen_overlay_on = false;
					screen_gameplay_current = GAMEPLAY_1_PLAYER;
					ResetGame();
				}
				else if (IsKeyPressed(KEY_TWO))
				{
					is_screen_overlay_on = false;
					screen_gameplay_current = GAMEPLAY_2_PLAYER;
					ResetGame();
				}
				else if (IsKeyPressed(KEY_C))
				{
					screen_overlay_current = CONTROLS;
				}

				break;
			case PAUSE:
				if (IsKeyPressed(KEY_R))
				{
					is_screen_overlay_on = false;
					is_game_update_on = true;
					timer.Resume();
				}
				else if (IsKeyPressed(KEY_X))
				{
					EndGame();
					is_game_update_on = true;
				}
				else if (IsKeyPressed(KEY_C))
				{
					screen_overlay_current = CONTROLS;
				}

				break;
			case CONTROLS:
				if (screen_gameplay_current == GAMEPLAY_2_CPU)
				{
					if (IsKeyPressed(KEY_B))
					{
						screen_overlay_current = TITLE;
					}
				}
				else
				{
					if (IsKeyPressed(KEY_B))
					{
						screen_overlay_current = PAUSE;
					}
				}

				break;
			default:
				break;
			}
		}

		// draw objects
		// --------------------------------------------------------------------------------
		const char *timer_text = TextFormat("%02i:%02i", static_cast<int>(timer.time_total) / 60, static_cast<int>(std::fmod(timer.time_total, 60)));
		const int timer_width = MeasureText(timer_text, font_size_h2);
		const char *speed_total_text = TextFormat("Speed: %.2f", floor(timer.speed_total * 100) / 100);
		const int speed_total_width = MeasureText(speed_total_text, font_size_p);

		auto DrawScoreLeft = [](int score)
		{
			DrawText(TextFormat("%i", score), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", score), font_size_h1) / 2), 20, font_size_h1, WHITE);
		};
		auto DrawScoreRight = [](int score)
		{
			DrawText(TextFormat("%i", score), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", score), font_size_h1) / 2), 20, font_size_h1, WHITE);
		};
		auto DrawPause = []()
		{
			const char *title_text = "Pause";
			const char *subtitle_text_1 = "Press r to resume";
			const char *subtitle_text_2 = "Press c to view controls";
			const char *subtitle_text_3 = "Press x to end game";

			const int title_width = MeasureText(title_text, font_size_h1);
			const int subtitle_width_1 = MeasureText(subtitle_text_1, font_size_h2);
			const int subtitle_width_2 = MeasureText(subtitle_text_2, font_size_h2);
			const int subtitle_width_3 = MeasureText(subtitle_text_3, font_size_h2);

			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, overlay);
			DrawText(title_text, SCREEN_WIDTH / 2 - title_width / 2, 250, font_size_h1, WHITE);
			DrawText(subtitle_text_1, SCREEN_WIDTH / 2 - subtitle_width_1 / 2, 350, font_size_h2, WHITE);
			DrawText(subtitle_text_2, SCREEN_WIDTH / 2 - subtitle_width_2 / 2, 400, font_size_h2, WHITE);
			DrawText(subtitle_text_3, SCREEN_WIDTH / 2 - subtitle_width_3 / 2, 500, font_size_h2, WHITE);
		};
		auto DrawControls = []()
		{
			const char *title_text = "Controls";
			const char *subtitle_text_1 = "w / s - move player 1";
			const char *subtitle_text_2 = "i / k - move player 2";
			const char *subtitle_text_3 = "Press b to go back";

			const int title_width = MeasureText(title_text, font_size_h1);
			const int subtitle_width_1 = MeasureText(subtitle_text_1, font_size_h2);
			const int subtitle_width_2 = MeasureText(subtitle_text_2, font_size_h2);
			const int subtitle_width_3 = MeasureText(subtitle_text_3, font_size_h2);

			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, overlay);
			DrawText(title_text, SCREEN_WIDTH / 2 - title_width / 2, 250, font_size_h1, WHITE);
			DrawText(subtitle_text_1, SCREEN_WIDTH / 2 - subtitle_width_1 / 2, 350, font_size_h2, WHITE);
			DrawText(subtitle_text_2, SCREEN_WIDTH / 2 - subtitle_width_2 / 2, 400, font_size_h2, WHITE);
			DrawText(subtitle_text_3, SCREEN_WIDTH / 2 - subtitle_width_3 / 2, 500, font_size_h2, WHITE);
		};

		BeginDrawing();
		ClearBackground(green_dark);
		DrawRectangle(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, green);
		DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 150, green_light);
		DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, WHITE);
		ball.Draw();
		DrawText(timer_text, SCREEN_WIDTH / 2 - timer_width / 2, 20, font_size_h2, WHITE);
		DrawText(speed_total_text, SCREEN_WIDTH / 2 - speed_total_width / 2, 64, font_size_p, WHITE);

		// handle gameplay
		switch (screen_gameplay_current)
		{
		case GAMEPLAY_2_CPU:
		{
			cpu_1_paddle.Draw();
			cpu_2_paddle.Draw();
			DrawScoreLeft(cpu_1_score);
			DrawScoreRight(cpu_2_score);

			if (screen_overlay_current == CONTROLS)
			{
				DrawControls();
			}
			else
			{
				const char *title_text = "King Pong";
				const char *subtitle_text_1 = "Press 1 for 1-player mode";
				const char *subtitle_text_2 = "Press 2 for 2-player mode";
				const char *subtitle_text_3 = "Press c to view controls";

				const int title_width = MeasureText(title_text, font_size_h1);
				const int subtitle_width_1 = MeasureText(subtitle_text_1, font_size_h2);
				const int subtitle_width_2 = MeasureText(subtitle_text_2, font_size_h2);
				const int subtitle_width_3 = MeasureText(subtitle_text_3, font_size_h2);

				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, overlay);
				DrawText(title_text, SCREEN_WIDTH / 2 - title_width / 2, 250, font_size_h1, WHITE);
				DrawText(subtitle_text_1, SCREEN_WIDTH / 2 - subtitle_width_1 / 2, 350, font_size_h2, WHITE);
				DrawText(subtitle_text_2, SCREEN_WIDTH / 2 - subtitle_width_2 / 2, 400, font_size_h2, WHITE);
				DrawText(subtitle_text_3, SCREEN_WIDTH / 2 - subtitle_width_3 / 2, 500, font_size_h2, WHITE);
			}

			break;
		}
		case GAMEPLAY_1_PLAYER:
			player_1_paddle.Draw();
			cpu_2_paddle.Draw();
			DrawScoreLeft(player_1_score);
			DrawScoreRight(cpu_2_score);

			if (is_screen_overlay_on)
			{
				if (screen_overlay_current == PAUSE)
				{
					DrawPause();
				}
				else if (screen_overlay_current == CONTROLS)
				{
					DrawControls();
				}
			}
			else
			{
				DrawText("Press p to pause", 10, SCREEN_HEIGHT - 30, font_size_p, WHITE);
			}

			break;
		case GAMEPLAY_2_PLAYER:
			player_1_paddle.Draw();
			player_2_paddle.Draw();
			DrawScoreLeft(player_1_score);
			DrawScoreRight(player_2_score);

			if (is_screen_overlay_on)
			{
				if (screen_overlay_current == PAUSE)
				{
					DrawPause();
				}
				else if (screen_overlay_current == CONTROLS)
				{
					DrawControls();
				}
			}
			else
			{
				DrawText("Press p to pause", 10, SCREEN_HEIGHT - 30, font_size_p, WHITE);
			}

			break;
		default:
			break;
		}

		EndDrawing();
	}

	// termination
	// --------------------------------------------------------------------------------
	CloseWindow();
	return 0;
}