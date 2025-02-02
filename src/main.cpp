#include <iostream>
#include <raylib.h>

typedef enum
{
	TITLE,
	MODE_1_PLAYER,
	MODE_2_PLAYER
} screen_t;

Color green = Color{38, 185, 154, 255};
Color green_dark = Color{20, 160, 133, 255};
Color green_light = Color{129, 204, 184, 255};
Color yellow = Color{243, 213, 91, 255};

int score_player1 = 0;
int score_cpu = 0;
int score_player2 = 0;

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
			score_player1 += 1;
			ResetBall();
		}
		if (x - radius <= 0)
		{
			score_player2 += 1;
			score_cpu += 1;
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
PaddlePlayer1 player1;
PaddlePlayer2 player2;
PaddleCpu cpu;

// --------------------
// entry point
// --------------------
int main()
{
	// initialization
	// --------------------
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "my pong game");
	SetTargetFPS(60);

	screen_t screen_current = TITLE;

	int font_size = 80;
	const char *text = "My Pong Game";
	const int text_width_x = MeasureText(text, font_size);

	ball.radius = 20;
	ball.x = SCREEN_WIDTH / 2;
	ball.y = SCREEN_HEIGHT / 2;
	ball.speed_x = 7;
	ball.speed_y = 7;

	player1.width = 25;
	player1.height = 120;
	player1.x = 10;
	player1.y = SCREEN_HEIGHT / 2 - player1.height / 2;
	player1.speed = 6;

	player2.width = 25;
	player2.height = 120;
	player2.x = SCREEN_WIDTH - player2.width - 10;
	player2.y = SCREEN_HEIGHT / 2 - player2.height / 2;
	player2.speed = 6;

	cpu.width = 25;
	cpu.height = 120;
	cpu.x = SCREEN_WIDTH - cpu.width - 10;
	cpu.y = SCREEN_HEIGHT / 2 - cpu.height / 2;
	cpu.speed = 6;

	// game loop
	// --------------------
	while (!WindowShouldClose())
	{
		// update objects
		// --------------------
		switch (screen_current)
		{
		case TITLE:
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
			player1.Update();
			cpu.Update(ball.y);

			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
			{
				ball.speed_x *= -1;
			}
			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player1.x, player1.y, player1.width, player1.height}))
			{
				ball.speed_x *= -1;
			}

			break;
		case MODE_2_PLAYER:
			ball.Update();
			player1.Update();
			player2.Update();

			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player1.x, player1.y, player1.width, player1.height}))
			{
				ball.speed_x *= -1;
			}
			if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player2.x, player2.y, player2.width, player2.height}))
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
			DrawText(text, SCREEN_WIDTH / 2 - text_width_x / 2, SCREEN_HEIGHT / 2 - font_size / 2, font_size, WHITE);

			break;
		case MODE_1_PLAYER:
			ball.Draw();
			player1.Draw();
			cpu.Draw();
			DrawText(TextFormat("%i", score_player1), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", score_player1), font_size) / 2), 20, font_size, WHITE);
			DrawText(TextFormat("%i", score_cpu), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", score_cpu), font_size) / 2), 20, font_size, WHITE);

			break;
		case MODE_2_PLAYER:
			ball.Draw();
			player1.Draw();
			player2.Draw();
			DrawText(TextFormat("%i", score_player1), (SCREEN_WIDTH / 4) - (MeasureText(TextFormat("%i", score_player1), font_size) / 2), 20, font_size, WHITE);
			DrawText(TextFormat("%i", score_player2), (SCREEN_WIDTH * 3 / 4) - (MeasureText(TextFormat("%i", score_player2), font_size) / 2), 20, font_size, WHITE);

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