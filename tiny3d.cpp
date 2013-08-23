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

class Vertex
{
public:
    float x, y, z;
};

class UV
{
public:
    float u, v;
};

struct Face
{
    unsigned int v1, v2, v3;
    unsigned int uv1, uv2, uv3;
    unsigned int norm1, norm2, norm3;
};

void tiny3d::fromObjToTiny(const char* cObjFile, const char* cTinyFile)
{
	vector<Vertex> vVerts;
    vector<Vertex> vNormals;
    vector<UV> vUVs;
    UV tmp;
    tmp.u = tmp.v = 0.0;
    vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
    list<Face> lFaces;

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
                    UV vt;
                    if(!(iss >> vt.u >> vt.v)) continue;
                    //Flip UV coordinates to match up right
                    vt.v = 1.0 - vt.v;
                    vUVs.push_back(vt);
                }
                else if(c[1] == 'n')    //"vn" denotes face normal
                {
                    bNorms = true;
                    Vertex vn;
                    if(!(iss >> vn.x >> vn.y >> vn.z)) continue; //Skip over malformed lines
                    vNormals.push_back(vn);
                }
                else    //"v" denotes vertex
                {
                    Vertex v;
                    if(!(iss >> v.x >> v.y >> v.z)) continue; //Skip over malformed lines
                    vVerts.push_back(v);
                }
                break;
            case 'f':
                Face f;
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
	
	vert* vertices = (vert*)malloc(sizeof(vert) * vVerts.size());
	face* faces = (face*)malloc(sizeof(face) * lFaces.size());
	
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
	fwrite(&header, 1, sizeof(tiny3dHeader), fp);
	int curFace = 0;
	for(list<Face>::iterator i = lFaces.begin(); i != lFaces.end(); i++)
    {
		vertices[i->v1-1].x = vVerts[i->v1-1].x;
		vertices[i->v1-1].y = vVerts[i->v1-1].y;
		vertices[i->v1-1].z = vVerts[i->v1-1].z;
		
		vertices[i->v2-1].x = vVerts[i->v2-1].x;
		vertices[i->v2-1].y = vVerts[i->v2-1].y;
		vertices[i->v2-1].z = vVerts[i->v2-1].z;
		
		vertices[i->v3-1].x = vVerts[i->v3-1].x;
		vertices[i->v3-1].y = vVerts[i->v3-1].y;
		vertices[i->v3-1].z = vVerts[i->v3-1].z;
		
		vertices[i->v1-1].texpos.u = vUVs[i->uv1].u;
		vertices[i->v1-1].texpos.v = vUVs[i->uv1].v;
		
		vertices[i->v2-1].texpos.u = vUVs[i->uv2].u;
		vertices[i->v2-1].texpos.v = vUVs[i->uv2].v;
		
		vertices[i->v3-1].texpos.u = vUVs[i->uv3].u;
		vertices[i->v3-1].texpos.v = vUVs[i->uv3].v;
		
		vertices[i->v1-1].norm.x = vNormals[i->norm1-1].x;
		vertices[i->v1-1].norm.y = vNormals[i->norm1-1].y;
		vertices[i->v1-1].norm.z = vNormals[i->norm1-1].z;
		
		vertices[i->v2-1].norm.x = vNormals[i->norm2-1].x;
		vertices[i->v2-1].norm.y = vNormals[i->norm2-1].y;
		vertices[i->v2-1].norm.z = vNormals[i->norm2-1].z;
		
		vertices[i->v3-1].norm.x = vNormals[i->norm3-1].x;
		vertices[i->v3-1].norm.y = vNormals[i->norm3-1].y;
		vertices[i->v3-1].norm.z = vNormals[i->norm3-1].z;
		
		faces[curFace].v1 = i->v1-1;
		faces[curFace].v2 = i->v2-1;
		faces[curFace].v3 = i->v3-1;
		/*face f;
		f.norm1.x = vNormals[i->norm1-1].x;
		f.norm1.y = vNormals[i->norm1-1].y;
		f.norm1.z = vNormals[i->norm1-1].z;
		
		f.norm2.x = vNormals[i->norm2-1].x;
		f.norm2.y = vNormals[i->norm2-1].y;
		f.norm2.z = vNormals[i->norm2-1].z;
		
		f.norm3.x = vNormals[i->norm3-1].x;
		f.norm3.y = vNormals[i->norm3-1].y;
		f.norm3.z = vNormals[i->norm3-1].z;
		
		f.v1.x = vVerts[i->v1-1].x;
		f.v1.y = vVerts[i->v1-1].y;
		f.v1.z = vVerts[i->v1-1].z;
		
		f.v2.x = vVerts[i->v2-1].x;
		f.v2.y = vVerts[i->v2-1].y;
		f.v2.z = vVerts[i->v2-1].z;
		
		f.v3.x = vVerts[i->v3-1].x;
		f.v3.y = vVerts[i->v3-1].y;
		f.v3.z = vVerts[i->v3-1].z;
		
		f.v1.u = vUVs[i->uv1].u;
		f.v1.v = vUVs[i->uv1].v;
		
		f.v2.u = vUVs[i->uv2].u;
		f.v2.v = vUVs[i->uv2].v;
		
		f.v3.u = vUVs[i->uv3].u;
		f.v3.v = vUVs[i->uv3].v;

		fwrite(&f, 1, sizeof(face), fp);*/
		curFace++;
	}
	
	fwrite(vertices, header.numVertices, sizeof(vert), fp);
	fwrite(faces, header.numFaces, sizeof(face), fp);
	
	fclose(fp);
	
	free(vertices);
	free(faces);
}

void tiny3d::fromTinyToObj(const char* cTinyFile, const char* cObjFile)
{
	//TODO
}