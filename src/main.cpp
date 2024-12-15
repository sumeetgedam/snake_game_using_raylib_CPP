#include <raylib.h>
#include<iostream>
#include <deque>
#include<raymath.h>

using namespace std;


Color white = {255, 255, 255, 255 };
Color black = {0, 0, 0, 255};
Color red = {230, 41, 55, 255};

int cellSize = 30;
int cellCount = 25;

int offSet = 75;


double lastUpdateTime =  0;
bool eventTrigger(double interval){
    double currentTime  = GetTime();
    if((currentTime - lastUpdateTime >= interval)){
        lastUpdateTime=currentTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, deque<Vector2> deque){

   for(unsigned int i = 0; i < deque.size(); i++){
        if(Vector2Equals(element, deque[i])){
            return true;
        }
   }
   return false;
}

class Snake
{
    
    public:
        deque<Vector2> snakeBody = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        
        Vector2 down = {0, 1};
        Vector2 direction = {1, 0};

        bool addSegment = false;
        void Draw(){
            for(unsigned int i = 0; i < snakeBody.size(); i++){
                float x = snakeBody[i].x;
                float y = snakeBody[i].y;
                Rectangle rec = Rectangle{offSet + x * cellSize, offSet+y * cellSize, 
                                (float)cellSize, (float)cellSize};

                DrawRectangleRounded(rec, 0.5, 6, black);
            }
        }

        void updateSnake(){

            snakeBody.push_front(Vector2Add(snakeBody[0],direction));
            if(addSegment == true){
                
                addSegment = false;
            }else{

                snakeBody.pop_back();
                
            }
            
        }

        void reset(){
            snakeBody = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
            direction = {1, 0};
        }

};


class Food{
    public:
        Vector2 foodPosition;

        Food(deque<Vector2> snakeBody){
            foodPosition = genrateRandomPosition(snakeBody);
        }

        ~Food(){

        }

        void Draw(){
            // DrawRectangle(foodPosition.x * cellSize, foodPosition.y * cellSize
            //                 , cellSize, cellSize, red);

            Rectangle foodRec = Rectangle{offSet + foodPosition.x * cellSize, offSet + foodPosition.y * cellSize, 
                                (float)cellSize, (float)cellSize};

            DrawRectangleRounded(foodRec, 0.5, 6, red);
            
        }

        Vector2 generateRandomCell(){
            float x = GetRandomValue(0, cellCount -1 );
            float y = GetRandomValue(0, cellCount - 1 );

           return Vector2{x, y};
        }

        Vector2 genrateRandomPosition(deque<Vector2> snakeBody){
            
            Vector2 position =generateRandomCell();
            while(elementInDeque(position, snakeBody)){
                position = generateRandomCell();
            }
            return position;
        }

        
};

class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.snakeBody);
        bool running = true;
        int score = 0;
        Sound eatSound;
        Sound wallSound;

        Game(){
            InitAudioDevice();
            eatSound = LoadSound("src/sounds/eat.mp3");
            wallSound = LoadSound("src/sounds/wall.mp3");
            
        }

        ~Game(){
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice();
        }

        void Draw(){
            food.Draw();
            snake.Draw();
        }

        void updateBoard(){

            if(running){
                snake.updateSnake();
                checkCollisionWithFood();
                checkCollisionWithEdges();
                checkCollisionWithBody();
            }
        }

        void checkCollisionWithFood(){
            if(Vector2Equals(snake.snakeBody[0], food.foodPosition)){
                // cout << "Snake ate the food" << endl;

                food.foodPosition = food.genrateRandomPosition(snake.snakeBody);
                snake.addSegment = true;
                score++;
                PlaySound(eatSound);
            }
        }
        void gameOver(){
            // cout << "Collision occurred, Game Over!!!!" << endl;
            // Todo - add a pop-up to ask user idf they want to replay or exit
            // make changes accordingly
            snake.reset();
            food.foodPosition = food.genrateRandomPosition(snake.snakeBody);
            running = false;
            score = 0;
            PlaySound(wallSound);
        }

        void checkCollisionWithEdges(){
            if(snake.snakeBody[0].x == cellCount || snake.snakeBody[0].x == -1){
                gameOver();
            }
            if(snake.snakeBody[0].y == cellCount || snake.snakeBody[0].y == -1){
                gameOver();
            }
        }

        void checkCollisionWithBody(){
            deque<Vector2> headlessBody = snake.snakeBody;

            headlessBody.pop_front();
            if(elementInDeque(snake.snakeBody[0], headlessBody)){
                gameOver();
            }
        }
};

int main() 
{
    cout << "Retro Snake Game using raylib..... " << endl;

    InitWindow(((2 * offSet) + cellSize * cellCount), 
                ((2  * offSet) + cellSize * cellCount), "Retro Snake Game");

    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false){
        BeginDrawing();

        if(eventTrigger(0.2)){
            game.updateBoard();
        }
        
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
            game.running = true;
        }

        if(IsKeyPressed(KEY_DOWN)  && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
            game.running = true;
        }

        if(IsKeyPressed(KEY_LEFT)  && game.snake.direction.x != 1){
            game.snake.direction = {-1, 0};
            game.running = true;
        }

        if(IsKeyPressed(KEY_RIGHT)  && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
            game.running = true;
        }

        ClearBackground(white);
        DrawRectangleLinesEx(Rectangle{(float)offSet-5, (float)offSet-5, 
                                        (float)cellSize*cellCount+10,(float)cellSize*cellCount+10}
                            , 5, black);
        
        DrawText("Retro Snake", offSet-5, 20, 40, black);
        DrawText(TextFormat("Score : %i", game.score), cellSize*cellCount-100, 20,  40, black);
        game.Draw();
        
        EndDrawing();
    }


    CloseWindow();

    return 0;
}