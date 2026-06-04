#include <raylib.h>
#include <stdio.h>

void drawNet(int canvasWidth, int canvasHeight, int tileWidth, int tileHeight,
             int tileVeritcalMargin, int topMargin);
void resetBallPosition(int *ballPosX, int *ballPosY, int ballInitialPosX,
                       int ballInitialPosY);

int main(void) {
  // Screen
  const int screenWidth = 640;
  const int screenHeight = 480;
  const int innerBorder = 5;
  const int playerXMargin = 10;
  const int blackScreenWidth = screenWidth - (innerBorder * 2);
  const int blackScreenHeight = screenHeight - (innerBorder * 2);

  // Player general info
  const int playerHeight = 80;
  const int playerWidth = 20;
  const int playerVelocity = 15;
  const int playerUpperLimit = playerHeight / 3;
  const int playerCenterLimit = playerHeight / 3 * 2;

  // Player 1
  const int playerInitialPosY = screenHeight / 2 - playerHeight / 2;
  int playerPosX = innerBorder + playerXMargin;
  int playerPosY = playerInitialPosY;
  int playerScore = 0;

  // Player 2
  const int upperYCoordinateAllowed = innerBorder * 2;
  const int lowerYCoordinateAllowed = screenHeight - innerBorder - playerHeight;
  int player2PosX = screenWidth - (innerBorder + playerXMargin + playerWidth);
  int player2PosY = playerInitialPosY;
  int player2Score = 0;

  // Ball
  const int ballWidth = 20;
  const int ballHeight = 20;
  const int ballVelocity = 10;
  const int lowerYBallCoordinateAllowed =
      screenHeight - innerBorder - ballHeight;
  const int upperYBallCoordinateAllowed = innerBorder;
  const int ballInitialPosX = (screenWidth / 2) - (ballWidth / 2);
  const int ballInitialPosY = (screenHeight / 2) - ballHeight;
  int ballPosX = ballInitialPosX;
  int ballPosY = ballInitialPosY;
  int ballXVelocity = ballVelocity;
  int ballYVelocity = ballVelocity;
  int ballRightEndX = 0;
  int ballRightEndY = 0;

  // Countdown
  const int countdownFontSize = 82;
  const int countdownPosX = screenWidth / 2 - countdownFontSize / 2;
  const int countdownPosY = screenHeight / 2 - countdownFontSize / 2;
  char showCountdown = 0;
  int countdown = 3;
  double countdownStartTime = 0.0;

  // Various cache
  double currentTime = 0;

  InitWindow(screenWidth, screenHeight,
             "PONGO - the best Italian pong game ever made");
  SetTargetFPS(50);

  printf("playerUpperLimit: %d\n", playerUpperLimit);
  printf("playerCenterLimit: %d\n", playerCenterLimit);

  while (!WindowShouldClose()) {
    ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);

    // Input control
    struct Vector2 mouseDelta = GetMouseDelta();
    float deltaY = mouseDelta.y;
    if (deltaY > 0)
      deltaY = playerVelocity;
    else if (deltaY < 0)
      deltaY = -playerVelocity;

    // Move and clamp the vertical position of player 1
    if (deltaY > 0) {
      if (playerPosY + deltaY > lowerYCoordinateAllowed)
        playerPosY = lowerYCoordinateAllowed;
      else
        playerPosY += deltaY;
    } else {
      if (playerPosY + deltaY < upperYCoordinateAllowed)
        playerPosY = upperYCoordinateAllowed;
      else
        playerPosY += deltaY;
    }

    // Check collision with walls
    ballPosY = ballPosY + ballYVelocity;
    if (ballPosY < innerBorder) // Top wall
      ballYVelocity = ballVelocity;
    else if (ballPosY > lowerYBallCoordinateAllowed) // Bottom wall
      ballYVelocity = -ballVelocity;

    // Move opponent
    if (ballXVelocity < 0)
      player2PosY +=
          player2PosY > playerInitialPosY ? -playerVelocity : +playerVelocity;
    else
      player2PosY += (ballPosY - player2PosY) / 2;

    // if(player2PosY > ballPosY){
    //     player2PosY -= playerVelocity / 2;
    // } else if (player2PosY < ballPosY){
    //     player2PosY +=  playerVelocity / 2;
    // }

    ballPosX = ballPosX + ballXVelocity;
    if (ballPosX < innerBorder) { // Left wall - POINT for player 2!
      resetBallPosition(&ballPosX, &ballPosY, ballInitialPosX, ballInitialPosY);
      ballXVelocity = 0;
      ballYVelocity = 0;
      player2Score++;
      player2PosY = ballInitialPosY;

      // start countdown
      showCountdown = true;
      countdownStartTime = GetTime();
    } else if (ballPosX >
               screenWidth - ballWidth) { // Right wall - POINT for player 1!
      resetBallPosition(&ballPosX, &ballPosY, ballInitialPosX, ballInitialPosY);
      ballXVelocity = 0;
      ballYVelocity = 0;
      playerScore++;
      player2PosY = ballInitialPosY;

      // start countdown
      showCountdown = true;
      countdownStartTime = GetTime();
    }

    int offset;
    int XVelocity = ballVelocity;
    int YVelocity = ballVelocity;
    // Check collision with player 1 and 2
    if (ballXVelocity > 0) { // ball going right
      ballRightEndX = ballPosX + ballWidth;
      ballRightEndY = ballPosY + ballHeight / 2;
      offset = ballRightEndY - player2PosY;

      if (ballRightEndY > player2PosY &&
          ballRightEndY < player2PosY + playerHeight) {
        if (offset < playerUpperLimit || offset > playerCenterLimit) {
          XVelocity = XVelocity * 0.7;
          YVelocity = YVelocity * 0.7;
        }
        if (ballRightEndX > player2PosX) {
          // Collision with opponent happened!
          printf("pleyer 2 offset:%d\n", offset);

          ballXVelocity = -XVelocity;
          ballYVelocity = -YVelocity;
        }
      }
    }

    offset = ballPosY - playerPosY;
    XVelocity = ballVelocity;
    YVelocity = ballVelocity;
    if (ballXVelocity < 0) { // ball going left
      if (ballPosY > playerPosY && ballPosY < playerPosY + playerHeight) {
        if (offset < playerUpperLimit || offset > playerCenterLimit) {
          XVelocity = XVelocity * 0.7;
          YVelocity = YVelocity * 0.7;
        }
        if (ballPosX < playerPosX + ballWidth) {
          printf("pleyer 1 offset:%d\n", offset);
          ballXVelocity = +XVelocity;
          ballYVelocity = +YVelocity;
        }
      }
    }

    // Countdown handling
    if (showCountdown) {
      currentTime = GetTime() - countdownStartTime;

      if (currentTime > 1) {
        countdown--;
        countdownStartTime = GetTime();
        if (countdown == 0) {
          showCountdown = false;
          ballXVelocity = ballVelocity;
          ballYVelocity = ballVelocity;
          countdown = 3;
        }
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawRectangle(innerBorder, innerBorder, screenWidth - innerBorder,
                  screenHeight - innerBorder, BLACK);

    drawNet(blackScreenWidth, blackScreenHeight, 6, 12, 3, innerBorder);
    DrawRectangle(playerPosX, playerPosY, playerWidth, playerHeight, LIGHTGRAY);
    DrawRectangle(player2PosX, player2PosY, playerWidth, playerHeight,
                  LIGHTGRAY);
    DrawRectangle(ballPosX, ballPosY, ballWidth, ballHeight, LIGHTGRAY);

    DrawText(TextFormat("%02i", playerScore), 200, innerBorder * 2, 36, WHITE);
    DrawText(TextFormat("%02i", player2Score), 400, innerBorder * 2, 36, WHITE);

    if (showCountdown) {
      DrawText(TextFormat("%01i", countdown), countdownPosX, countdownPosY,
               countdownFontSize, RED);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void resetBallPosition(int *ballPosX, int *ballPosY, int ballInitialPosX,
                       int ballInitialPosY) {
  *ballPosX = ballInitialPosX;
  *ballPosY = ballInitialPosY;
}

void drawNet(int canvasWidth, int canvasHeight, int tileWidth, int tileHeight,
             int tileVeritcalMargin, int topMargin) {
  int netX = (canvasWidth / 2) - tileWidth / 2;
  int numberOfTiles = canvasHeight / (tileHeight + tileVeritcalMargin);

  for (int i = 0; i < numberOfTiles; i++) {
    int posY = topMargin + ((tileHeight + tileVeritcalMargin) * i + 1);

    // DrawRectangle(int posX, int posY, int width, int height, Color color);
    DrawRectangle(netX, posY, tileWidth, tileHeight, LIGHTGRAY);
  }
}
