#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <functional>

const int CELL_SIZE = 50;
const int ROWS = 15;
const int COLS = 20;
const int WINDOW_WIDTH = COLS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

enum CellType { WALL, PATH, START, END, VISITED, CURRENT };
CellType maze[ROWS][COLS];

sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The MAZE - Search Algorithm Visualizer");
sf::Font font;

void initializeMaze() {
    srand(time(0));
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            maze[i][j] = (rand() % 3 == 0) ? WALL : PATH;
        }
    }
    maze[0][0] = START;
    maze[ROWS-1][COLS-1] = END;
}

void drawMaze() {
    window.clear(sf::Color::White);
    
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
    
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            
            switch(maze[i][j]) {
                case WALL:
                    cell.setFillColor(sf::Color::Black);
                    break;
                case PATH:
                    cell.setFillColor(sf::Color::White);
                    break;
                case START:
                    cell.setFillColor(sf::Color::Green);
                    break;
                case END:
                    cell.setFillColor(sf::Color::Red);
                    break;
                case VISITED:
                    cell.setFillColor(sf::Color::Yellow);
                    break;
                case CURRENT:
                    cell.setFillColor(sf::Color::Blue);
                    break;
            }
            
            window.draw(cell);
        }
    }
    
    window.display();
}

void animateStep(int row, int col) {
    maze[row][col] = CURRENT;
    drawMaze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    maze[row][col] = VISITED;
}

bool isValid(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS &&
           (maze[row][col] == PATH || maze[row][col] == END);
}

void bfs() {
    std::queue<std::pair<int, int>> q;
    q.push({0, 0});
    
    while(!q.empty()) {
        auto [row, col] = q.front();
        q.pop();
        
        if(maze[row][col] == END) {
            std::cout << "Path found (BFS)!\n";
            return;
        }
        
        animateStep(row, col);
        
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        
        for(int i = 0; i < 4; i++) {
            int newRow = row + dr[i];
            int newCol = col + dc[i];
            
            if(isValid(newRow, newCol)) {
                q.push({newRow, newCol});
                if(maze[newRow][newCol] != END) {
                    maze[newRow][newCol] = VISITED;
                }
            }
        }
    }
    
    std::cout << "No path found (BFS).\n";
}

void dfs() {
    std::stack<std::pair<int, int>> s;
    s.push({0, 0});
    
    while(!s.empty()) {
        auto [row, col] = s.top();
        s.pop();
        
        if(maze[row][col] == END) {
            std::cout << "Path found (DFS)!\n";
            return;
        }
        
        animateStep(row, col);
        
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        
        for(int i = 0; i < 4; i++) {
            int newRow = row + dr[i];
            int newCol = col + dc[i];
            
            if(isValid(newRow, newCol)) {
                s.push({newRow, newCol});
                if(maze[newRow][newCol] != END) {
                    maze[newRow][newCol] = VISITED;
                }
            }
        }
    }
    
    std::cout << "No path found (DFS).\n";
}

int manhattan(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

void astar() {
    auto cmp = [](const std::pair<std::pair<int, int>, int>& a, const std::pair<std::pair<int, int>, int>& b) {
        return a.second > b.second;
    };
    std::priority_queue<std::pair<std::pair<int, int>, int>, std::vector<std::pair<std::pair<int, int>, int>>, decltype(cmp)> pq(cmp);
    
    pq.push({{0, 0}, manhattan(0, 0, ROWS-1, COLS-1)});
    
    while(!pq.empty()) {
        auto [pos, _] = pq.top();
        auto [row, col] = pos;
        pq.pop();
        
        if(maze[row][col] == END) {
            std::cout << "Path found (A*)!\n";
            return;
        }
        
        animateStep(row, col);
        
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        
        for(int i = 0; i < 4; i++) {
            int newRow = row + dr[i];
            int newCol = col + dc[i];
            
            if(isValid(newRow, newCol)) {
                int newCost = manhattan(newRow, newCol, ROWS-1, COLS-1);
                pq.push({{newRow, newCol}, newCost});
                if(maze[newRow][newCol] != END) {
                    maze[newRow][newCol] = VISITED;
                }
            }
        }
    }
    
    std::cout << "No path found (A*).\n";
}

void resetMaze() {
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            if(maze[i][j] == VISITED || maze[i][j] == CURRENT) {
                maze[i][j] = PATH;
            }
        }
    }
}

void drawMenu() {
    sf::Text title("The MAZE - Search Algorithm Visualizer", font, 24);
    title.setPosition(10, 10);
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    sf::Text instructions("Press: B - BFS | D - DFS | A - A* | R - Reset | ESC - Exit", font, 18);
    instructions.setPosition(10, WINDOW_HEIGHT - 30);
    instructions.setFillColor(sf::Color::Black);
    window.draw(instructions);
}

int main() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    initializeMaze();
    
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::B:
                        resetMaze();
                        bfs();
                        break;
                    case sf::Keyboard::D:
                        resetMaze();
                        dfs();
                        break;
                    case sf::Keyboard::A:
                        resetMaze();
                        astar();
                        break;
                    case sf::Keyboard::R:
                        initializeMaze();
                        break;
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    default:
                        break;
                }
            }
        }
        
        drawMaze();
        drawMenu();
        window.display();
    }
    
    return 0;
}