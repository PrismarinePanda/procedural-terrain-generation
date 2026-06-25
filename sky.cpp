#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define SIZE 101
float lattice[SIZE][SIZE];
int interval=5;
double amp=0.75;
int seed=time(NULL);

//1D interpolation
float lerp (float a, float b, float t) {
  return (b - a)*((t * (t * 6.0 - 15.0) + 10.0) * t * t * t) + a;
}

//2D interpolation Entire Lattice
float bilerp(int i, int j, int interval){
  int top = (j/interval)*interval;
  int left = (i/interval)*interval;
  int right = min(left + interval, SIZE);
  int bottom = min(top + interval, SIZE);

  float xPerc = (i-left)/(float) interval;
  float yPerc = (j-top)/(float) interval;
  
  //interpolation of top left and right
  float topValue = lerp(lattice[top][left], lattice[top][right], xPerc);
  //interpolation of bottom left and bottom right
  float bottomValue = lerp(lattice[bottom][left], lattice[bottom][right], xPerc);
  //interpolation of a and b
  return lerp(topValue, bottomValue, yPerc);
}

int main() {
  //set the rand function to generate unique values each run, c++ defaults to a seed of 1
  srand(seed);
  cout<<"seed:"<<seed<<endl;

  //GENERATING LATTICE POINTS
  for(int i=0; i<SIZE; i+=interval){
    for (int j=0; j<SIZE; j+=interval){
      lattice[i][j] = (255*((rand() % 1000)/1000.00)*amp);
    }
    if(i>(SIZE/4)){
      amp-=0.1;
    }
  }
   
  //INTERPOLATION
  for (int r = 0; r < SIZE; r++) { 
    for (int c = 0; c < SIZE; c++){  
      lattice[r][c] = (int) (bilerp(c, r, interval));
      if(lattice[r][c]<0){
        lattice[r][c]=abs(lattice[r][c]);
      }
    }
  }
  
  //PRINT data
  for (int r = 0; r < SIZE-1; r++) { 
    for (int c = 0; c < SIZE-1; c++){
      cout<<lattice[r][c]<<",";
    }
    cout<<endl;
  }
}