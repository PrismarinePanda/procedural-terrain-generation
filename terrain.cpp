#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define SIZE 65
#define BIOMENUM 2
#define BIOMESIZE 32
float lattice[SIZE][SIZE];
float biome[BIOMESIZE][BIOMESIZE];
int seed=time(NULL);

//1D interpolation
float lerp (float a, float b, float t) {
  return (b - a)*((t * (t * 6.0 - 15.0) + 10.0) * t * t * t) + a;
}

//2D interpolation Entire Lattice
float bilerpLattice(int i, int j, int interval){
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

//2D interpolation Biome specific
float bilerpBiome(int i, int j, int interval, int biomenum){
  int top = (j/interval)*interval;
  int left = (i/interval)*interval;
  int right = min(left + interval, BIOMESIZE);
  int bottom = min(top + interval, BIOMESIZE);

  float xPerc = (i-left)/(float) interval;
  float yPerc = (j-top)/(float) interval;
  
  //interpolation of top left and right
  float topValue = lerp(biome[top][left], biome[top][right], xPerc);
  //interpolation of bottom left and bottom right
  float bottomValue = lerp(biome[bottom][left], biome[bottom][right], xPerc); 
  //interpolation of a and b
  return lerp(topValue, bottomValue, yPerc);
}

int main() {
  //set the rand function to generate unique values each run, c++ defaults to a seed of 1
  srand(seed);
  cout<<"seed:"<<seed<<endl;

  //generating lattice points by biome
  int biomestartr=0;
  int biomestartc=0;
  int biomeendr=BIOMESIZE;
  int biomeendc=BIOMESIZE;

  int biomenum=1;
  int freq;
  int interval;
  double amp;
  
  while(biomeendr<=SIZE){
    //what type each biome is
    int biometype=(abs((seed*biomenum))%5)%2;
    cout<<"current biome: "<<biomenum<<" biome type: "<<biometype<<endl;

    //ocean
    bool bottomedge=true;
    bool topedge=true;
    bool leftedge=true;
    bool rightedge=true;
    if(biometype==1){
      freq=12;
      interval=(SIZE/freq);
      if (biomeendr==SIZE){
        //cout<<"border on the bottom"<<endl;
        bottomedge=false;
      }
      if((biomenum+BIOMENUM<=4) && ((abs(seed*(biomenum+BIOMENUM))%5)%2==1)){
        //cout<<"ocean below"<<endl;
        bottomedge=false;
      }
      if(biomestartr==0){
        //cout<<"border on the top"<<endl;
        topedge=false;
      }
      if((biomenum-BIOMENUM>=1) && ((abs(seed*(biomenum-BIOMENUM))%5)%2==1)){
        //cout<<"ocean above"<<endl;
        topedge=false;
      }
      if (biomeendc==SIZE){
        //cout<<"border on the right"<<endl;
        rightedge=false;
      }
      if((biomenum+1<=4)&&(abs(seed*(biomenum+1))%5)%2==1){
        rightedge=false;
        //cout<<"ocean to the right"<<endl;
      }
      if(biomestartc==0){
        //cout<<"border on the left"<<endl;
        leftedge=false;
      }
      if((biomenum-1>=1) && (abs(seed*(biomenum-1))%5)%2==1){
        leftedge=false;
        //cout<<"ocean to the left"<<endl;
      }
    }
    
    //mountain
    if(biometype==0){
      freq=32;
      interval=(SIZE/freq);
      amp =1.5;
    }
    //actually set lattice values
    for (int r = biomestartr; r < biomeendr; r+=interval) { 
      for (int c = biomestartc; c < biomeendc; c+=interval){ 
        //beach code 
        if(biometype==1){
          if(c-biomestartc>BIOMESIZE-(freq/2) &&rightedge){
            //cout<<"rightedge"<<endl;
            amp=0.05;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
          }
          else if(r-biomestartr>BIOMESIZE-(freq/2) &&bottomedge){
            //cout<<"bottomedge"<<endl;
            amp=0.05;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
          }
          else if(r-biomestartr<(freq/2) &&topedge){
            //cout<<"top edge"<<r<<c<<endl;
            amp=0.05;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
            if(r==32 && c==10){
              //cout<<"TOP: "<<lattice[r][c]<<endl;
            }
          }
          else if(c-biomestartc<(freq/2) &&leftedge){
            //cout<<"left edge"<<c-biomestartc<<"<"<<(freq/2)<<endl;
            amp=0.05;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
          }
          else{
            //cout<<"else"<<endl;
            amp=0.4;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp)+10;
          }
        }
        //end beach code
        else{
          lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
        }
        //cout<<lattice[32][10]<<endl;
      }
    } 
    //move to next biome, in the same row
    if(biomeendc<SIZE){
      biomeendc+=BIOMESIZE+1;
      biomestartc+=BIOMESIZE;
    }
    //move to next biome, in the next row
    else{
      biomeendr+=BIOMESIZE+1;
      biomestartr+=BIOMESIZE;
      //reset these to start in the next row
      biomeendc=BIOMESIZE;
      biomestartc=0;
    }
    biomenum++;
  }
  
  //INTERPOLATION
  //create a 2d array of just that biome
  biomenum=1;
  biomestartr=0;
  biomestartc=0;
  biomeendr=BIOMESIZE;
  biomeendc=BIOMESIZE;
  while(biomenum<=(BIOMENUM*BIOMENUM)){
    int i = 0; 
    int j = 0;
      for (int r = biomestartr; r < biomeendr; r++) { 
        for (int c = biomestartc; c < biomeendc; c++){ 
          biome[i][j]=lattice[r][c];
          j++;
        }
        i++;
        j=0;
      } 
    
    int biometype=(abs((seed*biomenum))%5)%2;
    
    //interpolate that array
    for (int r = 0; r < BIOMESIZE; r++) { 
      for (int c = 0; c < BIOMESIZE; c++){
        if (biometype==0){
          interval=(SIZE/32);
        }
        if(biometype==1){
          interval=(SIZE/12);
        }
        biome[r][c] = (int) (bilerpBiome(c, r, interval, biomenum));
      }
    }

    //copy values back to lattice
    i = 0; 
    j = 0;
    for (int r = biomestartr; r < biomeendr; r++) { 
      for (int c = biomestartc; c < biomeendc; c++){ 
        lattice[r][c]=biome[i][j];
        j++;
      }
      i++;
      j=0;
    }

    //move to next biome, in the same row
    if(biomeendc<SIZE){
      biomeendc+=BIOMESIZE+1;
      biomestartc+=BIOMESIZE;
    }
    //move to next biome, in the next row
    else{
      biomeendr+=BIOMESIZE+1;
      biomestartr+=BIOMESIZE;
      //reset these to start in the next row
      biomeendc=BIOMESIZE;
      biomestartc=0;
    }
    biomenum++;
  }

  //interpolate the whole thing to smooth between biomes
  for (int r = 0; r < SIZE; r++) { 
    for (int c = 0; c < SIZE; c++){  
      lattice[r][c] = (int) (bilerpLattice(c, r, 5));
      if(lattice[r][c]<0){
        lattice[r][c]=abs(lattice[r][c]);
      }
    }
  }
  
  //print data
  for (int r = 0; r < SIZE-1; r++) { 
    for (int c = 0; c < SIZE-1; c++){
      cout<<lattice[r][c]<<",";
    }
    cout<<endl;
  }
}