/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CTerrain.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
#define SIZE 33
#define BIOMENUM 2
//becuase the biomes are squares this is wonky
#define BIOMESIZE 16
int freq = 8;
double amp = 1;
int interval = (SIZE / freq);
float lattice[SIZE][SIZE];

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Default constructor.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CTerrain::CTerrain()
{
    m_pHeight = NULL;
    m_pTexture = NULL;
    m_numVertices = m_numIndices = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Smooth Step Interpolation
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
float lerp(float a, float b, float t) {
    return (b - a) * ((t * (t * 6.0 - 15.0) + 10.0) * t * t * t) + a;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: 2d interpolation Interpolation
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
float bilerp(int i, int j) {
    int top = (j / interval) * interval;
    int left = (i / interval) * interval;
    int right = min(left + interval, SIZE - 1);
    int bottom = min(top + interval, SIZE - 1);

    float xPerc = (i - left) / (float)interval;
    float yPerc = (j - top) / (float)interval;

    //interpolation of top left and right
    float topValue = lerp(lattice[top][left], lattice[top][right], xPerc);
    //interpolation of bottom left and bottom right
    float bottomValue = lerp(lattice[bottom][left], lattice[bottom][right], xPerc);
    //interpolation of a and b
    return lerp(topValue, bottomValue, yPerc);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Create a new Terrain object.
Parameters:
[in] pDevice - D3D Device
[in] rawFile - Name of the height map file
[in] terrainTexture - Texture file name
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CTerrain::Initialize( LPDIRECT3DDEVICE9 pDevice, char *rawFile, char *terrainTexture )
{
    Release();

    //using input as a seed, set the rand function to generate unique values each run, c++ defaults to a seed of 1
    srand(time(NULL));

    cout << "setting lattice points" << endl;
    int biomestartr = 0;
    int biomestartc = 0;
    int biomeendr = BIOMESIZE;
    int biomeendc = BIOMESIZE;

    bool bottomedge=false;
    bool topedge=false;
    bool leftedge=false;
    bool rightedge=false;

    int biomenum = 1;
    while (biomeendr <= SIZE) {
        cout << "current biome: " << biomenum << endl;
        //cout << "startr: " << biomestartr << " startc: "<< biomestartc <<" endr: "<<biomeendr << " endc: "<<biomeendc<<endl;

        int biome = 2;
        //ocean
        if (biome == 1) {
            freq = 8;
            if (biomeendr == SIZE) {
                bottomedge = true;
            }
            if (biomestartr == 0) {
                topedge = true;
            }
            if (biomeendc == SIZE) {
                rightedge = true;
            }
            if (biomestartc == 0) {
                leftedge = true;
            }
            cout << "top: " << topedge << endl;
            cout << "bottom: " << bottomedge << endl;
            cout << "left: " << leftedge << endl;
            cout << "right: " << rightedge << endl;
        }
        //mountain
        if (biome == 2) {
            freq = 8;
            amp = 0.000001;
        }
        interval = (SIZE / freq);
        //actually set lattice values
        for (int r = biomestartr; r < biomeendr; r += interval) {
            for (int c = biomestartc; c < biomeendc; c += interval) {
                //this is beach code 
                if ((r - biomestartr < freq && leftedge) || (c - biomestartc < freq && topedge) || (r - biomestartr > biomeendr - freq && bottomedge) || (c - biomestartc > biomeendc - freq && rightedge)) {
                    amp = 0.05;
                }
                else {
                    amp = 1;
                }
                //end beach code
                lattice[r][c] = (255 * ((rand() % 1000) / 1000.00) * amp);
            }
        }
        //move to next biome
        if (biomeendc < SIZE) {
            biomeendc += BIOMESIZE + 1;
            biomestartc += BIOMESIZE;
        }
        else {
            biomeendr += BIOMESIZE + 1;
            biomestartr += BIOMESIZE;
            //reset these to fill in the next row
            biomeendc = BIOMESIZE;
            biomestartc = 0;
        }
        biomenum++;
    }

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            lattice[r][c] = bilerp(r, c);
            lattice[r][c] = (char)(lattice[r][c]);
        }
    }
    
    
    int m_numVertices = SIZE * SIZE;

    // Allocate memory and read the data
    m_pHeight = new UCHAR[m_numVertices];
    int i = 0;
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            m_pHeight[i] = lattice[r][c];
                i++;
        }
    }
    

    // Generate vertices
    UINT width = (int)sqrt( (float)m_numVertices );
    cuCustomVertex::PositionTextured* pVertices = NULL; 
    CTriangleStripPlane::GeneratePositionTexturedWithHeight( &pVertices, width, width, m_pHeight );
    m_vb.CreateBuffer( pDevice, m_numVertices, D3DFVF_XYZ | D3DFVF_TEX1, sizeof( cuCustomVertex::PositionTextured ) );
    m_vb.SetData( m_numVertices, pVertices, 0 );

    // Generate indices
    int* pIndices = NULL;
    m_numIndices = CTriangleStripPlane::GenerateIndices( &pIndices, width, width );
    m_ib.CreateBuffer( pDevice, m_numIndices, D3DFMT_INDEX32 );
    m_ib.SetData( m_numIndices, pIndices, 0 );
    m_vb.SetIndexBuffer( &m_ib );
    
    /*CUtility::GetMediaFile(terrainTexture, path);
    if ( FAILED( D3DXCreateTextureFromFile( pDevice, path, &m_pTexture ) ) )
    {
        SHOWERROR( "Unable to load terrain textures.", __FILE__, __LINE__ );
        return FALSE;

    }*/
    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Renders the terrain.
Parameters:
[in] pDevice - D3D Device
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTerrain::Render( LPDIRECT3DDEVICE9 pDevice )
{
    pDevice->SetTransform( D3DTS_WORLD, GetTransform() );
    pDevice->SetTexture( 0, m_pTexture );
    m_vb.Render( pDevice, m_numIndices - 2, D3DPT_TRIANGLESTRIP );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Release resources
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTerrain::Release()
{
    SAFE_DELETE_ARRAY( m_pHeight );
    SAFE_RELEASE( m_pTexture );
    m_vb.Release();
    m_ib.Release();
}