#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{
    base = baseim;
    maze = mazeim;
    start = s;
}

void treasureMap::setGrey(PNG & im, pair<int,int> loc) {
    RGBAPixel *pixel = im.getPixel(loc.first, loc.second);

    pixel->r = 2 * (pixel->r / 4);
    pixel->g = 2 * (pixel->g / 4);
    pixel->b = 2 * (pixel->b / 4);
    pixel->a = 1.0;
}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d) {
    // cout << d << " d" << endl;
    // int v = d % 64;
    // cout << v << " v" << endl;
    // int v1 = v;
    // int v2 = v;

    // int vR = v1 >>= 4;
    // cout << (int)vR << " vR" << endl;
    // cout << v << " v1" << endl;

    // int vG = v2 >>= 2;
    // vG &= 3;

    // int vB = v;
    // vB &= 3;

    // RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
    // cout << (int)pixel->r << " r0" << endl;
    
    // pixel->r = (int)pixel->r & 252;
    // cout << (int)pixel->r << " r1" << endl;
    // pixel->r = (int)pixel->r | vR;
    // cout << (int)pixel->r << " r2" << endl;

    // char vR = (d >> 4) & 3;
    // pixel->r &= 252;
    // pixel->r |= vR;

    // pixel->g &= 252;
    // pixel->g != vG;
    // // pixel->g = 255;

    // pixel->b &= 252;
    // pixel->b |= vB;
    // pixel->b = 255;

    unsigned char v = d % 64;

    RGBAPixel *Pixel = im.getPixel(loc.first, loc.second);

    unsigned char r = Pixel-> r & 0b11111100;
    unsigned char g = Pixel-> g & 0b11111100;
    unsigned char b = Pixel-> b & 0b11111100;

    Pixel->r = r | (v >> 4);
    Pixel->g = r | (v >> 2 & 0b00000011);
    Pixel->b = r | (v & 0b00000011);
}

PNG treasureMap::renderMap() {
    PNG treasure = base;

    vector<vector<bool>> visited(treasure.height(), vector<bool>(treasure.width()));
    for (int j = 0; j < treasure.height(); j++) {
        for (int i; i < treasure.width(); i++) {
            visited[j][i] = false;
        }
    }

    vector<vector<int>> path(treasure.height(), vector<int>(treasure.width()));
    for (int j = 0; j < treasure.height(); j++) {
        for (int i; i < treasure.width(); i++) {
            path[j][i] = -1;
        }
    }

    Queue<pair<int, int>> todo;

    visited[start.second][start.first] = true;
    
    path[start.second][start.first] = 0;

    setLOB(treasure, start, 0);

    todo.enqueue(start);

    while (!todo.isEmpty()) {
        pair<int, int> curr = todo.dequeue();
        // cout << curr.first << "  " << curr.second << endl;

        vector<pair<int, int>> p = neighbors(curr);

        for (int i = 0; i < 4; i++) {
            if (good(visited, curr, p[i])) {
                visited[p[i].second][p[i].first] = true;

                path[p[i].second][p[i].first] = path[curr.second][curr.first] + 1;
                // cout << path[p[i].second][p[i].first] << " path" << endl;

                setLOB(treasure, p[i], path[p[i].second][p[i].first]);

                todo.enqueue(p[i]);
                // cout << p[i].first << "  " << p[i].second << " p" << endl;
            }
        }
    }

    return treasure;
}


PNG treasureMap::renderMaze() {
    PNG treasure = base;

    vector<vector<bool>> visited(base.height(), vector<bool>(base.width(), false));

    vector<vector<int>> path(base.height(), vector<int>(base.width(), -1));

    Queue<pair<int, int>> todo;

    visited[start.second][start.first] = true;
    
    path[start.second][start.first] = 0;

    setGrey(treasure, start);

    todo.enqueue(start);

    while (!todo.isEmpty()) {
        pair<int, int> curr = todo.dequeue();

        vector<pair<int, int>> p = neighbors(curr);

        for (int i = 0; i < p.size(); i++) {
            if (good(visited, curr, p[i])) {
                visited[p[i].second][p[i].first] = true;

                path[p[i].second][p[i].first] = path[curr.second][curr.first] + 1;

                setGrey(treasure, p[i]);

                todo.enqueue(p[i]);
            }
        }
    }

    int x = start.first;
    int y = start.second;

    for (int i = x - 3; i <= x + 3; i++) {
        for (int j = y - 3; j <= y + 3; j++) {
            if (i >= 0 && i <= treasure.width() && j >= 0 && j <= treasure.height()) {
                RGBAPixel *pixel = treasure.getPixel(i, j);
                pixel->r = 255;
                pixel->g = 0;
                pixel->b = 0;
            }
        }
    }

    return treasure;
}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next) {
    int x = next.first;
    int y = next.second;
    bool inbounds = x >= 0 && x < (int)base.width() && y >= 0 && y < (int)base.height();

    if (!inbounds) {
        return false;
    }
    
    RGBAPixel *pixelCurr = maze.getPixel(curr.first, curr.second);
    RGBAPixel *pixelNext = maze.getPixel(x, y);

    return (v[y][x] == false) && *pixelNext == *pixelCurr;
    // (pixelNext->r == pixelCurr->r && pixelNext->g == pixelCurr->g && pixelNext->b == pixelCurr->b);

    // inbbounds && !v[next.second][next.first] && (curr_pix == next_pix);
}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
    vector<pair<int, int>> neighbor(4);

    neighbor[0] = pair<int, int>(curr.first - 1, curr.second);
    neighbor[1] = pair<int, int>(curr.first, curr.second + 1);
    neighbor[2] = pair<int, int>(curr.first + 1, curr.second);
    neighbor[3] = pair<int, int>(curr.first, curr.second - 1);

    return neighbor;
}

