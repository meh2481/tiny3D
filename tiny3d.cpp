//Functions for use by tiny3d
#include "tiny3d.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <stdlib.h>
using namespace std;

void tiny3d::fromObjToTiny(const char* cObjFile, const char* cTinyFile)
{
	vector<vert> vVerts;
    vector<normal> vNormals;
    vector<uv> vUVs;
    uv tuv;
    tuv.u = tuv.v = 0.0;
    vUVs.push_back(tuv);
	normal tn;
	tn.x = tn.y = tn.z = 0.0;
	vNormals.push_back(tn);
	vert tv;
	tv.x = tv.y = tv.z = 0.0;
	vVerts.push_back(tv);
    list<face> lFaces;

    bool bUVs = false;
    bool bNorms = false;

    ifstream infile(cObjFile);
    if(infile.fail())
    {
        cout << "Error: Unable to open wavefront object file " << cObjFile << endl;
        return;    //Abort
    }
    //Loop until we hit eof or fail
    while(!infile.eof() && !infile.fail())
    {
        string s;
        getline(infile, s);
        if(infile.eof() || infile.fail())
            break;
        istringstream iss(s);
        string c;
        if(!(iss >> c)) break;
        switch (c[0])
        {
            case 'v': //Vertex
                if(c[1] == 't') //"vt" denotes UV coordinate
                {
                    bUVs = true;
                    uv vt;
                    if(!(iss >> vt.u >> vt.v)) continue;
                    //Flip UV coordinates to match up right
                    vt.v = 1.0 - vt.v;
                    vUVs.push_back(vt);
                }
                else if(c[1] == 'n')    //"vn" denotes face normal
                {
                    bNorms = true;
                    normal vn;
                    if(!(iss >> vn.x >> vn.y >> vn.z)) continue; //Skip over malformed lines
                    vNormals.push_back(vn);
                }
                else    //"v" denotes vertex
                {
                    vert v;
                    if(!(iss >> v.x >> v.y >> v.z)) continue; //Skip over malformed lines
                    vVerts.push_back(v);
                }
                break;
            case 'f':
                face f;
                char ctmp;
                iss.get(ctmp);
                if(iss.eof() || infile.eof() || iss.fail() || infile.fail())
                    break;
                for(int i = 0; i < 3; i++)
                {
                    uint32_t vertPos = 0;
                    uint32_t uvPos = 0;
                    uint32_t normPos = 0;
                    string sCoord;
                    getline(iss, sCoord, ' ');
                    istringstream issCord(sCoord);
                    issCord >> vertPos;
                    if(bNorms)
                    {
                        issCord.ignore();   //Ignore the '/' character
                        if(bUVs)
                            issCord >> uvPos;
                        issCord.ignore();
                        issCord >> normPos;
                    }
                    else if(bUVs)
                    {
                        issCord.ignore();
                        issCord >> uvPos;
                    }

                    switch(i)
                    {
                        case 0:
                            f.v1 = vertPos;
                            f.uv1 = uvPos;
                            f.norm1 = normPos;
                            break;
                        case 1:
                            f.v2 = vertPos;
                            f.uv2 = uvPos;
                            f.norm2 = normPos;
                            break;
                        case 2:
                            f.v3 = vertPos;
                            f.uv3 = uvPos;
                            f.norm3 = normPos;
                            break;

                    }
                }
                lFaces.push_back(f);
                break;
            default:    //Skip anything else we don't care about (Comment lines; mtl definitions, etc)
                continue;
        }
    }
    infile.close();
	
	//Now write to output file
	FILE* fp = fopen(cTinyFile, "wb");
	if(fp == NULL)
	{
		cout << "Error: Couldn't open " << cTinyFile << " for writing." << endl;
		return;
	}
	
	//Write header
	tiny3dHeader header;
	header.numFaces = lFaces.size();
	header.numVertices = vVerts.size();
	header.numNormals = vNormals.size();
	header.numUVs = vUVs.size();
	fwrite(&header, 1, sizeof(tiny3dHeader), fp);
	
	//Write normals
	for(vector<normal>::iterator i = vNormals.begin(); i != vNormals.end(); i++)
	{
		fwrite(&(*i), 1, sizeof(normal), fp);
	}
	
	//Write UVs
	for(vector<uv>::iterator i = vUVs.begin(); i != vUVs.end(); i++)
	{
		fwrite(&(*i), 1, sizeof(uv), fp);
	}
	
	//Write vertices
	for(vector<vert>::iterator i = vVerts.begin(); i != vVerts.end(); i++)
	{
		fwrite(&(*i), 1, sizeof(vert), fp);
	}
	
	//Write faces
	for(list<face>::iterator i = lFaces.begin(); i != lFaces.end(); i++)
	{
		fwrite(&(*i), 1, sizeof(face), fp);
	}
	
	fclose(fp);
}

void tiny3d::fromTinyToObj(const char* cTinyFile, const char* cObjFile)
{
	//TODO: I don't really care for now.
}