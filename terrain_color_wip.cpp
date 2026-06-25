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
    bool bottomedge=false;
    bool topedge=false;
    bool leftedge=false;
    bool rightedge=false;
    int biometype=(abs((seed*biomenum))%5)%2;
    //ocean
    if(biometype==1){
      freq=12;
      interval=(SIZE/freq);
      if (biomeendr==SIZE){
        bottomedge=true;
        if((abs(seed*(biomenum+BIOMENUM))%5)%2==1 && (biomenum+BIOMENUM<=4)){
          bottomedge=false;
        }
      }
      if(biomestartr==0){
        topedge=true;
        if((abs(seed*(biomenum-BIOMENUM))%5)%2==1 && (biomenum+BIOMENUM>=0)){
          bottomedge=false;
        }
      }
      if (biomeendc==SIZE){
        rightedge=true;
        if((abs(seed*(biomenum+1))%5)%2==1 && (biomenum+BIOMENUM<=4)){
          bottomedge=false;
        }
      }
      if(biomestartc==0){
        leftedge=true;
        if((abs(seed*(biomenum-1))%5)%2==1 && (biomenum+BIOMENUM>=0)){
          bottomedge=false;
        }
      }
    }
    //mountain
    if(biometype==0){
      freq=32;
      interval=(SIZE/freq);
      amp =1.5;
    }
    //print what type each biome is
    cout<<"current biome: "<<biomenum<<" biome type: "<<biometype<<endl;
    //actually set lattice values
    for (int r = biomestartr; r < biomeendr; r+=interval) { 
      for (int c = biomestartc; c < biomeendc; c+=interval){ 
        //beach code 
        if(biometype==1){
          if((c-biomestartc>BIOMESIZE-(freq/2)&&leftedge) || (r-biomestartr>BIOMESIZE-(freq/2)&&topedge) || (r-biomestartr<(freq/2)&&bottomedge) || (c-biomestartc<(freq/2)&&rightedge)){
            amp=0.05;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
          }
          else{
            amp=0.4;
            lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp)+10;
          }
        }
        //end beach code
        else{
          lattice[r][c] = (255*((rand() % 1000)/1000.00)*amp);
        }
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
        lattice[r][c]=lattice[r][c]+INT8_MAX;
        lattice[r][c]=(int)lattice[r][c]%500;
      }
    }
  }
  
  //COLOR
  int color[SIZE][SIZE][3];
  biomenum=1;
  int biometype=(abs((seed*biomenum))%5)%2;
  
  for (int r = 0; r < SIZE; r++){ 
    for (int c = 0; c < SIZE; c++){
      //ocean
      if(biometype==1){
        if((r<=29&&r>=0 || r>=38)&&(c<=29&&c>=0 || c>=38)){
          if(lattice[r][c]>100){
            //light blue
            color[r][c][0]=51;
            color[r][c][1]=206;
            color[r][c][2]=255;
          }
          else{
            //dark blue
            color[r][c][0]=0;
            color[r][c][1]=40;
            color[r][c][2]=120;
          }
        }
        else{
          //beach
          if(lattice[r][c]<20){
            //yellow
            color[r][c][0]=255;  
            color[r][c][1]=243;
            color[r][c][2]=51;
          }  
          else if(lattice[r][c]<50){
            //green
            color[r][c][0]=0;
            color[r][c][1]=99;
            color[r][c][2]=21;
          }
          else if(lattice[r][c]<200){
            //grey
            color[r][c][0]=120;
            color[r][c][1]=120;
            color[r][c][2]=120;
          }
          else{
            //white
            color[r][c][0]=255;
            color[r][c][1]=255;
            color[r][c][2]=255;
          }
        }
      }
      //mountain
      if(biometype==0){
        if(lattice[r][c]<50){
          //green
          color[r][c][0]=0;
          color[r][c][1]=99;
          color[r][c][2]=21;
        }
        else if(lattice[r][c]<200){
          //grey
          color[r][c][0]=120;
          color[r][c][1]=120;
          color[r][c][2]=120;
        }
        else{
          //white
          color[r][c][0]=255;
          color[r][c][1]=255;
          color[r][c][2]=255;
        }
      }
      //error
      else{ 
        //red
        color[r][c][0]=255;
        color[r][c][1]=0;
        color[r][c][2]=0;
      }
      if(c%32==0 && r%32==0 && c!=0 && r!=0){
        biomenum++;
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