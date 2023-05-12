#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {
      parents.resize(mapImg.height());
      for (int i = 0; i < mapImg.height(); i++) {
         parents[i].resize(mapImg.width());
      }

      Stack<pair<int, int>> work;

      pair<int, int> end;
      end = findSpot();

      work.push(end);

      pair<int, int> curr;
      curr = end;

      while (curr != start) {
         work.push(parents[curr.second][curr.first]);
         curr = parents[curr.second][curr.first];
      }

      while (!work.isEmpty()) {
         pathPts.push_back(work.pop());
      }

      // pathPts.push_back(start);
}

PNG decoder::renderSolution(){
   PNG treasure = mapImg;

   for (int i = 0; i < pathPts.size(); i++) {
      RGBAPixel *pixel = treasure.getPixel(pathPts[i].first, pathPts[i].second);
      pixel->r = 255;
      pixel->g = 0;
      pixel->b = 0;
   }

   return treasure;
}

PNG decoder::renderMaze(){
   PNG treasure = mapImg;

   vector<vector<bool>> visited(mapImg.height(), vector<bool>(mapImg.width(), false));

   vector<vector<int>> path(mapImg.height(), vector<int>(mapImg.width(), -1));

   Queue<pair<int, int>> todo;

   visited[start.second][start.first] = true;
    
   path[start.second][start.first] = 0;

   setGrey(treasure, start);

   todo.enqueue(start);

   while (!todo.isEmpty()) {
      pair<int, int> curr = todo.dequeue();

      vector<pair<int, int>> p = neighbors(curr);

      for (int i = 0; i < p.size(); i++) {
         if (good(visited, path, curr, p[i])) {
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

void decoder::setGrey(PNG & im, pair<int,int> loc){   
   RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
   
   pixel->r = 2 * (pixel->r / 4);
   pixel->g = 2 * (pixel->g / 4);
   pixel->b = 2 * (pixel->b / 4);
   pixel->a = 1.0;
}

pair<int,int> decoder::findSpot(){
   vector<vector<bool>> visited(mapImg.height(), vector<bool>(mapImg.width(), false));

   vector<vector<int>> path(mapImg.height(), vector<int>(mapImg.width(), -1));

   Queue<pair<int, int>> todo;

   visited[start.second][start.first] = true;
    
   path[start.second][start.first] = 0;

   // parents[start.second][start.first] = pair<int, int>(-1, -1);

   todo.enqueue(start);

   pair<int, int> curr;

   while (!todo.isEmpty()) {
      curr = todo.dequeue();

      vector<pair<int, int>> p = neighbors(curr);

      for (int i = 0; i < p.size(); i++) {
         if (good(visited, path, curr, p[i])) {
            visited[p[i].second][p[i].first] = true;

            path[p[i].second][p[i].first] = path[curr.second][curr.first] + 1;

            parents[p[i].second][p[i].first] = curr;

            todo.enqueue(p[i]);
         }
      }
   }

   return curr;
}

int decoder::pathLength(){
   return pathPts.size();
}

bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
   int x = next.first;
   int y = next.second;
   int i = curr.first;
   int j = curr.second;

   bool inbounds1 = x >= 0 && x < mapImg.width() && y >= 0 && y < mapImg.height();
   bool inbounds2= i >= 0 && i < mapImg.width() && j >= 0 && j < mapImg.height();

    if (!inbounds1 || !inbounds2) {
        return false;
    } 

    if (v[y][x]) {
      return false;
    }
    
   RGBAPixel *pixelCurr = mapImg.getPixel(curr.first, curr.second);
   RGBAPixel *pixelNext = mapImg.getPixel(x, y);

   return compare(*pixelNext, d[curr.second][curr.first]);
}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {
   vector<pair<int, int>> neighbor(4);

   neighbor[0] = pair<int, int>(curr.first - 1, curr.second);
   neighbor[1] = pair<int, int>(curr.first, curr.second + 1);
   neighbor[2] = pair<int, int>(curr.first + 1, curr.second);
   neighbor[3] = pair<int, int>(curr.first, curr.second - 1);

   return neighbor;
}


bool decoder::compare(RGBAPixel p, int d){
   // unsigned char vR = p.r & 3;
   // vR <<= 4;

   // unsigned char vG = (p.g & 3) << 2;

   // unsigned char vB =p.b & 3;

   // unsigned char value = (vR | vG) | vB;

   int v = (d + 1) % 64;
   int pV = (p.r % 4) * 16 + (p.g % 4) * 4 + (p.b % 4);

   return v == pV;
}
