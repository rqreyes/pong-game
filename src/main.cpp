#include <iostream>
#include <raylib.h>

typedef enum
{
	TITLE,
	MODE_1_PLAYER,
	MODE_2_PLAYER
} screen_t;

const int font_size_h1 = 80;
const int font_size_h2 = 40;
const int font_size_p = 20;

Color overlay = Color{0, 0, 0, 180};
Color green = Color{38, 185, 154, 255};
Color green_dark = Color{20, 160, 133, 255};
Color green_light = Color{129, 204, 184, 255};
Color yellow = Color{243, 213, 91, 255};

int cpu_1_score = 0;
int cpu_2_score = 0;
int player_1_score = 0;
int player_2_score = 0;

class Ball
{
public:
	float x, y;
	int speed_x, speed_y;
	int radius;

	void Draw()
	{
		DrawCircle(x, y, radius, yellow);
	}
	void Update()
	{
		x += speed_x;
		y += speed_y;

		if (y + radius >= GetScreenHeight() || y - radius <= 0)
		{
			speed_y *= -1;
		}
		if (x + radius >= GetScreenWidth())
		{
			cpu_1_score += 1;
			player_1_score += 1;
			ResetBall();
		}
		if (x - radius <= 0)
		{
			cpu_2_score += 1;
			player_2_score += 1;
			ResetBall();
		}
	}
	void ResetBall()
	{
		x = GetScreenWidth() / 2;
		y = GetScreenHeight() / 2;

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
};
class PaddlePlayer1 : public Paddle
{
public:
	void Update()
	{
		if (IsKeyDown(KEY_W))
		{
			y -= speed;
		}
		if (IsKeyDown(KEY_S))
		{
			y += speed;
		}
		LimitMovement();
	}
};
class PaddlePlayer2 : public Paddle
{
public:
	void Update()
	{
		if (IsKeyDown(KEY_I))
		{
			y -= speed;
		}
		if (IsKeyDown(KEY_K))
		{
			y += speed;
		}
		LimitMovement();
	}
};
class PaddleCpu : public Paddle
{
public:
	void Update(int ball_y)
	{
		if (y + height / 2 > ball_y)
		{
			y -= speed;
		}
		if (y + height / 2 <= ball_y)
		{
			y += speed;
		}
		LimitMovement();
	}
};

Ball ball;
PaddleCpu cpu_1_paddle;
PaddleCpu cpu_2_paddle;
PaddlePlayer1 player_1_paddle;
PaddlePlayer2 player_2_paddle;

// --------------------
// entry point
// --------------------
int main()
{
	// initialization
	// --------------------
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My Pong Game");
	SetTargetFPS(60);

	screen_t screen_current = TITLE;

	ball.radius = 20;
	ball.x = SCREEN_WIDTH / 2;
	ball.y = SCREEN_HEIGHT / 2;
	ball.speed_x = 7;
	ball.speed_y = 7;

	cpu_1_paddle.width = 25;
	cpu_1_paddle.height = 120;
	cpu_1_paddle.x = 10;
	cpu_1_paddle.y = SCREEN_HEIGHT / 2 - cpu_1_paddle.height / 2;
	cpu_1_paddle.speed = 6;

	cpu_2_paddle.width = 25;
	cpu_2_paddle.height = 120;
	cpu_2_paddle.x = SCREEN_WIDTH - cpu_2_paddle.width - 10;
	cpu_2_paddle.y = SCREEN_HEIGHT / 2 - cpu_2_paddle.height / 2;
	cpu_2_paddle.speed = 6;

	player_1_paddle.width = 25;
	player_1_paddle.height = 120;
	player_1_paddle.x = 10;
	player_1_paddle.y = SCREEN_HEIGHT / 2 - player_1_paddle.height / 2;
	player_1_paddle.speed = 6;

	player_2_paddle.width = 25;
	player_2_paddle.height = 120;
	player_2_paddle.x = SCREEN_WIDTH - player_2_paddle.width - 10;
	player_2_paddle.y = SCREEN_HEIGHT / 2 - player_2_paddle.height / 2;
	player_2_paddle.speed = 6;

	// game loop
	// --------------------
	while (!WindowShouldClose())
	{
		// update objects
		// --------------------
		switch (screen_current)
		{
		case TITLE:
			ball.Update();
			cpu_1_paddle.Update(ball.y);
			cpu_2_paddle.Update(ball.y);

			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu_1_paddle.x, cpu_1_paddle.y, cpu_1_paddle.width, cpu_1_paddle.height}))
			{
				ball.speed_x *= -1;
			}
			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu_2_paddle.x, cpu_2_paddle.y, cpu_2_paddle.width, cpu_2_paddle.height}))
			{
				ball.speed_x *= -1;
			}
			if (IsKeyPressed(KEY_ONE))
			{
				screen_current = MODE_1_PLAYER;
			}
			else if (IsKeyPressed(KEY_TWO))
			{
				screen_current = MODE_2_PLAYER;
			}

			break;
		case MODE_1_PLAYER:
			ball.Update();
			player_1_paddle.Update();
			cpu_2_paddle.Update(ball.y);

			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player_1_paddle.x, player_1_paddle.y, player_1_paddle.width, player_1_paddle.height}))
			{
				ball.speed_x *= -1;
			}
			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu_2_paddle.x, cpu_2_paddle.y, cpu_2_paddle.width, cpu_2_paddle.height}))
			{
				ball.speed_x *= -1;
			}

			break;
		case MODE_2_PLAYER:
			ball.Update();
			player_1_paddle.Update();
			player_2_paddle.Update();

			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player_1_paddle.x, player_1_paddle.y, player_1_paddle.width, player_1_paddle.height}))
			{
				ball.speed_x *= -1;
			}
			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player_2_paddle.x, player_2_paddle.y, player_2_paddle.width, player_2_paddle.height}))
			{
				ball.speed_x *= -1;
			}

			break;
		default:
			break;
		}

		// draw objects
		// --------------------
		BeginDrawing();
		ClearBackground(green_dark);
		DrawRectangle(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, green);
		DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 150, green_light);
		DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, WHITE);

		switch (screen_current)
		{
		case TITLE:
		{
			const char *title_text = "My Pong Game";
			const int title_width = MeasureText(title_text, font_size_h1);
			const char *subtitle_text_1 = "Press 1 for 1-player mode";
			const int subtitle_width_1 = MeasureText(subtitle_text_1, font_size_h2);
			const char *subtitle_text_2 = "Press 2 for 2-player mode";
			const int subtitle_width_2 = MeasureText(subtitle_text_2, font_size_h2);

			ball.Draw();
			cpu_1_paddle.Draw();
			cpu_2_paddle.Draw();
			DrawText(TextFormat("%i", cpu_1_score), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", cpu_1_score), font_size_h1) / 2), 20, font_size_h1, WHITE);
			DrawText(TextFormat("%i", cpu_2_score), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", cpu_2_score), font_size_h1) / 2), 20, font_size_h1, WHITE);

			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, overlay);
			DrawText(title_text, SCREEN_WIDTH / 2 - title_width / 2, 300, font_size_h1, WHITE);
			DrawText(subtitle_text_1, SCREEN_WIDTH / 2 - subtitle_width_1 / 2, 400, font_size_h2, WHITE);
			DrawText(subtitle_text_2, SCREEN_WIDTH / 2 - subtitle_width_2 / 2, 450, font_size_h2, WHITE);

			break;
		}
		case MODE_1_PLAYER:
			ball.Draw();
			player_1_paddle.Draw();
			cpu_2_paddle.Draw();
			DrawText(TextFormat("%i", player_1_score), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", player_1_score), font_size_h1) / 2), 20, font_size_h1, WHITE);
			DrawText(TextFormat("%i", cpu_2_score), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", cpu_2_score), font_size_h1) / 2), 20, font_size_h1, WHITE);

			break;
		case MODE_2_PLAYER:
			ball.Draw();
			player_1_paddle.Draw();
			player_2_paddle.Draw();
			DrawText(TextFormat("%i", player_1_score), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", player_1_score), font_size_h1) / 2), 20, font_size_h1, WHITE);
			DrawText(TextFormat("%i", player_2_score), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", player_2_score), font_size_h1) / 2), 20, font_size_h1, WHITE);

			break;
		default:
			break;
		}

		EndDrawing();
	}

	// termination
	// --------------------
	CloseWindow();
	return 0;
}