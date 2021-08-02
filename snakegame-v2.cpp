#include<bits/stdc++.h>
#include<windows.h>
#include <conio.h>
using namespace std;

const int MAXN = 100;
const int WIDTH  = 60;     // 界面宽度
const int HEIGHT = 20;     // 界面长度

class node{
public:
    short x, y;
    // node(int a, int b){
    //     x = a, y = b;
    // }
    bool operator !=(const node& b){
        return (x != b.x) || (y != b.y);
    }
    bool operator ==(const node& b){
        return x == b.x && y == b.y;
    }
} head, tail;

char GUI[MAXN][MAXN];      // 界面图形数组
short direc[MAXN][MAXN];   // 界面流场数组。上1下2左3右4
bool food[MAXN][MAXN];     // 界面食物数组
int gametime;
short x=3, y=3;
HANDLE hOut; COORD pos;

void init(); // 初始化
void draw();   // 绘制结果
void draw_interface(); // 绘制界面
void draw_snake(); // 绘制蛇
void draw_point(node p, string s); // 绘制一个坐标点
void release_point(node p);        // 清除一个坐标点
void move_point(node& p);          // 按照流场方向更新一步坐标点
void show();       // 把绘制结果输出到命令行
bool is_legal();   // 判断合法性
void move();       // 进入下一个游戏状态

int main(){
    init();
    char order;
    while(1){
        if (_kbhit()){    // 键盘监控
            order = _getch();
        }
        int wait = 50;   // 刷新率 = 1000/50 = 20 Hz
        Sleep(wait); gametime += wait;
        if(gametime % (wait * 20) == 0){
            node rand_node;
            rand_node.x = rand()%HEIGHT, rand_node.y = rand()%WIDTH;
            food[rand_node.x][rand_node.y] = true;
            draw_point(rand_node, "food");
        }

        if(order == 'w')
            direc[head.x][head.y] = (direc[head.x][head.y] == 2) ? 2 : 1;
        else if(order == 's')
            direc[head.x][head.y] = (direc[head.x][head.y] == 1) ? 1 : 2;
        else if(order == 'a')
            direc[head.x][head.y] = (direc[head.x][head.y] == 4) ? 4 : 3;
        else if(order == 'd')
            direc[head.x][head.y] = (direc[head.x][head.y] == 3) ? 3 : 4;
        order = 'F';
        move();
        draw();

        hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取句柄
        x = 15; y = HEIGHT + 2;
        pos = {x, y};
        SetConsoleCursorPosition(hOut, pos); //移动光标
        cout << double(gametime) * 0.001 <<endl;
    }
}

void init(){
    tail.x = HEIGHT / 2, tail.y = 2;
    head.x = HEIGHT / 2, head.y = 3;
    // food[HEIGHT / 2][5] = food[HEIGHT / 2][11] = food[HEIGHT / 2][16] = food[HEIGHT / 2][17] = food[HEIGHT / 2][26] = food[HEIGHT / 2][40] = food[HEIGHT / 2][50] = true;
    direc[tail.x][tail.y] = direc[head.x][head.y] = 4;
    draw_interface();
    draw();
    show();

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
    CursorInfo.bVisible = false; //隐藏控制台光标
    SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
}

void move(){
    // 头部
    short tmp = direc[head.x][head.y];
    move_point(head); direc[head.x][head.y] = tmp;
    
    // 吃东西
    if(food[head.x][head.y]){
        food[head.x][head.y] = false; // 吃掉
    } else{
        // 尾部
        int tmpx = tail.x, tmpy = tail.y;
        release_point(tail);
        move_point(tail);  direc[tmpx][tmpy] = 0;
    }
}

void draw(){
    draw_snake();
}

void show(){
    system("cls");
    for(int i=0;i<=HEIGHT+1;i++){
        for(int j=0;j<=WIDTH+1;j++)
            cout <<GUI[i][j];
        cout << endl;
    }
    cout << "game time (s): " << double(gametime) * 0.001 <<endl;
}

void draw_interface(){
    // cout << "draw_inteface" <<endl;
    GUI[0][0] = '/';
    for(int i=1;i<=WIDTH;i++) GUI[0][i] = '-';
    GUI[0][WIDTH+1] = '\\';
    for(int i=1;i<=HEIGHT;i++) {
        GUI[i][0] = '|';
        for(int j=1;j<=WIDTH;j++)  {
            if(food[i][j])
                GUI[i][j] = '@';
            else GUI[i][j] = ' ';
        }
        GUI[i][WIDTH+1] = '|';
    }
    GUI[HEIGHT+1][0] = '\\';
    for(int i=1;i<=WIDTH;i++) GUI[HEIGHT+1][i] = '-';
    GUI[HEIGHT+1][WIDTH+1] = '/';
}

void draw_snake(){
    // tail.x = 5, tail.y = 5;
    // head.x = 10, head.y = 7;
    // direc[5][5] = 4, direc[5][6] = 4;
    // direc[5][7] = 2, direc[6][7] = 2,direc[7][7] = 2;
    // direc[8][7] = 3, direc[8][6] = 3, direc[8][5] = 3;
    // direc[8][4] = 2, direc[9][4] = 2;
    // direc[10][4] = 4, direc[10][5] = 4, direc[10][6] = 4, direc[10][7] = 4;
    // 从尾巴开始按照矩阵中记载的流线绘制
    // cout << "draw_snake" <<endl;
    draw_point(tail, "tail");
    draw_point(head, "head");
    node tmp = tail;
    while(1){if(tmp == head) break;
        move_point(tmp);
        if(tmp != tail && tmp != head)
            draw_point(tmp, "body");
    }
}


void draw_point(node p, string s){
    char t;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取句柄
    pos = {p.y, p.x};
    SetConsoleCursorPosition(hOut, pos); //移动光标
    if(s == "head")  cout << 'O';
    if(s == "tail")  cout << 'o';
    if(s == "body")  cout << '*';
    if(s == "space") cout << ' ';
    if(s == "food")  cout << '@';
}

void release_point(node p){
    char t;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取句柄
    pos = {p.y, p.x};
    SetConsoleCursorPosition(hOut, pos); //移动光标
    cout << ' ';
}


void move_point(node& p){
    if(direc[p.x][p.y] == 1){
        p.x = p.x - 1;  // 上
    } else if(direc[p.x][p.y] == 2){
        p.x = p.x + 1;  // 下
    } else if(direc[p.x][p.y] == 3){
        p.y = p.y - 1;  // 左
    } else if(direc[p.x][p.y] == 4){
        p.y = p.y + 1;  // 右
    }
    p.x = (p.x-1+HEIGHT)%HEIGHT+1; p.y = (p.y-1+WIDTH)%WIDTH+1;
}